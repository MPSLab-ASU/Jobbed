#ifndef SYS_CORE_H
#define SYS_CORE_H

#ifndef SYS_CORE_C
extern char* os_info_v;
#endif

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

static inline void preserveregs(void)
{
	asm volatile("push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11}");
}

static inline void restoreregs(void)
{
	asm volatile("pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11}");
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
