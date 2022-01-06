#ifndef LIB_LL_H
#define LIB_LL_H

struct LL {
	struct LL* prev;
	struct LL* next;
	void* data;
} __attribute__((packed));

struct LL* new_ll(void* val);
void push_ll(struct LL* l, void* val);
void pop_ll(struct LL* l);
void remove_ll(struct LL* l, unsigned long idx);
unsigned long length_ll(struct LL* l);

#define show_ll(L, TYPE) { \
	struct LL* t = L; \
	do { \
		uart_hex(*(TYPE*)t->data); \
		t = t->next; \
		if (t != l) \
			uart_char(' '); \
	} while (t != l); \
}

#endif
