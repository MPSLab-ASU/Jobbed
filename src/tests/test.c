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
#define TEST_STR_CLR "   "
#define TEST_RESULT_WIDTH 14
#define TEST_COUNT 128
#define TEST_BIN_COUNT 32
void test_entry(void)
{
	x = 0;
	draw_string(0, y+4, "Starting tests");
	unsigned long long ti, tf, dt=0,len;
	unsigned int tidx = 0;
	unsigned long bins[TEST_BIN_COUNT];
	for (int i = 0; i < TEST_BIN_COUNT; i++) {
		bins[i] = 0;
	}

	// Test 1: Tracing Time
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sys0_64(SYS_TIME, &ti);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if ((tf-ti) < TEST_BIN_COUNT)
			bins[(tf-ti)]++;
	}
	for (int i = 0; i < TEST_BIN_COUNT; i++) {
		draw_hex32(tidx, y+6+i, i);
		draw_string(tidx+9, y+6+i, TEST_STR_CLR);
		draw_u10(tidx+9, y+6+i, bins[i]);
		bins[i] = 0;
	}
	draw_string(tidx, y+5, "      ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;

	// Test 2: Yield Time
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sys0_64(SYS_TIME, &ti);
		sys0(SYS_YIELD);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if ((tf-ti) < TEST_BIN_COUNT)
			bins[(tf-ti)]++;
	}
	for (int i = 0; i < TEST_BIN_COUNT; i++) {
		draw_hex32(tidx, y+6+i, i);
		draw_string(tidx+9, y+6+i, TEST_STR_CLR);
		draw_u10(tidx+9, y+6+i, bins[i]);
		bins[i] = 0;
	}
	draw_string(tidx, y+5, "      ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;

	// Test 3: Add Thread, Lower Priority
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sys0_64(SYS_TIME, &ti);
		add_thread(atest, 0, 3);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if ((tf-ti) < TEST_BIN_COUNT)
			bins[(tf-ti)]++;
	}
	for (int i = 0; i < TEST_BIN_COUNT; i++) {
		draw_hex32(tidx, y+6+i, i);
		draw_string(tidx+9, y+6+i, TEST_STR_CLR);
		draw_u10(tidx+9, y+6+i, bins[i]);
		bins[i] = 0;
	}
	draw_string(tidx, y+5, "      ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;

	// Test 4: Add Thread, Higher Priority
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sys0_64(SYS_TIME, &ti);
		add_thread(atest, 0, 0);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if ((tf-ti) < TEST_BIN_COUNT)
			bins[(tf-ti)]++;
	}
	for (int i = 0; i < TEST_BIN_COUNT; i++) {
		draw_hex32(tidx, y+6+i, i);
		draw_string(tidx+9, y+6+i, TEST_STR_CLR);
		draw_u10(tidx+9, y+6+i, bins[i]);
		bins[i] = 0;
	}
	draw_string(tidx, y+5, "      ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;

	// Test 5: Create Mutex
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sys0_64(SYS_TIME, &ti);
		struct Mutex* m = create_mutex(0);
		sys0_64(SYS_TIME, &tf);
		delete_mutex(m);
		dt += tf - ti;
		if ((tf-ti) < TEST_BIN_COUNT)
			bins[(tf-ti)]++;
	}
	for (int i = 0; i < TEST_BIN_COUNT; i++) {
		draw_hex32(tidx, y+6+i, i);
		draw_string(tidx+9, y+6+i, TEST_STR_CLR);
		draw_u10(tidx+9, y+6+i, bins[i]);
		bins[i] = 0;
	}
	draw_string(tidx, y+5, "      ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;

	// Test 6: Delete Mutex
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		struct Mutex* m = create_mutex(0);
		sys0_64(SYS_TIME, &ti);
		delete_mutex(m);
		sys0_64(SYS_TIME, &tf);
		dt += tf - ti;
		if ((tf-ti) < TEST_BIN_COUNT)
			bins[(tf-ti)]++;
	}
	for (int i = 0; i < TEST_BIN_COUNT; i++) {
		draw_hex32(tidx, y+6+i, i);
		draw_string(tidx+9, y+6+i, TEST_STR_CLR);
		draw_u10(tidx+9, y+6+i, bins[i]);
		bins[i] = 0;
	}
	draw_string(tidx, y+5, "      ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;

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

static struct Mutex* dead1 = 0;
static struct Mutex* dead2 = 0;

void dtest2(void)
{
	draw_cletter(x++, y+1, 'S', 0xFF00);
	draw_cletter(x++, y+1, 'l', 0xFF00);
	lock_mutex(dead1);
	draw_cletter(x++, y+1, 'L', 0xFF00);
	lock_mutex(dead2);
	draw_cletter(x++, y+1, 'u', 0xFF00);
	unlock_mutex(dead2);
	draw_cletter(x++, y+1, 'U', 0xFF00);
	unlock_mutex(dead1);
	draw_cletter(x++, y+1, 'F', 0xFF00);
}

void dtest1(void)
{
	draw_cletter(x++, y+2, ' ', 0xFF0000);
	draw_cletter(x++, y+2, 'S', 0xFF0000);
	dead1 = create_mutex((void*)0xDEADBEEF);
	dead2 = create_mutex((void*)0x12345678);
	draw_cletter(x++, y+2, 'L', 0xFF0000);
	lock_mutex(dead2);
	draw_cletter(x++, y+2, 'A', 0xFF0000);
	add_thread(dtest2, 0, 6);
	draw_cletter(x++, y+2, 'l', 0xFF0000);
	lock_mutex(dead1);
	draw_cletter(x++, y+2, 'u', 0xFF0000);
	unlock_mutex(dead2);
	draw_cletter(x++, y+2, 'U', 0xFF0000);
	unlock_mutex(dead1);
	delete_mutex(dead1);
	delete_mutex(dead2);
	draw_cletter(x++, y+2, 'F', 0xFF0000);
}

void btest(void)
{
	draw_string(0, y+0, "                   ");
	draw_string(0, y+1, "                   ");
	draw_string(0, y+2, "                   ");
	draw_string(0, y+3, "                   ");
	x = 0;
	add_thread(ctest1, 0, 3);
	//add_thread(stest1, 0, 6);
	//add_thread(stest2, 0, 7);
	add_thread(dtest1, 0, 7);
}
