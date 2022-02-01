.section ".text.exceptions"
.globl svc
svc:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	ldr r0, [lr, #-4]
	bic r0, #0xFF000000
	// SVC #0 returns to supervisor mode
	// TODO: Make supervisor mode return to a specific location
	//  (rather than to a user location) such as the kernel loop
	cmp r0, #3
	bgt svc_exit
	beq svc_000003
	cmp r0, #2
	beq svc_000002
	cmp r0, #1
	beq svc_000001
	cmp r0, #1
	beq svc_000000
svc_000000:
	cps #0x13
	b svc_exit
svc_000001:
	b svc_exit
svc_000002:
	ldmfd sp!, {r0-r12,lr}
	b schedule
svc_000003:
	ldr r3, =scheduler
	ldr r2, [r3, #0]
	ldr r1, [r2, #8] // sp_base
	cmp r1, #-1
	beq svc_exit
	ldr r3, =stacks_table
	mov r0, #0
	strb r0, [r3, r1]
	// Free the thread after freeing the stack
	mov r0, r2
	bl free
	b svc_exit
svc_exit:
	ldmfd sp!, {r0-r12,pc}^

.section .data
svc_msg:	.asciz "SVC Handler #"
