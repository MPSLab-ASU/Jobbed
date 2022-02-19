// To keep this in the first portion of the binary.
.section ".text.boot"

// Make _start global.
.globl _start

.include "../include/macros.inc"

_start:
reset:
	cpsid aif
	// disable core0,1,2.
	mrc p15, #0, r1, c0, c0, #5
	and r1, r1, #3
	cmp r1, #1
	beq core1run
	cmp r1, #2
	beq core2run
	cmp r1, #3
	bge core3run

	init_core 0

	// Clear out bss.
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b       2f

1:	// store multiple at r4.
	stmia r4!, {r5-r8}

2:	// If we are still below bss_end, loop.
	cmp r4, r9
	blo 1b

	// Clear mailboxes
	mov r4, #0
	ldr r5, =mbox_core0
	str r4, [r5]
	ldr r5, =mbox_core1
	str r4, [r5]
	ldr r5, =mbox_core2
	str r4, [r5]
	ldr r5, =mbox_core3
	str r4, [r5]

	// Call kernel_main
	ldr r3, =kernel_main
	blx r3

core1run:
	init_core 1
	b io_halt
core2run:
	init_core 2
	b io_halt
core3run:
	init_core 3
	b io_halt
.globl io_halt
io_halt:
	wfi
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
prefetch_handler:   .word prefetch
data_handler:       .word data
unused_handler:     .word io_halt
irq_handler:        .word irq
fiq_handler:        .word fiq

.section .data
mbox_core0: .word 0
mbox_core1: .word 0
mbox_core2: .word 0
mbox_core3: .word 0

.section .bss.estacks
.align 4
	.space 4096
undefined_stack_core0:
	.space 4096
svc_stack_core0:
	.space 4096
data_stack_core0:
	.space 4096
irq_stack_core0:
	.space 4096
fiq_stack_core0:
	.space 4096
sys_stack_core0:
	.space 4096
undefined_stack_core1:
	.space 4096
svc_stack_core1:
	.space 4096
data_stack_core1:
	.space 4096
irq_stack_core1:
	.space 4096
fiq_stack_core1:
	.space 4096
sys_stack_core1:
	.space 4096
undefined_stack_core2:
	.space 4096
svc_stack_core2:
	.space 4096
data_stack_core2:
	.space 4096
irq_stack_core2:
	.space 4096
fiq_stack_core2:
	.space 4096
sys_stack_core2:
	.space 4096
undefined_stack_core3:
	.space 4096
svc_stack_core3:
	.space 4096
data_stack_core3:
	.space 4096
irq_stack_core3:
	.space 4096
fiq_stack_core3:
	.space 4096
sys_stack_core3:
