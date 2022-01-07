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

	// restore cpsr.
	msr cpsr_c, r0

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
	cpsid i
	bl c_irq_handler
	cpsie i
	pop  {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	subs pc, lr, #4

.globl io_halt
io_halt:
	wfi
	b io_halt

io_halt_undefined:
	ldr r0, =undefined_msg
	ldr r0, [r0]
	bl uart_char
	b io_halt
io_halt_swi:
	ldr r0, =swi_msg
	ldr r0, [r0]
	bl uart_char
	b io_halt
io_halt_prefetch:
	ldr r0, =prefetch_msg
	ldr r0, [r0]
	bl uart_char
	b io_halt
io_halt_data:
	ldr r0, =data_msg
	ldr r0, [r0]
	bl uart_char
	b io_halt
io_halt_unused:
	ldr r0, =unused_msg
	ldr r0, [r0]
	bl uart_char
	b io_halt
io_halt_fiq:
	ldr r0, =fiq_msg
	ldr r0, [r0]
	bl uart_char
	b io_halt

.align 5
vector:
	ldr pc, reset_handler
	ldr pc, undefined_handler
	ldr pc, swi_handler
	ldr pc, prefetch_handler
	ldr pc, data_handler
	ldr pc, unused_handler
	ldr pc, irq_handler
	ldr pc, fiq_handler

reset_handler:      .word reset
undefined_handler:  .word io_halt_undefined
swi_handler:        .word io_halt_swi
prefetch_handler:   .word io_halt_prefetch
data_handler:       .word io_halt_data
unused_handler:     .word io_halt_unused
irq_handler:        .word irq
fiq_handler:        .word io_halt_fiq
undefined_msg:	.asciz "Undefined\n"
swi_msg:	.asciz "SWI\n"
prefetch_msg:	.asciz "Prefetch\n"
data_msg:	.asciz "Data\n"
unused_msg:	.asciz "unused\n"
fiq_msg:	.asciz "FIQ\n"
