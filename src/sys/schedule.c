#include <drivers/uart.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <util/mutex.h>

#define SYS_SCHEDULE_C
struct Scheduler scheduler = {
	.tlist = {
		{.prev = 0, .next = 0, .data = 0},
		{.prev = 0, .next = 0, .data = 0},
		{.prev = 0, .next = 0, .data = 0},
		{.prev = 0, .next = 0, .data = 0},
		{.prev = 0, .next = 0, .data = 0},
		{.prev = 0, .next = 0, .data = 0},
	},
	.rthread_ll = 0,
};

void init_scheduler(void)
{
	for(int i = 0; i < PRIORITIES; i++) {
		scheduler.tlist[i].prev = &scheduler.tlist[i];
		scheduler.tlist[i].next = &scheduler.tlist[i];
		scheduler.tlist[i].data = 0;
	}
	scheduler.rthread_ll = 0;
}

unsigned char stacks_table[MAX_THREADS] = {0, };

void* get_stack(void)
{
	for (int i = 0; i < MAX_THREADS; i++) {
		if (stacks_table[i] == 0) {
			stacks_table[i] = 1;
			return (void*)heap_end() - STACK_SIZE*i;
		}
	}
	return 0;
}

static unsigned long nextpid = 3;
void add_thread(void (*thread_fxn)(void), unsigned char priority)
{
	struct Thread* thread = (struct Thread*)malloc(sizeof(struct Thread));
	// Set the program counter to the entry
	thread->thread = thread_fxn;
	// Get a stack frame
	thread->stack = get_stack();
	thread->stack_base = thread->stack;
	// Put in error state for no stack
	if(thread->stack == 0)
		thread->data.status = THREAD_STACK_ERROR;
	else
		thread->data.status = THREAD_READY;
	// Doesn't wait for mutex at start
	thread->data.mutex_waiting = 0;
	// Set PID
	thread->data.pid = nextpid++;
	unsigned char p = priority;
	if (p >= PRIORITIES) {
		p = PRIORITIES - 1;
	}
	thread->data.priority = p;
	push_ll(&scheduler.tlist[p], thread);
}

struct LL* get_next_thread(void)
{
	for(unsigned long i = 0; i < PRIORITIES; i++) {
		struct LL* thread_ll = scheduler.tlist[i].next;
		if (thread_ll == &scheduler.tlist[i])
			continue;
		do {
			struct Thread* thread = thread_ll->data;
			if((thread->data.status == THREAD_RUNNING) || (thread->data.status == THREAD_READY))
				return thread_ll;
			thread_ll = thread_ll->next;
		} while(thread_ll != &scheduler.tlist[i]);
	}
	return 0;
}

unsigned long syssp = 0;
void schedule(void)
{
	// Preserve current process's registers
	//  in the current stack
	asm volatile ("push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}");
	// Get current thread
	struct LL* current_thread_ll = scheduler.rthread_ll;
	// Get next thread
	struct LL* next_thread_ll = get_next_thread();

	// If there is a current thread
	if (current_thread_ll) {
		// If we are switching the thread
		if (current_thread_ll != next_thread_ll) {
			// Context switch
			struct Thread* current_thread = current_thread_ll->data;
			struct Thread* next_thread = next_thread_ll->data;
			preservestack(current_thread);
			preservepc(current_thread);
			restorestack(next_thread);
			//restoreregs(next_thread);
			scheduler.rthread_ll = next_thread_ll;
		}
	}
	else if (next_thread_ll) {
		struct Thread* next_thread = next_thread_ll->data;
		preservesysstack(&syssp);
		//preservesysregs(&regloc)
		restorestack(next_thread);
		//restoreregs(next_thread);
		scheduler.rthread_ll = next_thread_ll;
	}
	if (scheduler.rthread_ll) {
		struct Thread* rthread = scheduler.rthread_ll->data;
		// Restore process's registers
		asm volatile ("pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}");
		// Run the thread - i.e. jump to the pc
		asm volatile ("blx lr");
		//rthread->thread();
		// Remove the currently running thread after completion
		remove_running_thread();
		// Schedule the next thread
		schedule();
	} else {
		//restoresysregs(&regloc);
		restoresysstack(&syssp);
	}
}

void remove_running_thread(void)
{
	if (scheduler.rthread_ll != 0) {
		struct LL* ll = scheduler.rthread_ll;
		if ((ll->next == ll->prev) && (ll->next == ll)) {
			ll->data = 0;
		}
		else {
			struct LL* prev = ll->prev;
			struct LL* next = ll->next;
			prev->next = ll->next;
			next->prev = ll->prev;
			free(ll);
		}
		scheduler.rthread_ll = 0;
	}
}
