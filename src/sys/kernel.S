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
