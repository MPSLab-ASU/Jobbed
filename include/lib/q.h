#ifndef LIB_Q_H
#define LIB_Q_H

struct Q_base {
	struct Q* next;
	struct Q* last;
} __attribute__((packed));

struct Q {
	struct Q* next;
	void* data;
} __attribute__((packed));

struct Q_base* new_q();
void push_q(struct Q_base* qb, void* val);
void pop_q(struct Q_base* qb);
unsigned long length_q(struct Q_base* qb);

#define show_q(QQ, TYPE) { \
	if (QQ->next != 0) { \
		struct Q* t = QQ->next; \
		while (t->next != 0) { \
			uart_hex(*(TYPE*)t->data); \
			t = t->next; \
			uart_char(' '); \
		} \
		uart_hex(*(TYPE*)t->data); \
	} \
}

#endif
