.section ".text.exceptions"
.globl svc
svc:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	// Get the SVC Exception #
	ldr r0, [lr, #-4]
	bic r0, #0xFF000000
	// Check it is within our defined SVC
	cmp r0, #7
	adrle r3, svc_table_1
	ldrle pc, [r3, r0, LSL #2]
	sub r0, #8
	cmp r0, #7
	bgt svc_exit
	//// Jump to the appropriate Call
	adr r3, svc_table_2
	ldr pc, [r3, r0, LSL #2]
svc_000000: // SYS_YIELD
	bl yield
	ldmfd sp!, {r0-r12,lr}
	b schedule
svc_000001: // SYS_TIME
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
svc_000003: // Add Thread
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	ldr r2, [sp, #8]
	and r2, #0xFF
	bl svc_add_thread
	str r0, [sp, #0]
	ldmfd sp!, {r0-r12,lr}
	b schedule
svc_000004: // Lock Lock (usr_r0 = struct Lock*)
	ldr r3, =scheduler
	ldr r2, [r3, #0] // struct Thread* rthread
	ldr r1, [r2, #0x10] // unsigned long pid
	ldr r0, [sp, #0] // struct Lock* m
1:	clrex
	ldrex r2, [r0, #0]
	cmp r2, #0
	// If it is not available, wait-queue the thread
	bne svc_000004_delay_mutex
	// Otherwise lock it
	strexeq r2, r1, [r0, #0]
	teq r2, #0
	bne 1b
	dmb
	b svc_exit
svc_000004_delay_mutex: // Wait-queue the current thread
	// r0 = struct Lock* m
	bl sched_mutex_yield
	ldmfd sp!, {r0-r12,lr}
	sub lr, #4
	b schedule
svc_000005: // Release Lock
	ldr r0, [sp, #0] // struct Lock* m
	mov r1, #0
	dmb
	// Unlock
	str r1, [r0, #0]
	dsb
	sev
	// Awake any threads waiting for this lock
	bl sched_mutex_resurrect
	ldmfd sp!, {r0-r12,lr}
	b schedule
	b svc_exit
svc_000006: // Semaphore decrease
	ldr r0, [sp, #0] // struct Semaphore* s
1:	clrex
	ldrex r2, [r0, #0]
	cmp r2, #0
	beq svc_000006_delay_semaphore
	sub r1, r2, #1
	strex r2, r1, [r0, #0]
	teq r2, #0
	bne 1b
	dmb
	b svc_exit
svc_000006_delay_semaphore:
	bl sched_semaphore_yield
	ldmfd sp!, {r0-r12,lr}
	sub lr, #4
	b schedule
	b svc_exit
svc_000007: // Semaphore increase
	ldr r0, [sp, #0] // struct Semaphore* s
1:	clrex
	ldrex r2, [r0, #0]
	add r1, r2, #1
	strexeq r2, r1, [r0, #0]
	teq r2, #0
	bne 1b
	dmb
	cmp r1, #1
	bne svc_exit
	mov r1, #1
	bl sched_semaphore_resurrect
	ldmfd sp!, {r0-r12,lr}
	b schedule
	b svc_exit
svc_000008: // Semaphore add #
	ldr r0, [sp, #0] // struct Semaphore* s
	ldr r3, [sp, #1] // unsigned long # times to increase
1:	clrex
	ldrex r2, [r0, #0]
	add r1, r2, #1
	strexeq r2, r1, [r0, #0]
	teq r2, #0
	bne 1b
	dmb
	mov r1, r3
	bl sched_semaphore_resurrect
	ldmfd sp!, {r0-r12,lr}
	b schedule
	b svc_exit
svc_000009: // SYS_TIME_2
	mrc p15, 0, r0, c14, c3, 0
	str r0, [sp, #0]
	b svc_exit
svc_exit:
	ldmfd sp!, {r0-r12,pc}^

svc_table_1:
	.word svc_000000
	.word svc_000001
	.word svc_000002
	.word svc_000003
	.word svc_000004
	.word svc_000005
	.word svc_000006
	.word svc_000007
svc_table_2:
	.word svc_000008
	.word svc_000009
