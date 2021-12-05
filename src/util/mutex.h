#ifndef UTIL_MUTEX_H
#define UTIL_MUTEX_H

#define NULL_PID 0
#define SCHED_PID 1

struct Mutex {
	unsigned long* addr;
	unsigned long pid;
};

unsigned char lock_mutex(struct Mutex*, unsigned long);
unsigned char release_mutex(struct Mutex*, unsigned long);

#endif
