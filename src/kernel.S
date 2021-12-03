.section ".text.kernel"

.globl kernel_main
kernel_main:
	push {lr}
	//bl uart_init
	bl sysinit
	bl enable_irq
	bl enable_fiq
	bl chk_irq_stat
	ldr r0, =os_info
	bl uart_string
kernel_main.loop:
	bl io_halt
	b kernel_main.loop
	pop {lr}

.section ".data"
.globl os_info
os_info:
	.asciz "\033[93mInitialized the Real Time Operating System\033[0m\n\033[96mName\033[0m:    \033[94mTarvaOS\033[0m\n\033[96mVersion\033[0m: \033[95m0.0a\033[0m\n\nQEMU\n====\n Exit        : Ctrl-A x\n Monitor     : Ctrl-A c\n\n"
.globl cntfrq
cntfrq:
	.word 0,0,0,0
