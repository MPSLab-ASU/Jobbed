.section ".text.kernel"

.globl kernel_main
kernel_main:
	push {lr}
	bl sysinit
	bl postinit
kernel_main.loop:
	wfe
	b kernel_main.loop
	pop {lr}

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
