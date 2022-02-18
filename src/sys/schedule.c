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
	scheduler.rthread = &usrloopthread;
	// Initialize Rotating Buffers
	struct ThreadQueues* tq;
	for (int i = 0; i < PRIORITIES; i++) {
		tq = &scheduler.thread_queues[i];
		struct ThreadRotBuffer* trb = &tq->ready;
		for (int i = 0; i < TQUEUE_CNT; i++) {
			trb->roffset = 0;
			trb->woffset = 0;
			for (int j = 0; j < TQUEUE_MAX; j++)
				trb->queue[j] = 0;
			trb += 1;
		}
	}
	sched_stack_count = 0;
	// Initialize nextpid
	nextpid = FIRST_AVAIL_PID;
}

struct RStack get_stack(void)
{
	struct RStack r = {.sp = 0, .idx = -1};
	for (int i = 0; i < MAX_THREADS; i++) {
		if (stacks_table[i] == 0) {
			stacks_table[i] = 1;
			r.idx = i;
			r.sp = (void*)0x20000000 - STACK_SIZE*i;
			return r;
		}
	}
	return r;
}

void draw_stacks(void)
{
	unsigned long ioff = 0;
	unsigned long yoff = STACK_DRAW_YOFF;
	for(int i = 0; i < MAX_THREADS; i++) {
		if(stacks_table[i])
			draw_cbox(ioff, yoff, STACK_DRAW_SIZE, STACK_DRAW_SIZE, 0xFFFFFF);
		else
			draw_cbox(ioff, yoff, STACK_DRAW_SIZE, STACK_DRAW_SIZE, 0x000000);
		ioff += STACK_DRAW_SIZE;
		if(ioff % STACK_DRAW_WIDTH == 0) {
			yoff += STACK_DRAW_SIZE;
			ioff = 0;
		}
	}
}

void add_thread(void* pc, void* arg, unsigned char priority)
{
	//void* sp = get_stack();
	struct RStack r = get_stack();
	//struct Thread* thread = (struct Thread*)malloca(sizeof(struct Thread), 4);
	struct Thread* thread = (struct Thread*)kmalloc(sizeof(struct Thread));
	thread->pc = pc;
	if (r.sp) {
		thread->sp_base = r.idx;
		unsigned long* argp = r.sp;
		argp -= 13;
		*argp = (unsigned long)arg; // Set r0 to the argument
		argp -= 1;
		*(unsigned long**)argp = (unsigned long*)cleanup; // Set lr to the cleanup function
		thread->sp = (void*)argp;
		thread->status = THREAD_READY;
		sched_stack_count++;
	} else {
		thread->sp_base = r.idx;
		thread->sp = r.sp;
		thread->status = THREAD_SERROR;
	}
	thread->mptr = (void*)0;
	thread->pid = nextpid++;
	thread->priority = priority % PRIORITIES;
	thread->old_priority = -1;
	thread->preempt = 0;
	// Add Thread* to scheduler's appropriate buffer
	struct ThreadQueues* tq = &scheduler.thread_queues[thread->priority];
	struct ThreadRotBuffer* trb;
	if (thread->status == THREAD_SERROR) {
		trb = &tq->serror;
	} else {
		trb = &tq->ready;
	}
	trb->queue[trb->woffset++] = thread;
	trb->woffset %= TQUEUE_MAX;
	unsigned long mode = getmode() & 0x1F;
	if (mode == 0x10) {
		sys0(SYS_YIELD_HIGH);
	}
}

void uart_scheduler(void)
{
	uart_string("Scheduler Info\n==============\nCurrent\n");
	uart_hex((unsigned long)scheduler.rthread);
	uart_char(' ');
	kmemshow32((void*)scheduler.rthread, 7);
	struct ThreadQueues* tq;
	for(int p = 0; p < PRIORITIES; p++) {
		uart_string("Priority ");
		uart_10(p);
		uart_char('\n');
		tq = &scheduler.thread_queues[p];
		struct ThreadRotBuffer* trb;
		trb = &tq->ready;
		for(int i = 0; i < TQUEUE_CNT; i++) {
			if (trb->roffset == trb->woffset) {
				trb += 1;
				continue;
			}
			uart_string("Queue ");
			uart_10(i);
			uart_char('\n');
			unsigned long roffset = trb->roffset;
			while (roffset != trb->woffset) {
				uart_hex((unsigned long)trb->queue[roffset]);
				uart_char(' ');
				kmemshow32((void*)trb->queue[roffset], 7);
				roffset++;
				roffset %= TQUEUE_MAX;
			}
			trb += 1;
		}
	}
	uart_string("==============\n");
}

