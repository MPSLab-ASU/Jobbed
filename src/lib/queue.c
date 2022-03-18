#include <lib/queue.h>

void push_to_queue(struct Entry* e, struct Queue* q)
{
	q->end.next->next = e;
	q->end.next = e;
	e->next = &q->end;
}

void prepend_to_queue(struct Entry* e, struct Queue* q)
{
	e->next = q->start.next;
	q->start.next = e;
	if (e->next->entry_type == END_ENTRY)
		q->end.next = e;
}

struct Entry* pop_from_queue(struct Queue* q)
{
	if (q->start.next->entry_type == END_ENTRY)
		return 0;
	struct Entry* e = q->start.next;
	q->start.next = e->next;
	if (e->next->entry_type == END_ENTRY)
		q->end.next = &q->start;
	return e;
}

struct Entry* remove_next_from_queue(struct Entry* e)
{
	struct Entry* prev = e;
	struct Entry* remove = e->next;
	struct Entry* next = remove->next;
	if (remove->entry_type != VALUE_ENTRY)
		return 0;
	prev->next = next;
	if (next->entry_type == END_ENTRY)
		next->next = prev;
	return remove;
}

struct Entry* find_value(void* value, struct Queue* q)
{
	struct Entry* prev;
	struct Entry* entry;
	prev = &q->start;
	entry = prev->next;
	while (entry->entry_type != END_ENTRY) {
		if (entry->value == value)
			return prev;
		prev = entry;
		entry = prev->next;
	}
	return 0;
}
