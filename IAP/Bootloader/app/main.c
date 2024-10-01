#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "button.h"
#include "console_uart.h"
#include "led.h"
#include "main.h"
#include "iap.h"


// #define LOG_LVL     ELOG_LVL_INFO
// #define LOG_TAG     "main"
// #include "elog.h"


extern void board_lowlevel_init(void);
extern void start_printf(void);      


int main(void)
{
	board_lowlevel_init();

	bl_delay_init();
	bl_button_init();
	bl_led_init();
	bl_uart_init();

	uint32_t time = 0;
	uint32_t oldcount = 0; // 旧的串口接收数据值
	uint32_t applenth = 0; // 接收到的app代码长度
	uint8_t start_flag = 0;

	start_printf();

	while (1)
	{
		if (UART_RX_CNT && start_flag == 0)
		{
			if (UART_RX_BUF_BIN[0] == 'y' && UART_RX_BUF_BIN[1] == 'e' && UART_RX_BUF_BIN[2] == 's')
			{
				start_flag = 1;
				printf("请发送更新固件包\r\n");
			}

			UART_RX_CNT = 0;
		}

		if (UART_RX_CNT && start_flag == 1)
		{
			if (oldcount == UART_RX_CNT) // 新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				applenth = UART_RX_CNT;
				oldcount = 0;
				UART_RX_CNT = 0;
				printf("用户程序接收完成！\r\n");
				printf("程序包长度: %d Bytes\r\n", applenth);

				if (applenth)
				{
					printf("开始更新固件包......\r\n");
					if ( ( ( *(__IO uint32_t *)(0X20001000 + 4) ) & 0xFF000000 ) == 0x08000000 ) // 判断是否为0X08XXXXXX.
					{
						bl_iap_write_app_bin(FLASH_APP1_ADDR, UART_RX_BUF_BIN, applenth); // 更新FLASH代码
						printf("地址为(0X20001000 + 4): %X\r\n", *(__IO uint32_t *)(0X20001000 + 4));
						printf("固件包更新完成\r\n");
					}
					else
					{
						printf("地址错误: %X!!!\r\n", *(__IO uint32_t *)(0X20001000 + 4));
					}
				}

				if ( ( ( *(__IO uint32_t *)(FLASH_APP1_ADDR + 4) ) & 0xFF000000 ) == 0x08000000 ) // 判断是否为0X08XXXXXX.
				{
					printf("开始执行Flash应用程序\r\n");
					bl_iap_load_app(FLASH_APP1_ADDR); // 执行FLASH APP代码
				}
				else
				{
					printf("地址错误: %X\r\n!!!", *(__IO uint32_t *)(FLASH_APP1_ADDR + 4));
				}
			}
			else
				oldcount = UART_RX_CNT;
		}

		time++;
		bl_delay_ms(10);

		if (time % 100 == 0 && start_flag == 0)
			printf("倒计时 %d s......\r\n", 11 - time / 100);

		if (time >= 1000)
			time = 1000;
		if (time == 1000 && start_flag == 0)
		{
			printf("开始运行应用程序\r\n");
			if ( ( ( *(__IO uint32_t *)(FLASH_APP1_ADDR + 4) ) & 0xFF000000 ) == 0x08000000 ) // 判断是否为0X08XXXXXX.
			{
				printf("开始执行FLASH应用程序\r\n");
				bl_iap_load_app(FLASH_APP1_ADDR); // 执行FLASH APP代码
			}
			else
			{
				printf("地址错误!!!\r\n");
			}
		}
	}
}
