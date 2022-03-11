#ifndef UTIL_LOCK_H
#define UTIL_LOCK_H

#include <util/mutex.h>

struct Lock {
	unsigned long pid;
} __attribute__((packed, aligned(4)));

void lock(struct Lock* l);
void unlock(struct Lock* l);

#endif
