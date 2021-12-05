#include "../sys/core.h"
#include "../sys/timer.h"
#include "../util/time.h"
#include "../util/mutex.h"
#include "../drivers/uart.a.h"
#include "../drivers/uart.h"

#define TIMER_C
extern char* os_info_v;

unsigned long exe_cnt = 0;
struct Mutex exe_cnt_m = {.addr = &exe_cnt, .pid = NULL_PID};

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
		uart_string("\033[?25l\033[s\033[1;1H\033[91mDendritOS \033[96mv");
		uart_string(os_info_v);
		uart_string("\033[0m #");
		uart_10(*(exe_cnt_m.addr));
		uart_string("\033[u\033[?25h");
		release_mutex(&exe_cnt_m, SCHED_PID);
	}
}
