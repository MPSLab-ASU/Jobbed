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
		for (unsigned long t = 0; t < TQUEUE_MAX; t++) {
			/// Ready Queue
			scheduler.ready[p].entry[t].thread = 0;
			if (t == 0)
				scheduler.ready[p].entry[t].prev = &scheduler.ready[p].entry[TQUEUE_MAX-1];
			else
				scheduler.ready[p].entry[t].prev = &scheduler.ready[p].entry[(t-1)%TQUEUE_MAX];
			scheduler.ready[p].entry[t].next = &scheduler.ready[p].entry[(t+1)%TQUEUE_MAX];
			/// Mutex Wait Queue
			scheduler.mwait[p].entry[t].thread = 0;
			if (t == 0)
				scheduler.mwait[p].entry[t].prev = &scheduler.mwait[p].entry[TQUEUE_MAX-1];
			else
				scheduler.mwait[p].entry[t].prev = &scheduler.mwait[p].entry[(t-1)%TQUEUE_MAX];
			scheduler.mwait[p].entry[t].next = &scheduler.mwait[p].entry[(t+1)%TQUEUE_MAX];
			/// Signal Wait Queue
			scheduler.swait[p].entry[t].thread = 0;
			if (t == 0)
				scheduler.swait[p].entry[t].prev = &scheduler.swait[p].entry[TQUEUE_MAX-1];
			else
				scheduler.swait[p].entry[t].prev = &scheduler.swait[p].entry[(t-1)%TQUEUE_MAX];
			scheduler.swait[p].entry[t].next = &scheduler.swait[p].entry[(t+1)%TQUEUE_MAX];
		}
		// Ready Queue
		scheduler.ready[p].read.entry  = &scheduler.ready[p].entry[0];
		scheduler.ready[p].write.entry = &scheduler.ready[p].entry[0];
		// Mutex Wait Queue
		scheduler.mwait[p].read.entry  = &scheduler.mwait[p].entry[0];
		scheduler.mwait[p].write.entry = &scheduler.mwait[p].entry[0];
		// Signal Wait Queue
		scheduler.swait[p].read.entry  = &scheduler.swait[p].entry[0];
		scheduler.swait[p].write.entry = &scheduler.swait[p].entry[0];
	}

	// Initialize nextpid
	nextpid = FIRST_AVAIL_PID;

	// Initialize Threads - Stack Base and Offsets
	for (unsigned long i = 0; i < MAX_THREADS; i++) {
		struct Thread* t = &threads[i];
		t->offset = i;
		t->sp_base = 0x20000000 - STACK_SIZE*i;
		thread_table[i] = 0;
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
	thread_table[thread->offset] = 1;
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
	// Get the Ready Queue
	struct ThreadQueue* ready_queue = &scheduler.ready[thread->priority];
	// Get the write pointer
	struct ThreadEntryIterator* ready_write = &ready_queue->write;
	if (ready_write->entry->thread == 0) {
		// Add the thread to the write pointer
		ready_write->entry->thread = thread;
		// Move the write pointer to the next entry
		ready_write->entry = ready_write->entry->next;
	} else {
		// Cannot add, mark the stack space as available
		//  so that it can be used on a subsequent add thread
		thread_table[thread->offset] = 0;
		return 1;
	}
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
		struct ThreadEntryIterator iter;

		struct ThreadQueue* rq = &scheduler.ready[p];
		uart_string("Ready Queue\n");
		iter.entry = rq->read.entry;
		while (iter.entry != rq->write.entry) {
			uart_hex((unsigned long)iter.entry->thread);
			uart_char(' ');
			kmemshow32((void*)iter.entry->thread, 9);
			iter.entry = iter.entry->next;
		}

		struct ThreadQueue* mq = &scheduler.mwait[p];
		uart_string("Mutex Wait Queue\n");
		iter.entry = mq->read.entry;
		while (iter.entry != mq->write.entry) {
			uart_hex((unsigned long)iter.entry->thread);
			uart_char(' ');
			kmemshow32((void*)iter.entry->thread, 9);
			iter.entry = iter.entry->next;
		}

		struct ThreadQueue* sq = &scheduler.swait[p];
		uart_string("Signal Wait Queue\n");
		iter.entry = sq->read.entry;
		while (iter.entry != sq->write.entry) {
			uart_hex((unsigned long)iter.entry->thread);
			uart_char(' ');
			kmemshow32((void*)iter.entry->thread, 9);
			iter.entry = iter.entry->next;
		}
	}
	uart_string("==============\n");
}

