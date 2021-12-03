.section ".text"

.globl uart_init
uart_init:
	//*UART0_IMSC = 1 << 4;
	// IGNORE OVERRUN
	mov r2, #0x1038
	movt r2, #0x3F20
	mov r3, #0b10000
	str r3, [r2]
	//*IRQ_ENABLE2 = 1 << 25;
	mov r2, #0xB214
	movt r2, #0x3F00
	mov r3, #0
	movt r3, #0b1000000000
	str r3, [r2]
	//*GPU_INTERRUPTS_ROUTING = 0x00;
	// Route GPU interrupts to Core0
	mov r2, #0x000C
	movt r2, #0x4000
	eor r3, r3
	str r3, [r2]
	bx lr

.globl uart_char
uart_char:
	mov r2, #0x1000
	movt r2, #0x3f20
uart_char.loop:
	ldr r3, [r2, #24]
	tst r3, #0b100000
	bne uart_char.loop
	str r0, [r2]
	bx lr

.globl uart_string
uart_string:
	push    {r4, lr}
	mov     r4, r0
	ldrb    r0, [r0]
	cmp     r0, #0
	popeq   {r4, pc}
uart_string.loop:
	bl      uart_char
	ldrb    r0, [r4, #1]!
	cmp     r0, #0
	bne     uart_string.loop
	pop     {r4, pc}

.globl uart_hex
uart_hex:
	push {r4, lr}
	mov r2, #0x1000
	movt r2, #0x3f20
uart_hex.loop:
	ldr r3, [r2, #24]
	tst r3, #0b100000
	bne uart_hex.loop
	mov r3, #7
uart_hex.hloop:
	mov r1, r0
	asr r1, r3
	asr r1, r3
	asr r1, r3
	asr r1, r3
	and r1, #0xf
	add r1, #0x30
	cmp r1, #0x3A
	blt uart_hex.print
	add r1, #7
uart_hex.print:
	str r1, [r2]
	subs r3, #1
	bge uart_hex.hloop
	pop {r4, pc}
