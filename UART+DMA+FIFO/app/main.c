#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "button.h"
#include "bsp_uart.h"
#include "dev_uart.h"

extern void board_lowlevel_init(void);

uint8_t buf[256];
static uint32_t s_count = 0;
uint16_t size = 0;


int main(void)
{
	board_lowlevel_init();
	
	bl_delay_init();
	uart_device_init(DEV_UART1);

	while (1)
	{
		s_count++;

		if (s_count % 1000 == 0)
		{
			size = uart_read(DEV_UART1, buf, sizeof(buf));
			uart_write(DEV_UART1, buf, size);

			/* 将fifo数据拷贝到dma buf，并启动dma传输 */
			uart_poll_dma_tx(DEV_UART1);
		}
	}
}


