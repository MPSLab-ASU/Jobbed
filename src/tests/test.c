#include <cpu.h>
#include <drivers/uart.h>

void utime(void);

void test_entry(void)
{
}

void utime(void)
{
	unsigned long long ti, tf, dt;
	sys0_64(SYS_TIME, &ti);
	sys0_64(SYS_TIME, &tf);
	dt = tf - ti;
	PRINT64(dt);
}
