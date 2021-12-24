#ifndef DRIVERS_USB_H
#define DRIVERS_USB_H

#ifndef DRIVERS_USB_C
extern unsigned char usb_buffer0[256];
extern unsigned char usb_buffer1[256];
#endif

#define size_t unsigned int
#define bool unsigned char
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long
#define uint64_t unsigned long long
#define true 1
#define false 0

#define USB_BASE          0x3F980000
//CORE
#define USB_CORE_GAHBCFG       ((volatile uint32_t *)(0x8   + USB_BASE))
#define USB_CORE_GINTSTS       ((volatile uint32_t *)(0x14  + USB_BASE))
#define USB_CORE_GINTMSK       ((volatile uint32_t *)(0x18  + USB_BASE))
#define USB_CORE_GUID          ((volatile uint32_t *)(0x3C  + USB_BASE))
#define USB_CORE_GSNPSID       ((volatile uint32_t *)(0x40  + USB_BASE))
//HOST
#define USB_HOST_HCFG          ((volatile uint32_t *)(0x400 + USB_BASE))
#define USB_HOST_HAINTMSK      ((volatile uint32_t *)(0x418 + USB_BASE))
#define USB_HOST_HPRT          ((volatile uint32_t *)(0x440 + USB_BASE))
//CHANNEL
#define USB_HOST_HCCHAR0       ((volatile uint32_t *)(0x500 + USB_BASE))
#define USB_HOST_HCINTMSK0     ((volatile uint32_t *)(0x50C + USB_BASE))
#define USB_HOST_HCTSIZ0       ((volatile uint32_t *)(0x510 + USB_BASE))
#define USB_HOST_HCDMA0        ((volatile uint32_t *)(0x514 + USB_BASE))
#define USB_HOST_HCCHAR1       ((volatile uint32_t *)(0x520 + USB_BASE))
#define USB_HOST_HCINTMSK1     ((volatile uint32_t *)(0x52C + USB_BASE))
#define USB_HOST_HCTSIZ1       ((volatile uint32_t *)(0x530 + USB_BASE))
#define USB_HOST_HCDMA1        ((volatile uint32_t *)(0x534 + USB_BASE))

#define USB_HOST_HCCHAR2       ((volatile uint32_t *)(0x540 + USB_BASE))
#define USB_HOST_HCINTMSK2     ((volatile uint32_t *)(0x54C + USB_BASE))
#define USB_HOST_HCTSIZ2       ((volatile uint32_t *)(0x550 + USB_BASE))
#define USB_HOST_HCDMA2        ((volatile uint32_t *)(0x554 + USB_BASE))
#define USB_HOST_HCCHAR3       ((volatile uint32_t *)(0x560 + USB_BASE))
#define USB_HOST_HCINTMSK3     ((volatile uint32_t *)(0x56C + USB_BASE))
#define USB_HOST_HCTSIZ3       ((volatile uint32_t *)(0x570 + USB_BASE))
#define USB_HOST_HCDMA3        ((volatile uint32_t *)(0x574 + USB_BASE))

struct UsbDeviceRequest {
	uint8_t Type;
	uint8_t Request;
	uint16_t Value;
	uint16_t Index;
	uint16_t Length;
} __attribute__((__packed__));

void send_packet(void);
void usbhost_start(void);
void usbhost_id(void);
void usbhost_get_descriptor(uint8_t dev, uint8_t descriptor_type, bool is_hub);
void usbhost_get_portstatus(uint8_t dev, uint8_t port,  bool is_hub);
void usbhost_set_address(uint8_t address);
void usbhost_set_config(uint8_t dev, uint8_t config);
void usbhost_set_portfeature(uint8_t dev, uint8_t port,  uint16_t feature);
void usbhost_clear_portfeature(uint8_t dev, uint8_t port, uint16_t feature);
void usbhost_poll_intr(void);
void usb_trig(void);

#endif
