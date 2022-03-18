#ifndef UTIL_LOCK_H
#define UTIL_LOCK_H

struct Lock {
	unsigned long pid;
} __attribute__((packed, aligned(4)));

struct Semaphore {
	unsigned long value;
} __attribute__((packed, aligned(4)));

void lock(struct Lock* l);
void unlock(struct Lock* l);

#endif
