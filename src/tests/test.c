#include <cpu.h>
//#include <drivers/uart.h>
#include <graphics/lfb.h>
#include <lib/kmem.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <util/mutex.h>
#include <util/lock.h>

extern void atest(void);
void btest(void);

static int x = 0;
static int y = 13;
void test_entry(void)
{
	x = 0;
	unsigned int z = 1;
	draw_string(0, y+4, "Starting tests");
	unsigned long long ti, tf, dt=0;
	for(int i = 0; i < 64; i++) {
		sys0_64(SYS_TIME, &ti);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if (z % 2) {
			DRAW64(0+17, y+((z+1)/2)+5, (tf-ti));
		} else {
			DRAW64(0, y+(z/2)+5, (tf-ti));
		}
		z++;
	}
	DRAW64(0, y+5, dt/64);
	DRAW64(17, y+5, dt%64);
	z = 1;

	// atest
	//add_thread(atest, 0, 0);
	dt = 0;
	for(int i = 0; i < 64; i++) {
		sys0_64(SYS_TIME, &ti);
		sys0(SYS_YIELD);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if (z % 2) {
			DRAW64(35+17, y+((z+1)/2)+5, (tf-ti));
		} else {
			DRAW64(35, y+(z/2)+5, (tf-ti));
		}
		z++;
	}
	DRAW64(35, y+5, dt/64);
	DRAW64(35+17, y+5, dt%64);

	z = 1;
	dt = 0;
	for(int i = 0; i < 64; i++) {
		sys0_64(SYS_TIME, &ti);
		add_thread(atest, 0, 3);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if (z % 2) {
			DRAW64(70+17, y+((z+1)/2)+5, (tf-ti));
		} else {
			DRAW64(70, y+(z/2)+5, (tf-ti));
		}
		z++;
	}
	DRAW64(70, y+5, dt/64);
	DRAW64(70+17, y+5, dt%64);

	z = 1;
	dt = 0;
	for(int i = 0; i < 64; i++) {
		sys0_64(SYS_TIME, &ti);
		add_thread(atest, 0, 0);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if (z % 2) {
			DRAW64(105+17, y+((z+1)/2)+5, (tf-ti));
		} else {
			DRAW64(105, y+(z/2)+5, (tf-ti));
		}
		z++;
	}
	DRAW64(105, y+5, dt/64);
	DRAW64(105+17, y+5, dt%64);
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
	draw_cletter(x++, y+2, 'S', 0xFF0000);
	//uart_string("1 Started\n");
	draw_cletter(x++, y+2, 'L', 0xFF0000);
	//uart_string("1 Locking\n");
	lock(&testm);
	add_thread(ctest3, 0, 2);
	draw_cletter(x++, y+2, 'U', 0xFF0000);
	//uart_string("1 Unlocking\n");
	unlock(&testm);
	draw_cletter(x++, y+2, 'F', 0xFF0000);
	//uart_string("1 Finished\n");
}

void ctest2(void)
{
	draw_cletter(x++, y+0, 'S', 0x0000FF);
	//uart_string("2 Started\n");
	add_thread(ctest4, 0, 3);
	draw_cletter(x++, y+0, 'L', 0x0000FF);
	//uart_string("2 Locking\n");
	lock(&testm);
	draw_cletter(x++, y+0, 'U', 0x0000FF);
	//uart_string("2 Unlocking\n");
	unlock(&testm);
	draw_cletter(x++, y+0, 'F', 0x0000FF);
	//uart_string("2 Finished\n");
}

void ctest3(void)
{
	draw_cletter(x++, y+1, 'S', 0x00FF00);
	//uart_string("3 Started\n");
	add_thread(ctest2, 0, 1);
	draw_cletter(x++, y+1, 'F', 0x00FF00);
	//uart_string("3 Finished\n");
}

void ctest4(void)
{
	draw_cletter(x++, y+2, 'S', 0xAFAF00);
	//uart_string("4 Started\n");
	draw_cletter(x++, y+2, 'F', 0xAFAF00);
	//uart_string("4 Finished\n");
}

static unsigned long test_semaphore = 0;

void stest1(void)
{
	draw_cletter(x++, y+1, ' ', 0xFF0000);
	draw_cletter(x++, y+1, 'S', 0xFF0000);
	draw_cletter(x++, y+1, 'P', 0xFF0000);
	sys1(SYS_SEMAPHORE_P, &test_semaphore);
	draw_cletter(x++, y+1, 'F', 0xFF0000);
}

void stest2(void)
{
	draw_cletter(x++, y+2, 'S', 0xFF00);
	draw_cletter(x++, y+2, 'V', 0xFF00);
	sys1(SYS_SEMAPHORE_V, &test_semaphore);
	draw_cletter(x++, y+2, 'F', 0xFF00);
}

void btest(void)
{
	draw_string(0, y+0, "                   ");
	draw_string(0, y+1, "                   ");
	draw_string(0, y+2, "                   ");
	draw_string(0, y+3, "                   ");
	x = 0;
	add_thread(ctest1, 0, 3);
	add_thread(stest1, 0, 6);
	add_thread(stest2, 0, 7);
}
