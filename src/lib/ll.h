#ifndef LIB_LL_H
#define LIB_LL_H

struct LL {
	struct LL* prev;
	struct LL* next;
	void* data;
};

struct LL* new(void* val);
void push(struct LL* l, void* val);
void remove(struct LL* l, unsigned long idx);

#define showl(L, TYPE) { \
	struct LL* t = L; \
	do { \
		uart_hex(*(TYPE*)t->data); \
		t = t->next; \
		if (t != l) \
			uart_char(' '); \
	} while (t != l); \
}

#endif
