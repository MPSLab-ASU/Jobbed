#ifndef SYS_POWER_H
#define SYS_POWER_H

#define PM_RSTC 0x1c
#define PM_RSTS 0x20
#define PM_WDOG 0x24

#define PM_PASSWORD       0x5a000000
#define BCM2835_PERI_BASE 0x3F000000

#define PM_WDOG_TIME_SET         0x000fffff
#define PM_RSTC_WRCFG_CLR        0xffffffcf
#define PM_RSTS_HADWRH_SET       0x00000040
#define PM_RSTC_WRCFG_SET        0x00000030
#define PM_RSTC_WRCFG_FULL_RESET 0x00000020
#define PM_RSTC_RESET            0x00000102
#define PM_RSTS_PARTITION_CLR    0xfffffaaa
#define SECS_TO_WDOG_TICS(x) ((x) << 16)

void wdt_start(void);
void __bcm2835_restart(unsigned char);
void bcm2835_power_off(void);

#endif
