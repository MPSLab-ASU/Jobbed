#ifndef SYS_CORE_H
#define SYS_CORE_H

static inline unsigned long load32(unsigned long addr)
{
	return *(volatile unsigned long*)addr;
}

static inline void store32(unsigned long value, unsigned long addr)
{
	*(volatile unsigned long*)addr = value;
}

static inline void delay(unsigned long cycles)
{
	asm volatile("__delay_%=: subs %[cycles], %[cycles], #1;bne __delay_%=\n"
			: "=r"(cycles): [cycles]"0"(cycles) : "cc");
}

static inline void* getlr(void)
{
	void* out;
	asm volatile ("mov %0, lr" : "=r"(out));
	return out;
}

static inline void* getpc(void)
{
	void* out;
	asm volatile ("mov %0, pc" : "=r"(out));
	return out;
}

static inline void* getsp(void)
{
	void* out;
	asm volatile ("mov %0, sp" : "=r"(out));
	return out;
}

static inline void setsp(void* in)
{
	asm volatile ("mov sp, %0" :: "r"(in));
}

static inline void* heap_end(void)
{
	unsigned long value;
	asm volatile ("ldr %0, =__bss_end": "=r"(value));
	return (void*)value;
}

void sysinit(void);

#endif
