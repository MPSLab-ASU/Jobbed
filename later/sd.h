#ifndef DRIVERS_SD_H
#define DRIVERS_SD_H

#define SD_OK                0
#define SD_TIMEOUT          (unsigned long)-1
#define SD_ERROR            (unsigned long)-2

int sd_init();
int sd_readblock(unsigned int lba, unsigned char *buffer, unsigned int num);

#endif
