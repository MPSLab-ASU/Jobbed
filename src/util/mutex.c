#include <cpu/atomic/swap.h>
#include <lib/mem.h>
#include <util/mutex.h>

unsigned char lock_mutex(struct Mutex* m, unsigned long pid)
{
	if (m->pid == NULL_PID) {
		atm_lock(pid, &m->pid);
		return 0;
	}
	return 1;
}

// Eventually, there will need to be a hook into the scheduling mechanism
//  that checks the currently running process and check that against the
//  mutex's pid lock
unsigned char release_mutex(struct Mutex* m, unsigned long pid)
{
	if (m->pid == pid) {
		atm_release(&m->pid);
		return 0;
	}
	return 1;
}

struct Mutex* create_mutex(void* addr)
{
	// Ensure aligned to word - Important for Atomic Swap
	struct Mutex* m = (struct Mutex*)malloca(sizeof(struct Mutex), 4);
	m->addr = addr;
	return m;
}
