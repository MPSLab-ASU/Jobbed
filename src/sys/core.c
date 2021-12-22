#include "../cpu/irq.h"
#include "../drivers/uart.h"
#include "../graphics/draw.h"
#include "../lib/mem.h"
#include "../lib/strings.h"
#include "../util/time.h"
#include "../util/mutex.h"
#include "../sys/core.h"
#include "../sys/timer.h"
#include "../sys/power.h"

extern void init_graphics(void);

#ifndef VERSION
char* os_info_v = "?";
#else
char* os_info_v = VERSION;
#endif

static char* irq_on  = " \033[92mEnabled\033[0m";
static char* irq_off = " \033[91mDisabled\033[0m";

// Initialize IRQs
void sysinit() {
	// Mask Overrun of UART0
	store32(1<<4, UART0_IMSC);

	// Enable UART GPU IRQ
	store32(1<<25, IRQ_ENABLE2);

	// Route GPU interrupts to Core 0
	store32(0x00, GPU_INTERRUPTS_ROUTING);
	//*(unsigned long*)

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
	//init_graphics();
	//draw_box(0x01FE, 0, 0, 640, 480);
	//draw_box(0xFFFF, 2, 2, 10, 10);

	// Enable IRQ & FIQ
	enableirq();
	enablefiq();
}

// Checks IRQ status
void chk_irq_stat() {
	uart_string((char*)"Checking Enabled Services...\n");

	// Basic IRQ
	unsigned long ib_val = load32(IRQ_BASIC_ENABLE);
	uart_string((char*)"IRQB Status: ");
	uart_hexn(ib_val);

	// IRQ 1
	unsigned long i1_val = load32(IRQ_ENABLE1);
	uart_string((char*)"IRQ1 Status: ");
	uart_hexn(i1_val);

	// IRQ 2
	unsigned long i2_val = load32(IRQ_ENABLE2);
	uart_string((char*)"IRQ2 Status: ");
	uart_hexn(i2_val);

	// Check UART IRQ
	uart_string((char*)" UART:");
	if (i2_val & (1<<25)) {
		uart_string(irq_on);
	} else {
		uart_string(irq_off);
	}
	uart_char(0x0a);

	// Check TIMER IRQ
	uart_string((char*)" TIMER:");
	if (ib_val & (1<<0)) {
		uart_string(irq_on);
		// Output the frequency
		uart_string((char*)"  Frequency: ");
		cntfrq = read_cntfrq();
		//uart_hexn(cntfrq);
		uart_10(cntfrq);
		uart_string((char*)" Hz\n");
	} else {
		uart_string(irq_off);
	}
	uart_char(0x0a);

	// Check FIQ
	unsigned long f_val = load32(FIQ_CONTROL);
	uart_string((char*)"FIQ Status: ");
	uart_hexn(f_val);
	if (f_val & 0x80) {
		uart_string(irq_on);
	} else {
		uart_string(irq_off);
	}
	uart_char(0x0a);

	// Check GPU Interrupt Routing
	unsigned long g_val = load32(GPU_INTERRUPTS_ROUTING);
	uart_string((char*)"GPU IRQ Routed to Core ");
	uart_char(0x30 + (g_val & 0x3));
	uart_char(0x0a);
	uart_string((char*)"GPU FIQ Routed to Core ");
	uart_char(0x30 + ((g_val>>2) & 0x3));
	uart_char(0x0a);

	uart_char(0x0a);
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

	uart_string("\033[5;1H");
	// Check GPU Interrupt Routing
	unsigned long g_val = load32(GPU_INTERRUPTS_ROUTING);
	uart_string((char*)"GPU IRQ: Core ");
	uart_char(0x30 + (g_val & 0x3));
	uart_string((char*)" | GPU FIQ: Core ");
	uart_char(0x30 + ((g_val>>2) & 0x3));

	uart_string("\033[6;1H");
	// Check UART IRQ
	uart_string((char*)"UART:");
	if (i2_val & (1<<25)) {
		uart_string(irq_on);
	} else {
		uart_string(irq_off);
	}
	// Check TIMER IRQ
	uart_string((char*)" TIMER:");
	if (ib_val & (1<<0)) {
		uart_string(irq_on);
		// Output the frequency
		uart_string(" @ ");
		unsigned long frq = read_cntfrq()/1000;
		uart_10(frq);
		uart_string((char*)" kHz");
	} else {
		uart_string(irq_off);
	}

	uart_string("\033[7;1H");
	uart_hex(ib_val);
	uart_char(0x20);
	uart_hex(i1_val);
	uart_char(0x20);
	uart_hex(i2_val);
	uart_char(0x20);
	uart_hex(f_val);
}

void postinit() {
	// OS Info
	uart_string("\033[2J\033[1;1H\033[91mDendritOS \033[96mv");
	uart_string(os_info_v);
	uart_string("\033[0m #");
	if (lock_mutex(&exe_cnt_m, SYS_PID) == 0) {
		uart_10(*(exe_cnt_m.addr));
		release_mutex(&exe_cnt_m, SYS_PID);
	}
	// Commands
	uart_string("\033[2;1HMonitor: Ctrl-A m\033[2;20HExit: Ctrl-A x");
	uart_string("\033[3;1HTimer:   Ctrl-T\033[3;20H");
	// Timer Status
	uart_string("\033[4;1HTimer: \033[92mEnabled\033[0m");
	// GPU IRQ Statuses
	output_irq_status();
	// Input line
	uart_string("\033[8;1H> ");
}
