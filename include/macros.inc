.macro init_core coreid
	// set vector address.
	ldr r0, =vector
	mcr p15, 0, r0, c12, c0, 0
	cps #0x12 // Setup sp in IRQ mode.
	ldr sp, =irq_stack_core\coreid
	cps #0x11 // Setup sp in FIQ mode.
	ldr sp, =fiq_stack_core\coreid
	cps #0x1B // Setup sp in UNDEF mode.
	ldr sp, =undefined_stack_core\coreid
	cps #0x17 // Setup sp in ABORT mode.
	ldr sp, =data_stack_core\coreid
	cps #0x1f // Setup sp in USR/SYS mode.
	ldr sp, =sys_stack_core\coreid
	cps #0x13 // Setup sp in SVC mode.
	ldr sp, =svc_stack_core\coreid
.endm
