#ifndef GLOBALS_H
#define GLOBALS_H
#include <lib/mem.h>
#include <sys/schedule.h>

#ifndef GLOBALS_C
extern unsigned long irqlr;
extern unsigned long cntfrq;

extern char* os_name;
extern char* os_info_v;
unsigned char kmem_begin[0x2000000];
unsigned char kmem_lookup[0xD000];
extern unsigned long exe_cnt;
extern struct Mutex exe_cnt_m;
extern unsigned char rpi_heap[MAX_MM];
extern void* rpi_heap_top;
extern unsigned long nextpid;
extern unsigned long stimel;
extern unsigned long stimeh;
extern struct Drawer g_Drawer;
extern struct Scheduler scheduler;
extern struct Thread usrloopthread;
extern unsigned int gwidth, gheight, gpitch, gisrgb;
extern unsigned char stacks_table[MAX_THREADS];
#endif

#endif
