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

#define syscall_h_quote(text)	#text
#define syscall_h_expand_and_quote(text)	syscall_h_quote(text)

#define sys0(sys_n) asm volatile("svc #" syscall_h_expand_and_quote(sys_n) ::: "r0", "r1", "r2", "r3");
#define sys1(sys_n,arg0) asm volatile("svc #" syscall_h_expand_and_quote(sys_n) ::[r0]"r"(arg0): "r0", "r1", "r2", "r3");

__attribute__((always_inline)) static inline unsigned long long get_sys_time(void)
{
	union {
		struct {
			unsigned long lo;
			unsigned long hi;
		}s;
		unsigned long long llv;
	}t;
	asm volatile("svc #1\nmov %0, r1\nmov %1, r0" : "=r"(t.s.lo), "=r"(t.s.hi));
	return t.llv;
}

#define SYS_YIELD       0
#define SYS_TIME        1
#define SYS_SCHED       2
#define SYS_FREE_STACK  3

#endif
