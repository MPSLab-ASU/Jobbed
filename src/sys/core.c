#include <cpu/irq.h>
#include <drivers/uart.h>
#include <graphics/drawer.h>
#include <graphics/lfb.h>
#include <lib/mem.h>
#include <lib/q.h>
#include <lib/strings.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/kernel.h>
#include <sys/power.h>
#include <sys/schedule.h>
#include <sys/timer.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>

#define SYS_CORE_C
#ifndef VERSION
char* os_info_v = "?";
#else
char* os_info_v = VERSION;
#endif

void testlocal(void);

// Initialize IRQs
void sysinit(void)
{
	// Route GPU interrupts to Core 0
	store32(0x00, GPU_INTERRUPTS_ROUTING);

	// Mask Overrun of UART0
	store32(1<<4, UART0_IMSC);
	// Enable UART GPU IRQ
	store32(1<<25, IRQ_ENABLE2);
	// Enable Timer
	// As an IRQ
	store32(1<<0, IRQ_BASIC_ENABLE);
	// Get the frequency
	cntfrq = read_cntfrq();
	// Clear cntv interrupt and set next 1 second timer
	write_cntv_tval(cntfrq);
	// Route timer to core0 irq
	routing_core0cntv_to_core0irq();
	// Enable timer
	enablecntv();

	// Graphics Initialize
	lfb_init();
	lfb_showpicture();

	// Enable IRQ & FIQ
	enableirq();
	enablefiq();

	// Start Scheduler
	init_scheduler();
	add_thread(testlocal, 0);
	add_thread(testlocal, 1);
	add_thread(testlocal, 3);
	//delay(0x20000000);
	schedule();
	heap_info();
	sched_info();
}

struct Mutex testm = {.addr = (void*)0xDEADBEEF, .pid = NULL_PID};
void testlocal1(void)
{
	unsigned long a = 5;
	struct Thread* t = scheduler.rthread_ll->data;
	uart_string("vRan Thread ");
	uart_10(t->data.pid);
	uart_string(" Pri. ");
	uart_10(t->data.priority);
	uart_string(" ...\n");
	add_thread(testlocal, 0);
	schedule();
	a += t->data.pid;
	uart_10(a);
	uart_string(" Done!\n");
}

void testlocal(void)
{
	struct Thread* t = scheduler.rthread_ll->data;
	uart_string("Ran Thread ");
	uart_10(t->data.pid);
	uart_string(" Pri. ");
	uart_10(t->data.priority);
	uart_string(" ...\n");
	//delay(0x80000000);
	if (t->data.pid == 5) {
		add_thread(testlocal1, 1);
		schedule();
	}
	uart_string("Done!\n");
}
