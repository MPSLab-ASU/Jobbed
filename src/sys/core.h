#ifndef SYS_CORE_H
#define SYS_CORE_H

extern unsigned long cntfrq;

static inline unsigned long load32(unsigned long addr) {
	return *(volatile unsigned long*)addr;
}

static inline void store32(unsigned long value, unsigned long addr) {
	*(volatile unsigned long*)addr = value;
}

static inline void delay(unsigned long cycles) {
	asm volatile("__delay_%=: subs %[cycles], %[cycles], #1;bne __delay_%=\n"
			: "=r"(cycles): [cycles]"0"(cycles) : "cc");
}

enum
{
	// The offset for the MMIO area
#ifdef BSP23
	MMIO_BASE = 0x3F000000, // For Raspberry Pi 2 and 3
#else
	MMIO_BASE = 0xFE000000,
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

	GPU_INTERRUPTS_ROUTING = 0x4000000C,

	CORE0_TIMER_IRQCNTL = 0x40000040,
	CORE0_IRQ_SOURCE    = 0x40000060,

	/* Power Management, Reset controller and Watchdog registers */
	//BCM2835_PERI_BASE        = 0x3F000000,
	BCM2835_PERI_BASE        = 0x20000000,
	PM_BASE                  = (BCM2835_PERI_BASE + 0x100000), 
	PM_RSTC                  = (PM_BASE+0x1c),
	PM_WDOG                  = (PM_BASE+0x24),
	PM_WDOG_RESET            = 0x00000000,
	PM_PASSWORD              = 0x5a000000,
	PM_WDOG_TIME_SET         = 0x000fffff,
	PM_RSTC_WRCFG_CLR        = 0xffffffcf,
	PM_RSTC_WRCFG_SET        = 0x00000030,
	PM_RSTC_WRCFG_FULL_RESET = 0x00000020,
	PM_RSTC_RESET            = 0x00000102,
};

#define bool unsigned char
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long

void sysinit();
void postinit();

#endif
