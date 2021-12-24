#include "../drivers/usb.h"
#include "../sys/core.h"

#define DRIVERS_USB_C
unsigned char usb_buffer0[256] = {0};
unsigned char usb_buffer1[256] = {0};

static struct UsbDeviceRequest udr = {
	.Type = 0xA0,     // DEVICE_TO_HOST | STDANDAD | DEVICE
	.Request = 0x06,  // GET_DESCRIPTOR
	.Value = 0x0100,  // descriptor.type = 0x01, decriptor.index = 0x00
	.Index = 0,
	.Length = 64,
};

void my_memcpy(void *dest, void *src, unsigned int n) {
   char *src_char = (char *)src;
   char *dest_char = (char *)dest;
   for (unsigned int i=0; i<n; i++) {
       dest_char[i] = src_char[i];
   }
}

static void txrx_control_msg(uint8_t txlen)
{
	// send setup & control packet
	// set dma buffer
	*USB_HOST_HCDMA0   = (unsigned long long) usb_buffer0;
	*USB_HOST_HCDMA0  |= 0xC0000000;
	// HCTSIZ0.Pid = 2'd3 (SETUP) , HCTSIZ0.PktCnt = 10'h1 , HCTSIZ0.XferSize = 18'd8
	*USB_HOST_HCTSIZ0 = 3 << 29 | 1 << 19 | txlen;
	// HCCAR1.ChEna = 1'b1
	*USB_HOST_HCCHAR0 |= 1<<31;

	// recieve control packet
	// set dma buffer
	*USB_HOST_HCDMA1   = (unsigned long long) usb_buffer1;
	*USB_HOST_HCDMA1  |= 0xC0000000;
	// HCTSIZ1.Pid = 2'd2 (DATA1) , HCTSIZ1.PktCnt = 10'h1 , HCTSIZ1.XferSize = 18'd64
	*USB_HOST_HCTSIZ1  = 2 << 29 | 1 << 19 | 64;
	// HCCAR1.ChEna = 1'b1
	*USB_HOST_HCCHAR1 |= 1<<31;
}

void send_packet(void)
{
	// HCCAR1.EPDir = 1'b0 (OUT) / 1'b01(IN), HCCAR1.MPS = 11'h40
	*USB_HOST_HCCHAR0   |= 0x40;            // OUT
	*USB_HOST_HCCHAR1   |= 1 << 15 | 0x40;  // IN

	// build packet
	my_memcpy(usb_buffer0, &udr, 8);

	txrx_control_msg(8);
}
