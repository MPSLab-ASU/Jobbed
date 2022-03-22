.section .text

a.btest:
	push {lr}
	mov r0, #5
	cmp r0, #4
	pop {pc}

.globl atest
atest:
	push {lr}
	ldr r0, =a.btest
	mov r1, #0
	mov r2, #0
	bl add_thread
	mov r0, #5
	subs r0, #5
	svc #0
	beq 1f
	mov r0, #0
	mov r1, #11
	mov r2, #0x4E
	mov r3, #0xFF0000
	bl draw_cletter
	pop {pc}
1:	mov r0, #0
	mov r1, #11
	mov r2, #0x59
	mov r3, #0xFF00
	bl draw_cletter
	pop {pc}
