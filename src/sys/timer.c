#include "../drivers/uart.h"
#include "../graphics/drawer.h"
#include "../sys/core.h"
#include "../sys/timer.h"
#include "../util/mutex.h"
#include "../util/time.h"

#define SYS_TIMER_C
extern char* os_info_v;

unsigned long exe_cnt = 0;
struct Mutex exe_cnt_m = {.addr = &exe_cnt, .pid = NULL_PID};

void c_timer(void)
{
	// Reset the counter
	write_cntv_tval(cntfrq/100);

	unsigned int x = g_Drawer.x;
	unsigned int y = g_Drawer.y;
	g_Drawer.x = 0;
	g_Drawer.y = 0;

	// Lock the execution counter
	if (lock_mutex(&exe_cnt_m, SCHED_PID) == 0) {
		*(exe_cnt_m.addr) += 1;
		write_cstring(&g_Drawer, "DendritOS", 0xFF0000);
		write_cstring(&g_Drawer, " v", 0x00FFFF);
		write_cstring(&g_Drawer, os_info_v, 0x00FFFF);
		write_string(&g_Drawer, " #");
		write_10(&g_Drawer, *(exe_cnt_m.addr));
		release_mutex(&exe_cnt_m, SCHED_PID);
	}

	g_Drawer.x = 29;
	g_Drawer.y = 5;
	// Output the value
	unsigned long v = read_cntv_tval();
	write_10(&g_Drawer, v);
	write_string(&g_Drawer, " | ");
	write_hex32(&g_Drawer, v);

	g_Drawer.x = 0;
	g_Drawer.y = 9;
	for(int i = 0; i < 128; i++)
		write_char(&g_Drawer, ' ');
	g_Drawer.x = 0;
	g_Drawer.y = 9;
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

	g_Drawer.x = x;
	g_Drawer.y = y;
}
