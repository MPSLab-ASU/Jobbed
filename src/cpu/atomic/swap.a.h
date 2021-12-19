#ifndef CPU_ATOMIC_SWAP_A_H
#define CPU_ATOMIC_SWAP_A_H

/// https://stackoverflow.com/questions/16329123/use-of-strexeq-instead-of-strex-for-spinlock-implementation-in-arm

static inline void atm_lock(unsigned long pid, unsigned long* addr) {
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

static inline void atm_unlock(unsigned long* addr) {
	unsigned long cleared = 0;
	asm volatile(
"	dmb\n"
"	str %0, [%1]\n"
"	dsb\n"
"	sev"
	:: "r" (cleared), "r" (addr)
	: "cc");
}

#endif
