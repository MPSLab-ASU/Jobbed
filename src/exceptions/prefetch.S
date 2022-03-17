.section ".text.exceptions"
.globl prefetch
prefetch:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	mov r4, lr
	mov r0, #98
	mov r1, #0
	ldr r2, =prefetch_msg
	bl draw_string
	// Output return address
	mov r0, #98
	mov r1, #1
	mov r2, r4
	bl draw_hex32
	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4

.section .data
prefetch_msg:	.asciz "Prefetch Handler"
