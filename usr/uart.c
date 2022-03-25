#define USR_UART_C
#include <graphics/lfb.h>
#include <sys/schedule.h>
#include <usr/string.h>
#include <usr/uart.h>

void handle_data(unsigned char data)
{
	// Newline Case
	if (data == 0x0D) {
	// Backspace Case
	} else if (data == 0x08 || data == 0x7F) {
	} else if (data == 0x61) {
		add_thread(uart_scheduler, 0, 2);
	} else if (data == 0x62) {
		//add_thread(test_entry, 0, 2);
	}
	// Draw it on the screen
	{
		draw_chex32(0, 9, data, 0xAA00FF);
	}
}
