#include <drivers/uart.h>
#include <lib/kmem.h>
#include <lib/strings.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <symbols.h>
#include <util/lock.h>

#define UART_BUFFER_SIZE 0x400
struct UartBuffer {
	char buffer[UART_BUFFER_SIZE];
	unsigned long roffset;
	unsigned long woffset;
	struct Lock l;
} ubuffer;

void uart_init(void)
{
	ubuffer.roffset = 0;
	ubuffer.woffset = 0;
	ubuffer.l.pid = 0;

	// Disable UART0
	store32(0x0, UART0_CR);
	// Setup GPIO on pin 14 and 15
	store32(0x0, (unsigned long)GPPUD);
	delay(150);
	store32((1 << 14) | (1 << 15), (unsigned long)GPPUDCLK0);
	delay(150);
	store32(0x0, (unsigned long)GPPUDCLK0);
	// Clear pending interrupts
	store32(0x7FF, UART0_ICR);
	// Set to 3Mhz
	store32(9, UART0_IBRD);
	store32(49, UART0_FBRD);
	// Enable FIFO and 8 bit transmission
	store32((1<<4)|(1<<5)|(1<<6), UART0_LCRH);
	// Mask all interrupts
	store32((1<<1)|(1<<4)|(1<<5)|(1<<6)|(1<<7)|(1<<8)|(1<<9)|(1<<10), UART0_IMSC);
	// Enable UART0
	store32((1<<0)|(1<<8)|(1<<9), UART0_CR);
}

// s = zero-terminated string
void* uart_print(char* s)
{
	lock(&ubuffer.l);
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
	// Low priority flush run whenever
	add_thread_without_duplicate(uart_flush, 0, PRIORITIES-1);
	unlock(&ubuffer.l);
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
	char* dptr = u32_to_str(val);
	uart_string(dptr);
}

void uart_hexn(unsigned long c_val)
{
	uart_hex(c_val);
	uart_char('\n');
}
