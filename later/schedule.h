#ifndef SYS_SCHEDULE_H
#define SYS_SCHEDULE_H

#define S_WAITING 0
#define S_READY 1

#define STACK_SIZE 0x800
struct Task {
	unsigned char priority;
	unsigned char status;
	unsigned long pid;
	void (*task)(void);
	unsigned long reg[16];
	unsigned long stacki;
	void* stack;
};

#ifdef FLAT
struct Scheduler {
	unsigned long running_pid;
	struct Task** tasks;
};
#elseif LL
#include "../lib/ll.h"
struct Scheduler {
	unsigned long running_pid;
	struct LL* tasks;
};
#else
#include "../lib/q.h"
struct Scheduler {
	unsigned long running_pid;
	struct Q_base* tasks;
};
#endif

void add_fxn(void (*task)(void), unsigned char priority);
unsigned char exists(void (*task)(void));
void add_no_repeat(void (*task)(void), unsigned char priority);
unsigned int get_task_length(void);
void execute_task(void);

#endif
