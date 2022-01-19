// To keep this in the first portion of the binary.
.section ".text.boot"
 
// Make _start global.
.globl _start
 
_start:
reset:
	// disable core0,1,2.
	mrc p15, #0, r1, c0, c0, #5
	and r1, r1, #3
	cmp r1, #0
	bne io_halt

	// set vector address.
	ldr r0, =vector
	mcr p15, 0, r0, c12, c0, 0

	// save cpsr.
	mrs r0, cpsr

	// setup sp in IRQ mode.
	bic r1, r0, #0x1f
	orr r1, r1, #0x12
	msr cpsr_c,r1
	mov sp,#0x4000
	// setup sp in FIQ mode.
	bic r1, r0, #0x1f
	orr r1, r1, #0x11
	msr cpsr_c,r1
	mov sp,#0x2000
	// setup sp in UNDEF mode.
	bic r1, r0, #0x1f
	orr r1, r1, #0x1B
	msr cpsr_c,r1
	mov sp,#0x1000
	// setup sp in ABORT mode.
	bic r1, r0, #0x1f
	orr r1, r1, #0x17
	msr cpsr_c,r1
	mov sp,#0x0800
	// Setup sp in USR/SYS mode.
	bic r1, r0, #0x1f
	orr r1, r1, #0x1f
	msr cpsr_c,r1
	mov sp,#0x6000

	// restore to SVC (in the case of a reset)
	bic r1, r0, #0x1f
	orr r1, r1, #0x13
	msr cpsr_c, r1
	//msr cpsr_c, r0

	// setup the stack in SVC mode.
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

.globl io_halt
io_halt:
	wfi
	b io_halt

// TODO: SETUP OTHER STACKS
undefined:
	stmfd sp!, {r0-r12,lr}
	ldr r0, [lr, #-4]
	push {r0}
	ldr r0, =undefined_msg
	bl uart_string
	pop {r0}
	bl uart_hex
	ldr r0, =undefined_at
	bl uart_string
	// Output lr
	mov r1, #0x1000
	ldr r0, [r1, #-4]
	bl uart_hexn
	// Skip instruction for now
	// In future, 
	//  ldmfd sp!, {r0-r12,lr} // Note the lack of ^ since subs will handle it
	//  subs pc, lr, #4
	ldmfd sp!, {r0-r12,pc}^
svc:
	stmfd sp!, {r0-r12,lr}
	ldr r0, [lr, #-4]
	bic r0, #0xFF000000
	push {r0}
	ldr r0, =svc_msg
	bl uart_string
	ldr r0, [sp]
	bl uart_hexn
	pop {r0}
	// SVC #0 returns to supervisor mode
	// TODO: Make supervisor mode return to a specific location
	//  (rather than to a user location) such as the kernel loop
	cmp r0, #0
	bne 1f
	mrs r0, spsr
	bic r0, #0x1f
	// Return in supervisor mode
	orr r0, #0x13
	msr spsr, r0
1:
	ldmfd sp!, {r0-r12,pc}^
io_halt_prefetch:
	stmfd sp!, {r0-r12,lr}
	ldr r0, =prefetch_msg
	bl uart_string
	ldmfd sp!, {r0-r12,pc}^
io_halt_data:
	stmfd sp!, {r0-r12,lr}
	ldr r0, =data_msg
	bl uart_string
	// Output return address
	mov r1, #0x800
	ldr r0, [r1, #-4]
	sub r0, #8
	bl uart_hexn
	ldmfd sp!, {r0-r12,pc}^
io_halt_fiq:
	ldr r0, =fiq_msg
	push {lr}
	bl uart_string
	pop {lr}
	b io_halt

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
prefetch_handler:   .word io_halt_prefetch
data_handler:       .word io_halt_data
unused_handler:     .word io_halt
irq_handler:        .word irq
fiq_handler:        .word io_halt_fiq

.section .data
undefined_msg:	.asciz "Undefined Handler\nUNDEF "
undefined_at:	.asciz " @ 0x"
svc_msg:	.asciz "SVC Handler\nSVC #"
prefetch_msg:	.asciz "Prefetch Handler\n"
data_msg:	.asciz "Data Handler\n"
fiq_msg:	.asciz "FIQ\n"
