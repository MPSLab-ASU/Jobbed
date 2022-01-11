#include <cpu/irq.h>
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
	.ctx = 0,
};
unsigned long syssp = 0;
struct cpu_context syscpu = {
	.r4 = 0, .r5 = 0, .r6 = 0, .r7 = 0,
	.r8 = 0, .r9 = 0, .r10 = 0, .r11 = 0,
	.r12 = 0, .lr = 0,
};

void init_scheduler(void)
{
	for(int i = 0; i < PRIORITIES; i++) {
		scheduler.tlist[i].prev = &scheduler.tlist[i];
		scheduler.tlist[i].next = &scheduler.tlist[i];
		scheduler.tlist[i].data = 0;
	}
	scheduler.rthread_ll = 0;
	scheduler.ctx = &syscpu;
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
	struct Thread* thread = (struct Thread*)malloca(sizeof(struct Thread), 4);
	// Set the program counter to the entry
	thread->thread = thread_fxn;
	// Get a stack frame
	thread->stack_base = get_stack();
	thread->stack = thread->stack_base;
	// Put in error state for no stack
	if(thread->stack == 0)
		thread->data.status = THREAD_STACK_ERROR;
	else
		thread->data.status = THREAD_READY;
	// Doesn't wait for mutex at start
	thread->data.mutex_waiting = 0;
	// Set PID
	thread->data.pid = nextpid++;
	thread->data.preempt_count = 0;
	thread->data.cpu_context.lr = (unsigned long)cleanup;
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

void schedule_c(void)
{
	// Preserve registers in current context
	preserve_ctx(scheduler.ctx);

	// Get current thread
	struct LL* current_thread_ll = scheduler.rthread_ll;
	// Get next thread
	struct LL* next_thread_ll = get_next_thread();

	// If there is a current thread
	if (current_thread_ll != 0) {
		// If we are switching the thread
		if (current_thread_ll != next_thread_ll) {
			// Context switch
			struct Thread* current_thread = current_thread_ll->data;
			struct Thread* next_thread = next_thread_ll->data;
			preserve_stack(current_thread);
			//preserve_pc(current_thread);
			current_thread->thread = (void*)current_thread->data.cpu_context.lr;
			restore_stack(next_thread);
			scheduler.rthread_ll = next_thread_ll;
			scheduler.ctx = &next_thread->data.cpu_context;
		}
	}
	else if (next_thread_ll != 0) {
		struct Thread* next_thread = next_thread_ll->data;
		preserve_sys_stack(&syssp);
		restore_stack(next_thread);
		scheduler.rthread_ll = next_thread_ll;
		scheduler.ctx = &next_thread->data.cpu_context;
	}
	if (scheduler.rthread_ll) {
		struct Thread* rthread = scheduler.rthread_ll->data;
		restore_ctx(scheduler.ctx);
		asm volatile ("bx %0" :: "r"(rthread->thread));
	} else {
		scheduler.ctx = &syscpu;
		restore_sys_stack(&syssp);
		restore_ctx(scheduler.ctx);
	}
}

void cleanup(void)
{
	if (scheduler.rthread_ll != 0) {
		// Mark the thread as finished
		struct Thread* t = scheduler.rthread_ll->data;
		//uart_string("Cleaning up thread ");
		//uart_10(t->data.pid);
		//uart_char('\n');
		t->data.status = THREAD_FINISHED;
		// Mark the stack space as free
		unsigned long sidx = (unsigned long)(heap_end() - t->stack_base)/STACK_SIZE;
		stacks_table[sidx] = 0;
		// Remove the thread
		struct LL* ll = scheduler.rthread_ll;
		struct LL* prev = ll->prev;
		struct LL* next = ll->next;
		prev->next = ll->next;
		next->prev = ll->prev;
		free(ll->data);
		free(ll);
		scheduler.rthread_ll = 0;
	}
	// Schedule next thread
	schedule();
}

void sched_info(void)
{
	uart_string("Scheduler Information\n");
	for(unsigned long i = 0; i < PRIORITIES; i++) {
		struct LL* ll = scheduler.tlist[i].next;
		uart_string("Queue ");
		uart_10(i);
		while (ll != &scheduler.tlist[i]) {
			uart_string("\nThread ");
			struct Thread* t = ll->data;
			uart_hex((unsigned long)t->thread);uart_char(' ');
			uart_hex((unsigned long)t->stack);uart_char(' ');
			uart_hex((unsigned long)t->stack_base);uart_char(' ');
			uart_10(t->data.priority);uart_char(' ');
			uart_10(t->data.preempt_count);uart_char(' ');
			uart_10(t->data.status);uart_char(' ');
			uart_hex((unsigned long)t->data.mutex_waiting);uart_char(' ');
			uart_10(t->data.pid);uart_char('\n');
			memshow32((unsigned long*)&t->data.cpu_context, 10);
			ll = ll->next;
		}
		uart_char('\n');
	}
	uart_string("Stacks:\n");
	memshow32((unsigned long*)stacks_table, 6);
}
