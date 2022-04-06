#include <cpu/irq.h>
#include <cpu.h>
#include <drivers/uart.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <lib/kmem.h>
#include <lib/mmu.h>
#include <lib/strings.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/power.h>
#include <sys/schedule.h>
#include <tests/test.h>
#include <usr/main.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>

// Initialize IRQs
void sysinit(void)
{
	// Get the frequency
	cntfrq = read_cntfrq();

	// Initialize UART
	uart_init();

	// Route GPU interrupts to Core 0
	store32(0x00, GPU_INTERRUPTS_ROUTING);

	// Graphics Initialize
	lfb_init(1920, 1080);
	clear_screen();

	// Initialize Memory Management Unit
	mmu_init();
	
	// Initialize Mutex Manager
	mutex_init();

	// Start Scheduler
	init_scheduler();
	//add_thread(test_entry, 0, 2);
	add_thread(main, 0, 0);
}
