#include <cpu.h>
//#include <drivers/uart.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <lib/kmem.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <util/lock.h>
#include <util/mutex.h>
#include <util/status.h>

extern void atest(void);
void qualitative_tests(void);

void nooptest(void) {}

void mutex_contention_helper(struct Mutex* m)
{
	lock_mutex(m);
	sys0(SYS_YIELD);
	unlock_mutex(m);
}

static int x = 0;
static int y = 13;
#define TEST_STR_CLR "    "
#define TEST_RESULT_WIDTH 15
#define TEST_COUNT 4096
#define TEST_BIN_COUNT 32
void test_entry(void)
{
	x = 0;
	draw_hex32(0, y-1, nextpid);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

	// Test 3: Add Thread, Lower Priority
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sys0_64(SYS_TIME, &ti);
		add_thread(nooptest, 0, 3);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

	// Test 4: Add Thread, Higher Priority
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sys0_64(SYS_TIME, &ti);
		add_thread(nooptest, 0, 0);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

	// Test 7: Lock Mutex
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		struct Mutex* m = create_mutex(0);
		sys0_64(SYS_TIME, &ti);
		lock_mutex(m);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

	// Test 7a: Lock Contended Mutex
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		struct Mutex* m = create_mutex(0);
		add_thread(mutex_contention_helper, m, 2);
		sys0(SYS_YIELD);
		sys0_64(SYS_TIME, &ti);
		lock_mutex(m);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

	// Test 8: Unlock Mutex
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		struct Mutex* m = create_mutex(0);
		lock_mutex(m);
		sys0_64(SYS_TIME, &ti);
		unlock_mutex(m);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

	// Semaphore
	static unsigned long sem = 0;

	// Test 9: Semaphore Decrease
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sem = 1;
		sys0_64(SYS_TIME, &ti);
		sys1(SYS_SEMAPHORE_P, &sem);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

	// Test 10: Semaphore Increase
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sem = 0;
		sys0_64(SYS_TIME, &ti);
		sys1(SYS_SEMAPHORE_V, &sem);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

	// Test 10a: Semaphore Increase - No Schedule
	dt = 0;
	for(int i = 0; i < TEST_COUNT; i++) {
		sem = 1;
		sys0_64(SYS_TIME, &ti);
		sys1(SYS_SEMAPHORE_V, &sem);
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
	draw_string(tidx, y+5, "       ");
	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
	tidx += TEST_RESULT_WIDTH;
	draw_hex32(0, y-1, nextpid);

//	// Test 7: Tick Latency
//#define DELAY_TIME 512000
//	unsigned long center = 0;
//	sys0_64(SYS_TIME, &ti);
//	delay(DELAY_TIME);
//	sys0_64(SYS_TIME, &tf);
//	center = (tf - ti - 10);
//	if (10 > (tf-ti))
//		center = 0;
//	dt = 0;
//	unsigned long j = 0;
//	for(int i = 0; i < TEST_COUNT; i++) {
//		sys0_64(SYS_TIME, &ti);
//		delay(DELAY_TIME);
//		sys0_64(SYS_TIME, &tf);
//		dt += tf - ti;
//		if ((tf-ti-center) < TEST_BIN_COUNT)
//			bins[(tf-ti)-center]++;
//		else
//			j++;
//	}
//	for (int i = 0; i < TEST_BIN_COUNT; i++) {
//		draw_hex32(tidx, y+6+i, i);
//		draw_string(tidx+9, y+6+i, TEST_STR_CLR);
//		draw_u10(tidx+9, y+6+i, bins[i]);
//		bins[i] = 0;
//	}
//	draw_hex32(tidx, y+4, j);
//	draw_string(tidx, y+5, "       ");
//	len = draw_u10(tidx, y+5, dt/TEST_COUNT);
//	draw_u10(tidx+len+1, y+5, dt%TEST_COUNT);
//	tidx += TEST_RESULT_WIDTH;
//	draw_hex32(0, y-1, nextpid);

	add_thread(qualitative_tests, 0, 4);
}

//static struct Mutex testm = {.addr = 0, .pid = 0};
static struct Lock testm = {.pid = 0};

void priority_inversion_test1(void);
void priority_inversion_test2(void);
void priority_inversion_test3(void);
void priority_inversion_test4(void);

void priority_inversion_test1(void)
{
	draw_cletter(x++, y+2, 'S', 0xFF0000);
	// Try Lock
	draw_cletter(x++, y+2, 'T', 0xFF0000);
	lock(&testm);
	// Lock Acquired
	draw_cletter(x++, y+2, 'L', 0xFF0000);
	// Add Thread to Assist with Priority Inversion
	//  Check
	//  - Show that this thread gets temporarily
	//     promoted
	add_thread(priority_inversion_test3, 0, 2);
	// Unlock
	draw_cletter(x++, y+2, 'U', 0xFF0000);
	unlock(&testm);
	draw_cletter(x++, y+2, 'F', 0xFF0000);
}

void priority_inversion_test2(void)
{
	draw_cletter(x++, y+0, 'S', 0x0000FF);
	// Add Thread to Assist with Priority Inversion
	//  Check
	//  - Show that Thread 1 is Prepended To Queue
	add_thread(priority_inversion_test4, 0, 3);
	// Try Lock
	draw_cletter(x++, y+0, 'T', 0x0000FF);
	lock(&testm);
	// Lock Acquired
	draw_cletter(x++, y+0, 'L', 0x0000FF);
	// Unlock
	draw_cletter(x++, y+0, 'U', 0x0000FF);
	unlock(&testm);
	draw_cletter(x++, y+0, 'F', 0x0000FF);
}

void priority_inversion_test3(void)
{
	draw_cletter(x++, y+1, 'S', 0x00FF00);
	// Add thread to Assist with Priority Inversion
	//  Check
	//  - Add high priority thread that will try
	//     to lock the mutex
	add_thread(priority_inversion_test2, 0, 1);
	draw_cletter(x++, y+1, 'F', 0x00FF00);
}

void priority_inversion_test4(void)
{
	draw_cletter(x++, y+2, 'S', 0xAFAF00);
	// Do nothing,
	//  just show that this is executed last
	draw_cletter(x++, y+2, 'F', 0xAFAF00);
}

static unsigned long test_semaphore = 0;

void semaphore_test1(void)
{
	draw_cletter(x++, y+1, ' ', 0xFF0000);
	draw_cletter(x++, y+1, 'S', 0xFF0000);
	// Try to decrement semaphore
	draw_cletter(x++, y+1, 'T', 0xFF0000);
	sys1(SYS_SEMAPHORE_P, &test_semaphore);
	// Semaphore decremented
	draw_cletter(x++, y+1, 'P', 0xFF0000);
	draw_cletter(x++, y+1, 'F', 0xFF0000);
}

void semaphore_test2(void)
{
	draw_cletter(x++, y+2, 'S', 0xFF00);
	// Increment semaphore
	draw_cletter(x++, y+2, 'V', 0xFF00);
	sys1(SYS_SEMAPHORE_V, &test_semaphore);
	draw_cletter(x++, y+2, 'F', 0xFF00);
}

static struct Mutex* dead1 = 0;
static struct Mutex* dead2 = 0;

void deadlock_test2(void)
{
	draw_cletter(x++, y+1, 'S', 0xFF0000);
	// Try Lock 1
	draw_cletter(x++, y+1, 'T', 0xFF0000);
	lock_mutex(dead1);
	// Lock 1 Acquired
	draw_cletter(x++, y+1, 'L', 0xFF0000);
	// Try Lock 2
	draw_cletter(x++, y+1, 't', 0xFF0000);
	lock_mutex(dead2);
	// Lock 2 Acquired
	draw_cletter(x++, y+1, 'l', 0xFF0000);
	// Unlock Locks
	draw_cletter(x++, y+1, 'u', 0xFF0000);
	unlock_mutex(dead2);
	draw_cletter(x++, y+1, 'U', 0xFF0000);
	unlock_mutex(dead1);
	draw_cletter(x++, y+1, 'F', 0xFF0000);
}

void deadlock_test1(void)
{
	draw_cletter(x++, y+2, ' ', 0xFF00);
	draw_cletter(x++, y+2, 'S', 0xFF00);
	dead1 = create_mutex((void*)0xDEADBEEF);
	dead2 = create_mutex((void*)0x12345678);
	// Try Lock 2
	draw_cletter(x++, y+2, 't', 0xFF00);
	lock_mutex(dead2);
	// Lock 2 Acquired
	draw_cletter(x++, y+2, 'l', 0xFF00);
	// Create Higher priority thread to
	//  check deadlock condition
	draw_cletter(x++, y+2, 'A', 0xFF00);
	add_thread(deadlock_test2, 0, 4);
	// Try Lock 1 - This would deadlock
	//  if no mechanism is in place to
	//  prevent it
	draw_cletter(x++, y+2, 'T', 0xFF00);
	lock_mutex(dead1);
	// Lock 1 Acquired - Deadlock condition
	//  properly handled
	draw_cletter(x++, y+2, 'L', 0xFF00);
	// Unlock Locks
	draw_cletter(x++, y+2, 'u', 0xFF00);
	unlock_mutex(dead2);
	draw_cletter(x++, y+2, 'U', 0xFF00);
	unlock_mutex(dead1);
	delete_mutex(dead1);
	delete_mutex(dead2);
	draw_cletter(x++, y+2, 'F', 0xFF00);
}

void qualitative_tests(void)
{
	draw_string(0, y+0, "                   ");
	draw_string(0, y+1, "                   ");
	draw_string(0, y+2, "                   ");
	draw_string(0, y+3, "                   ");
	x = 0;
	add_thread(atest, 0, 0);
	add_thread(priority_inversion_test1, 0, 3);
	add_thread(deadlock_test1, 0, 5);
	add_thread(semaphore_test1, 0, 6);
	add_thread(semaphore_test2, 0, 7);
	add_thread(time_status, 0, 8);
}
