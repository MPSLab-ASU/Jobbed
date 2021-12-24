#include "../cpu/irq.h"
#include "../drivers/uart.h"
#include "../drivers/usb.h"
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
					g_Drawer.y = 5;
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
							lock_mutex(&exe_cnt_m, SCHED_PID);
						// Release Case
						} else if (data == 0x72) {
							cmd[off] = (char) data;
							off += 1;
							release_mutex(&exe_cnt_m, SCHED_PID);
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
				enableirq();
				return;
			}
		} else if (load32(IRQ_PENDING1) & (1 << 9))  {
			unsigned long reg = *USB_CORE_GINTSTS;
			*USB_CORE_GINTSTS = reg;  //clear
			g_Drawer.y = 20;
			write_hex32(&g_Drawer, reg);
			g_Drawer.x = 0;
			g_Drawer.y = 0;
			if (reg & 1 << 28) { uart_string("Connector ID Status Change\n"); }
			if (reg & 1 << 27) { uart_string("LPM Transaction Received Interrupt\n"); }
			if (reg & 1 << 26) { uart_string("Periodic TxFIFO Empty\n"); }
			if (reg & 1 << 25) { uart_string("Host Channels Interrupt\n"); 
			                   }
			if (reg & 1 << 24) { *USB_HOST_HPRT |= 1 << 1;  //clear host port interrupt
			                     uart_string("Host Port Interrupt\n"); }
			if (reg & 1 <<  6) { uart_string("Global IN Non-periodic NAK Effective\n"); }
			if (reg & 1 <<  5) { uart_string("Non-periodic TxFIFO Empty\n"); }
			if (reg & 1 <<  4) { uart_string("Host and Device RxFIFO Non-Empty (RxFLvl) \n"); }
			if (reg & 1 <<  3) { uart_string("Host and Device Start of Frame\n"); }
			if (reg & 1 <<  2) { uart_string("OTG Interrupt\n"); }
			if (reg & 1 <<  1) { uart_string("Mode Mismatch Interrupt\n"); }
		}
	} else if (source & (1 << 3)) {
		c_timer();
		enableirq();
		return;
	}
	return;
}
