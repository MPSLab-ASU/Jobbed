#include <symbols.h>
#include <sys/core.h>
#include <sys/power.h>

//https://github.com/raspberrypi/linux/blob/aeaa2460db088fb2c97ae56dec6d7d0058c68294/drivers/watchdog/bcm2835_wdt.c
void wdt_start(void)
{
	store32(BCM2835_PERI_BASE + PM_WDOG, PM_PASSWORD | (SECS_TO_WDOG_TICS(100) & PM_WDOG_TIME_SET));
	unsigned long cur = load32(BCM2835_PERI_BASE + PM_RSTC);
	store32(BCM2835_PERI_BASE + PM_RSTC, PM_PASSWORD | (cur & PM_RSTC_WRCFG_CLR) | PM_RSTC_WRCFG_FULL_RESET);
}

void wdt_stop(void)
{
	store32(BCM2835_PERI_BASE + PM_RSTC, PM_PASSWORD | PM_RSTC_RESET);
}

void __bcm2835_restart(unsigned char partition)
{
	unsigned long val, rsts;
	rsts = (partition & 1) | ((partition & 0b10) << 1) |
		((partition & 0b100) << 2) | ((partition & 0b1000) << 3) |
		((partition & 0b10000) << 4) | ((partition & 0b100000) << 5);
	val = load32(BCM2835_PERI_BASE + PM_RSTS);
	val &= PM_RSTS_PARTITION_CLR;
	val |= PM_PASSWORD | rsts;
	store32(BCM2835_PERI_BASE + PM_RSTS, val);
	store32(BCM2835_PERI_BASE + PM_WDOG, 10 | PM_PASSWORD);
	val = load32(BCM2835_PERI_BASE + PM_RSTC);
	val &= PM_RSTC_WRCFG_CLR;
	val |= PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET;
	store32(BCM2835_PERI_BASE + PM_RSTC, val);
	delay(1);
}

void bcm2835_power_off(void)
{
	__bcm2835_restart(63); // Partition 63 => Halt
}
