#include <lib/ll.h>
#include <lib/mem.h>

struct LL* new_ll(void* val)
{
	struct LL* ll = (struct LL*)malloc(sizeof(struct LL));
	ll->prev = ll;
	ll->next = ll;
	ll->data = val;
	return ll;
}

void push_ll(struct LL* l, void* val)
{
	struct LL* ll = (struct LL*)malloc(sizeof(struct LL));
	l->prev->next = ll;
	ll->prev = l->prev;
	ll->next = l;
	l->prev = ll;
	ll->data = val;
}

void pop_ll(struct LL* l)
{
	if ((l->prev == l->next) && (l->prev == l))
		l->data = 0;
	else {
		l->prev->next = l->next;
		l->next->prev = l->prev;
		free(l);
	}
}

void remove_ll(struct LL* l, unsigned long idx)
{
	struct LL* t = l;
	for(unsigned long i = 0; i < idx; i++) {
		t = t->next;
	}
	t->prev->next = t->next;
	t->next->prev = t->prev;
	free(t);
}

unsigned long length_ll(struct LL* l)
{
	struct LL* t = l;
	unsigned long len = 0;
	while (1) {
		if (t->next == l) {
			return len;
		}
		len++;
		t = t->next;
	}
	return len;
}
