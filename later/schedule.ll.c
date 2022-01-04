#ifdef LL

#include "../sys/schedule.h"
#include "../lib/ll.h"
static struct LL bl = {
	.prev = 0,
	.next = 0,
};
static struct Scheduler scheduler = {
	.tasks = &bl,
};

#endif
