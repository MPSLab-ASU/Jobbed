#ifdef FLAT

#include "../sys/schedule.h"
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

#endif
