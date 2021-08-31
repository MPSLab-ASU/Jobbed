.section ".text"

.globl uart_init
uart_init:
    //*UART0_IMSC = 1 << 4;
    mov r2, #0x1038
    movt r2, #0x3F20
    mov r3, #0b10000
    str r3, [r2]
    //*IRQ_ENABLE2 = 1 << 25;
    mov r2, #0xB214
    movt r2, #0x3F00
    mov r3, #0
    movt r3, #0b1000000000
    str r3, [r2]
    //*GPU_INTERRUPTS_ROUTING = 0x00;
    mov r2, #0x000C
    movt r2, #0x4000
    eor r3, r3
    str r3, [r2]
    bx lr

.globl uart_char
uart_char:
    mov r2, #0x1000
    movt r2, #0x3f20
uart_char.loop:
    ldr r3, [r2, #24]
    tst r3, #0b100000
    bne uart_char.loop
    str r0, [r2]
    bx lr

.globl uart_string
uart_string:
    push    {r4, lr}
    mov     r4, r0
    ldrb    r0, [r0]
    cmp     r0, #0
    popeq   {r4, pc}
uart_string.loop:
    bl      uart_char
    ldrb    r0, [r4, #1]!
    cmp     r0, #0
    bne     uart_string.loop
    pop     {r4, pc}
