.section ".text.exceptions"
.globl svc
svc:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	ldr r0, [lr, #-4]
	bic r0, #0xFF000000
	cmp r0, #3
	bgt svc_exit
	beq svc_000003
	cmp r0, #2
	beq svc_000002
	cmp r0, #1
	beq svc_000001
	cmp r0, #0
	beq svc_000000
svc_000000:
	bl yield
	ldmfd sp!, {r0-r12,lr}
	b schedule
svc_000001: // Get time
	mov r2, #0x3004
	movt r2, #0x3F00
	ldr r0, [r2, #4] // <- SYS_TIMER_CLO
	ldr r1, [r2, #0] // <- SYS_TIMER_CHI
	str r0, [sp]	 // Return value
	str r1, [sp, #4] // Return value hi
	b svc_exit
svc_000002: // Run Schedule
	ldmfd sp!, {r0-r12,lr}
	b schedule
svc_000003: // Clean task stack
	ldr r3, =scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	ldr r1, [r2, #8] // sp_base
	cmp r1, #-1
	beq svc_exit
	ldr r3, =stacks_table
	mov r0, #0
	strb r0, [r3, r1]
	// Free the thread after freeing the stack
	mov r0, r2
	bl kfree
	b svc_exit
svc_exit:
	ldmfd sp!, {r0-r12,pc}^

.section .data
svc_msg:	.asciz "SVC Handler #"
