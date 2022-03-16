#include <cpu.h>
#include <globals.h>
#include <graphics/lfb.h>
#include <drivers/uart.h>
#include <lib/kmem.h>
#include <sys/schedule.h>
#include <util/mutex.h>

extern void kernel_usr_task_loop(void);

void init_scheduler(void)
{
	// Set rthread to usrloopthread - an infinitely running thread so that the pointer will never be null
	usrloopthread.pc = (void*)kernel_usr_task_loop;
	usrloopthread.sp = (void*)0x5FC8;
	*(unsigned long**)usrloopthread.sp = (unsigned long*)kernel_usr_task_loop;
	usrloopthread.sp_base = -1;
	usrloopthread.mptr = 0;
	usrloopthread.pid = -1;
	usrloopthread.priority = -1;
	usrloopthread.old_priority = -1;
	usrloopthread.status = THREAD_READY;
	usrloopthread.offset = -1;
	scheduler.rthread = &usrloopthread;

	// Initialize Scheduling Queues
	for (unsigned long p = 0; p < PRIORITIES; p++) {
		// Ready Init
		scheduler.ready[p].start.thread = 0;
		scheduler.ready[p].start.next = &scheduler.ready[p].end;
		scheduler.ready[p].start.entry_type = START_ENTRY;
		scheduler.ready[p].end.thread = 0;
		scheduler.ready[p].end.next = &scheduler.ready[p].start;
		scheduler.ready[p].end.entry_type = END_ENTRY;
		// Mutex Wait Init
		scheduler.mwait[p].start.thread = 0;
		scheduler.mwait[p].start.next = &scheduler.mwait[p].end;
		scheduler.mwait[p].start.entry_type = START_ENTRY;
		scheduler.mwait[p].end.thread = 0;
		scheduler.mwait[p].end.next = &scheduler.mwait[p].start;
		scheduler.mwait[p].end.entry_type = END_ENTRY;
		// Signal Wait Init
		scheduler.swait[p].start.thread = 0;
		scheduler.swait[p].start.next = &scheduler.swait[p].end;
		scheduler.swait[p].start.entry_type = START_ENTRY;
		scheduler.swait[p].end.thread = 0;
		scheduler.swait[p].end.next = &scheduler.swait[p].start;
		scheduler.swait[p].end.entry_type = END_ENTRY;
	}

	// Initialize nextpid
	nextpid = FIRST_AVAIL_PID;

	// Initialize Threads - Stack Base and Offsets
	for (unsigned long i = 0; i < MAX_THREADS; i++) {
		struct Thread* t = &threads[i];
		t->offset = i;
		t->sp_base = 0x20000000 - STACK_SIZE*i;
		// Clear the stack use
		thread_table[i] = 0;
		struct ThreadEntry* te = &thread_entries[i];
		te->thread = t;
		// Initialize To No Next Entry Initially
		te->next = 0;
		te->entry_type = THREAD_ENTRY;
	}
}

void push_to_queue(struct Thread* t, unsigned char type, unsigned char priority)
{
	struct ThreadEntry* entry = &thread_entries[t->offset];
	struct ThreadQueue* queue;
	if (type == THREAD_READY) {
		queue = &scheduler.ready[priority];
	} else if (type == THREAD_MWAIT) {
		queue = &scheduler.mwait[priority];
	} else if (type == THREAD_SWAIT) {
		queue = &scheduler.swait[priority];
	} else {
		return;
	}
	queue->end.next->next = entry;
	queue->end.next = entry;
	entry->next = &queue->end;
}

struct ThreadEntry* pop_from_queue(unsigned char type, unsigned char priority)
{
	struct ThreadEntry* entry = 0;
	struct ThreadQueue* queue;
	if (type == THREAD_READY) {
		queue = &scheduler.ready[priority];
	} else if (type == THREAD_MWAIT) {
		queue = &scheduler.mwait[priority];
	} else if (type == THREAD_SWAIT) {
		queue = &scheduler.swait[priority];
	} else {
		return entry;
	}
	if (queue->start.next->entry_type == END_ENTRY)
		return entry;
	entry = queue->start.next;
	queue->start.next = entry->next;
	if (entry->next->entry_type == END_ENTRY)
		queue->end.next = &queue->start;
	return entry;
}

struct ThreadEntry* remove_next_from_queue(struct ThreadEntry* te)
{
	struct ThreadEntry* prev = te;
	struct ThreadEntry* remove = te->next;
	struct ThreadEntry* next = remove->next;
	if (remove->entry_type == END_ENTRY || remove->entry_type == START_ENTRY)
		return 0;
	prev->next = next;
	if (next->entry_type == END_ENTRY)
		next->next = prev;
	return remove;
}

