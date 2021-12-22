#ifndef CPU_IRQ_H
#define CPU_IRQ_H

static inline void enableirq() {
	asm volatile("cpsie i");
}
static inline void disableirq() {
	asm volatile("cpsid i");
}
static inline void enablefiq() {
	asm volatile("cpsie f");
}
static inline void disablefiq() {
	asm volatile("cpsid f");
}

void c_irq_handler(void);

#endif
