#include "../util/mutex.h"
#include "../cpu/atomic/swap.h"

unsigned char lock_mutex(struct Mutex* m, unsigned long pid) {
	if (m->pid == NULL_PID) {
		//m->pid = pid;
		atm_lock(pid, &m->pid);
		return 0;
	}
	return 1;
}

// Eventually, there will need to be a hook into the scheduling mechanism
//  that checks the currently running process and check that against the
//  mutex's pid lock
unsigned char release_mutex(struct Mutex* m, unsigned long pid) {
	if (m->pid == pid) {
		//m->pid = NULL_PID;
		atm_release(&m->pid);
		return 0;
	}
	return 1;
}
