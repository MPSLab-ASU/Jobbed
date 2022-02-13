#include <cpu.h>
#include <globals.h>
#include <graphics/drawer.h>
#include <graphics/lfb.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <sys/timer.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>

void output_irq_status(void)
{
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
	write_c10(&g_Drawer, g_val & 0b11, 0x1EA1A1);
	write_c10(&g_Drawer, (g_val >> 2) & 0b11, 0x1EA1A1);

	write_string(&g_Drawer, " ");
	write_chex32(&g_Drawer, ib_val, 0x1EA1A1);
	write_chex32(&g_Drawer, i1_val, 0x1EA1A1);
	write_chex32(&g_Drawer, i2_val, 0x1EA1A1);
	write_chex32(&g_Drawer,  f_val, 0x1EA1A1);
	write_char(&g_Drawer, '\n');

	// Check UART IRQ
	if (i2_val & (1<<25)) {
		write_cstring(&g_Drawer, "UART ", 0x00FF00);
	} else if (f_val == 57) {
		write_cstring(&g_Drawer, "UART ", 0xFFA500);
	} else {
		write_cstring(&g_Drawer, "UART ", 0xFF0000);
	}

	// Check UART IRQ
	if (i1_val & (1<<0)) {
		write_cstring(&g_Drawer, "STIMERCMP ", 0x00FF00);
	} else {
		write_cstring(&g_Drawer, "STIMERCMP ", 0xFF0000);
	}

	if (load32(CORE0_TIMER_IRQCNTL) & 0xF) {
		write_cstring(&g_Drawer, "LTIMER ", 0x00FF00);
	} else if (load32(CORE0_TIMER_IRQCNTL) & 0xF0) {
		write_cstring(&g_Drawer, "LTIMER ", 0xFFA500);
	} else {
		write_cstring(&g_Drawer, "LTIMER ", 0xFF0000);
	}
}

void status(void)
{
	unsigned int x = g_Drawer.x;
	unsigned int y = g_Drawer.y;
	g_Drawer.x = 0;
	g_Drawer.y = 0;
	// OS Info
	write_cstring(&g_Drawer, os_name, 0xFF0000);
	write_cstring(&g_Drawer, " v", 0x00FFFF);
	write_cstring(&g_Drawer, os_info_v, 0x00FFFF);
	write_cstring(&g_Drawer, "    # TStacks: ", 0xFFDF00);
	draw_string(g_Drawer.x, g_Drawer.y, "           ");
	write_c10(&g_Drawer, sched_stack_count, 0xFFDF00);

	// GPU IRQ Statuses
	write_string(&g_Drawer, "\n");
	output_irq_status();

	// Timer Status
	write_cstring(&g_Drawer, "\nTIMER", 0x00FF00);
	// Output the frequency
	write_string(&g_Drawer, " @ ");
	unsigned long frq = read_cntfrq()/1000;
	write_10(&g_Drawer, frq);
	write_string(&g_Drawer, " kHz ");
	// Output the value
	unsigned long v = read_cntv_tval();
	write_10(&g_Drawer, v);
	write_string(&g_Drawer, " | ");
	write_hex32(&g_Drawer, v);

	// Video Status
	write_cstring(&g_Drawer, "\nVIDEO ", 0x00FF00);
	write_10(&g_Drawer, gwidth);
	write_string(&g_Drawer, "x");
	write_10(&g_Drawer, gheight);
	if(gisrgb) {
		write_string(&g_Drawer, " RGB");
	} else {
		write_string(&g_Drawer, " BGR");
	}

	g_Drawer.x = 0;
	g_Drawer.y = 5;
	write_string(&g_Drawer, "SVC      IRQ      FIQ      User/SYS\n");
	for(int i = 0; i < 128; i++)
		write_char(&g_Drawer, ' ');
	g_Drawer.x = 0;
	g_Drawer.y = 6;

	unsigned long sp = (unsigned long)getsvcstack();
	write_hex32(&g_Drawer, sp);
	write_char(&g_Drawer, ' ');
	sp = (unsigned long)getirqstack();
	write_hex32(&g_Drawer, sp);
	write_char(&g_Drawer, ' ');
	sp = (unsigned long)getfiqstack();
	write_hex32(&g_Drawer, sp);
	write_char(&g_Drawer, ' ');
	sp = (unsigned long)getsysstack();
	write_hex32(&g_Drawer, sp);
	write_char(&g_Drawer, '\n');
	unsigned long coren;
	asm volatile (
		"mrc p15, #0, %0, c0, c0, #5\n"
		"and %0, %0, #3" : "=r"(coren) :: "cc");
	write_string(&g_Drawer, "Status Updated by Core #");
	write_10(&g_Drawer, coren);
	write_string(&g_Drawer, "\nSys Timer Status ");
	coren = *(volatile unsigned long*)SYS_TIMER_CS;
	write_10(&g_Drawer, coren);
	write_string(&g_Drawer, ":");
	unsigned long long tval = get_time();
	write_hex32(&g_Drawer, (tval >> 32));
	write_hex32(&g_Drawer, tval);
	write_string(&g_Drawer, ":");
	coren = *(volatile unsigned long*)SYS_TIMER_C0;
	write_10(&g_Drawer, coren);
	write_string(&g_Drawer, " | ");
	draw_string(g_Drawer.x, g_Drawer.y, "           ");
	write_10(&g_Drawer, ((unsigned long)tval)/1000000);

	g_Drawer.x = x;
	g_Drawer.y = y;

	draw_stacks();
}
