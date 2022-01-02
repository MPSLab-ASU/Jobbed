#ifndef LIB_MEM_H
#define LIB_MEM_H

void memcpy(unsigned char* src, unsigned char* dest, unsigned int n);
unsigned char memcmp(unsigned char* a, unsigned char* b, unsigned int n);

void memshow32(unsigned long* addr, unsigned int n);
void memcpy32(unsigned long* src, unsigned long* dest, unsigned int n);
unsigned char memcmp32(unsigned long* a, unsigned long* b, unsigned int n);

void* malloc(unsigned char size);
void free(void* memloc);
void* heap_base(void);
void* heap_top(void);
void heap_info(void);

#endif
