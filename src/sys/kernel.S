.section ".text.kernel"

.globl kernel_main
kernel_main:
	bl sysinit
	// ///https://wiki.osdev.org/ARM_Paging
	// // Query the ID_MMFR0 register
	// mrc p15, 0, r0, c0, c1, 4
	// bl uart_hexn
	// // Switch to user mode
	// cps #0x10
	// // Intentional undefined instruction
	// //.word 0xf7f0a000
	// // This will fail in user mode
	// mrc p15, 3, r0, c15, c0, 0
	// // Supervisor Call #1 - Does nothing special
	// svc #1
	// mrs r0, cpsr
	// bl uart_hexn
	cps #0x10
	svc #2 // Start scheduling!
1:
	wfe
	b 1b
