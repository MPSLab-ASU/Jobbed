#ifndef LIB_QUEUE_H
#define LIB_QUEUE_H

enum EntryType {
	VALUE_ENTRY = 0,
	START_ENTRY = 1,
	END_ENTRY = 2,
};

struct Entry {
	void* value;
	struct Entry* next;
	unsigned long entry_type;
};

struct Queue {
	struct Entry start;
	struct Entry end;
};

// Add to end of queue
void push_to_queue(struct Entry* e, struct Queue* q);
// Add to beginning of queue
void prepend_to_queue(struct Entry* e, struct Queue* q);
// Remove from beginning of queue
struct Entry* pop_from_queue(struct Queue* q);
// Remove the entry after this one from its queue
struct Entry* remove_next_from_queue(struct Entry* e);
// Find an entry in a queue
struct Entry* find_value(void* value, struct Queue* q);

#endif
