.section ".bss"
.globl irqlr
irqlr:
	.word 0
.globl cntfrq
cntfrq:
	.word 0
.globl cmdidx
cmdidx:
	.word 0
.global cmd
cmd:
	.space 2049

.section ".bss.heap"
mheap:
	.space 0x100000
