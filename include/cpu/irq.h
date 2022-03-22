#ifndef CPU_IRQ_H
#define CPU_IRQ_H

static inline void enableirq(void)
{
	asm volatile("cpsie i");
}
static inline void disableirq(void)
{
	asm volatile("cpsid i");
}
static inline void enablefiq(void)
{
	asm volatile("cpsie f");
}
static inline void disablefiq(void)
{
	asm volatile("cpsid f");
}

unsigned long c_irq_handler(void);
unsigned long c_fiq_handler(void);

#endif
