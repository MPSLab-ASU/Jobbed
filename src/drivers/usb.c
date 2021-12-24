#include "../drivers/uart.h"
#include "../drivers/usb.h"
#include "../sys/core.h"

#define DRIVERS_USB_C
unsigned char usb_buffer0[256] = {0};
unsigned char usb_buffer1[256] = {0};
unsigned char usb_buffer3[256] = {0};

void my_memcpy(void *dest, void *src, size_t n) {
	char *src_char = (char *)src;
	char *dest_char = (char *)dest;
	for (size_t i=0; i<n; i++) {
		dest_char[i] = src_char[i];
	}
}

static void *my_memset(void *s, int c, size_t n)
{
	const uint8_t uc = c % 0xff;
	uint8_t  *p = (uint8_t *)s;

	while (n-- > 0)
		*p++ = uc;

	return (s);
}

static void txrx_control_msg(uint8_t txlen)
{
	// send setup & control packet
	// set dma buffer
	*USB_HOST_HCDMA0   = (uint64_t) usb_buffer0;
	*USB_HOST_HCDMA0  |= 0xC0000000;
	// HCTSIZ0.Pid = 2'd3 (SETUP) , HCTSIZ0.PktCnt = 10'h1 , HCTSIZ0.XferSize = 18'd8
	*USB_HOST_HCTSIZ0 = 3 << 29 | 1 << 19 | txlen;
	// HCCAR1.ChEna = 1'b1
	*USB_HOST_HCCHAR0 |= 1<<31;

	// recieve control packet
	// set dma buffer
	*USB_HOST_HCDMA1   = (uint64_t) usb_buffer1;
	*USB_HOST_HCDMA1  |= 0xC0000000;
	// HCTSIZ1.Pid = 2'd2 (DATA1) , HCTSIZ1.PktCnt = 10'h1 , HCTSIZ1.XferSize = 18'd64
	*USB_HOST_HCTSIZ1  = 2 << 29 | 1 << 19 | 64;
	// HCCAR1.ChEna = 1'b1
	*USB_HOST_HCCHAR1 |= 1<<31;
}

void usbhost_id(void)
{
	uart_string("USB_CORE_GUID    ");
	uart_hex(*USB_CORE_GUID);
	uart_string("\nUSB_CORE_GSNPSID ");
	uart_hex(*USB_CORE_GSNPSID);
	uart_char('\n');
}

void usbhost_start(void)
{
	// USB Host power on
	// HPRT.PrtPwr = 1'b1 -> HPRT.PrtRst = 1'b1 -> wait 60msec -> HPRT.PrtRst = 1'b0
	*USB_HOST_HPRT |= 1 << 12;
	*USB_HOST_HPRT |= 1 << 8;
	delay(0x100000);
	*USB_HOST_HPRT &= ~(1 << 8);
	
	// enable irq
	// GAHBCFG.GlblIntrMsk = 1'b1
	// GINTMSK.ConIDStsChngMsk = 1'b1, GINTMSK.PrtIntMsk = 1'b1, GINTMSK.SofMsk = 1'b1
	*USB_CORE_GAHBCFG   |= 1;
	*USB_CORE_GINTMSK    =  1 << 28 | 1 << 24 | 0 << 3;
	
	// port enable and retry detect
	// HPRT.PrtPwr = 1'b1, HPRT.PrtEnChng = 1'b1, HPRT.PrtConnDet = 1'b1
	*USB_HOST_HPRT = 1 << 12 | 1 << 3 | 1 << 1; 
	
	// enable channel irq
	// HAINTMASK.HAINTMsk = 16'h3
	// HCINTMSK0.XferComplMsk = 1'b1
	*USB_HOST_HAINTMSK   |= 0x3;
	*USB_HOST_HCINTMSK0  |= 1;
	*USB_HOST_HCINTMSK1  |= 1;
	
	// HCCAR1.EPDir = 1'b0 (OUT) / 1'b01(IN), HCCAR1.MPS = 11'h40
	*USB_HOST_HCCHAR0   |= 0x40;            // OUT
	*USB_HOST_HCCHAR1   |= 1 << 15 | 0x40;  // IN
}

