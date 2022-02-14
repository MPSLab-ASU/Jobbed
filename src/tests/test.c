#include <cpu.h>
#include <drivers/uart.h>
#include <graphics/lfb.h>
#include <lib/kmem.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <util/mutex.h>
#include <util/lock.h>

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
	dt = tf - ti;
	DRAW64(34, 19, dt/64);
	DRAW64(34+17, 19, dt%64);
	add_thread(btest, 0, 4);
}

//static struct Mutex testm = {.addr = 0, .pid = 0};
static struct Lock testm = {.pid = 0};

void ctest1(void);
void ctest2(void);
void ctest3(void);
void ctest4(void);

void ctest1(void)
{
	uart_string("1 Started\n");
	uart_string("1 Locking\n");
	lock(&testm);
	add_thread(ctest3, 0, 3);
	add_thread(ctest2, 0, 2);
	uart_string("1 Unlocking\n");
	unlock(&testm);
	uart_string("1 Finished\n");
}

void ctest2(void)
{
	uart_string("2 Started\n");
	add_thread(ctest4, 0, 3);
	uart_string("2 Locking\n");
	lock(&testm);
	uart_string("2 Unlocking\n");
	unlock(&testm);
	uart_string("2 Finished\n");
}

void ctest3(void)
{
	uart_string("3 Started\n");
	uart_string("3 Finished\n");
}

void ctest4(void)
{
	uart_string("4 Started\n");
	uart_string("4 Finished\n");
}

void btest(void)
{
	add_thread(ctest1, 0, 3);
}
