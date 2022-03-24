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
#include <usr/main.h>

#define CPS 1000

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

				// Handle the recieved data
#ifdef DEBUG
				// Ctrl+G to output scheduler debug info
				if (data == 0x7) {
					uart_scheduler();
					uart_mutexes();
				}
#endif
				// Add task to handle the data
				if (irqs[UART_IRQ].handler != 0) {
					add_thread(irqs[UART_IRQ].handler, (void*)data, *(unsigned long*)irqs[UART_IRQ].handler_info);
					return 1;
				}
			}
		}
		// Check if System Time Compare 0 Triggered the Interrupt
		if (*(volatile unsigned long*)SYS_TIMER_CS & SYS_TIMER_SC_M0 && irqs[SYS_TIMER_0_IRQ].handler != 0) {
			volatile unsigned long* timer_cs = (volatile unsigned long*)SYS_TIMER_CS;
			volatile unsigned long* timer_chi = (volatile unsigned long*)SYS_TIMER_CHI;
			volatile unsigned long* nexttime = (volatile unsigned long*)SYS_TIMER_C0;
			add_thread(irqs[SYS_TIMER_0_IRQ].handler, 0, 1);
			*nexttime = *timer_chi + *(unsigned long*)irqs[SYS_TIMER_0_IRQ].handler_info;
			*timer_cs = SYS_TIMER_SC_M0;
			return 1;
		}
	}
	// Check if CNTV triggered the interrupt
	else if (source & (1 << 3)) {
		// Reset the counter
		write_cntv_tval(cntfrq);
		counter++;
		if (counter % 0x6000 == 0)
			counter = 0;
	}
	return 0;
}

unsigned long c_fiq_handler(void)
{
	unsigned long source = load32(CORE0_FIQ_SOURCE);
	// Check if CNTV triggered the interrupt
	if (source & (1 << 3) && irqs[LOCAL_TIMER_IRQ].handler != 0) {
		add_thread(irqs[LOCAL_TIMER_IRQ].handler, 0, 1);
		write_cntv_tval(cntfrq);
	}
	return 0;
}

void subscribe_irq(unsigned long irq_num, void* handler, void* handler_info)
{
	if (irq_num >= MAX_IRQS)
		return;
	irqs[irq_num].handler = handler;
	irqs[irq_num].handler_info = handler_info;
	switch (irq_num) {
		case UART_IRQ:
			store32(1<<4, UART0_IMSC);
			store32(1<<25, IRQ_ENABLE2);
			break;
		case SYS_TIMER_0_IRQ:
			store32(SYS_TIMER_SC_M0, IRQ_ENABLE1);
			*(volatile unsigned long*)SYS_TIMER_C0 = *(volatile unsigned long*)SYS_TIMER_CHI + *(unsigned long*)handler_info;
			break;
		case SYS_TIMER_1_IRQ:
			store32(SYS_TIMER_SC_M1, IRQ_ENABLE1);
			*(volatile unsigned long*)SYS_TIMER_C1 = *(volatile unsigned long*)SYS_TIMER_CHI + *(unsigned long*)handler_info;
			break;
		case SYS_TIMER_2_IRQ:
			store32(SYS_TIMER_SC_M2, IRQ_ENABLE1);
			*(volatile unsigned long*)SYS_TIMER_C2 = *(volatile unsigned long*)SYS_TIMER_CHI + *(unsigned long*)handler_info;
			break;
		case SYS_TIMER_3_IRQ:
			store32(SYS_TIMER_SC_M3, IRQ_ENABLE1);
			*(volatile unsigned long*)SYS_TIMER_C3 = *(volatile unsigned long*)SYS_TIMER_CHI + *(unsigned long*)handler_info;
			break;
	}
}
