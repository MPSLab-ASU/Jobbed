#ifndef DRIVERS_UART_H
#define DRIVERS_UART_H

#define PRINTS_DBG(X) uart_char('[');uart_string(__FILE__);uart_char(':');uart_10(__LINE__);uart_string("] ");uart_string(X);
#define PRINTI_DBG(X) uart_char('[');uart_string(__FILE__);uart_char(':');uart_10(__LINE__);uart_string("] ");uart_10(X);

extern void uart_char(unsigned char c);
extern void uart_hex(unsigned long data);
extern void uart_string(char* message);
void uart_hexn(unsigned long);
void uart_10(unsigned long);

#endif
