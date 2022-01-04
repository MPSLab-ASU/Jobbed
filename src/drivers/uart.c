#include "../drivers/uart.h"

void uart_10(unsigned long val)
{
	unsigned long t = val;
	unsigned long c;
	static char buffer[11] = "0000000000\0";
	char* dptr = buffer + 9;
	for(int i = 0; i <= 10; i++) {
		c = t%10;
		*dptr = 0x30 + (c&0xF);
		t /= 10;
		if (t==0)
			break;
		dptr -= 1;
	}
	uart_string(dptr);
}

void uart_hexn(unsigned long c_val)
{
	uart_hex(c_val);
	uart_char('\n');
}
