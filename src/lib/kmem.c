#include <globals.h>
#include <drivers/uart.h>
#include <lib/kmem.h>

// Output longs at address
void kmemshow32(void* data, unsigned long length)
{
	unsigned long* ptr = data;
	for(unsigned long i = 0; i < length; i++) {
		uart_hex(*ptr);
		ptr+=1;
		if (i != length-1)
			uart_char(' ');
	}
	uart_char('\n');
}

// Output bytes at address
void kmemshow(void* data, unsigned long length)
{
	unsigned char* ptr = data;
	for(unsigned long i = 0; i < length; i++) {
		char tmp = *ptr>>4;
		tmp += 0x30;
		if (tmp > 0x39)
			tmp += 0x7;
		uart_char(tmp);
		tmp = *ptr&0xF;
		tmp += 0x30;
		if (tmp > 0x39)
			tmp += 0x7;
		uart_char(tmp);
		ptr+=1;
		if (i != length-1)
			uart_char(' ');
	}
	uart_char('\n');
}