struct Thread* next_thread(void)
{
	// Recurse through all priorities to try to find a ready thread
	for (int p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* rq = &scheduler.ready[p];
		if (rq->read.entry == rq->write.entry)
			continue;
		return rq->read.entry->thread;
	}
	// No thread found, use basic usrloopthread while waiting for new thread
	return &usrloopthread;
}

void c_cleanup(void)
{
	struct Thread* rt = scheduler.rthread;
	struct ThreadEntryIterator* read = &scheduler.ready[rt->priority].read;
	// Clear the thread pointer
	read->entry->thread = 0;
	// Move read pointer forward
	read->entry = read->entry->next;
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
	trq->read.entry = trq->read.entry->next;
	trq->write.entry->thread = rthread;
	trq->write.entry = trq->write.entry->next;
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
	struct ThreadQueue* trq = &scheduler.ready[priority];
	struct ThreadQueue* tmq = &scheduler.mwait[priority];
	// Move to next thread in the current thread priority's ready queue
	trq->read.entry = trq->read.entry->next;
	// Add thread to waiting queue
	tmq->write.entry->thread = rthread;
	tmq->write.entry = tmq->write.entry->next;
	// Find the thread with the mutex
	struct ThreadEntryIterator iter;
	// Search through each priority
	for (unsigned long p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* rq = &scheduler.ready[p];
		iter = rq->read;
		while (iter.entry != rq->write.entry) {
			// Check if it is the Mutex's thread
			if (iter.entry->thread->pid == ((struct Mutex*)m)->pid) {
				// Promote the thread's priority
				if (iter.entry->thread->priority > priority) {
					// Add it to the higher priority queue
					scheduler.ready[priority].write.entry->thread = iter.entry->thread;
					// Move the Write Iterator Forward
					scheduler.ready[priority].write.entry = scheduler.ready[priority].write.entry->next;
					// Set the old priority if not set
					if (iter.entry->thread->old_priority == 0xFF)
						iter.entry->thread->old_priority = p;

					// Set the new priority
					iter.entry->thread->priority = priority;
					
					// Prune the old priority entry
					struct ThreadEntry* prev = iter.entry->prev;
					struct ThreadEntry* next = iter.entry->next;
					prev->next = next;
					next->prev = prev;
					// Put the newly freed entry at end of write queue
					//iter.entry->prev = rq->write.entry;
					//iter.entry->next = rq->write.entry->next;
					if (iter.entry == rq->read.entry)
						rq->read.entry = rq->read.entry->next;
					rq->write.entry->next->prev = iter.entry;
					rq->write.entry->next = iter.entry;
					iter.entry->thread = 0;
				}
				return;
			}
			iter.entry = iter.entry->next;
		}
	}
}

void sched_mutex_resurrect(void* m)
{
	// Look through each priority
	for (int p = 0; p < PRIORITIES; p++) {
		struct ThreadQueue* tmq = &scheduler.mwait[p];
		struct ThreadEntryIterator iter;
		iter = tmq->read;
		// Look through the lock wait queue
		while (iter.entry != tmq->write.entry) {
			// Check if the thread is waiting for the released mutex
			if (iter.entry->thread->mptr == m) {
				// Ressurect the thread
				iter.entry->thread->mptr = 0;
				scheduler.ready[iter.entry->thread->priority].write.entry->thread = iter.entry->thread;
				scheduler.ready[iter.entry->thread->priority].write.entry = scheduler.ready[iter.entry->thread->priority].write.entry->next;
				// Move the read pointer ahead
				if (iter.entry == tmq->read.entry)
					tmq->read.entry = tmq->read.entry->next;
				iter.entry->prev->next = iter.entry->next;
				iter.entry->next->prev = iter.entry->prev;
				tmq->write.entry->next->prev = iter.entry;
				tmq->write.entry->next = iter.entry;
				struct Thread* rthread = scheduler.rthread;
				// Move the current thread to its old priority if it was promoted earlier
				if (rthread->old_priority != 0xFF) {
					struct ThreadQueue* cur_trq = &scheduler.ready[rthread->priority];
					struct ThreadQueue* old_trq = &scheduler.ready[rthread->old_priority];
					// Prune from the current ready queue
					cur_trq->read.entry->thread = 0;
					cur_trq->read.entry = cur_trq->read.entry->next;
					// Prepend to original ready queue
					old_trq->read.entry = old_trq->read.entry->prev;
					old_trq->read.entry->thread = rthread;
					// Reset Priority
					rthread->priority = rthread->old_priority;
					rthread->old_priority = -1;
				}
				return;
			}
			iter.entry = iter.entry->next;
		}
	}
}

// TODO: Check offsets
