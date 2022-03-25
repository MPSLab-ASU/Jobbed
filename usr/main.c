#include <graphics/lfb.h>
#include <globals.h>
#include <symbols.h>
#include <sys/schedule.h>
#include <util/time.h>

void loop(void);

void handle_data(unsigned char data)
{
	// Newline Case
	if (data == 0x0D) {
	// Backspace Case
	} else if (data == 0x08 || data == 0x7F) {
	} else if (data == 0x61) {
		add_thread(uart_scheduler, 0, 2);
	} else if (data == 0x62) {
		//add_thread(test_entry, 0, 2);
	}
	// Draw it on the screen
	{
		draw_chex32(0, 9, data, 0xAA00FF);
	}
}

char* ulong_to_string(unsigned long value, char* data)
{
	unsigned long t = value;
	unsigned long c;
	char* dptr = data + 10;
	for (int i = 0; i <= 10; i++) {
		c = t%10;
		*dptr = 0x30 + (c&0xF);
		t /= 10;
		if (t==0)
			break;
		dptr -= 1;
		if (i == 5) {
			*dptr = '.';
			dptr -= 1;
		}
	}
	return dptr;
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
	wait_msec(3000);
	add_thread(loop, 0, 3);
}

void loopt(void)
{
	static char str[13];
	static char cnt = 18;
	draw_string(0, 14, ulong_to_string(*(volatile unsigned long*)SYS_TIMER_CHI, str));
	cnt--;
	if (cnt == 2)
		unsubscribe_irq(SYS_TIMER_1_IRQ);
	if (cnt == 0)
		unsubscribe_irq(SYS_TIMER_0_IRQ);
}

static unsigned long TICK_RATE_0 = 5000000;
static unsigned long TICK_RATE_1 =  300000;
static unsigned long UART_PRIORITY = 2;
void main(void)
{
	subscribe_irq(UART_IRQ, handle_data, &UART_PRIORITY);
	subscribe_irq(SYS_TIMER_0_IRQ, loopt, &TICK_RATE_0);
	subscribe_irq(SYS_TIMER_1_IRQ, loopt, &TICK_RATE_1);
	add_thread(loop, 0, 0);
}
