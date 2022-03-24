#include <graphics/lfb.h>
#include <symbols.h>

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

void main(void)
{
	static char str[13];
	static unsigned long previous = 0;
	char* start;
	unsigned long current = *(volatile unsigned long*)SYS_TIMER_CHI;
	start = ulong_to_string(current, str);
	draw_string(0, 10, start);
	start = ulong_to_string(current - previous, str);
	draw_string(0, 11, "            ");
	draw_string(0, 11, start);
	previous = current;
}
