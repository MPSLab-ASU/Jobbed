#ifndef DRIVERS_UART_A_H
#define DRIVERS_UART_A_H

extern void uart_char(unsigned char c);
extern void uart_hex(unsigned long data);
extern void uart_string(char* message);

#endif