struct ThreadEntry* find_pid(unsigned long pid)
{
	for (unsigned char p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* queue;
		struct ThreadEntry* prev;
		struct ThreadEntry* entry;

		queue = &scheduler.ready[p];
		prev = &queue->start;
		entry = prev->next;
		while (entry->entry_type != END_ENTRY) {
			if (entry->thread->pid == pid)
				return prev;
			prev = entry;
			entry = entry->next;
		}

		queue = &scheduler.mwait[p];
		prev = &queue->start;
		entry = prev->next;
		while (entry->entry_type != END_ENTRY) {
			if (entry->thread->pid == pid)
				return prev;
			prev = entry;
			entry = entry->next;
		}

		queue = &scheduler.swait[p];
		prev = &queue->start;
		entry = prev->next;
		while (entry->entry_type != END_ENTRY) {
			if (entry->thread->pid == pid)
				return prev;
			prev = entry;
			entry = entry->next;
		}
	}
	return 0;
}

struct ThreadEntry* find_mutex_wait_next(void* m)
{
	for (unsigned char p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* queue = &scheduler.mwait[p];
		struct ThreadEntry* prev = &queue->start;
		struct ThreadEntry* entry = prev->next;
		while (entry->entry_type != END_ENTRY) {
			if (entry->thread->mptr == m)
				return prev;
			prev = entry;
			entry = entry->next;
		}
	}
	return 0;
}

struct ThreadEntry* find_signal_wait_next(void* m)
{
	for (unsigned char p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* queue = &scheduler.swait[p];
		struct ThreadEntry* prev = &queue->start;
		struct ThreadEntry* entry = prev->next;
		while (entry->entry_type != END_ENTRY) {
			if (entry->thread->mptr == m)
				return prev;
			prev = entry;
			entry = entry->next;
		}
	}
	return 0;
}

struct Thread* get_unused_thread(void)
{
	for(unsigned long i = 0; i < MAX_THREADS; i++) {
		if (thread_table[i] == 0)
			return &threads[i];
	}
	return 0;
}

unsigned char find_duplicate(void* pc)
{
	for (unsigned char p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* queue = &scheduler.ready[p];
		struct ThreadEntry* entry = queue->start.next;
		while (entry->entry_type == THREAD_ENTRY) {
			if (entry->thread->pc == pc) {
				return 1;
			}
		}
	}
	return 0;
}

unsigned char add_thread_without_duplicate(void* pc, void* arg, unsigned char priority)
{
	if (!find_duplicate(pc)) {
		return add_thread(pc, arg, priority);
	}
	return 1;
}

unsigned char add_thread(void* pc, void* arg, unsigned char priority)
{
	struct Thread* thread = get_unused_thread();
	// The only point-of-failure is not having a thread available
	if (thread == 0)
		return 1;
	/// Mark the Stack Space as In-Use
	thread_table[thread->offset] = 1;
	/// Thread Setup
	thread->pc = pc;
	unsigned long* argp = (void*)thread->sp_base;
	argp -= 13;
	*argp = (unsigned long)arg; // Set r0 to the argument
	argp -= 1;
	*(unsigned long**)argp = (unsigned long*)cleanup; // Set lr to the cleanup function
	thread->sp = argp;
	thread->status = THREAD_READY;
	thread->mptr = (void*)0;
	thread->pid = nextpid++;
	// Reset next pid on overflow
	if (nextpid < FIRST_AVAIL_PID) {
		nextpid = FIRST_AVAIL_PID;
	}
	// Cap Priority Level
	if (priority >= PRIORITIES)
		thread->priority = PRIORITIES - 1;
	else
		thread->priority = priority;
	// This thread is new
	thread->old_priority = -1;
	// Reserved for non-preemptible tasking
	thread->preempt = 0;
	/// Add Thread to Scheduler
	push_to_queue(thread, THREAD_READY, thread->priority);
	// Schedule if this was called in usermode
	unsigned long mode = getmode() & 0x1F;
	if (mode == 0x10) {
		sys0(SYS_YIELD_HIGH);
	}
	return 0;
}

