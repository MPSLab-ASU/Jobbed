#ifndef GLOBALS_H
#define GLOBALS_H
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
extern struct Thread usrloopthread;
extern unsigned int gwidth, gheight, gpitch, gisrgb;
extern unsigned char thread_table[MAX_THREADS];
extern struct Thread threads[MAX_THREADS];
extern unsigned long mutex_table[MAX_MUTEXS];
extern struct Mutex mutexs[MAX_MUTEXS];
#endif

#endif
