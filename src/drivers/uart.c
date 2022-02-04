#include <drivers/uart.h>

#define UART_BUFFER_SIZE 0x100
struct UartBuffer {
	char buffer[UART_BUFFER_SIZE];
	unsigned long roffset;
	unsigned long woffset;
} ubuffer;

void uart_init(void)
{
	ubuffer.roffset = 0;
	ubuffer.woffset = 0;
}

// s = zero-terminated string
void* uart_print(char* s)
{
	char* ptr = s;
	while (1) {
		if (*ptr == 0)
			break;
		ubuffer.buffer[ubuffer.woffset] = *ptr;
		if ((ubuffer.woffset+1)%UART_BUFFER_SIZE == ubuffer.roffset)
			return ptr;
		ubuffer.woffset++;
		ubuffer.woffset %= UART_BUFFER_SIZE;
		ptr += 1;
	}
	return 0;
}

void uart_flush(void)
{
	while (ubuffer.roffset != ubuffer.woffset) {
		uart_char(ubuffer.buffer[ubuffer.roffset++]);
		ubuffer.roffset %= UART_BUFFER_SIZE;
	}
}

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
