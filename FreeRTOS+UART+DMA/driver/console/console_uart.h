#ifndef  __CONSOLE_UART_H
#define  __CONSOLE_UART_H

#include <stdio.h>
#include "stm32f4xx.h"


void uart_init(void);
void uart_write(const uint8_t *data, uint32_t length);

#endif /* __CONSOLE_UART_H */
