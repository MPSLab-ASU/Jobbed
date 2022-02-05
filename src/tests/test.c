#include <cpu.h>
#include <drivers/uart.h>
#include <graphics/lfb.h>
#include <lib/kmem.h>
#include <sys/core.h>
#include <sys/schedule.h>

extern void atest(void);

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
}
