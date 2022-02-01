// To keep this in the first portion of the binary.
.section ".text.boot"
 
// Make _start global.
.globl _start
 
_start:
reset:
	cpsid aif
	// disable core0,1,2.
	mrc p15, #0, r1, c0, c0, #5
	and r1, r1, #3
	cmp r1, #1
	beq core1run
	cmp r1, #2
	beq core2run
	cmp r1, #3
	bge core3run

	// set vector address.
	ldr r0, =vector
	mcr p15, 0, r0, c12, c0, 0

	// Setup sp in IRQ mode.
	cps #0x12
	mov sp,#0x4000
	// Setup sp in FIQ mode.
	cps #0x11
	mov sp,#0x2000
	// Setup sp in UNDEF mode.
	cps #0x1B
	mov sp,#0x1000
	// Setup sp in ABORT mode.
	cps #0x17
	mov sp,#0x0800
	// Setup sp in USR/SYS mode.
	cps #0x1f
	mov sp,#0x6000

	// Setup sp in SVC mode.
	cps #0x13
	mov sp, #0x8000
 
	// Clear out bss.
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b       2f
 
1:	// store multiple at r4.
	stmia r4!, {r5-r8}
 
2:	// If we are still below bss_end, loop.
	cmp r4, r9
	blo 1b

	// Call kernel_main
	ldr r3, =kernel_main
	blx r3

// TODO: Each core needs to set up their stacks
core1run:
core2run:
core3run:
.globl io_halt
io_halt:
	wfi
	b io_halt

.align 5
vector:
	ldr pc, reset_handler
	ldr pc, undefined_handler
	ldr pc, svc_handler
	ldr pc, prefetch_handler
	ldr pc, data_handler
	ldr pc, unused_handler
	ldr pc, irq_handler
	ldr pc, fiq_handler

reset_handler:      .word reset
undefined_handler:  .word undefined
svc_handler:        .word svc
prefetch_handler:   .word prefetch
data_handler:       .word data
unused_handler:     .word io_halt
irq_handler:        .word irq
fiq_handler:        .word fiq
