static inline unsigned long load32(unsigned long addr) {
	return *(volatile unsigned long*)addr;
}

static inline void store32(unsigned long value, unsigned long addr) {
	*(volatile unsigned long*)addr = value;
}

#define uint64_t unsigned long long
#define uint32_t unsigned long

#define CORE0_TIMER_IRQCNTL 0x40000040
#define CORE0_IRQ_SOURCE 0x40000060

void routing_core0cntv_to_core0irq(void)
{
	// IRQ
	store32(0x08, CORE0_TIMER_IRQCNTL);
	// FIQ
	//store32(0x80, CORE0_TIMER_IRQCNTL);
}

uint32_t read_core0timer_pending(void)
{
	uint32_t tmp;
	tmp = load32(CORE0_IRQ_SOURCE);
	return tmp;
}

void enable_cntv(void)
{
	uint32_t cntv_ctl;
	cntv_ctl = 1;
	asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl) ); // write CNTV_CTL
}

void disable_cntv(void)
{
	uint32_t cntv_ctl;
	cntv_ctl = 0;
	asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl) ); // write CNTV_CTL
}

uint64_t read_cntvct(void)
{
	uint64_t val;
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (val));
	return (val);
}

uint64_t read_cntvoff(void)
{
	uint64_t val;
	asm volatile("mrrc p15, 4, %Q0, %R0, c14" : "=r" (val));
	return (val);
}

uint32_t read_cntv_tval(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c14, c3, 0" : "=r"(val) );
	return val;
}

void write_cntv_tval(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c14, c3, 0" :: "r"(val) );
	return;
}

uint32_t read_cntfrq(void)
{
	uint32_t val;
	asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r"(val) );
	return val;
}