struct Thread* next_thread(void)
{
	struct Thread* next = &usrloopthread;
	for (int p = 0; p < PRIORITIES; p++) {
		struct ThreadRotBuffer* rb = &scheduler.thread_queues[p].ready;
		if (rb->roffset == rb->woffset)
			continue;
		return rb->queue[rb->roffset];
	}
	return next;
}

void* get_rthread_roffset(void)
{
	return &scheduler.thread_queues[scheduler.rthread->priority].ready.roffset;
}

void yield(void)
{
	struct Thread* rthread = scheduler.rthread;
	if (rthread == &usrloopthread)
		return;
	unsigned char priority = rthread->priority;
	struct ThreadRotBuffer* trb = &scheduler.thread_queues[priority].ready;
	trb->roffset += 1;
	trb->roffset %= TQUEUE_MAX;
	trb->queue[trb->woffset++] = rthread;
	trb->woffset %= TQUEUE_MAX;
}

void sched_mutex_yield(void* m)
{
	struct Thread* rthread = scheduler.rthread;
	if (rthread == &usrloopthread)
		return;
	unsigned char priority = rthread->priority;
	rthread->mptr = m;
	struct ThreadRotBuffer* trbb = &scheduler.thread_queues[priority].ready;
	struct ThreadRotBuffer* trbm = &scheduler.thread_queues[priority].mwait;
	trbb->roffset += 1;
	trbb->roffset %= TQUEUE_MAX;
	trbm->queue[trbm->woffset++] = rthread;
	trbm->woffset %= TQUEUE_MAX;
	// Find the thread with the mutex
	struct ThreadQueues* tq;
	for (int i = 0; i < PRIORITIES; i++) {
		tq = &scheduler.thread_queues[i];
		struct ThreadRotBuffer* trb = &tq->ready;
		for (int i = 0; i < TQUEUE_CNT; i++) {
			unsigned long roffset = trb->roffset;
			unsigned long woffset = trb->woffset;
			while(roffset != woffset) {
				// Found thread
				if (trb->queue[roffset]->pid == ((struct Mutex*)m)->pid) {
					// Promote the thread to the new priority
					if (trb->queue[roffset]->priority > priority) {
						trbb->queue[trbb->woffset++] = trb->queue[roffset];
						if(trb->queue[roffset]->old_priority == 0xFF)
							trb->queue[roffset]->old_priority = trb->queue[roffset]->priority;
						trb->queue[roffset]->priority = priority;
						trbb->woffset %= TQUEUE_MAX;
						unsigned long coffset = roffset;
						while (coffset != woffset) {
							trb->queue[coffset] = trb->queue[(coffset+1)%TQUEUE_MAX];
							coffset++;
							coffset %= TQUEUE_MAX;
						}
						if (trb->woffset == 0)
							trb->woffset = TQUEUE_MAX-1;
						else
							trb->woffset--;
					}
					return;
				}
				roffset++;
				roffset %= TQUEUE_MAX;
			}
			trb += 1;
		}
	}
}

void sched_mutex_resurrect(void* m)
{
	for (int p = 0; p < PRIORITIES; p++) {
		struct ThreadRotBuffer* trbm = &scheduler.thread_queues[p].mwait;
		unsigned long roffset = trbm->roffset;
		while (roffset != trbm->woffset) {
			if (trbm->queue[roffset]->mptr == m) {
				trbm->queue[roffset]->mptr = 0;
				struct ThreadRotBuffer* trb = &scheduler.thread_queues[trbm->queue[roffset]->priority].ready;
				trb->queue[trb->woffset++] = trbm->queue[roffset];
				trb->woffset %= TQUEUE_MAX;
				// Copy all next backward to fill space
				unsigned long coffset = roffset;
				while (coffset != trbm->woffset) {
					trbm->queue[coffset] = trbm->queue[(coffset+1)%TQUEUE_MAX];
					coffset++;
					coffset %= TQUEUE_MAX;
				}
				if(trbm->woffset == 0)
					trbm->woffset = TQUEUE_MAX-1;
				else
					trbm->woffset--;
				// Move the read pointer ahead
				struct Thread* rthread = scheduler.rthread;
				if (rthread->old_priority != 0xFF) {
					struct ThreadRotBuffer* rtrb = &scheduler.thread_queues[rthread->priority].ready;
					struct ThreadRotBuffer* ntrb = &scheduler.thread_queues[rthread->old_priority].ready;
					rtrb->roffset++;
					rtrb->roffset %= TQUEUE_MAX;
					if (ntrb->roffset == 0)
						ntrb->roffset = TQUEUE_MAX-1;
					else
						ntrb->roffset--;
					ntrb->queue[ntrb->roffset] = rthread;
					rthread->priority = rthread->old_priority;
					rthread->old_priority = -1;
				}
				return;
			}
			roffset++;
			roffset %= TQUEUE_MAX;
		}
	}
}

// TODO: Check offsets
