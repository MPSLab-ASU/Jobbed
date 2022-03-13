#include <util/mutex.h>
#include <globals.h>

struct Mutex* create_mutex(void* addr)
{
	for (unsigned long m = 0; m < MAX_MUTEXS; m++) {
		if (mutex_table[m] == 0) {
			mutex_table[m] = (unsigned long)addr;
			mutexs[m].pid = 0;
			mutexs[m].addr = addr;
			return &mutexs[m];
		}
	}
	return 0;
}

unsigned char delete_mutex(struct Mutex* m)
{
	for (unsigned long i = 0; i < MAX_MUTEXS; i++) {
		if (mutex_table[i] == (unsigned long)m->addr) {
			mutex_table[i] = 0;
			return 0;
		}
	}
	return 1;
}
