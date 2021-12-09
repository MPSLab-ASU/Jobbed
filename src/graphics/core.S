.section .text
.globl init_graphics
init_graphics:
	push {lr}
	@ set Video Controller resolution to 640x480x16bit
	@ 16-bit, cause the 8-bit depth needs a palette
	ldr r1, =vc_set_res
	bl  mb0_c8_write
	bl  mb0_c8_read
	tst r0, #0x80000000
	beq .vc_init_fail

	@ get VC framebuffer address
	ldr r1, =vc_alloc_fb
	bl  mb0_c8_write
	bl  mb0_c8_read
	tst r0, #0x80000008
	beq .vc_init_fail

	@ check if the address is correct
	ldr r0, [r1, #20]
	cmp r0, #0
	beq .vc_init_fail

	@ draw "NO OS" text
	@bl vc_draw_no_os_bmp
	ldr r1, =vram_base
	str r0, [r1]
	pop {pc}

.vc_init_fail:
	ldr r1, =txt_vc_fail
	bl uart_string
	pop {pc}

.globl draw_pix
draw_pix:
	push {lr, r0, r1, r2, r3, r4}
	ldr r4, =vram_base
	ldr r4, [r4]
	mov r3, #2
	mul r0, r3
	add r4, r0
	push {r2}
	mov r2, #640
	mul r2, r3
	mul r3, r1, r2
	add r4, r3
	pop {r1}
	str r1, [r4]
	pop {pc, r0, r1, r2,  r3, r4}

@@@@@@@@@@@@@@@ VC @@@@@@@@@@@@@@

.equ MBOX0, 0x3f00b880

@ writes to mailbox #0, channel 8
@ r1 - message
mb0_c8_write:
	message .req r1
	mailbox .req r3
	status  .req r2

	ldr mailbox, =MBOX0
.mb0_full:
	ldr status, [mailbox, #0x18]
	tst status, #0x80000000  @ mailbox full flag
	bne .mb0_full 
	add message, #8          @ channel 8
	str message, [mailbox, #0x20] @ write addr
	sub message, #8

	.unreq mailbox
	.unreq message
	.unreq status
	mov pc, lr

@ reads from mailbox #0, channel 8
@ r1 - message
@ returns status in r0
mb0_c8_read:
	message .req r1
	mailbox .req r2
	status  .req r3
	value   .req r4

	ldr mailbox, =MBOX0
.mb0_empty:
	ldr status, [mailbox, #0x18] 
	tst status, #0x40000000  @ mailbox empty flag
	bne .mb0_empty

	ldr value, [mailbox] @ check if the message channel is 8
	and r0, value, #0xf
	teq r0, #8
	bne .mb0_empty

	ldr r0, [message, #4] 
	.unreq message
	.unreq mailbox
	.unreq status
	.unreq value
	mov pc, lr

@ raspi mailbox requests, must be padded to 16 bytes
.align 4
vc_set_res:  .word 80, 0                      @ total size, code (0=req)
	.word 0x00048003, 8, 8, 640, 480 @ set physical size (640x480)
	.word 0x00048004, 8, 8, 640, 480 @ set virtual size (640x480)
	.word 0x00048005, 4, 4, 16       @ set depth (16-bit)
	.word 0, 0, 0, 0                 @ end tag & padding

vc_alloc_fb: .word 32, 0                      @ total size, code (0=req)
	.word 0x00040001, 8, 4, 16, 0    @ allocate framebuffer
	.word 0                          @ end tag & padding

.align 2
txt_welcome: .asciz "No OS installed\r\n"
txt_vc_fail: .asciz "VC initialization failed\r\n"

.align 2
.globl vram_base
vram_base: .word 0,0
vmem: .word 0xFFFF
