#define USR_TIMED_C
#include <cpu.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <symbols.h>
#include <usr/string.h>
#include <usr/timed.h>
#include <util/time.h>

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

void loop(void)
{
	static char str[13];
	static unsigned long previous = 0;
	char* start;
	unsigned long current = *(volatile unsigned long*)SYS_TIMER_CHI;
	start = ulong_to_string(current, str);
	draw_string(0, 10, "            ");
	draw_string(0, 10, start);
	start = ulong_to_string(previous, str);
	draw_string(0, 11, "            ");
	draw_string(0, 11, start);
	start = ulong_to_string(nextpid, str);
	draw_string(0, 12, "            ");
	draw_string(0, 12, start);
	previous++;
	//unsigned long gplev0 = *(volatile unsigned long*)GPLEV0;
	//static unsigned long count = 0;
	//draw_hex32(0, 13, gplev0);
	//if (gplev0 & (1 << 16)) {
	//	draw_hex32(0, 17, count++);
	//	add_thread(producer, 0, 4);
	//}
	wait_msec(30000);
	add_thread(loop, 0, 3);
}

void loopt(void)
{
	static char str[13];
	static char cnt = 18;
	draw_string(0, 14, ulong_to_string(*(volatile unsigned long*)SYS_TIMER_CHI, str));
	cnt--;
	if (cnt == 6)
		unsubscribe_irq(SYS_TIMER_3_IRQ);
	if (cnt == 4)
		unsubscribe_irq(SYS_TIMER_2_IRQ);
	else if (cnt == 2)
		unsubscribe_irq(SYS_TIMER_1_IRQ);
	else if (cnt == 0)
		unsubscribe_irq(SYS_TIMER_0_IRQ);
}
