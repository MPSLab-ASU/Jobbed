#if !(defined(LL) || defined(FLAT))
#include "../sys/schedule.h"
#include "../lib/q.h"
#include "../lib/mem.h"
#include "../drivers/uart.h"

extern void preserveregs(void*);
extern void restoreregs(void*);
extern void exetask(void*);

static unsigned long next_pid = 3;
unsigned char table[256] = {0, };

static struct __attribute__((packed,align(4))) Q_base bq = {
	.next = 0,
	.last = 0,
};
struct __attribute__((packed,align(4))) Scheduler scheduler = {
	.running_pid = 0,
	.tasks = &bq,
};

extern unsigned long __stacks_start;

unsigned long getstack(void)
{
	for(unsigned int i = 0; i < 256; i++) {
		if (table[i] == 0) {
			table[i] = 1;
			return i;
		}
	}
	return -1;
}

void add_fxn(void (*task)(void), unsigned char priority)
{
	struct Task* t = (struct Task*)malloc(sizeof(struct Task));
	// Allocate a stack frame and space for registers to be preserved on context switches
	t->priority = priority;
	t->task = task;
	t->stacki = getstack();
	t->pid = next_pid;
	next_pid += 1;
	if(t->stacki > 256)
		t->stack = 0;
	else
		t->stack = (void*)(__stacks_start + STACK_SIZE*t->stacki);
	t->status = S_READY;
	for(unsigned char i = 0; i < 13; i++)
		t->reg[i] = 0;
	t->reg[13] = (unsigned long)t->stack;
	t->reg[14] = 0; // LR
	t->reg[15] = (unsigned long)task; // PC
	pushq(scheduler.tasks, t);
}

unsigned char exists(void (*task)(void))
{
	if (scheduler.tasks->next == 0)
		return 0;
	struct Q* q = scheduler.tasks->next;
	while (q != 0) {
		struct Task* t = q->data;
		if (t->task == task)
			return 1;
		q = q->next;
	}
	return 0;
}

void add_no_repeat(void (*task)(void), unsigned char priority)
{
	if (!(exists(task)))
		add_fxn(task, priority);
}

unsigned int get_task_length(void)
{
	unsigned int length = 0;
	if (scheduler.tasks->last == 0)
		return length;
	else if (scheduler.tasks->next == scheduler.tasks->last)
		return 1;
	else {
		struct Q* q = scheduler.tasks->next;
		length += 1;
		while (q->next != 0) {
			q = q->next;
			length += 1;
		}
		return length;
	}
}
void execute_task(void)
{
	// Preserve Current Running PID's Data
	if (scheduler.tasks->last != 0) {
		struct Q* q = (struct Q*)scheduler.tasks->next;
		while (((struct Task*)q->data)->status == S_WAITING) {
			q = q->next;
		}
		struct Task* tsk = (struct Task*)q->data;
		// Restore registers 
		//  Including program counter as the entry point
		//  Including allocated stack position
		//  Set lr to the return address to restore system stack
		//preserveregs(tsk->reg);
		//restoreregs(tsk->reg);
		scheduler.running_pid = tsk->pid;
		exetask(tsk->reg);
		scheduler.running_pid = 0;
		table[tsk->stacki] = 0;
		popq(scheduler.tasks);
	}
}

#endif
