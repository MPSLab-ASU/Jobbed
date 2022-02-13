#include <cpu.h>
#include <drivers/uart.h>
#include <graphics/lfb.h>
#include <lib/kmem.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <util/mutex.h>

extern void atest(void);
void btest(void);

void test_entry(void)
{
	draw_string(0, 18, "Starting tests");
	unsigned long long ti, tf, dt=0;
	for(int i = 0; i < 64; i++) {
		sys0_64(SYS_TIME, &ti);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
	}
	DRAW64(0, 19, dt/64);
	DRAW64(17, 19, dt%64);

	// atest
	add_thread(atest, 0, 0);
	sys0_64(SYS_TIME, &ti);
	sys0(SYS_YIELD);
	sys0_64(SYS_TIME, &tf);
	dt += tf - ti;
	DRAW64(34, 19, dt/64);
	DRAW64(34+17, 19, dt%64);
	btest();
}

static struct Mutex testm = {.addr = 0, .pid = 0};
static int testi = 0;

void ctest1(void)
{
	uart_string("1 Started\n");
	sys1(4, &testm);
	uart_string("1 Finished\n");
}

void ctest2(void)
{
	uart_string("2 Started\n");
	sys1(4, &testm);
	uart_string("2 Finished\n");
	sys1(5, &testm);
}

void ctest3(void)
{
	uart_string("3 Started\n");
	sys1(5, &testm);
	uart_string("3 Finished\n");
}

void btest(void)
{
	if (testi % 3 == 0) {
		testi++;
		ctest1();
	}
	else if (testi % 3 == 1) {
		testi++;
		ctest2();
	}
	else if (testi % 3 == 2) {
		testi++;
		ctest3();
	}
	uart_hexn(&testm);
}
