#include <cpu.h>
#include <cpu/atomic/swap.h>
#include <util/mutex.h>
#include <util/lock.h>

// TODO: Improve locking for system
//  1. Deadlock prevention by going through mutex list
void lock(struct Lock* l)
{
	unsigned long mode = getmode() & 0x1F;
	if (mode == 0x10) {
		sys1(SYS_LOCK, l);
	}
}

void unlock(struct Lock* l)
{
	unsigned long mode = getmode() & 0x1F;
	if (mode == 0x10) {
		sys1(SYS_UNLOCK, l);
	}
}
