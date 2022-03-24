.section ".text"

.globl uart_char
uart_char:
	mov r2, #0x1000
	movt r2, #0x3f20
1:
	ldr r3, [r2, #24]
	tst r3, #0b100000
	bne 1b
	str r0, [r2]
	bx lr

.globl uart_string
uart_string:
	push    {r4, lr}
	mov     r4, r0
	ldrb    r0, [r0]
	cmp     r0, #0
	popeq   {r4, pc}
1:
	bl      uart_char
	ldrb    r0, [r4, #1]!
	cmp     r0, #0
	bne 1b
	pop     {r4, pc}

.globl uart_hex
uart_hex:
	push {r4, lr}
	mov r2, #0x1000
	movt r2, #0x3f20
1:
	ldr r3, [r2, #24]
	tst r3, #0b100000
	bne 1b
	mov r3, #7
2:
	mov r1, r0
	asr r1, r3
	asr r1, r3
	asr r1, r3
	asr r1, r3
	and r1, #0xf
	add r1, #0x30
	cmp r1, #0x3A
	blt 3f
	add r1, #7
3:
	str r1, [r2]
	subs r3, #1
	bge 2b // Jump back to wait for availablilty
	pop {r4, pc}
