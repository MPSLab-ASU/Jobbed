.section .text
.globl preserveregs
preserveregs:
	str r4,  [r0, #0x10]
	str r5,  [r0, #0x14]
	str r6,  [r0, #0x18]
	str r7,  [r0, #0x1c]
	str r8,  [r0, #0x20]
	str r9,  [r0, #0x24]
	str r10, [r0, #0x28]
	str r11, [r0, #0x2c]
	bx lr

.globl restoreregs
restoreregs:
	ldr r4,  [r0, #0x10]
	ldr r5,  [r0, #0x14]
	ldr r6,  [r0, #0x18]
	ldr r7,  [r0, #0x1c]
	ldr r8,  [r0, #0x20]
	ldr r9,  [r0, #0x24]
	ldr r10, [r0, #0x28]
	ldr r11, [r0, #0x2c]
	bx lr

.globl exetask
exetask:
	push {r0, r4, r5, r6, r7, r8, r9, r10, r11, lr}
	// Restore registers from context switch
	ldr r4,  [r0, #0x10]
	ldr r5,  [r0, #0x14]
	ldr r6,  [r0, #0x18]
	ldr r7,  [r0, #0x1c]
	ldr r8,  [r0, #0x20]
	ldr r9,  [r0, #0x24]
	ldr r10, [r0, #0x28]
	ldr r11, [r0, #0x2c]

	// Preserve system stack
	ldr r1,  =msp
	str sp, [r1]
	// Setup task's stack
	ldr sp, [r0, #0x34]
	// Switch to task
	ldr lr, [r0, #0x3c]
	blx lr
	// Restore system stack
	ldr r1, =msp
	ldr sp, [r1]
	pop {r0, r4, r5, r6, r7, r8, r9, r10, r11, pc}

.section .data
msp:
	.space 4
.section .data.stacks
.globl stacks
stacks:
	.space 0x100000
