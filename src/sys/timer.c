#include "../drivers/uart.h"
#include "../graphics/drawer.h"
#include "../sys/core.h"
#include "../sys/timer.h"
#include "../util/mutex.h"
#include "../util/status.h"
#include "../util/time.h"

/// Cycles Per Second
#define CPS 10

#define SYS_TIMER_C
static unsigned long exe_cnt = 0;
struct Mutex exe_cnt_m = {.addr = &exe_cnt, .pid = NULL_PID};

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
