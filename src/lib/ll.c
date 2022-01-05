#include "../lib/ll.h"
#include "../lib/mem.h"

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
	ll->prev = l->prev;
	ll->next = l;
	ll->prev->next = ll;
	l->prev = ll;
	ll->data = val;
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
