#include <cpu.h>
#include <cpu/irq.h>
#include <drivers/uart.h>
#include <globals.h>
#include <graphics/drawer.h>
#include <symbols.h>
#include <sys/core.h>
#include <sys/schedule.h>
#include <sys/timer.h>
#include <util/mutex.h>
#include <util/status.h>
#include <util/time.h>

void utime(void);
void testfxn(void);
void handle_data(unsigned char);

void c_irq_handler(void)
{
	unsigned long source = load32(CORE0_IRQ_SOURCE);
	if (source & (1 << 8)) {
		if(load32(IRQ_PENDING2) & (1 << 25)) {
			if(load32(UART0_MIS) & (1<<4)) {
				unsigned long data = load32(UART0_DR);
				{
					unsigned int x = g_Drawer.x;
					unsigned int y = g_Drawer.y;
					g_Drawer.x = 0;
					g_Drawer.y = 14;
					write_hex32(&g_Drawer, data);
					g_Drawer.x = x;
					g_Drawer.y = y;
				}

				// Ctrl+T to toggle timer
				if(data == 0x14) {
					unsigned long timer_status;
					asm volatile("mrc p15, 0, %0, c14, c3, 1" : "=r"(timer_status));
					unsigned int x = g_Drawer.x;
					unsigned int y = g_Drawer.y;
					g_Drawer.x = 0;
					g_Drawer.y = 5;
					write_string(&g_Drawer, "TIMER: ");
					if(timer_status == 0) {
						cntfrq = read_cntfrq();
						write_cntv_tval(cntfrq/CPS);
						enablecntv();
						write_cstring(&g_Drawer, "Enabled ", 0x00FF00);
					} else {
						disablecntv();
						write_cstring(&g_Drawer, "Disabled", 0xFF0000);
					}
					g_Drawer.x = x;
					g_Drawer.y = y;
				// Ctrl+R to reset
				} else if(data == 0x12) {
					_start();
				} else {
					add_thread(handle_data, (void*)data, 1);
				}
				return;
			}
		} else if (*(unsigned long*)SYS_TIMER_CS == SYS_TIMER_SC_M0) {
			volatile unsigned long* timer_cs = (unsigned long*)SYS_TIMER_CS;
			volatile unsigned long* timer_chi = (unsigned long*)SYS_TIMER_CHI;
			volatile unsigned long* nexttime = (unsigned long*)SYS_TIMER_C0;
			*timer_cs = SYS_TIMER_SC_M0;
			*nexttime = *timer_chi + 60000000;
		}
	} else if (source & (1 << 3)) {
		c_timer();
		return;
	}
	return;
}

static unsigned long counter = 0;
unsigned long c_fiq_handler(void)
{
	unsigned long source = load32(CORE0_FIQ_SOURCE);
	if (source & (1 << 3)) {
		c_timer();
		counter++;
		if (counter % 0x6000 == 0) {
			counter = 0;
		}
		if (counter % 0x30 == 0) {
			return 1;
		} 
		return 0;
	}
	return 0;
}

void handle_data(unsigned char data)
{
	// Newline Case
	if (data == 0x0D) {
	// Backspace Case
	} else if (data == 0x08 || data == 0x7F) {
	// Lock Case
	} else if (data == 0x6C) {
		lock_mutex(&exe_cnt_m, SYS_PID);
	// Release Case
	} else if (data == 0x72) {
		release_mutex(&exe_cnt_m, SYS_PID);
	} else if (data == 0x61) {
		add_thread(testfxn, 0, 3);
	} else if (data == 0x62) {
		add_thread(uart_scheduler, 0, 2);
	} else if (data == 0x63) {
		add_thread(heap_info, 0, 2);
	} else if (data == 0x64) {
		add_thread(utime, 0, 2);
	} else {
	}
	g_Drawer.x = 0;
	g_Drawer.y = 7;
	for(int i = 0; i < 128; i++)
		write_char(&g_Drawer, ' ');
	g_Drawer.x = 0;
	g_Drawer.y = 7;
	write_string(&g_Drawer, "> ");
}

void utime(void)
{
	unsigned long thi, tlo;
	unsigned long long t = get_sys_time();
	thi = t >> 32;
	tlo = t;
	uart_hex(thi);
	uart_hexn(tlo);
}

void testfxn2(void)
{
	uart_string("Ran testfxn2\n");
}

void testfxn(void)
{
	unsigned long long ti = get_sys_time();
	unsigned int i = 0xDEADBEEF;
	void* a = malloc(5);
	void* b = malloc(3);
	void* c = malloc(4);
	void* d = malloc(4);
	uart_string("Start\n");
	add_thread(testfxn2, 0, 0);
	sys0(SYS_YIELD);
	uart_string("Freeing B\n");
	free(b);
	uart_string("Freeing A\n");
	free(a);
	uart_string("Freeing C\n");
	free(c);
	delay(0x20000000);
	uart_string("Freeing D\n");
	free(d);
	delay(0x20000000);
	uart_hexn(i);
	uart_string("End\n");
	unsigned long long tf = get_sys_time();
	unsigned long long dt = tf-ti;
	unsigned long thi, tlo;
	thi = dt >> 32;
	tlo = dt;
	uart_hex(thi);
	uart_hexn(tlo);
}
