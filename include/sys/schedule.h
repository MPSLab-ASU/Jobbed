#ifndef SYS_SCHEDULE_H
#define SYS_SCHEDULE_H
#include <cpu.h>
#include <lib/mem.h>
#include <lib/ll.h>
#include <sys/core.h>

enum ThreadStatus {
	THREAD_READY,
	THREAD_WAITING,
	THREAD_WAITING_FOR_MUTEX,
	THREAD_STACK_ERROR,
	THREAD_RUNNING,
	THREAD_FINISHED,
};

struct cpu_context {
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	unsigned long r7;
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	unsigned long lr;
};

struct ThreadData {
	unsigned char priority;
	unsigned char preempt_count;
	unsigned short status;
	void* mutex_waiting;
	unsigned long pid;
	struct cpu_context cpu_context;
};

struct Thread {
	//void (*thread)(void);
	void* thread;
	void* stack;
	void* stack_base;
	struct ThreadData data;
};

#define MAX_THREADS 0x100
#define STACK_SIZE 0x1000
#define PRIORITIES 6
struct Scheduler {
	struct LL* rthread_ll;
	struct cpu_context* ctx;
	struct LL tlist[PRIORITIES];
};

#ifndef SYS_SCHEDULE_C
#define SYS_SCHEDULE_C
extern struct Scheduler scheduler;
#endif

void init_scheduler(void);
void add_thread(void (*thread_fxn)(void), unsigned char priority);
extern void schedule(void);
void schedule_c(void);
void schedule_irq(void);
void cleanup(void);
void sched_info(void);
void yield(void);

static inline void preserve_stack(struct Thread* thread)
{
	// Get current mode
	unsigned long mode = getmode();
	// Set supervisor mode - "User mode"
	setsvc();
	// Store the stack pointer
	void* ssp = getsp() + 4*4; // Ignore 4 words pushed on by (schedule)
	thread->stack = ssp;
	// Restore mode
	setmode(mode);
}

static inline void restore_stack(struct Thread* thread)
{
	// Get current mode
	unsigned long mode = getmode();
	// Set supervisor mode - "User mode"
	setsvc();
	// Set stack pointer to thread's stack pointer
	asm volatile("mov sp, %0" :: "r"(thread->stack));
	// Restore mode
	setmode(mode);
}

static inline void preserve_sys_stack(unsigned long* sp)
{
	if (*sp == 0) {
		unsigned long mode = getmode();
		setsvc();
		*sp = (unsigned long)getsp();
		setmode(mode);
	}
}

static inline void restore_sys_stack(unsigned long* sp)
{
	if (*sp) {
		unsigned long mode = getmode();
		setsvc();
		setsp((void*)*sp);
		setmode(mode);
		*sp = 0;
	}
}

static inline void preserve_pc(struct Thread* t)
{
	t->thread = (void*)t->data.cpu_context.lr;
}

static inline void preserve_ctx(struct cpu_context* cpuctx)
{
	asm volatile ("mov %0, r4" : "=r"(cpuctx->r4));
	asm volatile ("mov %0, r5" : "=r"(cpuctx->r5));
	asm volatile ("mov %0, r6" : "=r"(cpuctx->r6));
	asm volatile ("mov %0, r7" : "=r"(cpuctx->r7));
	asm volatile ("mov %0, r8" : "=r"(cpuctx->r8));
	asm volatile ("mov %0, r9" : "=r"(cpuctx->r9));
	asm volatile ("mov %0, r10" : "=r"(cpuctx->r10));
	asm volatile ("mov %0, r11" : "=r"(cpuctx->r11));
	asm volatile ("mov %0, r12" : "=r"(cpuctx->r12));
	asm volatile ("mov %0, lr" : "=r"(cpuctx->lr));
}

static inline void restore_ctx(struct cpu_context* cpuctx)
{
	asm volatile ("mov r4,  %0" :: "r"(cpuctx->r4));
	asm volatile ("mov r5,  %0" :: "r"(cpuctx->r5));
	asm volatile ("mov r6,  %0" :: "r"(cpuctx->r6));
	asm volatile ("mov r7,  %0" :: "r"(cpuctx->r7));
	asm volatile ("mov r8,  %0" :: "r"(cpuctx->r8));
	asm volatile ("mov r9,  %0" :: "r"(cpuctx->r9));
	asm volatile ("mov r10, %0" :: "r"(cpuctx->r10));
	asm volatile ("mov r11, %0" :: "r"(cpuctx->r11));
	asm volatile ("mov r12, %0" :: "r"(cpuctx->r12));
	asm volatile ("mov lr,  %0" :: "r"(cpuctx->lr));
}

#endif
