.section ".text.kernel"

.globl kernel_main
kernel_main:
	bl sysinit
kernel_main.loop:
	//bl schedule
	wfe
	b kernel_main.loop

.section ".data"
.globl cntfrq
cntfrq:
	.word 0,0,0,0
.globl cmdidx
cmdidx:
	.word 0
.globl cmd
cmd:
	.space 2049

.section ".bss.heap"
mheap:
	.space 0x100000
