#include <cpu.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <symbols.h>
#include <sys/schedule.h>
#include <usr/string.h>
#include <usr/timed.h>
#include <usr/uart.h>

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
	.tick_rate = 70000,
	.priority = 0,
	.arg = 0,
};

static struct UartInfo UART_INFO = {
	.priority = 2,
};

static unsigned long simulated = 0;

void producer(void)
{
	draw_string(0, 15, "Producing...");
	sys1(SYS_SEMAPHORE_V, &simulated);
	draw_string(0, 15, "Produced!   ");
}

void consumer(void)
{
	add_thread(producer, 0, 4);
	while (1) {
		draw_string(0, 16, "Consuming...");
		sys1(SYS_SEMAPHORE_P, &simulated);
		draw_string(0, 16, "Consumed!   ");
	}
}

void main(void)
{
	subscribe_irq(UART_IRQ, handle_data, &UART_INFO);
	subscribe_irq(SYS_TIMER_0_IRQ, loopt, &stime_0);
	subscribe_irq(SYS_TIMER_1_IRQ, loopt, &stime_1);
	subscribe_irq(SYS_TIMER_2_IRQ, loopt, &stime_2);
	subscribe_irq(SYS_TIMER_3_IRQ, loopt, &stime_3);
	add_thread(loop, 0, 8);
	add_thread(consumer, 0, 3);
}
