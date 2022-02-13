#ifndef UTIL_MUTEX_H
#define UTIL_MUTEX_H

#define NULL_PID 0
#define CORE0_PID 1
#define CORE1_PID 2
#define CORE2_PID 3
#define CORE3_PID 4
#define FIRST_AVAIL_PID CORE3_PID+1

struct Mutex {
	void* addr;
	unsigned long pid;
} __attribute__((packed, aligned(4)));;

struct Mutex* create_mutex(void* addr);

#endif
