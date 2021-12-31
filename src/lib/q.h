#ifndef LIB_Q_H
#define LIB_Q_H

struct Q_base {
	struct Q* next;
	struct Q* last;
};

struct Q {
	struct Q* next;
	void* data;
};

struct Q_base* newq();
void pushq(struct Q_base* qb, void* val);
void popq(struct Q_base* qb);

#define showq(QQ, TYPE) { \
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
