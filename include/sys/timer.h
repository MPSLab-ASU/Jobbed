#ifndef SYS_TIMER_H
#define SYS_TIMER_H

/// Cycles Per Second
#define CPS 100

#ifndef SYS_TIMER_C
extern struct Mutex exe_cnt_m;
#endif

void c_timer(void);

#endif
