#include <drivers/uart.h>
#include <globals.h>
#include <graphics/drawer.h>
#include <sys/core.h>
#include <sys/timer.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>
#include <symbols.h>

void c_timer(void)
{
	// Reset the counter
	write_cntv_tval(cntfrq/CPS);

	status();
}
