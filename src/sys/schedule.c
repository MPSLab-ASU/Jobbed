#include "../sys/schedule.h"
#include "../lib/ll.h"
#include "../lib/q.h"

#ifdef FLAT
static struct Task* task_list[256];

static struct Scheduler scheduler = {
	.tasks = task_list,
};

static unsigned int ntask_i = 0;

void add_task(struct Task* t)
{
	scheduler.tasks[ntask_i] = t;
	ntask_i += 1;
	if (ntask_i > 256) {
		ntask_i = 0;
	}
}

unsigned int get_task_length(void)
{
	return ntask_i;
}

void execute_task(void)
{
	if (scheduler.tasks[ntask_i-1] != 0)
		scheduler.tasks[ntask_i-1]->task();
}
#elseif LL
static struct LL bl = {
	.prev = 0,
	.next = 0,
};
static struct Scheduler scheduler = {
	.tasks = &bl,
};
#else
static struct Q_base bq = {
	.next = 0,
	.last = 0,
};
static struct Scheduler  scheduler = {
	.tasks = &bq,
};

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
