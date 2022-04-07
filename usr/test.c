#include <cpu.h>
#include <cpu/irq.h>
#include <graphics/lfb.h>
#include <sys/schedule.h>
#include <usr/math.h>
#include <usr/string.h>
#include <util/mutex.h>

#define MAX_ITER 4096
//Multiplier for SYS_TIME_2
//#define MULTIPLIER 1000000/900
//#define TIMER(x) sys0_32(SYS_TIME_2, &x)
//static unsigned long ti, tf;

//Multiplier for SYS_TIME
#define MULTIPLIER 1000
#define TIMER(x) sys0_64(SYS_TIME, &x)
static unsigned long long ti, tf;

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
	stdev = sqrt_rnd(stdev);
	char str[] = "            us\0";
	char* start;
	start = ulong_to_string(mean, str);
	draw_string(off*15, 12, start);
	start = ulong_to_string(stdev, str);
	draw_string(off*15, 13, start);
	start = ulong_to_string(max, str);
	draw_string(off*15, 14, start);
}

void nopfxn(void) {}

void trace_test(void)
{
	TIMER(ti);
	TIMER(tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void switch_test(void)
{
	TIMER(ti);
	sys0(SYS_YIELD);
	TIMER(tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void add_low_test(void)
{
	TIMER(ti);
	add_thread(nopfxn, 0, 4);
	TIMER(tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void add_high_test(void)
{
	TIMER(ti);
	add_thread(nopfxn, 0, 0);
	TIMER(tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void mutex_create_test(void)
{
	struct Mutex* m;
	TIMER(ti);
	m = create_mutex(0);
	TIMER(tf);
	delete_mutex(m);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void mutex_delete_test(void)
{
	struct Mutex* m = create_mutex(0);
	TIMER(ti);
	delete_mutex(m);
	TIMER(tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void mutex_lock_test(struct Mutex* m)
{
	TIMER(ti);
	lock_mutex(m);
	TIMER(tf);
	unlock_mutex(m);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void mutex_unlock_test(struct Mutex* m)
{
	lock_mutex(m);
	TIMER(ti);
	unlock_mutex(m);
	TIMER(tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void semaphore_p_test(unsigned long* sem)
{
	TIMER(ti);
	sys1(SYS_SEMAPHORE_P, sem);
	TIMER(tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void semaphore_v_test(unsigned long* sem)
{
	TIMER(ti);
	sys1(SYS_SEMAPHORE_V, sem);
	TIMER(tf);
	times[idx++] = (tf-ti)*MULTIPLIER;
}

void test_super(void)
{
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(trace_test, 0, 2);
	}
	draw_string(0*15, 11, "Trace");
	add_thread(test_results,(void*) 0, 0);idx = 0;

	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(switch_test, 0, 2);
	}
	draw_string(1*15, 11, "Thread Switch");
	add_thread(test_results,(void*) 1, 0);idx = 0;

	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(add_low_test, 0, 2);
	}
	draw_string(2*15, 11, "LThread Create");
	add_thread(test_results,(void*) 2, 0);idx = 0;

	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(add_high_test, 0, 2);
	}
	draw_string(3*15, 11, "HThread Create");
	add_thread(test_results,(void*) 3, 0);idx = 0;

	struct Mutex* m;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(mutex_create_test, 0, 2);
	}
	draw_string(4*15, 11, "Mutex Create");
	add_thread(test_results,(void*) 4, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		add_thread(mutex_delete_test, 0, 2);
	}
	draw_string(5*15, 11, "Mutex Delete");
	add_thread(test_results,(void*) 5, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		m = create_mutex(0);
		lock_mutex(m);
		add_thread(mutex_lock_test, 0, 2);
		unlock_mutex(m);
		delete_mutex(m);
	}
	draw_string(6*15, 11, "Mutex Lock(C)");
	add_thread(test_results,(void*) 6, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		m = create_mutex(0);
		add_thread(mutex_lock_test, 0, 2);
		delete_mutex(m);
	}
	draw_string(7*15, 11, "Mutex Lock");
	add_thread(test_results,(void*) 7, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		m = create_mutex(0);
		add_thread(mutex_unlock_test, 0, 2);
		delete_mutex(m);
	}
	draw_string(8*15, 11, "Mutex Unlock");
	add_thread(test_results,(void*) 8, 0);idx = 0;

	static unsigned long semp = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		semp = 1;
		add_thread(semaphore_p_test, &semp, 2);
	}
	draw_string(9*15, 11, "Semaphore P");
	add_thread(test_results,(void*) 9, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		semp = 0;
		add_thread(semaphore_v_test, &semp, 2);
	}
	draw_string(10*15, 11, "Semaphore0 V");
	add_thread(test_results,(void*) 10, 0);idx = 0;
	for (unsigned long i = 0; i < MAX_ITER; i++) {
		semp = 1;
		add_thread(semaphore_v_test, &semp, 2);
	}
	draw_string(11*15, 11, "Semaphore V");
	add_thread(test_results,(void*) 11, 0);idx = 0;
}

void delaytest(void)
{
	static unsigned long count = 0;
	static unsigned long long ts[4096*2+20];
	sys0_64(SYS_TIME, &ts[count++]);
	if (count == (4096*2+18)) {
		unsubscribe_irq(SYS_TIMER_3_IRQ);
		static char str[14];
		char* start;
		unsigned long mean=0, stdev=0, max=0, min=0xFFFFFFFF;
		for (unsigned long i = 0; i < 4096; i++) {
			unsigned long elapsed = ts[2*(i+2)+1]-ts[2*(i+2)];
			elapsed *= 1000;
			mean += elapsed;
			if (elapsed > max)
				max = elapsed;
			if (elapsed < min)
				min = elapsed;
		}
		mean /= 4096;
		for (unsigned long i = 0; i < 4096; i++) {
			unsigned long elapsed = ts[2*(i+2)+1]-ts[2*(i+2)];
			elapsed *= 1000;
			unsigned long term = (elapsed-mean)*(elapsed-mean)/4096;
			stdev += term;
		}
		stdev = sqrt_rnd(stdev);
		start = ulong_to_string(mean, str);
		draw_string(0, 10, start);
		start = ulong_to_string(stdev, str);
		draw_string(0, 11, start);
		start = ulong_to_string(min, str);
		draw_string(0, 12, start);
		start = ulong_to_string(max, str);
		draw_string(0, 13, start);
	}
}

void gptest(void)
{
	static unsigned long count = 0;
	static unsigned long long ts[4096*2+20];
	sys0_64(SYS_TIME, &ts[count++]);
	if (count == (4096*2+18)) {
		unsubscribe_irq(GPIO_BANK_1_IRQ);
		//unsubscribe_irq(SYS_TIMER(_3_IRQ);
		static char str[14];
		char* start;
		unsigned long mean=0, stdev=0, max=0, min=0xFFFFFFFF;
		for (unsigned long i = 0; i < 4096; i++) {
			unsigned long elapsed = ts[2*(i+2)+1]-ts[2*(i+2)];
			elapsed *= 1000;
			mean += elapsed;
			if (elapsed > max)
				max = elapsed;
			if (elapsed < min)
				min = elapsed;
		}
		mean /= 4096;
		for (unsigned long i = 0; i < 4096; i++) {
			unsigned long elapsed = ts[2*(i+2)+1]-ts[2*(i+2)];
			elapsed *= 1000;
			unsigned long term = (elapsed-mean)*(elapsed-mean)/4096;
			stdev += term;
		}
		stdev = sqrt_rnd(stdev);
		start = ulong_to_string(mean, str);
		draw_string(0, 10, start);
		start = ulong_to_string(stdev, str);
		draw_string(0, 11, start);
		start = ulong_to_string(min, str);
		draw_string(0, 12, start);
		start = ulong_to_string(max, str);
		draw_string(0, 13, start);
	}
}