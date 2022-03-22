.section ".text.exceptions"
.globl irq
irq:
	cpsid ai
	stmfd sp!, {r0-r12,lr}
	// Run IRQ handler
	bl c_irq_handler
	cmp r0, #1
	bne 1f
	// Schedule if interrupted a thread
	mrs r1, spsr
	and r1, r1, #0x1f
	cmp r1, #0x10
	bne 1f
	ldmfd sp!, {r0-r12,lr}
	// Don't skip missed instruction upon return
	sub lr, #4
	push {r3}
	// Store the instruction in a special area for
	//  future processing
	ldr r3, =irqlr
	str lr, [r3, #0]
	pop {r3}
	cps #0x13
	b schedule
1:
	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4
