#include <cpu.h>
#include <cpu/atomic/swap.h>
#include <lib/kmem.h>
#include <util/lock.h>

void lock(struct Lock* l)
{
	unsigned long mode = getmode() & 0x1F;
	if (mode == 0x10) {
		sys1(SYS_LOCK, l);
	} else {
		atm_lock(CORE0_PID, (unsigned long*)l);
	}
}

void unlock(struct Lock* l)
{
	unsigned long mode = getmode() & 0x1F;
	if (mode == 0x10) {
		sys1(SYS_UNLOCK, l);
	} else {
		atm_release((unsigned long*)l);
	}
}

struct Lock* create_lock(void)
{
	struct Lock* l = (struct Lock*)kmalloc(sizeof(struct Lock));
	l->pid = 0;
	return l;
}
