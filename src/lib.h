enum
{
	// The offset for the MMIO area
#ifdef BSP23
	MMIO_BASE = 0x3F000000, // For Raspberry Pi 2 and 3
#else
	MMIO_BASE = 0xFE000000, // For Raspberry Pi 2 and 3
#endif

	// The offsets for reach register.
	GPIO_BASE = (MMIO_BASE + 0x200000),

	// Controls actuation of pull up/down to ALL GPIO pins.
	GPPUD = (GPIO_BASE + 0x94),

	// Controls actuation of pull up/down for specific GPIO pin.
	GPPUDCLK0 = (GPIO_BASE + 0x98),

	// The base address for UART.
	UART0_BASE = (GPIO_BASE + 0x1000), // for raspi4 0xFE201000, raspi2 & 3 0x3F201000, and 0x20201000 for raspi1

	// The offsets for reach register for the UART.
	UART0_DR     = (UART0_BASE + 0x00),
	UART0_RSRECR = (UART0_BASE + 0x04),
	UART0_FR     = (UART0_BASE + 0x18),
	UART0_ILPR   = (UART0_BASE + 0x20),
	UART0_IBRD   = (UART0_BASE + 0x24),
	UART0_FBRD   = (UART0_BASE + 0x28),
	UART0_LCRH   = (UART0_BASE + 0x2C),
	UART0_CR     = (UART0_BASE + 0x30),
	UART0_IFLS   = (UART0_BASE + 0x34),
	UART0_IMSC   = (UART0_BASE + 0x38),
	UART0_RIS    = (UART0_BASE + 0x3C),
	UART0_MIS    = (UART0_BASE + 0x40),
	UART0_ICR    = (UART0_BASE + 0x44),
	UART0_DMACR  = (UART0_BASE + 0x48),
	UART0_ITCR   = (UART0_BASE + 0x80),
	UART0_ITIP   = (UART0_BASE + 0x84),
	UART0_ITOP   = (UART0_BASE + 0x88),
	UART0_TDR    = (UART0_BASE + 0x8C),

	// IRQ REGISTERS
	IRQ_BASE          = (MMIO_BASE + 0xB000),
	IRQ_BASIC_PENDING = (IRQ_BASE + 0x200),
	IRQ_PENDING1      = (IRQ_BASE + 0x204),
	IRQ_PENDING2      = (IRQ_BASE + 0x208),
	FIQ_CONTROL       = (IRQ_BASE + 0x20C),
	IRQ_ENABLE1       = (IRQ_BASE + 0x210),
	IRQ_ENABLE2       = (IRQ_BASE + 0x214),
	IRQ_BASIC_ENABLE  = (IRQ_BASE + 0x218),
	IRQ_DISABLE1      = (IRQ_BASE + 0x21C),
	IRQ_DISABLE2      = (IRQ_BASE + 0x220),
	IRQ_BASIC_DISABLE = (IRQ_BASE + 0x224),

	// Peripherals Interrupts
	UART_IRQ   = 57,
	GPIO_IRQ_0 = 49,
	GPIO_IRQ_1 = 50,
	GPIO_IRQ_2 = 51,
	GPIO_IRQ_3 = 52,

	FIQ_ENABLE_FLAG = 1<<7,

	// ARM Peripheral Interrupts
	TIMER_ARM_IRQ     = 0,
	MAILBOX_ARM_IRQ   = 1,
	DOORBELL0_ARM_IRQ = 2,
	DOORBELL1_ARM_IRQ = 3,
	GPU0HALT_ARM_IRQ  = 4,
	GPU1HALT_ARM_IRQ  = 5,

	// The offsets for Mailbox registers
	MBOX_BASE    = 0xB880,
	MBOX_READ    = (MBOX_BASE + 0x00),
	MBOX_STATUS  = (MBOX_BASE + 0x18),
	MBOX_WRITE   = (MBOX_BASE + 0x20),

	GPU_INTERRUPTS_ROUTING = 0x4000000C
};
