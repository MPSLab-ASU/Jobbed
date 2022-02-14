#ifndef UTIL_LOCK_H
#define UTIL_LOCK_H

#include <util/mutex.h>

struct Lock {
	unsigned long pid;
} __attribute__((packed, aligned(4)));

struct Lock* create_lock(void);

#endif