void usbhost_poll_intr(void)
{
	uint8_t dev   = 2;

	my_memset(usb_buffer3, 0, 256);

	// HCCAR3.EPType = 2'b11 (Interupt), HCCAR3.EPDir = 1'b01(IN), HCCAR3.MPS = 11'h40
	*USB_HOST_HCCHAR3   |= 3 << 18 | 1 << 15 | 1 << 11 | 0x40;  // IN
	*USB_HOST_HCCHAR3   |= dev << 22 ;

	// recieve interrupt transfer
	// set dma buffer
	*USB_HOST_HCDMA3   = (uint64_t) usb_buffer3;
	*USB_HOST_HCDMA3  |= 0xC0000000;
	// HCTSIZ1.Pid = 2'd2 (DATA1) , HCTSIZ1.PktCnt = 10'h1 , HCTSIZ1.XferSize = 18'd64
	*USB_HOST_HCTSIZ3  = 2 << 29 | 1 << 19 | 64;
	// HCCAR1.ChEna = 1'b1
	*USB_HOST_HCCHAR3 |= 1<<31;

	uart_string("HUB INT ");
	for (int i = 0; i < 2; i++) {
		uart_hex(usb_buffer3[i]);
		uart_char(' ');
	}
	uart_char('\n');
}

static void prepare_control_msg(void)
{
	my_memset(usb_buffer0, 0, 256);
	my_memset(usb_buffer1, 0, 256);

	// HCCAR1.EPDir = 1'b0 (OUT) / 1'b01(IN), HCCAR1.MPS = 11'h40
	*USB_HOST_HCCHAR0   |= 0x40;            // OUT
	*USB_HOST_HCCHAR1   |= 1 << 15 | 0x40;  // IN

	//clear dev
	*USB_HOST_HCCHAR0   &= ~(0x7f << 22);
	*USB_HOST_HCCHAR1   &= ~(0x7f << 22);
}


void usbhost_get_descriptor(uint8_t dev, uint8_t descriptor_type, bool is_hub)
{
	prepare_control_msg();

	*USB_HOST_HCCHAR0   |= dev << 22;
	*USB_HOST_HCCHAR1   |= dev << 22;

	// build packet
	my_memcpy(usb_buffer0, &(struct UsbDeviceRequest) {
			.Type = is_hub ? 0xA0 : 0x80,     // DEVICE_TO_HOST | STANDARD | DEVICE
			.Request = 0x06,  // GET_DESCRIPTOR
			.Value = descriptor_type << 8 | 0x00,  // descriptor.type = 0x01, decriptor.index = 0x00
			.Index = 0,
			.Length = 64,
			}, 8);

	txrx_control_msg(8);
}

void usbhost_get_portstatus(uint8_t dev, uint8_t port,  bool is_hub)
{
	prepare_control_msg();

	*USB_HOST_HCCHAR0   |= dev << 22;
	*USB_HOST_HCCHAR1   |= dev << 22;

	// build packet
	my_memcpy(usb_buffer0, &(struct UsbDeviceRequest) {
			.Type = is_hub ? 0xA3 : 0x80,     // DEVICE_TO_HOST | CLASS or STANDARD | DEVICE
			.Request = 0x00,  // GET_STATUS
			.Index = port,
			.Length = 4,
			}, 8);

	txrx_control_msg(8);

	uart_string("PORT STATUS ");
	uart_10(port);
	for (int i = 0; i < 4; i++) {
		uart_hex(usb_buffer1[i]);
		uart_char(' ');
	}
	uart_char('\n');
}

void usbhost_set_address(uint8_t address)
{
	prepare_control_msg();

	// build packet
	my_memcpy(usb_buffer0, &(struct UsbDeviceRequest) {
			.Type = 0,
			.Request = 5,		// Set address request
			.Value = address,	// Address to set
			}, 8);

	txrx_control_msg(8);
}

void usbhost_set_config(uint8_t dev, uint8_t config)
{
	prepare_control_msg();

	*USB_HOST_HCCHAR0   |= dev << 22;
	*USB_HOST_HCCHAR1   |= dev << 22;

	// build packet
	my_memcpy(usb_buffer0, &(struct UsbDeviceRequest) {
			.Type = 0,
			.Request = 9,		// Set config
			.Value = config,	// Address to set
			.Length = 0,
			}, 8);

	txrx_control_msg(8);
}

