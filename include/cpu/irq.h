#ifndef CPU_IRQ_H
#define CPU_IRQ_H

#define UART_IRQ 0
#define SYS_TIMER_0_IRQ 1
#define SYS_TIMER_1_IRQ 2
#define SYS_TIMER_2_IRQ 3
#define SYS_TIMER_3_IRQ 4
#define LOCAL_TIMER_IRQ 5
#define MAX_IRQS LOCAL_TIMER_IRQ+1

struct IrqEntry {
	void* handler;
	void* handler_info;
};

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
void subscribe_irq(unsigned long irq_num, void* handler, void* handler_info);
void unsubscribe_irq(unsigned long irq_num);

#endif
