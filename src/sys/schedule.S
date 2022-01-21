.section ".text"
.globl schedule
.macro preserve_ctx
	cps #0x1f // Sys mode
	// Store Usr regs
	stmfd sp!, {r0-r12}
	stmfd sp!, {lr}
	ldr r3, =scheduler // struct Scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	str lr, [r2, #0] // svc_lr -> void* pc
	str sp, [r2, #4] // svc_lr -> void* sp
	cps #0x10 // Svc mode
.endm

.macro restore_ctx
	ldr r3, =scheduler // struct Scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	ldr lr, [r2, #0] // void* pc -> lr_svc
	ldr r0, [r2, #4] // void* sp -> r0
	cps #0x1f // Sys mode
	mov sp, r0 // Set stack pointer
	// Restore Usr regs
	ldmfd sp!, {lr}
	ldmfd sp!, {r0-r12}
	cps #0x10 // Svc mode
.endm

// Assumption: Enter in SVC mode
schedule:
	preserve_ctx
	ldr r1, =irqlr
	ldr r0, [r1]
	cmp r0, #0
	beq 1f
	// Replace LR with IRQ's LR
	mov lr, r0
	// Clear IRQ's LR
	mov r0, #0
	str r0, [r1]
1:
	// bl get_next_thread // next_thread -> r0
	ldr r3, =scheduler
	ldr r2, [r3, #0] // struct Thread* current
	cmp r0, r2 // current = next?
	beq 2f
	str r0, [r3, #0] // next -> rthread
2:
	restore_ctx
	movs pc, lr
