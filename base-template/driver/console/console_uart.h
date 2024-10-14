#ifndef __CONSOLE_UART_H
#define __CONSOLE_UART_H


#include <stdint.h>


#define RX_BUFFER_SIZE 120*1024

extern uint8_t 	UART_RX_BUF_BIN[RX_BUFFER_SIZE];
extern uint32_t UART_RX_CNT;

void bsp_uart_init(void);
void bsp_uart_deinit(void);
void uart_send(uint8_t data);
void uart_sendstring( char *str);


#endif /* __CONSOLE_UART_H */
