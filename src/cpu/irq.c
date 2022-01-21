#include <cpu/irq.h>
#include <drivers/uart.h>
#include <globals.h>
#include <graphics/drawer.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <sys/timer.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>

void localtest(void);

void c_irq_handler(void)
{
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
					g_Drawer.y = 5;
					write_string(&g_Drawer, "TIMER: ");
					if(timer_status == 0) {
						cntfrq = read_cntfrq();
						write_cntv_tval(cntfrq/CPS);
						enablecntv();
						write_cstring(&g_Drawer, "Enabled ", 0x00FF00);
					} else {
						disablecntv();
						write_cstring(&g_Drawer, "Disabled", 0xFF0000);
					}
					g_Drawer.x = x;
					g_Drawer.y = y;
				} else {
					unsigned long off = cmdidx;
					if (off < 2048) {
						// Newline Case
						if (data == 0x0D) {
							for(int i = off; i>=0;i--)
								cmd[i] = 0x0;
							off = 0;
						// Backspace Case
						} else if (data == 0x08 || data == 0x7F) {
							if (off > 0) {
								off -= 1;
							}
							cmd[off] = 0x0;
						// Lock Case
						} else if (data == 0x6C) {
							cmd[off] = (char) data;
							off += 1;
							lock_mutex(&exe_cnt_m, SYS_PID);
						// Release Case
						} else if (data == 0x72) {
							cmd[off] = (char) data;
							off += 1;
							release_mutex(&exe_cnt_m, SYS_PID);
						// New Task Case
						} else if (data == 0x61) {
							cmd[off] = (char) data;
							off += 1;
							_start(); // Trigger reset
							//heap_info_u();
						// Else output
						} else {
							cmd[off] = (char) data;
							off += 1;
						}
					} else if (off == 2048) {
						if (data == 0x0D) {
							for(int i = off; i>=0;i--)
								cmd[i] = 0x0;
							off = 0;
						} else if (data == 0x08 || data == 0x7F) {
							if (off > 0) {
								off -= 1;
							}
							cmd[off] = 0x0;
						}
					}
					cmdidx = off;
				}
				g_Drawer.x = 0;
				g_Drawer.y = 7;
				for(int i = 0; i < 128; i++)
					write_char(&g_Drawer, ' ');
				g_Drawer.x = 0;
				g_Drawer.y = 7;
				write_string(&g_Drawer, "> ");
				write_string(&g_Drawer, cmd);
				return;
			}
		} else if (*(unsigned long*)SYS_TIMER_CS == SYS_TIMER_SC_M0) {
			volatile unsigned long* timer_cs = (unsigned long*)SYS_TIMER_CS;
			volatile unsigned long* timer_chi = (unsigned long*)SYS_TIMER_CHI;
			volatile unsigned long* nexttime = (unsigned long*)SYS_TIMER_C0;
			*timer_cs = SYS_TIMER_SC_M0;
			*nexttime = *timer_chi + 60000000;
		} else {
			uart_string("Pending?");
		}
	} else if (source & (1 << 3)) {
		c_timer();
		return;
	}
	return;
}

void c_fiq_handler(void)
{
	unsigned long source = load32(CORE0_FIQ_SOURCE);
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
					g_Drawer.y = 5;
					write_string(&g_Drawer, "TIMER: ");
					if(timer_status == 0) {
						cntfrq = read_cntfrq();
						write_cntv_tval(cntfrq/CPS);
						enablecntv();
						write_cstring(&g_Drawer, "Enabled ", 0x00FF00);
					} else {
						disablecntv();
						write_cstring(&g_Drawer, "Disabled", 0xFF0000);
					}
					g_Drawer.x = x;
					g_Drawer.y = y;
				} else {
					unsigned long off = cmdidx;
					if (off < 2048) {
						// Newline Case
						if (data == 0x0D) {
							for(int i = off; i>=0;i--)
								cmd[i] = 0x0;
							off = 0;
						// Backspace Case
						} else if (data == 0x08 || data == 0x7F) {
							if (off > 0) {
								off -= 1;
							}
							cmd[off] = 0x0;
						// Lock Case
						} else if (data == 0x6C) {
							cmd[off] = (char) data;
							off += 1;
							lock_mutex(&exe_cnt_m, SYS_PID);
						// Release Case
						} else if (data == 0x72) {
							cmd[off] = (char) data;
							off += 1;
							release_mutex(&exe_cnt_m, SYS_PID);
						// New Task Case
						} else if (data == 0x61) {
							cmd[off] = (char) data;
							off += 1;
							//_start(); // Trigger reset
							//heap_info_u();
						// Else output
						} else {
							cmd[off] = (char) data;
							off += 1;
						}
					} else if (off == 2048) {
						if (data == 0x0D) {
							for(int i = off; i>=0;i--)
								cmd[i] = 0x0;
							off = 0;
						} else if (data == 0x08 || data == 0x7F) {
							if (off > 0) {
								off -= 1;
							}
							cmd[off] = 0x0;
						}
					}
					cmdidx = off;
				}
				g_Drawer.x = 0;
				g_Drawer.y = 7;
				for(int i = 0; i < 128; i++)
					write_char(&g_Drawer, ' ');
				g_Drawer.x = 0;
				g_Drawer.y = 7;
				write_string(&g_Drawer, "> ");
				write_string(&g_Drawer, cmd);
				return;
			}
		} else if (*(unsigned long*)SYS_TIMER_CS == SYS_TIMER_SC_M0) {
			volatile unsigned long* timer_cs = (unsigned long*)SYS_TIMER_CS;
			volatile unsigned long* timer_chi = (unsigned long*)SYS_TIMER_CHI;
			volatile unsigned long* nexttime = (unsigned long*)SYS_TIMER_C0;
			*timer_cs = SYS_TIMER_SC_M0;
			//*nexttime = *timer_chi + 60000000;
			*nexttime = *timer_chi + 10000;
		}
	} else if (source & (1 << 3)) {
		c_timer();
		return;
	}
	return;
}

void localtest(void)
{
	struct Thread* t = scheduler.rthread_ll->data;
	uart_string("Running IRQ Task... ");
	uart_10(t->data.pid);
	uart_char('\n');
	uart_string("Finished! ");
	uart_10(t->data.pid);
	uart_char('\n');
	sched_info();
}
