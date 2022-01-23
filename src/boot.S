// To keep this in the first portion of the binary.
.section ".text.boot"
 
// Make _start global.
.globl _start
 
_start:
reset:
	cpsid if
	// disable core0,1,2.
	mrc p15, #0, r1, c0, c0, #5
	and r1, r1, #3
	cmp r1, #1
	beq core1run
	cmp r1, #2
	beq core2run
	cmp r1, #3
	bge core3run

	// set vector address.
	ldr r0, =vector
	mcr p15, 0, r0, c12, c0, 0

	// save cpsr.
	mrs r0, cpsr

	// setup sp in IRQ mode.
	///bic r1, r0, #0x1f
	///orr r1, r1, #0x12
	///msr cpsr_c,r1
	cps #0x12
	mov sp,#0x4000
	// setup sp in FIQ mode.
	///bic r1, r0, #0x1f
	///orr r1, r1, #0x11
	///msr cpsr_c,r1
	cps #0x11
	mov sp,#0x2000
	// setup sp in UNDEF mode.
	///bic r1, r0, #0x1f
	///orr r1, r1, #0x1B
	///msr cpsr_c,r1
	cps #0x1B
	mov sp,#0x1000
	// setup sp in ABORT mode.
	///bic r1, r0, #0x1f
	///orr r1, r1, #0x17
	///msr cpsr_c,r1
	cps #0x17
	mov sp,#0x0800
	// Setup sp in USR/SYS mode.
	///bic r1, r0, #0x1f
	///orr r1, r1, #0x1f
	///msr cpsr_c,r1
	cps #0x1f
	mov sp,#0x6000

	// restore to SVC (in the case of a reset)
	///bic r1, r0, #0x1f
	///orr r1, r1, #0x13
	///msr cpsr_c, r1
	//msr cpsr_c, r0

	// setup the stack in SVC mode.
	cps #0x13
	mov sp, #0x8000
 
	// Clear out bss.
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b       2f
 
1:
	// store multiple at r4.
	stmia r4!, {r5-r8}
 
	// If we are still below bss_end, loop.
2:
	cmp r4, r9
	blo 1b

	// Call kernel_main
	ldr r3, =kernel_main
	blx r3

irq:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	bl c_irq_handler
	pop  {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	subs pc, lr, #4

// TODO: Each core needs to set up their stacks
core1run:
core2run:
core3run:
.globl io_halt
io_halt:
	wfi
	b io_halt

undefined:
	stmfd sp!, {r0-r12,lr}
	ldr r4, [lr, #-4]
	mov r0, #0
	mov r1, #17
	ldr r2, =undefined_msg
	bl draw_string
	mov r0, #18
	mov r1, #17
	mov r2, r4
	bl draw_hex32
	mov r0, #27
	mov r1, #17
	ldr r2, =undefined_at
	bl draw_string
	// Output lr
	mov r1, #0x1000
	ldr r0, [r1, #-4]
	sub r2, r0, #4
	mov r0, #31
	mov r1, #17
	bl draw_hex32
	// Skip instruction for now
	// In future, 
	//  ldmfd sp!, {r0-r12,lr} // Note the lack of ^ since subs will handle it
	//  subs pc, lr, #4
	ldmfd sp!, {r0-r12,pc}^
svc:
	stmfd sp!, {r0-r12,lr}
	ldr r0, [lr, #-4]
	bic r0, #0xFF000000
	// SVC #0 returns to supervisor mode
	// TODO: Make supervisor mode return to a specific location
	//  (rather than to a user location) such as the kernel loop
	cmp r0, #0
	bne 3f
	cps #0x13
	b   1f
3:
	cmp r0, #2
	bne 2f
	ldmfd sp!, {r0-r12,lr}
	b schedule
2:
	cmp r0, #3
	bne 1f
	ldr r3, =scheduler
	ldr r2, [r3, #0]
	ldr r1, [r2, #8] // sp_base
	cmp r1, #-1
	beq 1f
	ldr r3, =stacks_table
	mov r0, #0
	strb r0, [r3, r1]
	// Free the thread after freeing the stack
	mov r0, r2
	bl free
1:
	ldmfd sp!, {r0-r12,pc}^
prefetch:
	stmfd sp!, {r0-r12,lr}
	mov r4, lr
	mov r0, #0
	mov r1, #16
	ldr r2, =prefetch_msg
	bl draw_string
	// Output return address
	mov r0, #17
	mov r1, #16
	mov r2, r4
	bl draw_hex32
	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4
data:
	stmfd sp!, {r0-r12,lr}
	mov r4, lr
	mov r0, #0
	mov r1, #15
	ldr r2, =data_msg
	bl draw_string
	// Output return address
	mov r0, #13
	mov r1, #15
	mov r2, r4
	sub r2, #8
	bl draw_hex32
	mov r0, #22
	mov r1, #15
	ldr r2, [r4, #-8]
	bl draw_hex32
	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4 // Should be 8 once I can actually handle the abort
fiq:
	stmfd sp!, {r0-r12,lr}
	bl c_fiq_handler
	cmp r0, #1
	bne 1f
	ldmfd sp!, {r0-r12,lr}
	sub lr, #4
	push {r3}
	ldr r3, =irqlr
	str lr, [r3, #0]
	pop {r3}
	cps #0x13
	b schedule
1:
	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4

.align 5
vector:
	ldr pc, reset_handler
	ldr pc, undefined_handler
	ldr pc, svc_handler
	ldr pc, prefetch_handler
	ldr pc, data_handler
	ldr pc, unused_handler
	ldr pc, irq_handler
	ldr pc, fiq_handler

reset_handler:      .word reset
undefined_handler:  .word undefined
svc_handler:        .word svc
prefetch_handler:   .word prefetch
data_handler:       .word data
unused_handler:     .word io_halt
irq_handler:        .word irq
fiq_handler:        .word fiq

.section .data
undefined_msg:	.asciz "Undefined Handler"
undefined_at:	.asciz "@ 0x"
svc_msg:	.asciz "SVC Handler #"
prefetch_msg:	.asciz "Prefetch Handler"
data_msg:	.asciz "Data Handler"
fiq_msg:	.asciz "FIQ\n"
