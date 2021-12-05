#include "../sys/core.h"
#include "../sys/timer.h"
#include "../util/time.h"
#include "../util/mutex.h"
#include "../drivers/uart.a.h"
#include "../drivers/uart.h"

static unsigned long exe_cnt = 0;
static struct Mutex exe_cnt_m = {.addr = &exe_cnt, .pid = NULL_PID};

//static unsigned long execution_count = 0;

void c_timer() {
	// Reset the counter
	write_cntv_tval(cntfrq);

	// Output the value
	//uart_string((char*)"Timer Value: ");
	//unsigned long v = read_cntv_tval();
	//uart_10(v);
	//uart_char(0x20);
	//uart_hexn(v);

	// Lock the execution counter
	if (lock_mutex(&exe_cnt_m, SCHED_PID) == 0) {
		*(exe_cnt_m.addr) += 1;
		uart_string((char*)"Executed #");
		uart_10(*(exe_cnt_m.addr));
		uart_string((char*)" Times\n");
		release_mutex(&exe_cnt_m, SCHED_PID);
	}
}
