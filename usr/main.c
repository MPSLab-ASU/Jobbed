#include <cpu.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <symbols.h>
#include <sys/schedule.h>
#include <usr/string.h>
#include <usr/timed.h>
#include <usr/uart.h>
#include <usr/math.h>

static struct SysTimerInfo stime_0 = {
	.tick_rate = 5000000,
	.priority = 0,
	.arg = 0,
};

static struct SysTimerInfo stime_1 = {
	.tick_rate = 700000,
	.priority = 0,
	.arg = 0,
};

static struct SysTimerInfo stime_2 = {
	.tick_rate = 300000,
	.priority = 0,
	.arg = 0,
};

static struct SysTimerInfo stime_3 = {
	.tick_rate = 6,
	.priority = 0,
	.arg = 0,
};

static struct UartInfo UART_INFO = {
	.priority = 2,
};

static struct GPIOInfo gpinfo = {
	.pin = (1<<16 | 1<<12),
	.priority = 0,
};

void gptest(void)
{
	static unsigned long count = 0;
	static unsigned long long ts[4096*2+20];
	sys0_64(SYS_TIME, &ts[count++]);
	if (count == (4096*2+18)) {
		//unsubscribe_irq(GPIO_BANK_1_IRQ);
		unsubscribe_irq(SYS_TIMER_3_IRQ);
		static char str[14];
		char* start;
		unsigned long mean=0, stdev=0, max=0;
		for (unsigned long i = 0; i < 4096; i++) {
			unsigned long elapsed = ts[2*(i+2)+1]-ts[2*(i+2)];
			elapsed *= 1000;
			mean += elapsed;
			if (elapsed > max)
				max = elapsed;
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
		start = ulong_to_string(max, str);
		draw_string(0, 12, start);
	}
	//unsigned long gplev0 = *GPLEV0;
	//draw_hex32(0, 30, gplev0);
	//start = ulong_to_string(count++, str);
	//draw_string(0, 31, start);
}

void test_super(void);

void main(void)
{
	//subscribe_irq(UART_IRQ, handle_data, &UART_INFO);
	//subscribe_irq(SYS_TIMER_0_IRQ, loopt, &stime_0);
	//subscribe_irq(SYS_TIMER_1_IRQ, loopt, &stime_1);
	//subscribe_irq(SYS_TIMER_2_IRQ, loopt, &stime_2);
	//subscribe_irq(SYS_TIMER_3_IRQ, loopt, &stime_3);
	subscribe_irq(SYS_TIMER_3_IRQ, gptest, &stime_3);
	//subscribe_irq(GPIO_BANK_1_IRQ, gptest, &gpinfo);
	//add_thread(loop, 0, 8);
	//add_thread(consumer, 0, 3);
	//add_thread(test_super, 0, 4);
}
