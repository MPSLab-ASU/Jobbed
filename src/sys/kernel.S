.section ".text.kernel"

.include "macros.inc"

.globl kernel_main
kernel_main:
	///https://wiki.osdev.org/ARM_Paging
	// Query the ID_MMFR0 register
	mrc p15, 0, r0, c0, c1, 4
	bl sysinit

	assign_ctask testf, 2
	assign_ctask testf, 1
	assign_ctask testf, 2
	assign_ctask testf, 3
	assign_ctask testf, 1
	// Intentional undefined instruction
	// .word 0xf7f0a000
	cpsie aif, #0x10
	svc #2 // Start scheduling!
1:
	wfe
	b 1b

testf:
	push {lr}
	cmp r0, #2
	blo 1f
	beq 2f
	bgt 3f
1:
	ldr r0, =t1
	bl uart_print
	pop {pc}
2:
	ldr r0, =t2
	bl uart_print
	pop {pc}
3:
	ldr r0, =t3
	bl uart_print
	pop {pc}

.section .data
t1: .asciz "C1\n"
t2: .asciz "C2\n"
t3: .asciz "C3\n"
