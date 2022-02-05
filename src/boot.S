// To keep this in the first portion of the binary.
.section ".text.boot"
 
// Make _start global.
.globl _start
 
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

	// set vector address.
	ldr r0, =vector
	mcr p15, 0, r0, c12, c0, 0

	cps #0x12 // Setup sp in IRQ mode.
	ldr sp, =core0_irq_stack
	cps #0x11 // Setup sp in FIQ mode.
	ldr sp, =core0_fiq_stack
	cps #0x1B // Setup sp in UNDEF mode.
	ldr sp, =core0_undefined_stack
	cps #0x17 // Setup sp in ABORT mode.
	ldr sp, =core0_data_stack
	cps #0x1f // Setup sp in USR/SYS mode.
	ldr sp, =core0_sys_stack
	cps #0x13 // Setup sp in SVC mode.
	ldr sp, =core0_svc_stack
 
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
	ldr r5, =core0_mbox
	str r4, [r5]
	ldr r5, =core1_mbox
	str r4, [r5]
	ldr r5, =core2_mbox
	str r4, [r5]
	ldr r5, =core3_mbox
	str r4, [r5]

	// Call kernel_main
	ldr r3, =kernel_main
	blx r3

core1run:
	cps #0x12 // Setup sp in IRQ mode.
	ldr sp, =core1_irq_stack
	cps #0x11 // Setup sp in FIQ mode.
	ldr sp, =core1_fiq_stack
	cps #0x1B // Setup sp in UNDEF mode.
	ldr sp, =core1_undefined_stack
	cps #0x17 // Setup sp in ABORT mode.
	ldr sp, =core1_data_stack
	cps #0x1f // Setup sp in USR/SYS mode.
	ldr sp, =core1_sys_stack
	cps #0x13 // Setup sp in SVC mode.
	ldr sp, =core1_svc_stack
	b io_halt
core2run:
	cps #0x12 // Setup sp in IRQ mode.
	ldr sp, =core2_irq_stack
	cps #0x11 // Setup sp in FIQ mode.
	ldr sp, =core2_fiq_stack
	cps #0x1B // Setup sp in UNDEF mode.
	ldr sp, =core2_undefined_stack
	cps #0x17 // Setup sp in ABORT mode.
	ldr sp, =core2_data_stack
	cps #0x1f // Setup sp in USR/SYS mode.
	ldr sp, =core2_sys_stack
	cps #0x13 // Setup sp in SVC mode.
	ldr sp, =core2_svc_stack
	b io_halt
core3run:
	cps #0x12 // Setup sp in IRQ mode.
	ldr sp, =core3_irq_stack
	cps #0x11 // Setup sp in FIQ mode.
	ldr sp, =core3_fiq_stack
	cps #0x1B // Setup sp in UNDEF mode.
	ldr sp, =core3_undefined_stack
	cps #0x17 // Setup sp in ABORT mode.
	ldr sp, =core3_data_stack
	cps #0x1f // Setup sp in USR/SYS mode.
	ldr sp, =core3_sys_stack
	cps #0x13 // Setup sp in SVC mode.
	ldr sp, =core3_svc_stack
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
core0_mbox: .word 0
core1_mbox: .word 0
core2_mbox: .word 0
core3_mbox: .word 0

.section .bss.estacks
.align 4
	.space 4096
core0_undefined_stack:
	.space 4096
core0_svc_stack:
	.space 4096
core0_data_stack:
	.space 4096
core0_irq_stack:
	.space 4096
core0_fiq_stack:
	.space 4096
core0_sys_stack:
	.space 4096
core1_undefined_stack:
	.space 4096
core1_svc_stack:
	.space 4096
core1_data_stack:
	.space 4096
core1_irq_stack:
	.space 4096
core1_fiq_stack:
	.space 4096
core1_sys_stack:
	.space 4096
core2_undefined_stack:
	.space 4096
core2_svc_stack:
	.space 4096
core2_data_stack:
	.space 4096
core2_irq_stack:
	.space 4096
core2_fiq_stack:
	.space 4096
core2_sys_stack:
	.space 4096
core3_undefined_stack:
	.space 4096
core3_svc_stack:
	.space 4096
core3_data_stack:
	.space 4096
core3_irq_stack:
	.space 4096
core3_fiq_stack:
	.space 4096
core3_sys_stack:
