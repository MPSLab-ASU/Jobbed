// To keep this in the first portion of the binary.
.section ".text.boot"
 
// Make _start global.
.globl _start
 
_start:
reset:
    // disable core0,1,2.
    mrc p15, #0, r1, c0, c0, #5
    and r1, r1, #3
    cmp r1, #0
    bne io_halt 

    // set vector address.
    ldr r0, =vector
    mcr p15, 0, r0, c12, c0, 0

    // save cpsr.
    mrs r0, cpsr

    // setup sp in IRQ mode.
    bic r1, r0, #0x1f
    orr r1, r1, #0x12
    msr cpsr_c,r1
    mov sp,#0x4000

    // restore cpsr.
    msr cpsr_c, r0

    // setup the stack in SVC mode.
    mov sp, #0x8000
 
    // Clear out bss.
    ldr r4, =__bss_start
    ldr r9, =__bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    b       2f
 
1:
    // store multiple at r4.
    stmia r4!, {r5-r8}
 
    // If we are still below bss_end, loop.
2:
    cmp r4, r9
    blo 1b

    // Call kernel_main
    ldr r3, =kernel_main
    blx r3

irq:
    push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
    bl a_irq_handler
    pop  {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
    subs pc, lr, #4

.globl io_halt
io_halt:
    wfi
    b io_halt

.globl enable_irq
enable_irq:
    cpsie i
    bx lr

.globl disable_irq
disable_irq:
    cpsid i
    bx lr

.globl enable_fiq
enable_fiq:
    cpsie i
    bx lr

.globl disable_fiq
disable_fiq:
    cpsid f
    bx lr

.align 5
vector:
    ldr pc, reset_handler
    ldr pc, undefined_handler
    ldr pc, swi_handler
    ldr pc, prefetch_handler
    ldr pc, data_handler
    ldr pc, unused_handler
    ldr pc, irq_handler
    ldr pc, fiq_handler

reset_handler:      .word reset
undefined_handler:  .word io_halt
swi_handler:        .word io_halt
prefetch_handler:   .word io_halt
data_handler:       .word io_halt
unused_handler:     .word io_halt
irq_handler:        .word irq
fiq_handler:        .word io_halt
//irq_handler:        .word io_halt
//fiq_handler:        .word irq

.global a_irq_handler
a_irq_handler:
    push {lr}
    bl disable_irq
    // r2 = CORE0_INTERRUPT_SOURCE
    // if r2 & 0b100000000
    mov r2, #0x40000000
    ldr r3, [r2, #0x60]
    tst r3, #256
    beq a_irq_handler.exit
    // r2 = IRQ_PEND2
    // r2 & 1 << 25
    mov r2, #0xB208
    movt r2, #0x3F00
    ldr r3, [r2]
    tst r3, #0x2000000
    beq a_irq_handler.exit
    mov r2, #0x1040
    movt r2, #0x3F20
    ldr r3, [r2]
    tst r3, #16
    beq a_irq_handler.exit
    mov r2, #0x1000
    movt r2, #0x3F20
    ldrb r0, [r2]
    push {r0}
    bl enable_irq
    pop {r0}
    bl uart_char
    ldr r0, =amsg
    bl uart_string
    pop {pc}
a_irq_handler.exit:
    bl enable_irq
    pop {pc}

.section ".data"
amsg:
    .asciz " a_irq_handler\n"
