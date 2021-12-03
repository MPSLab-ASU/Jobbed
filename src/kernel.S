.section ".text.kernel"

.globl kernel_main
kernel_main:
	push {lr}
	bl sysinit
	bl enable_irq
	bl enable_fiq
	bl chk_irq_stat
	bl postinit
kernel_main.loop:
	bl io_halt
	b kernel_main.loop
	pop {lr}

.section ".data"
.globl cntfrq
cntfrq:
	.word 0,0,0,0
