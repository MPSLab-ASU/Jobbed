.section ".text.exceptions"
.globl undefined
undefined:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	ldr r4, [lr, #-4]
	mov r0, #0
	mov r1, #17
	ldr r2, =undefined_msg
	bl draw_string
	mov r0, #18
	mov r1, #17
	mov r2, r4
	bl draw_hex32
	mov r0, #27
	mov r1, #17
	ldr r2, =undefined_at
	bl draw_string
	// Output lr
	mov r1, #0x1000
	ldr r0, [r1, #-4]
	sub r2, r0, #4
	mov r0, #31
	mov r1, #17
	bl draw_hex32
	// Skip instruction for now
	// In future, 
	//  ldmfd sp!, {r0-r12,lr} // Note the lack of ^ since subs will handle it
	//  subs pc, lr, #4
	ldmfd sp!, {r0-r12,pc}^

.section .data
undefined_msg:	.asciz "Undefined Handler"
undefined_at:	.asciz "@ 0x"
