#include <graphics/drawer.h>
#include <graphics/lfb.h>
#include <symbols.h>
#include <sys/core.h>
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
	write_string(&g_Drawer, "   TIMER: ");
	if (ib_val & (1<<0)) {
		write_cstring(&g_Drawer, "Enabled", 0x00FF00);
	} else {
		write_cstring(&g_Drawer, "Disabled", 0xFF0000);
	}
}

void status(void)
{
	unsigned int x = g_Drawer.x;
	unsigned int y = g_Drawer.y;
	g_Drawer.x = 0;
	g_Drawer.y = 0;
	// OS Info
	write_cstring(&g_Drawer, "DendritOS", 0xFF0000);
	write_cstring(&g_Drawer, " v", 0x00FFFF);
	write_cstring(&g_Drawer, os_info_v, 0x00FFFF);
	write_string(&g_Drawer, " #");
	write_10(&g_Drawer, *((unsigned long*)exe_cnt_m.addr));
	/* if (lock_mutex(&exe_cnt_m, SYS_PID) == 0) {
		write_10(&g_Drawer, *((unsigned long*)exe_cnt_m.addr));
		release_mutex(&exe_cnt_m, SYS_PID);
	} */

	// Commands
	write_string(&g_Drawer, "\nMonitor: Ctrl-A m  Exit: Ctrl-A x  Timer: Ctrl-T");

	// GPU IRQ Statuses
	write_string(&g_Drawer, "\n");
	output_irq_status();

	// Timer Status
	write_string(&g_Drawer, "\nTIMER: ");
	write_cstring(&g_Drawer, "Enabled ", 0x00FF00);
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
	write_string(&g_Drawer, "\nVIDEO: ");
	write_cstring(&g_Drawer, "Enabled ", 0x00FF00);
	write_10(&g_Drawer, width);
	write_string(&g_Drawer, "x");
	write_10(&g_Drawer, height);
	if(isrgb) {
		write_string(&g_Drawer, " RGB");
	} else {
		write_string(&g_Drawer, " BGR");
	}

	g_Drawer.x = 0;
	g_Drawer.y = 8;
	for(int i = 0; i < 128; i++)
		write_char(&g_Drawer, ' ');
	g_Drawer.x = 0;
	g_Drawer.y = 8;
	write_string(&g_Drawer, "Timer Counter: ");
	if (exe_cnt_m.pid == NULL_PID) {
		write_cstring(&g_Drawer, "Free!", 0xFF00FF);
	} else {
		write_cstring(&g_Drawer, "Locked by ", 0xFF00FF);
		if (exe_cnt_m.pid == SYS_PID)
			write_cstring(&g_Drawer, "System", 0xFF00FF);
		else if (exe_cnt_m.pid == SCHED_PID)
			write_cstring(&g_Drawer, "Scheduler", 0xFF00FF);
		else {
			write_cstring(&g_Drawer, "Process ", 0xFF00FF);
			write_c10(&g_Drawer, exe_cnt_m.pid, 0xFF00FF);
		}
		write_cchar(&g_Drawer, '!', 0xFF00FF);
	}

	g_Drawer.x = 0;
	g_Drawer.y = 9;
	for(int i = 0; i < 128; i++)
		write_char(&g_Drawer, ' ');
	g_Drawer.x = 0;
	g_Drawer.y = 9;
	/*
	struct Q* q = scheduler.tasks->next;
	while (q != 0) {
		struct Task* t = q->data;
		write_hex32(&g_Drawer, (unsigned long)t->task);
		write_char(&g_Drawer, ' ');
		q = q->next;
	}
	write_char(&g_Drawer, '\n');
	*/

	unsigned long sp = (unsigned long)getsp();
	write_hex32(&g_Drawer, sp);

	g_Drawer.x = x;
	g_Drawer.y = y;
}
