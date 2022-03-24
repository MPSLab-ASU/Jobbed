#include <graphics/lfb.h>
#include <symbols.h>

char* ulong_to_string(unsigned long value, char* data)
{
	unsigned long t = value;
	unsigned long c;
	char* dptr = data + 9;
	for (int i = 0; i <= 10; i++) {
		c = t%10;
		*dptr = 0x30 + (c&0xF);
		t /= 10;
		if (t==0)
			break;
		dptr -= 1;
	}
	return dptr;
}

void main(void)
{
	static char str[13];
	char* start = ulong_to_string(*(volatile unsigned long*)SYS_TIMER_CHI, str);
	draw_string(0, 10, start);
}
