#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "button.h"
#include "uart_fifo.h"
#include "timer.h"

extern void board_lowlevel_init(void);

int main(void)
{
	board_lowlevel_init();
	
	bsp_led_init();
	bsp_button_init();
	bsp_uart_init();
	bsp_timer_init();

	uint8_t ucKeyCode;
	uint8_t read;
	const char buf1[] = "接收到串口命令1\r\n";
	const char buf2[] = "接收到串口命令2\r\n";
	const char buf3[] = "接收到串口命令3\r\n";
	const char buf4[] = "接收到串口命令4\r\n";
	
	printf("Test dddsfadffd asg df: \r\n");
	
	while (1)
	{
		if (com_get_char(COM1, &read))
		{
			switch(read)
			{
				case '1': 
					com_send_buf(COM1, (uint8_t *)buf1, strlen(buf1));
					break;
				case '2':
					com_send_buf(COM1, (uint8_t *)buf2, strlen(buf2));
					break;
				case '3':
					com_send_buf(COM1, (uint8_t *)buf3, strlen(buf3));
					break;
				case '4':
					com_send_buf(COM1, (uint8_t *)buf4, strlen(buf4));
					break;
				default:
					printf("Error Code!\r\n");
					break;
			}
		}
		
		if (bsp_button_pressed())
		{
			printf("Button clicked!!!\r\n");
		}
	}
}
