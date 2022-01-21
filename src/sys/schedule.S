.section ".text"
.globl schedule

// Implicit: SVC Mode
.macro preserve_ctx
	cps #0x1f // Sys mode
	push {r0-r12, lr} // Restore usr regs
	cps #0x10 // Svc mode
	ldr r3, =scheduler // struct Scheduler
	ldr r3, [r3, #4] // struct Thread* rthread
	str lr, [r3, #0] // svc_lr -> void* pc
.endm

.macro restore_ctx
	ldr r3, =scheduler // struct Scheduler
	ldr r3, [r3, #4] // struct Thread* rthread
	ldr lr, [r3, #0] // void* pc -> lr_svc
	cps #0x1f // Sys mode
	pop {r0-r12,lr} // Restore usr regs
	cps #0x10 // Svc mode
.endm

schedule:
	//preserve_ctx
	//restore_ctx
	//bx lr
