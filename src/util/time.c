#include <symbols.h>
#include <sys/core.h>

// CCNT - Cycle Timer (Close to ns resolution)

void routing_core0cntv_to_core0fiq(void)
{
	store32(0x80, CORE0_TIMER_IRQCNTL);
}

void routing_core0cntv_to_core0irq(void)
{
	store32(0x08, CORE0_TIMER_IRQCNTL);
}

unsigned long read_core0timer_pending(void)
{
	unsigned long tmp;
	tmp = load32(CORE0_IRQ_SOURCE);
	return tmp;
}

unsigned long long read_cntvct(void)
{
	unsigned long long val;
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (val));
	return (val);
}

unsigned long long read_cntvoff(void)
{
	unsigned long long val;
	asm volatile("mrrc p15, 4, %Q0, %R0, c14" : "=r" (val));
	return (val);
}

unsigned long read_cntv_tval(void)
{
	unsigned long val;
	asm volatile ("mrc p15, 0, %0, c14, c3, 0" : "=r"(val) );
	return val;
}

void write_cntv_tval(unsigned long val)
{
	asm volatile ("mcr p15, 0, %0, c14, c3, 0" :: "r"(val) );
	return;
}

unsigned long read_cntfrq(void)
{
	unsigned long val;
	asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r"(val) );
	return val;
}

unsigned long long get_time(void)
{
	union {
		unsigned long long tval;
		struct {
			unsigned long high;
			unsigned long low;
		} tvalb;
	} t;
	t.tvalb.low = *(unsigned long*)SYS_TIMER_CLO;
	t.tvalb.high = *(unsigned long*)SYS_TIMER_CHI;
	return t.tval;
}

void wait_msec(unsigned int n)
{
	unsigned long start = *(volatile unsigned long*)SYS_TIMER_CHI;
	while (*(volatile unsigned long*)SYS_TIMER_CHI - start < n)
		asm volatile("nop");
}
