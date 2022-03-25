#include <globals.h>
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

static struct UartInfo UART_INFO = {
	.priority = 2,
};

static struct SysTimerInfo stime_3 = {
	.tick_rate = 70000,
	.priority = 0,
	.arg = 0,
};

void main(void)
{
	subscribe_irq(UART_IRQ, handle_data, &UART_INFO);
	subscribe_irq(SYS_TIMER_0_IRQ, loopt, &stime_0);
	subscribe_irq(SYS_TIMER_1_IRQ, loopt, &stime_1);
	subscribe_irq(SYS_TIMER_2_IRQ, loopt, &stime_2);
	subscribe_irq(SYS_TIMER_3_IRQ, loopt, &stime_3);
	add_thread(loop, 0, 0);
	unsigned long long counter;
	asm volatile ("mrrc p15, 0, %0, c14" : "=r"(counter));
	draw_hex32(0, 20, counter>>32);
	draw_hex32(9, 20, counter);
}
