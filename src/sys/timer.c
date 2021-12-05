#include "../sys/core.h"
#include "../sys/timer.h"
#include "../util/time.h"
#include "../drivers/uart.a.h"
#include "../drivers/uart.h"

void c_timer() {
	// Reset the counter
	write_cntv_tval(cntfrq);

	// Output the value
	uart_string((char*)"Timer Value: ");
	unsigned long v = read_cntv_tval();
	uart_10(v);
	uart_char(0x20);
	uart_hexn(v);
}
