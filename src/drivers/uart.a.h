#ifndef UART_AH
#define UART_AH

extern void uart_char(unsigned char c);
extern void uart_hex(unsigned long data);
extern void uart_string(char* message);

#endif
