#include <cpu.h>
#include <cpu/irq.h>
#include <globals.h>
#include <graphics/drawer.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <sys/timer.h>
#include <tests/test.h>
#include <util/mutex.h>
#include <util/time.h>

void handle_data(unsigned char);

void c_irq_handler(void)
{
	unsigned long source = load32(CORE0_IRQ_SOURCE);
	if (source & (1 << 8)) {
		if(load32(IRQ_PENDING2) & (1 << 25)) {
			if(load32(UART0_MIS) & (1<<4)) {
				unsigned long data = load32(UART0_DR);
				{
					unsigned int x = g_Drawer.x;
					unsigned int y = g_Drawer.y;
					g_Drawer.x = 0;
					g_Drawer.y = 14;
					write_hex32(&g_Drawer, data);
					g_Drawer.x = x;
					g_Drawer.y = y;
				}

				// Ctrl+T to toggle timer
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
					add_thread(handle_data, (void*)data, 1);
				}
				return;
			}
		} else if (*(unsigned long*)SYS_TIMER_CS == SYS_TIMER_SC_M0) {
			volatile unsigned long* timer_cs = (unsigned long*)SYS_TIMER_CS;
			volatile unsigned long* timer_chi = (unsigned long*)SYS_TIMER_CHI;
			volatile unsigned long* nexttime = (unsigned long*)SYS_TIMER_C0;
			*timer_cs = SYS_TIMER_SC_M0;
			*nexttime = *timer_chi + 60000000;
		}
	} else if (source & (1 << 3)) {
		c_timer();
		return;
	}
	return;
}

static unsigned long counter = 0;
unsigned long c_fiq_handler(void)
{
	unsigned long source = load32(CORE0_FIQ_SOURCE);
	if (source & (1 << 3)) {
		c_timer();
		counter++;
		if (counter % 0x6000 == 0) {
			counter = 0;
		}
		if (counter % 0x30 == 0) {
			return 1;
		}
		return 0;
	}
	return 0;
}

void handle_data(unsigned char data)
{
	// Newline Case
	if (data == 0x0D) {
	// Backspace Case
	} else if (data == 0x08 || data == 0x7F) {
	} else if (data == 0x61) {
		add_thread(uart_scheduler, 0, 2);
	} else if (data == 0x62) {
		add_thread(test_entry, 0, 2);
	}
}