void usbhost_set_portfeature(uint8_t dev, uint8_t port,  uint16_t feature)
{
	prepare_control_msg();

	*USB_HOST_HCCHAR0   |= dev << 22;
	*USB_HOST_HCCHAR1   |= dev << 22;

	// build packet
	my_memcpy(usb_buffer0, &(struct UsbDeviceRequest) {
			.Type = 0x23,     // ??
			.Request = 0x03,  // SET_FEATURE
			.Value = (uint16_t)feature,                             // Feature we are changing
			.Index = port,
			.Length = 0,
			}, 8);

	txrx_control_msg(8);
}

void usbhost_clear_portfeature(uint8_t dev, uint8_t port, uint16_t feature)
{
	prepare_control_msg();

	*USB_HOST_HCCHAR0   |= dev << 22;
	*USB_HOST_HCCHAR1   |= dev << 22;

	// build packet
	my_memcpy(usb_buffer0, &(struct UsbDeviceRequest) {
			.Type = 0x23,     // ??
			.Request = 0x01,  // CLEAR_FEATURE
			.Value = feature,
			.Index = port,
			.Length = 0,
			}, 8);

	txrx_control_msg(8);
}

void usb_trig(void)
{
	usbhost_id();
	usbhost_start();

	uart_string("GET DEVICE_DESCRIPTOR\n");
	usbhost_get_descriptor(0, 0x01, false);
	for (int i = 0; i < 18; i++) {
		uart_hex(usb_buffer1[i]);
		uart_char(' ');
	}
	uart_char('\n');

	uart_string("SET ADDRESS 2\n");
	usbhost_set_address(2);

	uart_string("GET CONFIG_DESCRIPTOR\n");
	usbhost_get_descriptor(2, 0x02, false);
	for (int i = 0; i < 18; i++) {
		uart_hex(usb_buffer1[i]);
		uart_char(' ');
	}
	uart_char('\n');

	uart_string("SET CONFIG 1\n");
	usbhost_set_config(2, 1);

	uart_string("GET HUB_DESCRIPTOR\n");
	usbhost_get_descriptor(2, 41, true);
	for (int i = 0; i < 18; i++) {
		uart_hex(usb_buffer1[i]);
		uart_char(' ');
	}
	uart_char('\n');
	uart_string("Hub device 0 has ");
	uart_10(usb_buffer1[2]);
	uart_string(" ports\n");

	usbhost_get_portstatus(2, 0, false);
	usbhost_get_portstatus(2, 1, true);
	usbhost_clear_portfeature(2, 1,  0x0010);
	usbhost_get_portstatus(2, 1, true);

	uart_string("SET PORT 1 POWER ON\n");
	usbhost_set_portfeature(2, 1, 0x0008);
	usbhost_get_portstatus(2, 1, true);

	delay(0x100000);

	uart_string("CLEAR PORT FEATURE\n");
	usbhost_clear_portfeature(2, 1, 0x0010);
	//    usbhost_get_portstatus(2, 1, true);
	//    usbhost_poll_intr();

	uart_string("SET PORT 1 RESET\n");
	usbhost_set_portfeature(2, 1, 0x0004);
	//    usbhost_poll_intr();

	uart_string("CLEAR PORT FEATURE\n");
	usbhost_clear_portfeature(2, 1,  0x0014);
	usbhost_clear_portfeature(2, 1,  0x0011);
	//    usbhost_get_portstatus(2, 1, true);
	//    usbhost_poll_intr();

	uart_string("GET DEVICE_DESCRIPTOR\n");
	usbhost_get_descriptor(0, 0x01, false);
	for (int i = 0; i < 18; i++) {
		uart_hex(usb_buffer1[i]);
		uart_char(' ');
	}
	uart_string("\n");

	usbhost_set_address(3);
	uart_string("GET CONFIG_DESCRIPTOR\n");
	usbhost_get_descriptor(3, 0x02, false);
	for (int i = 0; i < 18; i++) {
		uart_hex(usb_buffer1[i]);
		uart_char(' ');
	}
	uart_string("\n");
}
