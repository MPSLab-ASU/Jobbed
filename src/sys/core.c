#include <cpu/irq.h>
#include <cpu.h>
#include <drivers/uart.h>
#include <globals.h>
#include <graphics/drawer.h>
#include <graphics/lfb.h>
#include <lib/kmem.h>
#include <lib/strings.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/power.h>
#include <sys/schedule.h>
#include <sys/timer.h>
#include <util/mutex.h>
#include <util/time.h>

// Initialize IRQs
void sysinit(void)
{
	// Initialize System Globals
	exe_cnt_m.addr = &exe_cnt;
	exe_cnt_m.pid = NULL_PID;
	stimeh = *(unsigned long*)SYS_TIMER_CHI;
	stimel = *(unsigned long*)SYS_TIMER_CLO;
	*(unsigned long*) SYS_TIMER_C0 = 60000000 + stimeh; // 60 second trigger
	uart_init();
	///...

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
	//// Route timer to core0 irq
	//routing_core0cntv_to_core0irq();
	// Route timer to core0 fiq
	routing_core0cntv_to_core0fiq();
	// Enable timer
	enablecntv();
	// Enable system timer
	store32(SYS_TIMER_SC_M0, IRQ_ENABLE1);

	// Graphics Initialize
	lfb_init();
	lfb_showpicture();

	// Start Scheduler
	init_scheduler();
}
