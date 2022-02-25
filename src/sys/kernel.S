.section ".text.kernel"

.include "macros.inc"

.globl kernel_main
kernel_main:
	///https://wiki.osdev.org/ARM_Paging
	// Query the ID_MMFR0 register
	mrc p15, 0, r0, c0, c1, 4
	bl sysinit

	// Intentional undefined instruction
	// .word 0xf7f0a000
	cpsie aif, #0x10
	svc #2 // Start scheduling!
1:
	wfe
	b 1b
