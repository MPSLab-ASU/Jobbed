#include <cpu.h>
#include <drivers/uart.h>
#include <util/mutex.h>
#include <util/lock.h>
#include <globals.h>

void mutex_init(void)
{
	for (unsigned long m = 0; m < MAX_MUTEXS; m++) {
		mutexs[m].pid = 0;
		mutexs[m].addr = 0;
		mutex_entries[m].value = &mutexs[m];
		mutex_entries[m].entry_type = VALUE_ENTRY;
		mutex_entries[m].next = &mutex_entries[m+1];
	}
	// Initialize Free Mutexs
	mutex_manager.free.start.value = 0;
	mutex_manager.free.start.next = &mutex_entries[0];
	mutex_manager.free.start.entry_type = START_ENTRY;
	mutex_manager.free.end.value = 0;
	mutex_manager.free.end.next = &mutex_entries[MAX_MUTEXS-1];
	mutex_entries[MAX_MUTEXS-1].next = &mutex_manager.free.end;
	mutex_manager.free.end.entry_type = END_ENTRY;
	// Initialize In-use Mutexs
	mutex_manager.used.start.value = 0;
	mutex_manager.used.start.next = &mutex_manager.used.end;
	mutex_manager.used.start.entry_type = START_ENTRY;
	mutex_manager.used.end.value = 0;
	mutex_manager.used.end.next = &mutex_manager.used.start;
	mutex_manager.used.end.entry_type = END_ENTRY;
}

struct Mutex* create_mutex(void* addr)
{
	struct Entry* e = pop_from_queue(&mutex_manager.free);
	if (e == 0)
		return 0;
	struct Mutex* m = e->value;
	m->pid = 0;
	m->addr = addr;
	push_to_queue(e, &mutex_manager.used);
	return e->value;
}

unsigned char delete_mutex(struct Mutex* m)
{
	struct Entry* entry = find_value(m, &mutex_manager.used);
	if (entry == 0)
		return 1;
	// Remove it from the queue
	struct Entry* theentry = remove_next_from_queue(entry);
	// Add it to the free queue
	prepend_to_queue(theentry, &mutex_manager.free);
	return 0;
}

void uart_mutexes(void)
{
	struct Entry* entry = mutex_manager.used.start.next;
	while (entry->entry_type == VALUE_ENTRY)
	{
		struct Mutex* m = entry->value;
		uart_hex((unsigned long)m);
		uart_char(' ');
		uart_hex(m->pid);
		uart_char(' ');
		uart_hexn((unsigned long)m->addr);
		entry = entry->next;
	}
	unsigned long count = 0;
	entry = mutex_manager.free.start.next;
	while (entry->entry_type == VALUE_ENTRY) {
		count++;
		entry = entry->next;
	}
	uart_hexn(count);
}

void lock_mutex(struct Mutex* m)
{
	struct Thread* rthread = scheduler.rthread;
	unsigned long rpid = rthread->pid;
	unsigned long mode = getmode() & 0x1F;
	if (mode == 0x10) {
		// Find this mutex
		struct Entry* mentry = find_value(m, &mutex_manager.used);
		// If it is not a managed mutex, break away
		if (mentry == 0)
			return;
		struct Entry* entry = mutex_manager.used.start.next;
		// Ensure this thread locks all mutexs sequentially
		//  To avoid a deadlock
		while (entry->entry_type == VALUE_ENTRY) {
			struct Mutex* vmutex = entry->value;
			// If this thread had locked it
			//  Toggle the lock to prevent deadlock
			if (vmutex->pid == rpid) {
				sys1(SYS_UNLOCK, vmutex);
				sys1(SYS_LOCK, vmutex);
			}
			entry = entry->next;
		}
		sys1(SYS_LOCK, m);
	}
}

void unlock_mutex(struct Mutex* m)
{
	unlock((struct Lock*)m);
}
