#ifndef SYS_SCHEDULE_H
#define SYS_SCHEDULE_H
#include <lib/queue.h>

#define TQUEUE_MAX 0x400
#define STACK_SIZE 0x4000
#define PRIORITIES 8
#define MAX_THREADS TQUEUE_MAX*PRIORITIES

enum ThreadStatus {
	THREAD_READY  = 0,
	THREAD_MWAIT  = 1,
	THREAD_SWAIT  = 2,
};

struct Thread {
	void* pc;
	void* sp; // Store r0-r12,lr on stack
	unsigned long sp_base;
	unsigned long cpsr;
	unsigned long pid;
	unsigned char priority;
	unsigned char preempt;
	unsigned short status;
	void* mptr;
	unsigned long offset;
	unsigned char old_priority;
	unsigned char c_reserved;
	unsigned short s_reserved;
};

struct Scheduler {
	struct Thread* rthread;
	struct Queue ready[PRIORITIES];
	struct Queue mwait[PRIORITIES];
	struct Queue swait[PRIORITIES];
	struct Queue free_threads;
};

void init_scheduler(void);
unsigned char add_thread_without_duplicate(void* pc, void* arg, unsigned char priority);
unsigned char add_thread(void* pc, void* arg, unsigned char priority);
void uart_scheduler(void);
struct Thread* next_thread(void);
extern void schedule(void);
extern void cleanup(void);
void yield(void);
void sched_mutex_yield(void* m);
void sched_semaphore_yield(void* s);
void sched_mutex_resurrect(void* m);
void sched_semaphore_resurrect(void* s, unsigned long count);

#endif
