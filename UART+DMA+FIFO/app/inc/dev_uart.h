/******************************************************************************
 * @file dev_uart.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-08
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef __DEV_UART_H
#define __DEV_UART_H


#include <stdint.h>
#include <stdbool.h>

#define DEV_UART1	0
#define DEV_UART2	1


void uart_device_init(uint8_t uart_id);
uint16_t uart_write(uint8_t uart_id, const uint8_t *buf, uint16_t size);
uint16_t uart_read(uint8_t uart_id, uint8_t *buf, uint16_t size);
void uart_dmarx_done_isr(uint8_t uart_id);
void uart_dmarx_half_done_isr(uint8_t uart_id);
void uart_dmarx_idle_isr(uint8_t uart_id);
void uart_dmatx_done_isr(uint8_t uart_id);
void uart_poll_dma_tx(uint8_t uart_id);

#endif /* __DEV_UART_H */
