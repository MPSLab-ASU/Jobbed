#include <drivers/uart.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <lib/mem.h>
#include <sys/schedule.h>

void memcpyrot(unsigned char* src, struct RotBuffer* rb, unsigned int n)
{
	if (n > rb->size)
		return;
	unsigned char* base = rb->base;
	unsigned int offset = rb->woffset;
	for (unsigned int i = 0; i < n; i++) {
		if (offset >= rb->size)
			offset = 0;
		base[offset] = src[i];
		offset++;
	}
	rb->woffset = offset;
}

void memshow(unsigned char* addr, unsigned int n)
{
	unsigned char temp;
	for(unsigned int i = 0; i < n; i++) {
		temp = addr[i] >> 4;
		temp += 0x30;
		if (temp > 0x39)
			temp += 7;
		uart_char(temp);
		temp = addr[i];
		temp += 0x30;
		if (temp > 0x39)
			temp += 7;
		uart_char(temp);
		if (i+1 != n)
			uart_char(0x20);
	}
	uart_char(0x0a);
}

void memshow32(unsigned long* addr, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++) {
		uart_hex(addr[i]);
		if (i+1 != n)
			uart_char(0x20);
	}
	uart_char(0x0a);
}

void memset(unsigned char* dest, unsigned char value, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++) {
		dest[i] = value;
	}
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

void memset32(unsigned long* dest, unsigned long value, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++) {
		dest[i] = value;
	}
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


void* malloc(unsigned char size)
{
	unsigned char* mem = (unsigned char*)rpi_heap;
	unsigned long i = 0;
	// TODO: Use Null PID
	while (((void*)(mem + i) < rpi_heap_top) && !(mem[i + MEM_SIZE_OFFSET] == size && mem[i + MEM_USE_OFFSET]==0)) {
		i += mem[i] + MEM_META_SIZE;
	}
	if (mem + i >= (unsigned char*)(rpi_heap_top + MAX_MM))
		return NULL;
	// Update top of heap
	if (mem[i] == 0)
		rpi_heap_top = (void*)&mem[i + MEM_META_SIZE + size];
	mem[i] = size;
	mem[i + MEM_BASE_SIZE + size] = size;
	// Use allocator's PID
	mem[i + MEM_USE_OFFSET] = 1;
	return (void*)&mem[i + MEM_BASE_SIZE];
}

void* calloc(unsigned char size)
{
	void* addr = malloc(size);
	if (addr != NULL) {
		memset(addr, 0, size);
		return addr;
	}
	return NULL;
}
void* realloc(void* old, unsigned char size)
{
	unsigned char osize = *(unsigned char*)(old - MEM_BASE_SIZE);
	if (size <= osize)
		return old;
	void* new = malloc(size);
	if (new != NULL) {
		free(old);
		return new;
	}
	return NULL;
}

void* malloca(unsigned char size, unsigned char amnt)
{
	// Return malloc if alignment size is 0 or 1 - trivial alignment
	if(amnt == 0 || amnt == 1)
		return malloc(size);
	unsigned char* mem = (unsigned char*)rpi_heap;
	unsigned long i = 0;
	// TODO: Use Null PID
	while(mem + i < rpi_heap + MAX_MM) {
		unsigned long diff = (unsigned long)mem + i + MEM_BASE_SIZE;
		diff %= amnt;
		diff = amnt - diff;
		diff %= amnt;
		if((mem[i + MEM_SIZE_OFFSET] == size) && mem[i + MEM_USE_OFFSET]==0) {
			if(diff == 0) {
				mem[i + MEM_SIZE_OFFSET] = size;
				mem[i + MEM_BASE_SIZE + size] = size;
				mem[i + MEM_USE_OFFSET] = 1;
				return (void*)&mem[i + MEM_BASE_SIZE];
			}
		} else if (mem[i] == 0) {
			if(diff == 0) {
				mem[i + MEM_SIZE_OFFSET] = size;
				mem[i + MEM_BASE_SIZE + size] = size;
				mem[i + MEM_USE_OFFSET] = 1;
				rpi_heap_top = (void*)&mem[i + MEM_META_SIZE + size];
				return (void*)&mem[i + MEM_BASE_SIZE];
			} else {
				while (diff <= MEM_BASE_SIZE) {
					diff += amnt;
				}
				unsigned long empty_size = diff - MEM_META_SIZE;
				if(empty_size == 0) {
					empty_size += amnt;
					diff += amnt;
				}
				mem[i + MEM_SIZE_OFFSET] = empty_size;
				mem[i + MEM_BASE_SIZE + empty_size] = empty_size;
				i += diff;
				mem[i + MEM_SIZE_OFFSET] = size;
				mem[i + MEM_BASE_SIZE + size] = size;
				mem[i + MEM_USE_OFFSET] = 1;
				rpi_heap_top = (void*)&mem[i + MEM_META_SIZE + size];
				return (void*)&mem[i + MEM_BASE_SIZE];
			}
		}

		i += mem[i] + MEM_META_SIZE;
	}
	return NULL;
}

