.section ".text.kernel"

.include "macros.inc"

.globl kernel_main
kernel_main:
	bl sysinit
	bl status
	ldr r2, =ttbr_msg
	mov r0, #23
	mov r1, #0
	mov r3, #0xFF00
	bl draw_cstring
	// Initialize System Cycle Counter
	mov r0, #1
	mcr p15, 0, r0, c9, c14, 0
	mov r0, #1
	mcr p15, 0, r0, c9, c12, 0
	mov r0, #0x80000000
	mcr p15, 0, r0, c9, c12, 1

	// Allow access to CNTKCTL from PL0
	mrc p15, 0, r0, c14, c1, 0
	orr r0, #1
	mcr p15, 0, r0, c14, c1, 0

	// Intentional undefined instruction
	// .word 0xf7f0a000
	cpsie ai, #0x10
	svc #2 // Start scheduling!
2:
	wfe
	b 2b

.section .data
ttbr_msg:
	.asciz "MMU Initialized!"
