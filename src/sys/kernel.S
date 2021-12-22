.section ".text.kernel"

.globl kernel_main
kernel_main:
	push {lr}
	bl sysinit
	cpsie i // Enable IRQ
	cpsie f // Enable FIQ
	bl postinit
kernel_main.loop:
	bl io_halt
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
