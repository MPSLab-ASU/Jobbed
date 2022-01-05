#include "../lib/mem.h"
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

#define MAX_MM 0x100000
static unsigned char rpi_heap[MAX_MM] = {0,};
static void* rpi_heap_top = &rpi_heap;

void* malloc(unsigned char size)
{
	unsigned char* mem = (unsigned char*)rpi_heap;
	unsigned long i = 0;
	// TODO: Use Null PID
	while (((void*)(mem+i) < rpi_heap_top) && !(mem[i] == size && mem[i+1]==0)) {
		i += mem[i]+2;
	}
	// Update top of heap
	if (mem[i] == 0)
		rpi_heap_top = (void*)&mem[i+2+size];
	mem[i] = size;
	// Use allocator's PID
	mem[i+1] = 1;
	return (void*)&mem[i+2];
}

void* malloca(unsigned char size, unsigned char amnt)
{
	unsigned char* mem = (unsigned char*)rpi_heap;
	unsigned long i = 0;
	// TODO: Use Null PID
	while(1) {
		unsigned long diff = (unsigned long)mem + i + 2;
		diff %= amnt;
		diff = amnt - diff;
		diff %= amnt;
		if((mem[i] == size) && mem[i+1]==0) {
			if(diff == 0) {
				mem[i] = size;
				mem[i+1] = 1;
				return (void*)&mem[i+2];
			}
		} else if (mem[i] == 0) {
			if(diff == 0 || diff == amnt) {
				mem[i] = size;
				mem[i+1] = 1;
				rpi_heap_top = (void*)&mem[i+2+size];
				return (void*)&mem[i+2];
			} else {
				if(diff <= 2) {
					diff += amnt;
				}
				mem[i] = diff-2;
				i += diff;
				mem[i] = size;
				mem[i+1] = 1;
				rpi_heap_top = (void*)&mem[i+2+size];
				return (void*)&mem[i+2];
			}
		}

		i += mem[i]+2;
	}
}

void free(void* memloc)
{
	// Don't try to free memory outside of heap
	if(!(((void*)rpi_heap <= memloc) && (memloc < rpi_heap_top)))
		return;
	unsigned char* base = memloc - 2;
	unsigned char size = *base;
	// TODO: Use Null PID
	base[1] = 0;
	// Clear out old memory
	for(unsigned int i = 0; i < size; i++) {
		base[i+2] = 0;
	}
	// If it is the last entry, clear it and move the heap top down
	if (base + size + 2 == rpi_heap_top) {
		base[0] = 0;
		rpi_heap_top = base;
	}
}

void* heap_base(void)
{
	return (void*)rpi_heap;
}

void* heap_top(void)
{
	return rpi_heap_top;
}

void heap_info(void)
{
	unsigned char* base = rpi_heap;
	while ((void*)base < rpi_heap_top) {
		unsigned char size = *base;
		if(base[1] == 0) {
			uart_char('F');
			uart_char(' ');
		}
		uart_hex((unsigned long)(base+2));
		uart_string(" Size: ");
		uart_10(size);
		uart_string("\n");
		static char* data = "00 \0";
		static unsigned char temp = 0;
		for(unsigned int i = 0; i < size; i++) {
			temp = (base[2+i]>>4)&0xF;
			if(temp > 9)
				temp += 7;
			temp += 0x30;
			data[0] = temp;
			temp = (base[2+i])&0xF;
			if(temp > 9)
				temp += 7;
			temp += 0x30;
			data[1] = temp;
			uart_string(data);
		}
		uart_char('\n');
		base += size + 2;
	}
	uart_char('\n');
}

void heap_info_u(void)
{
	unsigned char* base = rpi_heap;
	while ((void*)base < rpi_heap_top) {
		unsigned char size = *base;
		if(base[1] == 0) {
			base += size + 2;
			continue;
		}
		uart_hex((unsigned long)(base+2));
		uart_string(" Size: ");
		uart_10(size);
		uart_string("\n");
		static char* data = "00 \0";
		static unsigned char temp = 0;
		for(unsigned int i = 0; i < size; i++) {
			temp = (base[2+i]>>4)&0xF;
			if(temp > 9)
				temp += 7;
			temp += 0x30;
			data[0] = temp;
			temp = (base[2+i])&0xF;
			if(temp > 9)
				temp += 7;
			temp += 0x30;
			data[1] = temp;
			uart_string(data);
		}
		uart_char('\n');
		base += size + 2;
	}
	uart_char('\n');
}
