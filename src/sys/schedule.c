#include <globals.h>
#include <sys/schedule.h>

void init_scheduler(void)
{
	// Set rthread to usrloopthread - an infinitely running thread so that the pointer will never be null
	usrloopthread.pc = (void*)loop;
	usrloopthread.sp = 0x0FCC;
	usrloopthread.sp_base = 0x1000;
	usrloopthread.mptr = 0;
	usrloopthread.pid = -1;
	usrloopthread.priority = -1;
	usrloopthread.status = THREAD_READY;
	scheduler.rthread = &usrloopthread;
	// Initialize Rotating Buffers
}

void loop(void)
{
	while(1)
		asm volatile ("wfe");
}
