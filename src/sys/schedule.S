// TODO: - Scheduler info function
//       - Get next thread
//       - Yield supervisor call
.section ".text"
.globl schedule
.macro preserve_ctx
	cps #0x1f // Sys mode
	// Store Usr regs
	push {r0-r12}
	push {lr}
	ldr r3, =scheduler // struct Scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	str lr, [r2, #0] // svc_lr -> void* pc
	str sp, [r2, #4] // svc_lr -> void* sp
	cps #0x13 // Svc mode
.endm

.macro restore_ctx
	ldr r3, =scheduler // struct Scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	ldr lr, [r2, #0] // void* pc -> lr_svc
	ldr r0, [r2, #4] // void* sp -> r0
	cps #0x1f // Sys mode
	mov sp, r0 // Set stack pointer
	// Restore Usr regs
	pop {lr}
	pop {r0-r12}
	cps #0x13 // Svc mode
.endm

// Assumption: Enter in SVC mode
schedule:
	preserve_ctx
	ldr r1, =irqlr
	ldr r0, [r1]
	cmp r0, #0
	beq 1f
	// Replace LR with IRQ's LR
	ldr r3, =scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	str r0, [r2, #0] // svc_lr -> void* pc
	// Clear IRQ's LR
	mov r0, #0
	str r0, [r1]
1:
	bl next_thread // Thread* next -> r0
	ldr r3, =scheduler
	ldr r2, [r3, #0] // Thread* current
	cmp r0, r2 // current = next?
	beq 2f
	str r0, [r3, #0] // next -> rthread
2:
	restore_ctx
	subs pc, lr, #0

.globl cleanup
cleanup:
	// roffset++
	bl get_rthread_roffset
	ldr r1, [r0, #0]
	add r1, #1
	str r1, [r0, #0]
	// cleanup stack
	svc #3
	// usrloop -> rthread
	ldr r3, =scheduler
	ldr r2, =usrloopthread
	str r2, [r3, #0]
	ldr sp, [r2, #4]
	ldmfd sp!,{lr}
	ldmfd sp!,{r0-r12}
	ldr lr, =kernel_usr_task_loop
	// svc sched
	svc #2
.globl kernel_usr_task_loop
kernel_usr_task_loop:
	wfe
	//svc #2
	b kernel_usr_task_loop
