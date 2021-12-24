#include "../drivers/uart.h"

void memshow32(unsigned long* addr, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++) {
		uart_hex(addr[i]);
		if (i+1 != n)
			uart_char(0x20);
	}
	uart_char(0x0a);
}

void memcpy(unsigned char* src, unsigned char* dest, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++) {
		dest[i] = src[i];
	}
}

unsigned char memcmp(unsigned char* a, unsigned char* b, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++) {
		if (a[i] != b[i])
			return 0;
	}
	return 1;
}

void memcpy32(unsigned long* src, unsigned long* dest, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++) {
		dest[i] = src[i];
	}
}

unsigned char memcmp32(unsigned long* a, unsigned long* b, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++) {
		if (a[i] != b[i])
			return 0;
	}
	return 1;
}
