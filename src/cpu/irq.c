#include <cpu.h>
#include <cpu/irq.h>
#include <globals.h>
#include <graphics/drawer.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <tests/test.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>

#define CPS 1000

void handle_data(unsigned char);

void c_irq_handler(void)
{
	unsigned long source = load32(CORE0_IRQ_SOURCE);
	// Check if GPU Interrupt
	if (source & (1 << 8)) {
		// Check if UART Interrupt
		if(load32(IRQ_PENDING2) & (1 << 25)) {
			// Check if UART Interrupt is Masked
			if(load32(UART0_MIS) & (1<<4)) {
				// Get the UART data
				unsigned long data = load32(UART0_DR);
				// Draw it on the screen
				{
					unsigned int x = g_Drawer.x;
					unsigned int y = g_Drawer.y;
					g_Drawer.x = 0;
					g_Drawer.y = 9;
					write_chex32(&g_Drawer, data, 0xAA00FF);
					g_Drawer.x = x;
					g_Drawer.y = y;
				}

				// Handle the recieved data
				// Ctrl+T to toggle timer
				if(data == 0x14) {
					unsigned long timer_status;
					asm volatile("mrc p15, 0, %0, c14, c3, 1" : "=r"(timer_status));
					unsigned int x = g_Drawer.x;
					unsigned int y = g_Drawer.y;
					g_Drawer.x = 0;
					g_Drawer.y = 3;
					if(timer_status == 0) {
						cntfrq = read_cntfrq();
						write_cntv_tval(cntfrq/CPS);
						enablecntv();
						write_cstring(&g_Drawer, "TIMER", 0x00FF00);
						write_string(&g_Drawer, ": ");
					} else {
						disablecntv();
						write_cstring(&g_Drawer, "TIMER", 0xFF0000);
						write_string(&g_Drawer, ": ");
					}
					g_Drawer.x = x;
					g_Drawer.y = y;
				}
				// Add task to handle the data
				else {
					//add_thread(handle_data, (void*)data, 1);
				}
				return;
			}
		}
		// Check if System Time Compare 0 Triggered the Interrupt
		else if (*(unsigned long*)SYS_TIMER_CS == SYS_TIMER_SC_M0) {
			volatile unsigned long* timer_cs = (unsigned long*)SYS_TIMER_CS;
			volatile unsigned long* timer_chi = (unsigned long*)SYS_TIMER_CHI;
			volatile unsigned long* nexttime = (unsigned long*)SYS_TIMER_C0;
			*timer_cs = SYS_TIMER_SC_M0;
			*nexttime = *timer_chi + 60000000;
		}
	}
	// Check if CNTV triggered the interrupt
	else if (source & (1 << 3)) {
		return;
	}
	return;
}

static unsigned long counter = 0;
unsigned long c_fiq_handler(void)
{
	unsigned long source = load32(CORE0_FIQ_SOURCE);
	// Check if CNTV triggered the interrupt
	if (source & (1 << 3)) {
		// Reset the counter
		write_cntv_tval(cntfrq/CPS);
		counter++;
		if (counter % 0x6000 == 0)
			counter = 0;
		if (counter % 0x10 == 0)
			status();
		if (counter % 0x40 == 0)
			return 1;
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
