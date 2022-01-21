#ifndef CPU_H
#define CPU_H

extern void _start(void);

static inline unsigned long getmode(void)
{
	unsigned long mode;
	asm volatile ("mrs %0, cpsr" : "=r"(mode));
	return mode;
}

static inline void setsvc(void)
{
	asm volatile ( "cps #0x13" );
}

static inline void setmode(unsigned long mode)
{
	asm volatile ("msr cpsr_c, %0" :: "r"(mode));
}

static inline void* getsysstack(void)
{
	void* sp;
	asm volatile (
	"mrs r0, cpsr\n"
	"bic r1, r0, #0x1F\n"
	"orr r1, r1, #0x1F\n"
	"msr cpsr_c, r1\n"
	"mov %0, sp\n"
	"msr cpsr_c, r0"
	: "=r"(sp));
	return sp;
}

static inline void* getsvcstack(void)
{
	void* sp;
	asm volatile (
	"mrs r0, cpsr\n"
	"bic r1, r0, #0x1F\n"
	"orr r1, r1, #0x13\n"
	"msr cpsr_c, r1\n"
	"mov %0, sp\n"
	"msr cpsr_c, r0"
	: "=r"(sp));
	return sp;
}

static inline void* getfiqstack(void)
{
	void* sp;
	asm volatile (
	"mrs r0, cpsr\n"
	"bic r1, r0, #0x1F\n"
	"orr r1, r1, #0x11\n"
	"msr cpsr_c, r1\n"
	"mov %0, sp\n"
	"msr cpsr_c, r0"
	: "=r"(sp));
	return sp;
}

static inline void* getirqstack(void)
{
	void* sp;
	asm volatile (
	"mrs r0, cpsr\n"
	"bic r1, r0, #0x1F\n"
	"orr r1, r1, #0x12\n"
	"msr cpsr_c, r1\n"
	"mov %0, sp\n"
	"msr cpsr_c, r0"
	: "=r"(sp));
	return sp;
}

#endif
