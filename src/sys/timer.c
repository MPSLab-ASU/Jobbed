#include "../graphics/drawer.h"
#include "../sys/core.h"
#include "../sys/timer.h"
#include "../util/time.h"
#include "../util/mutex.h"
#include "../drivers/uart.h"

#define SYS_TIMER_C
extern char* os_info_v;

unsigned long exe_cnt = 0;
struct Mutex exe_cnt_m = {.addr = &exe_cnt, .pid = NULL_PID};

void c_timer() {
	// Reset the counter
	write_cntv_tval(cntfrq);

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

	g_Drawer.x = 0;
	g_Drawer.y = 10;
	// Output the value
	write_string(&g_Drawer, "Timer Value: ");
	unsigned long v = read_cntv_tval();
	write_10(&g_Drawer, v);
	write_string(&g_Drawer, " | ");
	write_hex32(&g_Drawer, v);

	g_Drawer.x = x;
	g_Drawer.y = y;
}
