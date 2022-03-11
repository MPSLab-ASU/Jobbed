.section ".text.kernel"

.include "macros.inc"

.globl kernel_main
kernel_main:
	///https://wiki.osdev.org/ARM_Paging
	// Query the ID_MMFR0 register
	//mrc p15, 0, r0, c0, c1, 4
	bl sysinit
	bl status
	ldr r2, =ttbr_msg
	mov r0, #23
	mov r1, #0
	mov r3, #0xFF00
	bl draw_cstring

	// Intentional undefined instruction
	// .word 0xf7f0a000
	cpsie aif, #0x10
	svc #2 // Start scheduling!
2:
	wfe
	b 2b

.section .data
ttbr_msg:
	.asciz "MMU Initialized!"
