#include "../cpu/irq.h"
#include "../drivers/uart.h"
#include "../graphics/lfb.h"
#include "../graphics/drawer.h"
#include "../lib/mem.h"
#include "../lib/strings.h"
#include "../util/time.h"
#include "../util/mutex.h"
#include "../sys/core.h"
#include "../sys/timer.h"
#include "../sys/power.h"

//extern void init_graphics(void);

#ifndef VERSION
char* os_info_v = "?";
#else
char* os_info_v = VERSION;
#endif

// Initialize IRQs
void sysinit() {
	// Mask Overrun of UART0
	store32(1<<4, UART0_IMSC);

	// Enable UART GPU IRQ
	store32(1<<25, IRQ_ENABLE2);

	// Route GPU interrupts to Core 0
	store32(0x00, GPU_INTERRUPTS_ROUTING);

	// Enable Timer
	// As an IRQ
	store32(1<<0, IRQ_BASIC_ENABLE);
	// As a  FIQ
	//store32(0xC0, FIQ_CONTROL);
	// Get the frequency
	cntfrq = read_cntfrq();
	// Clear cntv interrupt and set next 1 second timer
	write_cntv_tval(cntfrq);
	// Route timer to core0 irq
	routing_core0cntv_to_core0irq();
	// Enable timer
	enable_cntv();

	// Graphics Initialize
	lfb_init();
	lfb_showpicture();

	// Enable IRQ & FIQ
	enableirq();
	enablefiq();
}

void output_irq_status(void) {
	// Basic IRQ
	unsigned long ib_val = load32(IRQ_BASIC_ENABLE);
	// IRQ 1
	unsigned long i1_val = load32(IRQ_ENABLE1);
	// IRQ 2
	unsigned long i2_val = load32(IRQ_ENABLE2);
	// FIQ
	unsigned long f_val = load32(FIQ_CONTROL);

	// Check GPU Interrupt Routing
	unsigned long g_val = load32(GPU_INTERRUPTS_ROUTING);
	write_string(&g_Drawer, "GPU IRQ: Core ");
	write_10(&g_Drawer, g_val & 0b11);
	write_string(&g_Drawer, " | GPU FIQ: Core ");
	write_10(&g_Drawer, (g_val >> 2) & 0b11);

	write_string(&g_Drawer, "\n");
	write_hex32(&g_Drawer, ib_val);
	write_string(&g_Drawer, " ");
	write_hex32(&g_Drawer, i1_val);
	write_string(&g_Drawer, " ");
	write_hex32(&g_Drawer, i2_val);
	write_string(&g_Drawer, " ");
	write_hex32(&g_Drawer,  f_val);

	// Check UART IRQ
	write_string(&g_Drawer, "\nUART: ");
	if (i2_val & (1<<25)) {
		write_cstring(&g_Drawer, "Enabled", 0x00FF00);
	} else {
		write_cstring(&g_Drawer, "Disabled", 0xFF0000);
	}
	// Check TIMER IRQ
	write_string(&g_Drawer, "\nTIMER: ");
	if (ib_val & (1<<0)) {
		write_cstring(&g_Drawer, "Enabled", 0x00FF00);
		// Output the frequency
		write_string(&g_Drawer, " @ ");
		unsigned long frq = read_cntfrq()/1000;
		write_10(&g_Drawer, frq);
		write_string(&g_Drawer, " kHz");
	} else {
		write_cstring(&g_Drawer, "Disabled", 0xFF0000);
	}
	write_string(&g_Drawer, "\nTIMER: ");
	write_cstring(&g_Drawer, "Enabled", 0x00FF00);
}

void postinit() {
	// OS Info
	write_cstring(&g_Drawer, "DendritOS", 0xFF0000);
	write_cstring(&g_Drawer, " v", 0x00FF00);
	write_cstring(&g_Drawer, os_info_v, 0x00FF00);
	write_string(&g_Drawer, " #");
	if (lock_mutex(&exe_cnt_m, SYS_PID) == 0) {
		write_10(&g_Drawer, *(exe_cnt_m.addr));
		release_mutex(&exe_cnt_m, SYS_PID);
	}
	// Commands
	write_string(&g_Drawer, "\nMonitor Ctrl-A m  Exit: Ctrl-A x");
	write_string(&g_Drawer, "\nTimer: Ctrl-T");
	// Timer Status
	//uart_string("Timer: \033[92mEnabled\033[0m");
	// GPU IRQ Statuses
	write_string(&g_Drawer, "\n");
	output_irq_status();
	write_string(&g_Drawer, "\n> ");
}
