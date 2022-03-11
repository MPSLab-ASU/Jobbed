#ifndef SYS_SCHEDULE_H
#define SYS_SCHEDULE_H

// If TQUEUE_MAX is changed, ensure sys/schedule.S's value gets changed
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
	struct ThreadEntry* prev;
	struct ThreadEntry* next;
};

struct ThreadEntryIterator {
	struct ThreadEntry* entry;
};

struct ThreadQueue {
	struct ThreadEntry entry[TQUEUE_MAX];
	struct ThreadEntryIterator read;
	struct ThreadEntryIterator write;
};

struct Scheduler {
	struct Thread* rthread;
	struct ThreadQueue ready[PRIORITIES];
	struct ThreadQueue mwait[PRIORITIES];
	struct ThreadQueue swait[PRIORITIES];
};

void init_scheduler(void);
void add_thread(void* pc, void* arg, unsigned char priority);
void uart_scheduler(void);
struct Thread* next_thread(void);
extern void schedule(void);
extern void cleanup(void);
void yield(void);
void sched_mutex_yield(void* m);
void sched_mutex_resurrect(void* m);

#endif
