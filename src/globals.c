#define GLOBALS_C
#include <sys/schedule.h>
#include <util/mutex.h>
char* os_name = "Jobbed";
#ifndef VERSION
char* os_info_v = "?";
#else
char* os_info_v = VERSION;
#endif

__attribute__((section(".bss"))) unsigned long nextpid;
__attribute__((section(".bss"))) unsigned long stimel;
__attribute__((section(".bss"))) unsigned long stimeh;
__attribute__((section(".bss"))) struct Scheduler scheduler;
__attribute__((section(".bss"))) struct MutexManager mutex_manager;
__attribute__((section(".bss"))) struct Thread usrloopthread;
__attribute__((section(".bss"))) unsigned int gwidth;
__attribute__((section(".bss"))) unsigned int gheight;
__attribute__((section(".bss"))) unsigned int gpitch;
__attribute__((section(".bss"))) unsigned int gisrgb;
__attribute__((section(".bss.mutexs"))) struct Mutex mutexs[MAX_MUTEXS];
__attribute__((section(".bss.mutexe"))) struct Entry mutex_entries[MAX_MUTEXS];
__attribute__((section(".bss.threads"))) struct Thread threads[MAX_THREADS];
__attribute__((section(".bss.threade"))) struct Entry thread_entries[MAX_THREADS];
