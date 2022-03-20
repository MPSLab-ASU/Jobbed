#include <lib/mmu.h>

#define CACHABLE 0x08
#define BUFFERABLE 0x04
#define NO_PERMISSIONS_REQUIRED 0b11 << 10
#define MMU_TABLE_BASE 0x00004000

void mmu_start(unsigned long base, unsigned long flags);

void mmu_section(unsigned long virtual, unsigned long physical, unsigned long flags)
{
	unsigned long offset = virtual >> 20;
	unsigned long* entry = (unsigned long*)(MMU_TABLE_BASE | (offset << 2));
	unsigned long physval = (physical & 0xFFF00000) | (flags & 0x7FFC) | 0x00C02;
	*entry = physval;
}

extern unsigned long __bss_end;
void mmu_init(void)
{
	for (unsigned long addr =  0x00000000;; addr += 0x00100000) {
		if ((addr+0x00100000) < (unsigned long)&__bss_end) {
			mmu_section(addr, addr, CACHABLE | BUFFERABLE);
		} else {
			mmu_section(addr, addr, NO_PERMISSIONS_REQUIRED);
		}
		if (addr == 0x02000000)
			mmu_section(addr, addr, CACHABLE | BUFFERABLE | NO_PERMISSIONS_REQUIRED);
		if (addr == 0xFFF00000)
			break;
	}
	mmu_start(MMU_TABLE_BASE,0x00000001|0x1000|0x0004);
}
