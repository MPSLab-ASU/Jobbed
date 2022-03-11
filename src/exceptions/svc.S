.section ".text.exceptions"
.globl svc
svc:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	ldr r0, [lr, #-4]
	bic r0, #0xFF000000
	cmp r0, #5
	bgt svc_exit
	beq svc_000005
	cmp r0, #4
	beq svc_000004
	cmp r0, #3
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
svc_000003: // Free Thread in Table
	ldr r3, =scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	ldr r1, [r2, #0x1c] // thread offset
	mov r0, #0
	ldr r2, =thread_table
	add r2, r1, r2
	str r0, [r2]
	b svc_exit
svc_000004: // Lock Lock (usr_r0 = struct Lock*)
	ldr r3, =scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	ldr r1, [r2, #0x10] // unsigned long pid
	ldr r0, [sp, #0] // struct Lock* m
1:	clrex
	ldrex r2, [r0, #0]
	cmp r2, #0
	bne svc_000004_delay_mutex
	strexeq r2, r1, [r0, #0]
	teq r2, #0
	bne 1b
	dmb
	b svc_exit
svc_000004_delay_mutex:
	// r0 = struct Lock* m
	bl sched_mutex_yield
	ldmfd sp!, {r0-r12,lr}
	b schedule
svc_000005: // Release Lock
	ldr r0, [sp, #0] // struct Lock* m
	mov r1, #0
	dmb
	str r1, [r0, #0]
	dsb
	sev
	bl sched_mutex_resurrect
	ldmfd sp!, {r0-r12,lr}
	b schedule
	b svc_exit
svc_exit:
	ldmfd sp!, {r0-r12,pc}^
