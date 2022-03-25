#define USR_TIMED_C
#include <globals.h>
#include <graphics/lfb.h>
#include <symbols.h>
#include <usr/string.h>
#include <usr/timed.h>
#include <util/time.h>

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
	wait_msec(3000);
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
