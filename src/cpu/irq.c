#include "../cpu/irq.h"
#include "../sys/core.h"
#include "../drivers/uart.a.h"
#include "../util/time.h"

extern void disable_irq(void);
extern void enable_irq(void);

void c_irq_handler(void) {
	disable_irq();
	unsigned long source = load32(CORE0_IRQ_SOURCE);
	if (source & (1 << 8)) {
		if(load32(IRQ_PENDING2) & (1 << 25)) {
			if(load32(UART0_MIS) & (1<<4)) {
				unsigned long data = load32(UART0_DR);
				if(data == 0x74) {
					unsigned long timer_status;
					asm volatile("mrc p15, 0, %0, c14, c3, 1" : "=r"(timer_status));
					if(timer_status == 0) {
						cntfrq = read_cntfrq();
						write_cntv_tval(cntfrq);
						enable_cntv();
					} else {
						disable_cntv();
					}
				}
				uart_char((unsigned char)data);
				//uart_string((char*)" c_irq_handler\n");
				enable_irq();
				return;
			}
		}
	} else if (source & (1 << 3)) {
		c_timer();
		enable_irq();
	}
	return;
}
