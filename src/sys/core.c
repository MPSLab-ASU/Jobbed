#include "../cpu/irq.h"
#include "../drivers/uart.h"
#include "../graphics/drawer.h"
#include "../graphics/lfb.h"
#include "../lib/ll.h"
#include "../lib/mem.h"
#include "../lib/q.h"
#include "../lib/strings.h"
#include "../sys/core.h"
#include "../sys/power.h"
#include "../sys/timer.h"
#include "../util/mutex.h"
#include "../util/status.h"
#include "../util/time.h"

#define SYS_CORE_C
#ifndef VERSION
char* os_info_v = "?";
#else
char* os_info_v = VERSION;
#endif

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
}
