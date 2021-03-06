#ifndef DRIVERS_UART_H
#define DRIVERS_UART_H

#define PRINTS_DBG(X) uart_char('[');uart_string(__FILE__);uart_char(':');uart_10(__LINE__);uart_string("] ");uart_string(X);uart_char('\n');
#define PRINTX_DBG(X) uart_char('[');uart_string(__FILE__);uart_char(':');uart_10(__LINE__);uart_string("] ");uart_hex((unsigned long)X);uart_char('\n');
#define PRINTI_DBG(X) uart_char('[');uart_string(__FILE__);uart_char(':');uart_10(__LINE__);uart_string("] ");uart_10((unsigned long)X);uart_char('\n');
#define PRINT64(x) uart_hex(x>>32);uart_hexn(x);

extern void uart_char(unsigned char c);
extern void uart_string(char* message);
extern void uart_hex(unsigned long data);

void uart_init();
void* uart_print(char*);
void uart_flush(void);
void uart_10(unsigned long);
void uart_hexn(unsigned long);

#endif
