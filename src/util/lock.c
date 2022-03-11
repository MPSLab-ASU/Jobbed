#include <cpu.h>
#include <cpu/atomic/swap.h>
#include <util/lock.h>

// TODO: Improve locking for system
//  1. Return code rather than hang?
//  2. Specific core PID rather than CORE0
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
