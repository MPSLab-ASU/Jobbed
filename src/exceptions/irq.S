.section ".text.exceptions"
.globl irq
irq:
	cpsid aif
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	// Run IRQ handler
	bl c_irq_handler
	pop  {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	subs pc, lr, #4