void* calloca(unsigned char size, unsigned char amnt)
{
	void* addr = malloca(size, amnt);
	if (addr != NULL) {
		memset(addr, 0, size);
		return addr;
	}
	return NULL;
}

void* realloca(void* old, unsigned char size, unsigned char amnt)
{
	unsigned char osize = *(unsigned char*)(old - MEM_BASE_SIZE);
	if (size <= osize)
		return old;
	void* new = malloca(size, amnt);
	if (new != NULL) {
		free(old);
		return new;
	}
	return NULL;
}

void free(void* memloc)
{
	// Don't try to free memory outside of heap
	if(!(((void*)rpi_heap <= memloc) && (memloc < rpi_heap_top)))
		return;
	unsigned char* base = memloc - MEM_BASE_SIZE;
	unsigned char size = base[MEM_SIZE_OFFSET];
	// TODO: Use Null PID
	base[MEM_USE_OFFSET] = 0;
	// Clear out old memory
	for(unsigned int i = 0; i < size; i++) {
		base[i + MEM_BASE_SIZE] = 0;
	}
	// If it is the last entry, clear it and move the heap top down
	if (base + size + MEM_META_SIZE == rpi_heap_top) {
		while(base[MEM_USE_OFFSET] == 0 && base >= rpi_heap) {
			base[MEM_SIZE_OFFSET] = 0;
			base[MEM_BASE_SIZE + size] = 0;
			rpi_heap_top = base;
			unsigned long psize = *(base - 1);
			base -= psize + MEM_META_SIZE;
		}
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

void vheap_info(void)
{
	unsigned char* base = rpi_heap;
	unsigned long ioff = STACK_DRAW_WIDTH*STACK_DRAW_SIZE;
	unsigned long yoff = STACK_DRAW_YOFF;
	draw_cbox(ioff, yoff, STACK_DRAW_SIZE*STACK_DRAW_WIDTH, STACK_DRAW_SIZE * 8, 0x0);
	while ((void*)base < rpi_heap_top) {
		unsigned char size = base[MEM_SIZE_OFFSET];
		if(base[MEM_USE_OFFSET] == 0) {
			draw_cbox(ioff, yoff, STACK_DRAW_SIZE, STACK_DRAW_SIZE, 0x00FF00);
		} else {
			draw_cbox(ioff, yoff, STACK_DRAW_SIZE, STACK_DRAW_SIZE, 0xFF0000);
		}
		ioff += STACK_DRAW_SIZE;
		if(ioff % STACK_DRAW_WIDTH == 0) {
			yoff += STACK_DRAW_SIZE;
			ioff = STACK_DRAW_WIDTH*STACK_DRAW_SIZE;
		}
		base += size + MEM_META_SIZE;
	}
}

void heap_info(void)
{
	unsigned char* base = rpi_heap;
	while ((void*)base < rpi_heap_top) {
		unsigned char size = base[MEM_SIZE_OFFSET];
		if(base[MEM_USE_OFFSET] == 0) {
			uart_char('F');
			uart_char(' ');
		} else {
		}
		uart_hex((unsigned long)(base + MEM_BASE_SIZE));
		uart_string(" Size: ");
		uart_10(size);
		uart_string("\n");
		static char* data = "00 \0";
		static unsigned char temp = 0;
		for(unsigned int i = 0; i < size; i++) {
			temp = (base[MEM_BASE_SIZE + i]>>4)&0xF;
			if(temp > 9)
				temp += 7;
			temp += 0x30;
			data[0] = temp;
			temp = (base[MEM_BASE_SIZE + i])&0xF;
			if(temp > 9)
				temp += 7;
			temp += 0x30;
			data[1] = temp;
			uart_string(data);
		}
		uart_char('\n');
		base += size + MEM_META_SIZE;
	}
	uart_char('\n');
}

void heap_info_u(void)
{
	unsigned char* base = rpi_heap;
	while ((void*)base < rpi_heap_top) {
		unsigned char size = base[MEM_SIZE_OFFSET];
		if(base[MEM_USE_OFFSET] == 0) {
			base += size + MEM_META_SIZE;
			continue;
		}
		uart_hex((unsigned long)(base + MEM_BASE_SIZE));
		uart_string(" Size: ");
		uart_10(size);
		uart_string("\n");
		static char* data = "00 \0";
		static unsigned char temp = 0;
		for(unsigned int i = 0; i < size; i++) {
			temp = (base[MEM_BASE_SIZE + i]>>4)&0xF;
			if(temp > 9)
				temp += 7;
			temp += 0x30;
			data[0] = temp;
			temp = (base[MEM_BASE_SIZE + i])&0xF;
			if(temp > 9)
				temp += 7;
			temp += 0x30;
			data[1] = temp;
			uart_string(data);
		}
		uart_char('\n');
		base += size + MEM_META_SIZE;
	}
	uart_char('\n');
}
