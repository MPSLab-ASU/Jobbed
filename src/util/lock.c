#include <util/lock.h>
#include <lib/kmem.h>

struct Lock* create_lock(void)
{
	struct Lock* l = (struct Lock*)kmalloc(sizeof(struct Lock));
	l->pid = 0;
	return l;
}
