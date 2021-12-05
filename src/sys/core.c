#include "../drivers/uart.a.h"
#include "../drivers/uart.h"
#include "../util/time.h"
#include "../sys/core.h"
#include "../sys/timer.h"
#include "../sys/power.h"

static char* os_info_h = "\033[93mInitialized the Real Time Operating System\033[0m\n\033[96mName\033[0m:    \033[94mDendritOS\033[0m\n\033[96mVersion\033[0m: \033[95m";
static char* os_info_t = "\033[0m\n\nQEMU\n====\n Monitor : Ctrl-A c\n Timer   : Ctrl-T\n Exit    : Ctrl-A x\n\n";
#ifndef VERSION
static char* os_info_v = "?";
#else
static char* os_info_v = VERSION;
#endif

static char* irq_on  = " \033[92mEnabled\033[0m\n";
static char* irq_off = " \033[91mDisabled\033[0m\n";

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

	// Check FIQ
	unsigned long f_val = load32(FIQ_CONTROL);
	uart_string((char*)"FIQ Status: ");
	uart_hexn(f_val);
	if (f_val & 0x80) {
		uart_string(irq_on);
	} else {
		uart_string(irq_off);
	}

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

void postinit() {
	uart_string(os_info_h);
	uart_string(os_info_v);
	uart_string(os_info_t);
}