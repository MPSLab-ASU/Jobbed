#ifndef GLOBALS_H
#define GLOBALS_H
#include <lib/mem.h>
#include <sys/schedule.h>

#ifndef GLOBALS_C
extern unsigned long irqlr;
extern unsigned long cntfrq;
extern unsigned long cmdidx;
extern char cmd[2048];

extern char* os_name;
extern char* os_info_v;
extern unsigned long exe_cnt;
extern struct Mutex exe_cnt_m;
extern unsigned char rpi_heap[MAX_MM];
extern void* rpi_heap_top;
extern unsigned long nextpid;
extern unsigned long stimel;
extern unsigned long stimeh;
extern struct Drawer g_Drawer;
extern struct Scheduler scheduler;
extern unsigned int gwidth, gheight, gpitch, gisrgb;
extern unsigned char stacks_table[MAX_THREADS];
#endif

#endif
