#if !(defined(LL) || defined(FLAT))
#include "../sys/schedule.h"
#include "../lib/q.h"
#include "../lib/mem.h"

static struct Q_base bq = {
	.next = 0,
	.last = 0,
};
static struct Scheduler  scheduler = {
	.tasks = &bq,
};

void add_fxn(void (*task)(void), unsigned char priority)
{
	struct Task* t = (struct Task*)malloc(sizeof(struct Task));
	t->priority = priority;
	t->task = task;
	add_task(t);
}

void add_task(struct Task* t)
{
	pushq(scheduler.tasks, t);
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
	if (scheduler.tasks->last != 0) {
		struct Task* tsk = (struct Task*)scheduler.tasks->next->data;
		(tsk->task)();
		popq(scheduler.tasks);
	}
}

#endif
