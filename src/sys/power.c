#include "../sys/core.h"
#include "../sys/power.h"

void reboot(void) {
	store32(PM_WDOG, PM_PASSWORD | 1);
	store32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
}
