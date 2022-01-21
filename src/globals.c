#define GLOBALS_C
#include <lib/mem.h>
#include <util/mutex.h>
#include <graphics/drawer.h>
#include <sys/schedule.h>
char* os_name = "Jobbed";
#ifndef VERSION
char* os_info_v = "?";
#else
char* os_info_v = VERSION;
#endif

__attribute__((section(".bss"))) unsigned long exe_cnt;
__attribute__((section(".bss"))) struct Mutex exe_cnt_m;
__attribute__((section(".bss.mmheap"))) unsigned char rpi_heap[MAX_MM];
__attribute__((section(".bss"))) void* rpi_heap_top;
__attribute__((section(".bss"))) unsigned long nextpid;
__attribute__((section(".bss"))) unsigned long stimel;
__attribute__((section(".bss"))) unsigned long stimeh;
__attribute__((section(".bss"))) struct Drawer g_Drawer;
__attribute__((section(".bss"))) struct Scheduler scheduler;
__attribute__((section(".bss"))) struct Thread usrloopthread;
__attribute__((section(".bss"))) unsigned int gwidth;
__attribute__((section(".bss"))) unsigned int gheight;
__attribute__((section(".bss"))) unsigned int gpitch;
__attribute__((section(".bss"))) unsigned int gisrgb;
__attribute__((section(".bss"))) unsigned char stacks_table[MAX_THREADS];
