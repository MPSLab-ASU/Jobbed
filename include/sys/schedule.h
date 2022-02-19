#ifndef SYS_SCHEDULE_H
#define SYS_SCHEDULE_H

#define TQUEUE_MAX 0x100
#define STACK_SIZE 0x4000
#define TQUEUE_CNT 4
#define PRIORITIES 8
#define MAX_THREADS TQUEUE_MAX*PRIORITIES*TQUEUE_CNT
#define STACK_DRAW_YOFF 320
#define STACK_DRAW_WIDTH 32
#define STACK_DRAW_SIZE 3

enum ThreadStatus {
	THREAD_READY  = 0,
	THREAD_MWAIT  = 1,
	THREAD_SWAIT  = 2,
	THREAD_SERROR = 3, // Stack Error
	THREAD_FINISH = 4, // Need to clean up
};

struct RStack {
	void* sp;
	unsigned long idx;
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
	unsigned char old_priority;
};

struct ThreadRotBuffer {
	unsigned int roffset;
	unsigned int woffset;
	struct Thread* queue[TQUEUE_MAX];
};

struct ThreadQueues {
	struct ThreadRotBuffer ready;
	struct ThreadRotBuffer mwait;
	struct ThreadRotBuffer swait;
	struct ThreadRotBuffer serror;
};

struct Scheduler {
	struct Thread* rthread;
	struct ThreadQueues thread_queues[PRIORITIES];
};


void init_scheduler(void);
void add_thread(void* pc, void* arg, unsigned char priority);
void uart_scheduler(void);
struct Thread* next_thread(void);
/// TODO: ENSURE IRQ/ FIQ entry switches
///  to user mode then calls the SVC call
extern void schedule(void);
extern void cleanup(void);
void yield(void);
void sched_mutex_yield(void* m);
void sched_mutex_resurrect(void* m);

#endif
