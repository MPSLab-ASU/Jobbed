#include <cpu/atomic/swap.h>
#include <globals.h>
#include <lib/mem.h>
#include <sys/schedule.h>
#include <util/mutex.h>

unsigned char lock_mutex(struct Mutex* m, unsigned long pid)
{
	if (m->pid == NULL_PID) {
		// Use currently running thread's PID if no pid given
		if (pid == 0) {
			struct Thread* thread = scheduler.rthread_ll->data;
			atm_lock(thread->data.pid, &m->pid);
		} else {
			atm_lock(pid, &m->pid);
		}
		return 0;
	}
	struct Thread* thread = scheduler.rthread_ll->data;
	thread->data.status = THREAD_WAITING_FOR_MUTEX;
	thread->data.mutex_waiting = m;
	return 1;
}

// Eventually, there will need to be a hook into the scheduling mechanism
//  that checks the currently running process and check that against the
//  mutex's pid lock
unsigned char release_mutex(struct Mutex* m, unsigned long pid)
{
	// Use current thread's PID if no pid
	if (pid == 0) {
		struct Thread* thread = scheduler.rthread_ll->data;
		if (m->pid == thread->data.pid) {
			atm_release(&m->pid);
			return 0;
		}
	}
	else if (m->pid == pid) {
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
