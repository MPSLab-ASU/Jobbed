#ifndef LIB_MEM_H
#define LIB_MEM_H

#define MEM_SIZE_OFFSET 0
#define MEM_USE_OFFSET 1
#define MEM_BASE_SIZE 2
#define MEM_META_SIZE 3

struct MemTab {
	unsigned char size;
	unsigned char in_use;
	void* data;
} __attribute__((packed));

struct RotBuffer {
	void* base;
	unsigned int size;
	unsigned int roffset;
	unsigned int woffset;
};

void memset(unsigned char* dest, unsigned char value, unsigned int n);
void memcpy(unsigned char* src, unsigned char* dest, unsigned int n);
unsigned char memcmp(unsigned char* a, unsigned char* b, unsigned int n);
void memcpyrot(unsigned char* src, struct RotBuffer* rb, unsigned int n);

void memshow32(unsigned long* addr, unsigned int n);
void memset32(unsigned long* dest, unsigned long value, unsigned int n);
void memcpy32(unsigned long* src, unsigned long* dest, unsigned int n);
unsigned char memcmp32(unsigned long* a, unsigned long* b, unsigned int n);

void* malloc(unsigned char size);
void* calloc(unsigned char size);
void* malloca(unsigned char size, unsigned char amnt);
void* calloca(unsigned char size, unsigned char amnt);
void free(void* memloc);
void* heap_base(void);
void* heap_top(void);
void heap_info(void);
void heap_info_u(void);

#endif
