.section ".text.exceptions"
.globl fiq
fiq:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	bl c_fiq_handler
	cmp r0, #1
	bne 1f
	mrs r1, spsr
	and r1, r1, #0x1f
	cmp r1, #0x10
	bne 1f
	ldmfd sp!, {r0-r12,lr}
	sub lr, #4
	push {r3}
	ldr r3, =irqlr
	str lr, [r3, #0]
	pop {r3}
	cps #0x13
	b schedule
1:
	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4

.section .data
fiq_msg:	.asciz "FIQ\n"
