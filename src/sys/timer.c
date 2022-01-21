#include <drivers/uart.h>
#include <globals.h>
#include <graphics/drawer.h>
#include <sys/core.h>
#include <sys/timer.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>
#include <symbols.h>

void increase_counter(void)
{
	if (lock_mutex(&exe_cnt_m, SCHED_PID) == 0) {
		unsigned long* counter = (unsigned long*)exe_cnt_m.addr;
		*counter += 1;
		release_mutex(&exe_cnt_m, SCHED_PID);
	}
}

void c_timer(void)
{
	// Reset the counter
	write_cntv_tval(cntfrq/CPS);

	increase_counter();
	status();
}
