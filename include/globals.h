#ifndef GLOBALS_H
#define GLOBALS_H
#include <cpu/irq.h>
#include <sys/schedule.h>
#include <util/mutex.h>

#ifndef GLOBALS_C
extern unsigned long irqlr;
extern unsigned long cntfrq;

extern char* os_name;
extern char* os_info_v;
extern unsigned long nextpid;
extern unsigned long stimel;
extern unsigned long stimeh;
extern struct Scheduler scheduler;
extern struct MutexManager mutex_manager;
extern struct Thread usrloopthread;
extern unsigned int gwidth, gheight, gpitch, gisrgb;
extern struct Mutex mutexs[MAX_MUTEXS];
extern struct Entry mutex_entries[MAX_MUTEXS];
extern struct Thread threads[MAX_THREADS];
extern struct Entry thread_entries[MAX_THREADS];
extern struct IrqEntry irqs[MAX_IRQS];
#endif

#endif
