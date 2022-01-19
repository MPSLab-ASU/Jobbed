.section ".text.kernel"

.globl kernel_main
kernel_main:
	bl sysinit
	cps #0x10
	// Intentional undefined instruction
	.word 0xf7f0a000
	// Supervisor Call #1 - Does nothing special
	svc #1
	mrs r0, cpsr
	bl uart_hexn
	// Supervisor Call #1 - Returns in supervisor mode
	svc #0
	mrs r0, cpsr
	bl uart_hexn
1:
	//bl schedule
	wfe
	b 1b

.section ".data"
.globl cntfrq
cntfrq: // 32 bits
	.word 0
.globl cmdidx
cmdidx:
	.word 0
.globl cmd
cmd:
	.space 2049

.section ".bss.heap"
mheap:
	.space 0x100000
