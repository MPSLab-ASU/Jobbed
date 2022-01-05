#ifndef LIB_LL_H
#define LIB_LL_H

struct LL {
	struct LL* prev;
	struct LL* next;
	void* data;
};

struct LL* new_ll(void* val);
void push_ll(struct LL* l, void* val);
void remove_ll(struct LL* l, unsigned long idx);

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
