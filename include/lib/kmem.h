#ifndef LIB_KMEM_H
#define LIB_KMEM_H

void* kmalloc(unsigned int size);
void* kcalloc(unsigned int size);
void  kfree(void* ptr);

void kmemshow32(void* data, unsigned long length);
void kmemshow(void* data, unsigned long length);

#endif
