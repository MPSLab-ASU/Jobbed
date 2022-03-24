.section ".text.exceptions"
.globl prefetch
prefetch:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	ldr r4, [lr, #-4]
	// Output return address
	mov r0, #98
	mov r1, #0
	mov r2, r4
	bl draw_hex32
	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4
