#include "../cpu/irq.h"
#include "../drivers/uart.h"
#include "../graphics/drawer.h"
#include "../sys/core.h"
#include "../sys/timer.h"
#include "../util/mutex.h"
#include "../util/time.h"

extern unsigned long cmdidx;
extern char cmd[2048];
extern void disable_irq(void);
extern void enable_irq(void);

void c_irq_handler(void) {
	//disable_irq();
	disableirq();
	unsigned long source = load32(CORE0_IRQ_SOURCE);
	if (source & (1 << 8)) {
		if(load32(IRQ_PENDING2) & (1 << 25)) {
			if(load32(UART0_MIS) & (1<<4)) {
				unsigned long data = load32(UART0_DR);
				// Ctrl+Tab to toggle timer
				if(data == 0x14) {
					unsigned long timer_status;
					asm volatile("mrc p15, 0, %0, c14, c3, 1" : "=r"(timer_status));
					unsigned int x = g_Drawer.x;
					unsigned int y = g_Drawer.y;
					g_Drawer.x = 0;
					g_Drawer.y = 7;
					write_string(&g_Drawer, "TIMER: ");
					if(timer_status == 0) {
						cntfrq = read_cntfrq();
						write_cntv_tval(cntfrq);
						enable_cntv();
						write_cstring(&g_Drawer, "Enabled ", 0x00FF00);
					} else {
						disable_cntv();
						write_cstring(&g_Drawer, "Disabled", 0xFF0000);
					}
					g_Drawer.x = x;
					g_Drawer.y = y;
				} else {
					unsigned long off = cmdidx;
					if (off < 2048) {
						// Newline Case
						if (data == 0x0D) {
							off = 0;
							cmd[0] = 0x0;
							uart_char(data);
							uart_string("\033[?25l> \033[0K\033[?25h");
						// Backspace Case
						} else if (data == 0x08 || data == 0x7F) {
							if (off > 0) {
								off -= 1;
							}
							cmd[off] = 0x0;
							uart_char((unsigned char)data);
							uart_char(0x20);
							uart_char((unsigned char)data);
						// Lock Case
						} else if (data == 0x6C) {
							uart_char((unsigned char)data);
							cmd[off] = (char) data;
							off += 1;
							lock_mutex(&exe_cnt_m, SCHED_PID);
						// Release Case
						} else if (data == 0x72) {
							uart_char((unsigned char)data);
							cmd[off] = (char) data;
							off += 1;
							release_mutex(&exe_cnt_m, SCHED_PID);
						// Else output
						} else {
							uart_char((unsigned char)data);
							cmd[off] = (char) data;
							off += 1;
						}
					} else if (off == 2048) {
						if (data == 0x0D) {
							off = 0;
							cmd[0] = 0x0;
							//uart_char(0x0a);
							uart_char(data);
							uart_string("\033[?25l> \033[0K\033[?25h");
						} else if (data == 0x08 || data == 0x7F) {
							if (off > 0) {
								off -= 1;
							}
							cmd[off] = 0x0;
							uart_char((unsigned char)data);
							uart_char(0x20);
							uart_char((unsigned char)data);
						}
					}
					cmdidx = off;
					//store32(off, (unsigned long*) cmdidx);
					uart_string("\033[?25l\033[9;1H\033[0K");
					uart_10(off);
					uart_string("\n\033[0K");
					uart_string(cmd);
					uart_string("\033[?25h\033[8;1H> ");
				}
				//enable_irq();
				enableirq();
				return;
			}
		}
	} else if (source & (1 << 3)) {
		c_timer();
		//enable_irq();
		enableirq();
		return;
	}
	return;
}
