#ifndef SYS_SCHEDULE_H
#define SYS_SCHEDULE_H

#define STACK_SIZE 0x1000
struct TaskMemory {
	unsigned long reg[16];
	unsigned char stack[STACK_SIZE];
};

struct Task {
	unsigned char priority;
	void (*task)(void);
};

#ifdef FLAT
struct Scheduler {
	struct Task** tasks;
};
#elseif LL
#include "../lib/ll.h"
struct Scheduler {
	struct LL* tasks;
};
#else
#include "../lib/q.h"
struct Scheduler {
	struct Q_base* tasks;
};
#endif

void add_fxn(void (*task)(void), unsigned char priority);
unsigned int get_task_length(void);
void execute_task(void);

#endif
