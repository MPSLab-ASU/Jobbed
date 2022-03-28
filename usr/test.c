#include <cpu.h>
#include <graphics/lfb.h>
#include <sys/schedule.h>
#include <usr/string.h>
#include <util/mutex.h>

#define MAX_ITER 8192
#define MULTIPLIER 1000000/900

static unsigned long ti, tf;
static unsigned long times[MAX_ITER];
static unsigned long idx = 0;

void test_results(unsigned long off)
{
	unsigned long mean=0, stdev=0, max=0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		mean += times[i];
		if (times[i] > max)
			max = times[i];
	}
	mean /= MAX_ITER;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		unsigned long term = (times[i]-mean)*(times[i]-mean)/MAX_ITER;
		stdev += term;
	}
	static char str[13];
	char* start;
	start = ulong_to_string(mean, str);
	draw_string(off*13, 12, start);
	start = ulong_to_string(stdev, str);
	draw_string(off*13, 13, start);
	start = ulong_to_string(max, str);
	draw_string(off*13, 14, start);
}

void nopfxn(void) {}

void trace_test(void)
{
	sys0_32(SYS_TIME_2, &ti);
	sys0_32(SYS_TIME_2, &tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void switch_test(void)
{
	sys0_32(SYS_TIME_2, &ti);
	sys0(SYS_YIELD);
	sys0_32(SYS_TIME_2, &tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void add_low_test(void)
{
	sys0_32(SYS_TIME_2, &ti);
	add_thread(nopfxn, 0, 6);
	sys0_32(SYS_TIME_2, &tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void add_high_test(void)
{
	sys0_32(SYS_TIME_2, &ti);
	add_thread(nopfxn, 0, 1);
	sys0_32(SYS_TIME_2, &tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void mutex_create_test(void)
{
	struct Mutex* m;
	sys0_32(SYS_TIME_2, &ti);
	m = create_mutex(0);
	sys0_32(SYS_TIME_2, &tf);
	delete_mutex(m);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void mutex_delete_test(void)
{
	struct Mutex* m = create_mutex(0);
	sys0_32(SYS_TIME_2, &ti);
	delete_mutex(m);
	sys0_32(SYS_TIME_2, &tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void mutex_lock_test(struct Mutex* m)
{
	sys0_32(SYS_TIME_2, &ti);
	lock_mutex(m);
	sys0_32(SYS_TIME_2, &tf);
	unlock_mutex(m);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void mutex_unlock_test(struct Mutex* m)
{
	lock_mutex(m);
	sys0_32(SYS_TIME_2, &ti);
	unlock_mutex(m);
	sys0_32(SYS_TIME_2, &tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void semaphore_p_test(unsigned long* sem)
{
	sys0_32(SYS_TIME_2, &ti);
	sys1(SYS_SEMAPHORE_P, sem);
	sys0_32(SYS_TIME_2, &tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void semaphore_v_test(unsigned long* sem)
{
	sys0_32(SYS_TIME_2, &ti);
	sys1(SYS_SEMAPHORE_V, sem);
	sys0_32(SYS_TIME_2, &tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void test_super(void)
{
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(trace_test, 0, 2);
	}
	add_thread(test_results,(void*) 0, 0);idx = 0;

	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(switch_test, 0, 2);
	}
	add_thread(test_results,(void*) 1, 0);idx = 0;

	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(add_low_test, 0, 2);
	}
	add_thread(test_results,(void*) 2, 0);idx = 0;

	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(add_high_test, 0, 2);
	}
	add_thread(test_results,(void*) 3, 0);idx = 0;

	struct Mutex* m;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(mutex_create_test, 0, 2);
	}
	add_thread(test_results,(void*) 4, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(mutex_delete_test, 0, 2);
	}
	add_thread(test_results,(void*) 5, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		m = create_mutex(0);
		lock_mutex(m);
		add_thread(mutex_lock_test, 0, 2);
		unlock_mutex(m);
		delete_mutex(m);
	}
	add_thread(test_results,(void*) 6, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		m = create_mutex(0);
		add_thread(mutex_lock_test, 0, 2);
		delete_mutex(m);
	}
	add_thread(test_results,(void*) 7, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		m = create_mutex(0);
		add_thread(mutex_unlock_test, 0, 2);
		delete_mutex(m);
	}
	add_thread(test_results,(void*) 8, 0);idx = 0;

	static unsigned long semp = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		semp = 1;
		add_thread(semaphore_p_test, &semp, 2);
	}
	add_thread(test_results,(void*) 9, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		semp = 0;
		add_thread(semaphore_v_test, &semp, 2);
	}
	add_thread(test_results,(void*) 10, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		semp = 1;
		add_thread(semaphore_v_test, &semp, 2);
	}
	add_thread(test_results,(void*) 11, 0);idx = 0;
}
