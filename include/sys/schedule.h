#ifndef SYS_SCHEDULE_H
#define SYS_SCHEDULE_H

#define TQUEUE_MAX 0x100
#define STACK_SIZE 0x4000
#define TQUEUE_CNT 5
#define PRIORITIES 6
#define MAX_THREADS TQUEUE_MAX*PRIORITIES*TQUEUE_CNT
#define STACK_DRAW_YOFF 320
#define STACK_DRAW_WIDTH 32
#define STACK_DRAW_SIZE 3

enum ThreadStatus {
	THREAD_READY  = 0,
	THREAD_MWAIT  = 1,
	THREAD_SWAIT  = 2,
	THREAD_FINISH = 3, // Need to clean up
	THREAD_SERROR = 4, // Stack Error
};

struct RStack {
	void* sp;
	unsigned long idx;
};

struct Thread {
	void* pc;
	void* sp; // Store r0-r12,lr on stack
	unsigned long sp_base;
	void* mptr;
	unsigned long pid;
	unsigned char priority;
	unsigned char preempt;
	unsigned short status;
	unsigned long cpsr;
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
	struct ThreadRotBuffer finish;
	struct ThreadRotBuffer serror;
};

struct Scheduler {
	struct Thread* rthread;
	struct ThreadQueues thread_queues[PRIORITIES];
};


void init_scheduler(void);
void add_thread(void* pc, void* arg, unsigned char priority);
void draw_stacks(void);
void uart_scheduler(void);
struct Thread* next_thread(void);
/// TODO: ENSURE IRQ/ FIQ entry switches 
///  to user mode then calls the SVC call
extern void schedule(void);
extern void cleanup(void);
void yield(void);

#endif
