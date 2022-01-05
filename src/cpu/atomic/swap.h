#ifndef CPU_ATOMIC_SWAP_A_H
#define CPU_ATOMIC_SWAP_A_H
#include "../../util/mutex.h"

/// https://stackoverflow.com/questions/16329123/use-of-strexeq-instead-of-strex-for-spinlock-implementation-in-arm
/// https://elixir.bootlin.com/linux/v4.9/source/arch/arm/include/asm/spinlock.h
/// https://elixir.bootlin.com/linux/v4.9/source/arch/arm/include/asm/spinlock_types.h#L23
/// https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/3_Processes.html
/// https://developer.arm.com/documentation/dht0008/a/arm-synchronization-primitives/practical-uses/implementing-a-semaphore?lang=en

// TODO: Once scheduling works, have a failed lock put thread in waiting state
static inline void atm_lock(unsigned long pid, unsigned long* addr)
{
	unsigned long tmp, current_lock_value;
	asm volatile(
"1:	ldrex	%0, [%3]\n"
"	cmp	%0, #0\n"
"	wfene\n"
"	strexeq	%1, %2, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b\n"
"	dmb"
	: "=&r" (current_lock_value), "=&r" (tmp)
	: "r" (pid), "r" (addr)
	: "cc");
}

static inline void atm_release(unsigned long* addr)
{
	unsigned long cleared = NULL_PID;
	asm volatile(
"	dmb\n"
"	str %0, [%1]\n"
"	dsb\n"
"	sev"
	:: "r" (cleared), "r" (addr)
	: "cc");
}

#endif
