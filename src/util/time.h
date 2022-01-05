#ifndef UTIL_TIME_H
#define UTIL_TIME_H

void routing_core0cntv_to_core0irq(void);
unsigned long read_core0timer_pending(void);
unsigned long long read_cntvct(void);
unsigned long long read_cntvoff(void);
unsigned long read_cntv_tval(void);
void write_cntv_tval(unsigned long val);
unsigned long read_cntfrq(void);

static inline void enablecntv(void)
{
	unsigned long cntv_ctl;
	cntv_ctl = 1;
	asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl) ); // write CNTV_CTL
}

static inline void disablecntv(void)
{
	unsigned long cntv_ctl;
	cntv_ctl = 0;
	asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl) ); // write CNTV_CTL
}

#endif
