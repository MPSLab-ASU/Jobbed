#include <globals.h>
#include <drivers/uart.h>
#include <lib/kmem.h>

void* kmalloc(unsigned int size)
{
	unsigned int sz = 1;
	while (sz < size && sz < 0x1000)
		sz *= 2;
	unsigned long offset = 0x1000*(sz/2);
	unsigned int exp = 0;
	unsigned int tmp = sz;
	while (tmp != 0) {
		exp++;
		tmp = tmp >> 1;
	}
	unsigned int i = 0;
	while (i < 0x1000) {
		if (kmem_lookup[0x1000*exp + i] == 0) {
			kmem_lookup[0x1000*exp + i] = 1;
			return (void*)kmem_begin + offset + i*sz;
		}
		i++;
	}
	return 0;
}

void* kcalloc(unsigned int size)
{
	unsigned char* ptr = kmalloc(size);
	if (ptr == 0)
		return 0;
	for(unsigned int i = 0; i < size; i++)
		ptr[i] = 0;
	return ptr;
}

void  kfree(void* ptr)
{
	if (!((unsigned long)kmem_begin <= (unsigned long)ptr && (unsigned long)ptr < (unsigned long)kmem_begin + 0x200000))
		return;
	unsigned long size = 1;
	while (!((unsigned long)kmem_begin + 0x1000*(size/2) <= (unsigned long)ptr && (unsigned long)ptr < (unsigned long)kmem_begin + 0x1000*size)) {
		size *= 2;
	}
	void* base = (void*)((unsigned long)ptr - ((unsigned long)ptr % size));
	void* block_base = kmem_begin + 0x1000*(size/2);
	unsigned int lookup_offset = (base - block_base)/size;
	unsigned int exp = 0;
	unsigned int tmp = size;
	while (tmp != 0) {
		exp++;
		tmp = tmp >> 1;
	}
	kmem_lookup[0x1000*exp + lookup_offset] = 0;
}

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
