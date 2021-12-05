#ifndef TIME_H
#define TIME_H

void routing_core0cntv_to_core0irq(void);
unsigned long read_core0timer_pending(void);
void enable_cntv(void);
void disable_cntv(void);
unsigned long long read_cntvct(void);
unsigned long long read_cntvoff(void);
unsigned long read_cntv_tval(void);
void write_cntv_tval(unsigned long val);
unsigned long read_cntfrq(void);

#endif
