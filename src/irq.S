.globl a_irq_handler
a_irq_handler:
	push {lr}
	bl disable_irq
	// r2 = CORE0_INTERRUPT_SOURCE
	// if r2 & 0b100000000
	mov r2, #0x40000000
	ldr r3, [r2, #0x60]
	tst r3, #256
	beq a_irq_handler.timer
	// r2 = IRQ_PEND2
	// r2 & 1 << 25
	mov r2, #0xB208
	movt r2, #0x3F00
	ldr r3, [r2]
	tst r3, #0x2000000
	beq a_irq_handler.exit
	mov r2, #0x1040
	movt r2, #0x3F20
	ldr r3, [r2]
	tst r3, #16
	beq a_irq_handler.exit
	mov r2, #0x1000
	movt r2, #0x3F20
	ldrb r0, [r2]
	push {r0}
	bl enable_irq
	pop {r0}
	bl uart_char
	ldr r0, =imsg
	bl uart_string
	pop {pc}
a_irq_handler.timer:
	// r2 = CORE0_INTERRUPT_SOURCE
	// if r2 & 0b1000
	mov r2, #0x40000000
	ldr r3, [r2, #0x60]
	tst r3, #8
	beq a_irq_handler.exit
	bl c_timer
	//ldr r0, =imsg
	//bl uart_string
a_irq_handler.exit:
	bl enable_irq
	pop {pc}

.globl a_fiq_handler
a_fiq_handler:
	push {lr}
	bl disable_fiq
	bl c_timer
	ldr r0, =fmsg
	bl uart_string
a_fiq_handler.exit:
	bl enable_fiq
	pop {pc}

.section ".data"
imsg:
	.asciz " asm_irq_handler\n"
fmsg:
	.asciz " asm_fiq_handler\n"
