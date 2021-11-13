.section ".text.kernel"

.globl kernel_main
kernel_main:
	push {lr}
	bl uart_init
	bl enable_irq
	bl enable_fiq
	bl check_fiq_status
	ldr r0, =os_info
	bl uart_string
kernel_main.loop:
	bl io_halt
	b kernel_main.loop
	pop {lr}

check_fiq_status:
	push {lr}
	mov r2, #0xB208
	movt r2, #0x3F00
	//mov r3, #57
	//movt r3, #0
	//str r3, [r2]
	ldr r3, [r2]
	push {r3}
	mov r0, r3
	bl uart_hex
	pop {r3}
	tst r3, #0x80
	beq check_fiq_status.off
	ldr r0, =fiq_on
	bl uart_string
	b check_fiq_status.exit
check_fiq_status.off:
	ldr r0, =fiq_off
	bl uart_string
check_fiq_status.exit:
	pop {pc}

.section ".data"
os_info:
    .asciz "Sergey Bilovytskyy's Real Time Operating System\n Version 0.0a\n Interrupt 01: uart rx interrupt\n Exit        : Ctrl-A x\n Monitor     : Ctrl-A c\n\n"
fiq_on:
    .asciz "\nFIQ - Enabled\n"
fiq_off:
    .asciz "\nFIQ - Disabled\n"
