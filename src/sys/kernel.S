.section ".text.kernel"

.globl kernel_main
kernel_main:
	///https://wiki.osdev.org/ARM_Paging
	// Query the ID_MMFR0 register
	mrc p15, 0, r2, c0, c1, 4
	mov r0, #0
	mov r1, #7
	bl draw_hex32
	bl sysinit
	// Intentional undefined instruction
	// .word 0xf7f0a000
	cpsie aif, #0x10
	svc #2 // Start scheduling!
1:
	wfe
	b 1b
