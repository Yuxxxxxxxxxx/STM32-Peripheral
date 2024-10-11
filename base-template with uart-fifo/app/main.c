#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "button.h"
#include "uart_fifo.h"

extern void board_lowlevel_init(void);

int main(void)
{
	board_lowlevel_init();
	
	bl_delay_init();
	bl_led_init();
	bl_button_init();
	bsp_InitUart();

	uint8_t ucKeyCode;
	uint8_t read;
	const char buf1[] = "接收到串口命令1\r\n";
	const char buf2[] = "接收到串口命令2\r\n";
	const char buf3[] = "接收到串口命令3\r\n";
	const char buf4[] = "接收到串口命令4\r\n";
	
	printf("Test dddsfadffd asg df: \r\n");
	
	while (1)
	{
		if (comGetChar(COM1, &read))
		{
			switch(read)
			{
				case '1': 
					comSendBuf(COM1, (uint8_t *)buf1, strlen(buf1));
					break;
				case '2':
					comSendBuf(COM1, (uint8_t *)buf2, strlen(buf2));
					break;
				case '3':
					comSendBuf(COM1, (uint8_t *)buf3, strlen(buf3));
					break;
				case '4':
					comSendBuf(COM1, (uint8_t *)buf4, strlen(buf4));
					break;
				default:
					printf("Error Code!\r\n");
					break;
			}
		}
	}
}
