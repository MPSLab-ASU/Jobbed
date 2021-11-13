.section ".text.kernel"

.globl kernel_main
kernel_main:
	push {lr}
	bl uart_init
	bl enable_irq
	ldr r0, =os_info
	bl uart_string
kernel_main.loop:
	bl io_halt
	b kernel_main.loop
	pop {lr}

.section ".data"
os_info:
    .asciz "Sergey Bilovytskyy's Real Time Operating System\n Version 0.0a\n Interrupt 01: uart rx interrupt\n Exit        : Ctrl-A x\n Monitor     : Ctrl-A c\n\n"