void uart_scheduler(void)
{
	uart_string("Scheduler Info\n==============\nCurrent\n");
	uart_hex((unsigned long)scheduler.rthread);
	uart_char(' ');
	kmemshow32((void*)scheduler.rthread, 9);
	for(int p = 0; p < PRIORITIES; p++) {
		uart_string("Priority ");
		uart_10(p);
		uart_char('\n');
		struct ThreadQueue* queue;
		struct ThreadEntry* entry;

		queue = &scheduler.ready[p];
		uart_string("Ready Queue\n");
		entry = queue->start.next;
		while (entry->entry_type != END_ENTRY) {
			uart_hex((unsigned long)entry->thread);
			uart_char(' ');
			kmemshow32((void*)entry->thread, 9);
			entry = entry->next;
		}

		queue = &scheduler.mwait[p];
		uart_string("Mutex Wait Queue\n");
		entry = queue->start.next;
		while (entry->entry_type != END_ENTRY) {
			uart_hex((unsigned long)entry->thread);
			uart_char(' ');
			kmemshow32((void*)entry->thread, 9);
			entry = entry->next;
		}

		queue = &scheduler.swait[p];
		uart_string("Signal Wait Queue\n");
		entry = queue->start.next;
		while (entry->entry_type != END_ENTRY) {
			uart_hex((unsigned long)entry->thread);
			uart_char(' ');
			kmemshow32((void*)entry->thread, 9);
			entry = entry->next;
		}
	}
	uart_string("==============\n");
}

struct Thread* next_thread(void)
{
	// Recurse through all priorities to try to find a ready thread
	for (int p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* rq = &scheduler.ready[p];
		if (rq->start.next->entry_type == END_ENTRY)
			continue;
		return rq->start.next->thread;
	}
	// No thread found, use basic usrloopthread while waiting for new thread
	return &usrloopthread;
}

void c_cleanup(void)
{
	struct Thread* rt = scheduler.rthread;
	pop_from_queue(THREAD_READY, rt->priority);
	// Mark Thread Unused
	thread_table[rt->offset] = 0;
}

void yield(void)
{
	struct Thread* rthread = scheduler.rthread;
	// usrloopthread should not be yielded
	if (rthread == &usrloopthread)
		return;
	// Put current thread at the end of its ready queue,
	//  thus any threads of the same priority can be run first
	unsigned char priority = rthread->priority;
	struct ThreadEntry* tq;
	// Remove from top of queue
	tq = pop_from_queue(THREAD_READY, priority);
	if (tq != 0) {
		// Add to bottom of queue
		push_to_queue(tq->thread, THREAD_READY, priority);
	}
}

void sched_mutex_yield(void* m)
{
	struct Thread* rthread = scheduler.rthread;
	// usrloopthread should not be yielded
	if (rthread == &usrloopthread)
		return;
	unsigned char priority = rthread->priority;
	// Signify which lock this thread is waiting for
	rthread->mptr = m;
	struct ThreadEntry* rt;
	// Remove from top of running queue
	rt = pop_from_queue(THREAD_READY, priority);
	if (rt != 0)
		// Push to bottom of wait queue
		push_to_queue(rt->thread, THREAD_MWAIT, priority);
	// Find the thread that has the mutex locked
	struct Mutex* mtex = m;
	struct ThreadEntry* mutex_next = find_pid(mtex->pid);
	if (mutex_next == 0)
		return;
	// The next thread is the one with the lock
	struct ThreadEntry* mutex_thread_entry = mutex_next->next;
	// Check if it is lower priority
	if (mutex_thread_entry->thread->priority > priority) {
		// Remove it from the old priority queue
		remove_next_from_queue(mutex_next);
		struct Thread* t = mutex_thread_entry->thread;
		// Preserve the old priority
		if (t->old_priority == 0xFF)
			t->old_priority = t->priority;
		t->priority = priority;
		// Add it to the higher priority queue
		push_to_queue(t, THREAD_READY, priority);
	}
}

void sched_mutex_resurrect(void* m)
{
	// Find any mutex to resurrect
	struct ThreadEntry* prev = find_mutex_wait_next(m);
	if (prev == 0)
		return;
	struct ThreadEntry* entry = prev->next;
	struct Thread* thread = entry->thread;
	// Resurrect the thread
	thread->mptr = 0;
	// Remove from wait queue
	entry = remove_next_from_queue(prev);
	if (entry == 0)
		return;
	// Add to ready queue
	push_to_queue(entry->thread, THREAD_READY, entry->thread->priority);
	// Demote current thread
	struct Thread* rthread = scheduler.rthread;
	unsigned long p = rthread->priority;
	unsigned long op = rthread->old_priority;
	// Restore the original priority level
	if (op != 0xFF) {
		struct ThreadEntry* tentry = pop_from_queue(THREAD_READY, p);
		push_to_queue(tentry->thread, THREAD_READY, p);
	}
}
