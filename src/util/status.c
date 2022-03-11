#include <cpu.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <symbols.h>
#include <lib/strings.h>
#include <lib/kmem.h>
#include <sys/core.h>
#include <sys/schedule.h>
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
	draw_cletter(0, 1, (g_val & 0b11) + 0x30, 0x1EA1A1);
	draw_cletter(1, 1, ((g_val >> 2) & 0b11) + 0x30, 0x1EA1A1);

	draw_chex32(4, 1, ib_val, 0x1EA1A1);
	draw_chex32(4+9, 1, i1_val, 0x1EA1A1);
	draw_chex32(4+9*2, 1, i2_val, 0x1EA1A1);
	draw_chex32(4+9*3, 1,  f_val, 0x1EA1A1);

	// Check UART IRQ
	if (i2_val & (1<<25)) {
		draw_cstring(0, 2, "UART", 0x00FF00);
	} else if (f_val == 57) {
		draw_cstring(0, 2, "UART", 0xFFA500);
	} else {
		draw_cstring(0, 2, "UART", 0xFF0000);
	}

	// Check UART IRQ
	if (i1_val & (1<<0)) {
		draw_cstring(5, 2, "STIMERCMP", 0x00FF00);
	} else if (f_val == 1) {
		draw_cstring(5, 2, "STIMERCMP", 0xFFA500);
	} else {
		draw_cstring(5, 2, "STIMERCMP", 0xFF0000);
	}

	if (load32(CORE0_TIMER_IRQCNTL) & 0xF) {
		draw_cstring(4+9+2, 2, "LTIMER", 0x00FF00);
	} else if (load32(CORE0_TIMER_IRQCNTL) & 0xF0) {
		draw_cstring(4+9+2, 2, "LTIMER", 0xFFA500);
	} else {
		draw_cstring(4+9+2, 2, "LTIMER", 0xFF0000);
	}
}

void status(void)
{
	// OS Info
	draw_cstring(9, 0, "v", 0x00FFFF);
	draw_cstring(0, 0, os_name, 0xFF0000);
	draw_cstring(10, 0, os_info_v, 0x00FFFF);

	// GPU IRQ Statuses
	output_irq_status();

	// Timer Status
	draw_cstring(0, 3, "TIMER", 0x00FF00);
	// Output the frequency
	draw_string(6, 3, "@");
	unsigned long frq = read_cntfrq()/1000;
	char* frq_str = u32_to_str(frq);
	unsigned long fs_len = strlen(frq_str)+1;
	draw_string(8, 3, frq_str);
	draw_string(8+fs_len, 3, "kHz");
	// Output the value
	unsigned long v = read_cntv_tval();
	char* v_str = u32_to_str(v);
	unsigned long vs_len = strlen(v_str) + 1;
	draw_string(8+fs_len+4, 3, v_str);
	draw_string(8+fs_len+4 +vs_len, 3, "                           ");
	draw_letter(8+fs_len+4 +vs_len+1, 3, '|');
	draw_hex32(8+fs_len+7+vs_len, 3, v);

	// Video Status
	draw_cstring(0, 4, "VIDEO", 0x00FF00);
	char* gwidth_str;
	gwidth_str = u32_to_str(gwidth);
	unsigned long gs_len = strlen(gwidth_str) + 1;
	draw_string(6, 4, gwidth_str);
	draw_letter(6+gs_len-1, 4, 'x');
	gwidth_str = u32_to_str(gheight);
	unsigned long gs_len1 = strlen(gwidth_str) + 1;
	draw_string(6+gs_len, 4, gwidth_str);
	if(gisrgb)
		draw_string(6+gs_len+gs_len1, 4, "RGB");
	else
		draw_string(6+gs_len+gs_len1, 4, "BGR");

	// Core Stacks
	draw_string(0, 5, "SVC      IRQ      FIQ      User/SYS\n");
	unsigned long sp = (unsigned long)getsvcstack();
	draw_hex32(0, 6, sp);
	sp = (unsigned long)getirqstack();
	draw_hex32(9, 6, sp);
	sp = (unsigned long)getfiqstack();
	draw_hex32(9*2, 6, sp);
	sp = (unsigned long)getsysstack();
	draw_hex32(9*3, 6, sp);

	// Report Core that updated status
	unsigned long coren;
	asm volatile (
		"mrc p15, #0, %0, c0, c0, #5\n"
		"and %0, %0, #3" : "=r"(coren) :: "cc");
	draw_string(0, 7, "Status Updated by Core #");
	draw_hex32(24, 7, coren);

	// Report Sys Timer Stataus
	draw_string(0, 8, "Sys Timer Status");
	coren = *(volatile unsigned long*)SYS_TIMER_CS;
	draw_hex32(17, 8, coren);
	draw_string(17+8, 8, ":");
	unsigned long long tval = get_time();
	draw_hex32(17+8, 8, (tval >> 32));
	draw_hex32(17+8+8, 8, tval);
	coren = *(volatile unsigned long*)SYS_TIMER_C0;
	draw_hex32(19+14+8+1, 8, coren);
	draw_string(19+14+9+8, 8, "|");
	draw_string(19+14+18, 8, "           ");
	char* t_str = u32_to_str(((unsigned long)tval)/1000000);
	draw_string(19+14+18, 8, t_str);
}
