.section .text
.globl mmu_start
mmu_start:
	mov r2, #0
	// Invalidate Caches
	mcr p15,0,r2,c7,c1,6 
	// Invalidate TLB entries
	mcr p15,0,r2,c8,c7,0 
	// Data synchronisation barrier
	mcr p15,0,r2,c7,c10,4 

	// Set all domains to 0b11
	mvn r2, #0
	bic r2, #0xC
	mcr p15,0,r2,c3,c0,0

	// Set the translation table base address (remember to align 16 KiB!)
	mcr p15,0,r0,c2,c0,0
	mcr p15,0,r0,c2,c0,1
	mov r3, #0
	mcr p15,0,r3,c2,c0,2

	// Set the bits mentioned above
	mrc p15,0,r2,c1,c0,0
	orr r2,r2,r1
	mcr p15,0,r2,c1,c0,0
	bx lr

.globl mmu_stop
mmu_stop:
	mrc p15,0,r2,c1,c0,0
	bic r2,#0x1000
	bic r2,#0x0004
	bic r2,#0x0001
	mcr p15,0,r2,c1,c0,0
	bx lr

.globl tlb_invalidate
tlb_invalidate:
	mov r2, #0
	// Invalidate Entries
	mcr p15, 0, r2, c8, c7, 0
	// DSB
	mcr p15, 0, r2, c7, c10, 4
	bx lr
