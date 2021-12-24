#ifndef DRIVERS_USB_H
#define DRIVERS_USB_H

#ifndef DRIVERS_USB_C
extern unsigned char usb_buffer0[256];
extern unsigned char usb_buffer1[256];
#endif

#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long

void send_packet(void);

#endif
