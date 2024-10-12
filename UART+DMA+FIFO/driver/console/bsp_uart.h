#ifndef __BSP_UART_H
#define __BSP_UART_H


#include <stdint.h>


void uart_dma_init(uint8_t *mem_addr, uint32_t mem_size);
void bsp_uart_dmatx_config(uint8_t *mem_addr, uint32_t mem_size);
void bsp_uart_dmarx_config(uint8_t *mem_addr, uint32_t mem_size);
uint16_t bsp_uart_get_dmarx_buf_remain_size(void);


#endif /* __BSP_UART_H */
