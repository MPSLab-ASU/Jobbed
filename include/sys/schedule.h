#ifndef SYS_SCHEDULE_H
#define SYS_SCHEDULE_H

#define TQUEUE_MAX 0x100
#define STACK_SIZE 0x4000
#define TQUEUE_CNT 3
#define PRIORITIES 8
#define MAX_THREADS TQUEUE_MAX*PRIORITIES*TQUEUE_CNT

enum ThreadStatus {
	THREAD_READY  = 0,
	THREAD_MWAIT  = 1,
	THREAD_SWAIT  = 2,
};

enum EntryTypes {
	THREAD_ENTRY = 0,
	START_ENTRY = 1,
	END_ENTRY = 2,
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

struct ThreadEntry {
	struct Thread* thread;
	struct ThreadEntry* next;
	unsigned long entry_type;
};

struct ThreadQueue {
	struct ThreadEntry start;
	struct ThreadEntry end;
};

struct Scheduler {
	struct Thread* rthread;
	struct ThreadQueue ready[PRIORITIES];
	struct ThreadQueue mwait[PRIORITIES];
	struct ThreadQueue swait[PRIORITIES];
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
void sched_semaphore_resurrect(void* s);

#endif
