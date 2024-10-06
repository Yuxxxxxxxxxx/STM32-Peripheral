#ifndef __CONSOLE_UART_H
#define __CONSOLE_UART_H


#include <stdint.h>


void uart_init(void);
void uart_send(uint8_t data);
void uart_sendstring( char *str);


#endif /* __CONSOLE_UART_H */
