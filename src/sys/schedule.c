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

struct Thread* get_unused_thread(void)
{
	for(unsigned long i = 0; i < MAX_THREADS; i++) {
		if (thread_table[i] == 0)
			return &threads[i];
	}
	return 0;
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
	// Get the Ready Queue
	struct ThreadQueue* ready_queue = &scheduler.ready[thread->priority];
	// Get thread's entry
	struct ThreadEntry* new_t_entry = &thread_entries[thread->offset];
	// Append to the end of the thread
	ready_queue->end.next->next = new_t_entry;
	ready_queue->end.next = new_t_entry;
	// Link thread's next entry to end of queue
	new_t_entry->next = &ready_queue->end;
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
	struct ThreadEntry* rte = &thread_entries[rt->offset];
	struct ThreadQueue* queue = &scheduler.ready[rt->priority];
	// Move head forward
	queue->start.next = rte->next;
	if (rte->next->entry_type == END_ENTRY)
		rte->next->next = &queue->start;
	// Clear thread entry's next
	rte->next = 0;
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
	struct ThreadQueue* trq = &scheduler.ready[priority];
	struct ThreadEntry* te = &thread_entries[rthread->offset];
	// Move head forward
	trq->start.next = te->next;
	if (te->next->entry_type == END_ENTRY)
		te->next->next = &trq->start;
	// Add to tail
	trq->end.next->next = te;
	trq->end.next = te;
	te->next = &trq->end;
}

// TODO: Figure out why two things are appearing in the wait queue by the end of this
void sched_mutex_yield(void* m)
{
	struct Thread* rthread = scheduler.rthread;
	struct ThreadEntry* rthread_e = &thread_entries[rthread->offset];
	// usrloopthread should not be yielded
	if (rthread == &usrloopthread)
		return;
	unsigned char priority = rthread->priority;
	// Signify which lock this thread is waiting for
	rthread->mptr = m;
	struct ThreadQueue* trq = &scheduler.ready[priority];
	struct ThreadQueue* tmq = &scheduler.mwait[priority];
	// Move to next thread in the current thread priority's ready queue
	trq->start.next = trq->start.next->next;
	if (trq->start.next->next->entry_type == END_ENTRY)
		trq->end.next = &trq->start;
	// Add thread to waiting queue
	tmq->end.next->next = rthread_e;
	tmq->end.next = rthread_e;
	rthread_e->next = &tmq->end;
	/// Find the thread with the mutex
	// Search through each priority
	for (unsigned long p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* queue = &scheduler.ready[p];
		// Keep track of the previous entry
		struct ThreadEntry* prev = &queue->start;
		struct ThreadEntry* entry = prev->next;
		while (entry->entry_type != END_ENTRY) {
			// Check if it is the Mutex's thread
			if (entry->thread->pid == ((struct Mutex*)m)->pid) {
				uart_scheduler();
				// Promote the thread's priority
				if (entry->thread->priority > priority) {
					struct ThreadQueue* new_queue = &scheduler.ready[priority];
					// Add it to the higher priority queue
					new_queue->end.next->next = entry;
					new_queue->end.next = entry;
					// Set the old priority if not set
					if (entry->thread->old_priority == 0xFF)
						entry->thread->old_priority = p;
					// Set the new priority
					entry->thread->priority = priority;
					// Remove it from the lower priority queue
					prev->next = entry->next;
					if (entry->next->entry_type == END_ENTRY) {
						entry->next->next = prev;
					}
					// Set the entry's next entry
					entry->next = &new_queue->end;
				}
				return;
			}
			// Continue forward
			prev = entry;
			entry = entry->next;
		}
	}
}

void sched_mutex_resurrect(void* m)
{
	// Look through each priority
	for (int p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* wait_queue = &scheduler.mwait[p];
		struct ThreadEntry* prev = &wait_queue->start;
		struct ThreadEntry* entry = prev->next;
		// Look through the lock wait queue
		while (entry->entry_type != END_ENTRY) {
			// Check if the thread is waiting for the release mutex
			if (entry->thread->mptr == m) {
				// Resurrect the thread
				entry->thread->mptr = 0;
				struct ThreadEntry* end = &scheduler.ready[entry->thread->priority].end;
				// Remove from old list
				prev->next = entry->next;
				if (entry->next->entry_type == END_ENTRY) {
					prev->next->next = prev;
				}
				// Add to new list
				end->next->next = entry;
				end->next = entry;
				entry->next = end;
				struct Thread* rthread = scheduler.rthread;
				struct ThreadEntry* rthread_e = &thread_entries[rthread->offset];
				// Move current thread to old thread if applicable
				if (rthread->old_priority != 0xFF) {
					struct ThreadQueue* cqueue = &scheduler.ready[rthread->priority];
					struct ThreadQueue* oqueue = &scheduler.ready[rthread->old_priority];
					// Move current queue up
					cqueue->start.next = cqueue->start.next->next;
					if (cqueue->start.next->next->entry_type == END_ENTRY)
						cqueue->end.next = &cqueue->start;
					// Add to old queue
					oqueue->end.next->next = rthread_e;
					oqueue->end.next = rthread_e;
					rthread_e->next = &oqueue->end;
					// Reset priority
					rthread->priority = rthread->old_priority;
					rthread->old_priority = 0xFF;
				}
				return;
			}
			prev = entry;
			entry = entry->next;
		}
	}
}
