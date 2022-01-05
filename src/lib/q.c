#include "../lib/q.h"
#include "../lib/mem.h"

struct Q_base* new_q()
{
	struct Q_base* q = (struct Q_base*)malloc(sizeof(struct Q_base));
	q->next = 0;
	q->last = 0;
	return q;
}

void push_q(struct Q_base* qb, void* val)
{
	struct Q* n = (struct Q*)malloc(sizeof(struct Q));
	n->next = 0;
	n->data = val;
	if (qb->last != 0)
		qb->last->next = n;
	else {
		qb->next = n;
	}
	qb->last = n;
}

void pop_q(struct Q_base* qb)
{
	if (qb->next == 0)
		return;
	if (qb->next == qb->last) {
		free(qb->next->data);
		free(qb->next);
		qb->next = 0;
		qb->last = 0;
		return;
	}
	struct Q* t = qb->next;
	qb->next = qb->next->next;
	free(t->data);
	free(t);
}
