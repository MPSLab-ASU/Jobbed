#include <cpu.h>
#include <cpu/irq.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <tests/test.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>

#define CPS 1000

void handle_data(unsigned char);

static unsigned long counter = 0;
unsigned long c_irq_handler(void)
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
					draw_chex32(0, 9, data, 0xAA00FF);
				}

				// Handle the recieved data
				// Ctrl+G to output scheduler debug info
				if (data == 0x7)
					uart_scheduler();
				// Ctrl+T to toggle timer
				else if(data == 0x14) {
					unsigned long timer_status;
					asm volatile("mrc p15, 0, %0, c14, c3, 1" : "=r"(timer_status));
					if(timer_status == 0) {
						cntfrq = read_cntfrq();
						write_cntv_tval(cntfrq/CPS);
						enablecntv();
						draw_cstring(0, 3, "TIMER", 0x00FF00);
					} else {
						disablecntv();
						draw_cstring(0, 3, "TIMER", 0xFF0000);
					}
				}
				// Add task to handle the data
				else {
					add_thread(handle_data, (void*)data, 7);
					return 1;
				}
			}
		}
		// Check if System Time Compare 0 Triggered the Interrupt
		if (*(volatile unsigned long*)SYS_TIMER_CS & SYS_TIMER_SC_M0) {
			volatile unsigned long* timer_cs = (volatile unsigned long*)SYS_TIMER_CS;
			volatile unsigned long* timer_chi = (volatile unsigned long*)SYS_TIMER_CHI;
			volatile unsigned long* nexttime = (volatile unsigned long*)SYS_TIMER_C0;
			add_thread_without_duplicate(test_entry, 0, 2);
			*nexttime = *timer_chi + 4000000;
			*timer_cs = SYS_TIMER_SC_M0;
			status();
			return 1;
		}
	}
	// Check if CNTV triggered the interrupt
	else if (source & (1 << 3)) {
		// Reset the counter
		write_cntv_tval(cntfrq/CPS);
		counter++;
		if (counter % 0x6000 == 0)
			counter = 0;
	}
	return 0;
}

unsigned long c_fiq_handler(void)
{
	unsigned long source = load32(CORE0_FIQ_SOURCE);
	if (source & (1 << 8)) {
		// Check if System Time Compare 0 Triggered the Interrupt
		if (*(volatile unsigned long*)SYS_TIMER_CS & SYS_TIMER_SC_M0) {
			volatile unsigned long* timer_cs = (volatile unsigned long*)SYS_TIMER_CS;
			volatile unsigned long* timer_chi = (volatile unsigned long*)SYS_TIMER_CHI;
			volatile unsigned long* nexttime = (volatile unsigned long*)SYS_TIMER_C0;
			add_thread_without_duplicate(test_entry, 0, 2);
			*nexttime = *timer_chi + 4000000;
			*timer_cs = SYS_TIMER_SC_M0;
			return 1;
		}
	}
	// Check if CNTV triggered the interrupt
	else if (source & (1 << 3)) {
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
		//add_thread(test_entry, 0, 2);
	}
}
