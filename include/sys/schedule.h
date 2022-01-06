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

struct ThreadData {
	unsigned short status;
	void* mutex_waiting;
	unsigned long pid;
	unsigned char priority;
};

struct Thread {
	struct ThreadData data;
	void (*thread)(void);
	void* stack;
	void* stack_base;
};

#define MAX_THREADS 0x100
#define STACK_SIZE 0x1000
#define PRIORITIES 6
struct Scheduler {
	struct LL tlist[PRIORITIES];
	struct LL* rthread_ll;
};

#ifndef SYS_SCHEDULE_C
#define SYS_SCHEDULE_C
extern struct Scheduler scheduler;
#endif

void init_scheduler(void);
void add_thread(void (*thread_fxn)(void), unsigned char priority);
void schedule(void);
void remove_running_thread(void);

static inline void preserveregs(struct Thread* thread)
{
	// Preserve current stack pointer
	void* sp = getsp();
	// Get current mode
	unsigned long mode = getmode();
	// Set supervisor mode - "User mode"
	setsvc();
	void* ssp = getsp();
	// Move stack to reserved register area
	setsp(thread->stack_base - 0x1000 + 16*4);
	// Push registers to the stack
	asm volatile ("push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}");
	// Restore stack to previous
	setsp(ssp);
	// Restore mode
	setmode(mode);
	// Restore current stack pointer
	setsp(sp);
}

static inline void preservestack(struct Thread* thread)
{
	// Get current mode
	unsigned long mode = getmode();
	// Set supervisor mode - "User mode"
	setsvc();
	// Store the stack pointer
	void* ssp = getsp();
	thread->stack = ssp;
	// Restore mode
	setmode(mode);
}

static inline void restoreregs(struct Thread* thread)
{
	// Preserve current stack pointer
	void* sp = getsp();
	// Get current mode
	unsigned long mode = getmode();
	// Set supervisor mode - "User mode"
	setsvc();
	void* ssp = getsp();
	// Move stack to reserved register area
	setsp(thread->stack_base - 0x1000 + 16*4 - 14*4);
	// Restore registers on the stack
	asm volatile ("pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}");
	// Restore stack to previous
	setsp(ssp);
	// Restore mode
	setmode(mode);
	// Restore current stack pointer
	setsp(sp);
}

static inline void restorestack(struct Thread* thread)
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

static inline void preservesysstack(unsigned long* sp)
{
	if (*sp == 0) {
		unsigned long mode = getmode();
		setsvc();
		*sp = (unsigned long)getsp();
		setmode(mode);
	}
}

static inline void restoresysstack(unsigned long* sp)
{
	if (*sp) {
		unsigned long mode = getmode();
		setsvc();
		setsp((void*)*sp);
		setmode(mode);
		*sp = 0;
	}
}

static inline void preservepc(struct Thread* t)
{
	asm volatile ("mov %0, lr" : "=r"(t->thread));
}

#endif
