.section ".text.exceptions"
.globl data
data:
	cpsid aif
	stmfd sp!, {r0-r12,lr}
	mov r4, lr
	mov r0, #0
	mov r1, #15
	ldr r2, =data_msg
	bl draw_string
	// Output return address
	mov r0, #13
	mov r1, #15
	mov r2, r4
	sub r2, #8
	bl draw_hex32
	mov r0, #22
	mov r1, #15
	ldr r2, [r4, #-8]
	bl draw_hex32
	mov r0, #32
	mov r1, #15
	mrs r2, spsr
	bl draw_hex32
	mov r0, #41
	mov r1, #15
	mrc p15, 0, r2, c5, c0, 0 //// https://developer.arm.com/documentation/ddi0464/d/System-Control/Register-descriptions/Data-Fault-Status-Register?lang=en
	bl draw_hex32
	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4 // Should be 8 once I can actually handle the abort

.section .data
data_msg:	.asciz "Data Handler"
