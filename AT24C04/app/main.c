/******************************************************************************
 * @file main.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  STM32F4 系列驱动AT24CX系列E2PROM测试程序
 * @version 0.1
 * @date 2024-09-23
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "console_uart.h"
#include "at24c.h"
#include "ctl_i2c.h"

extern void board_lowlevel_init(void);

uint8_t test_array1[3 * AT24CX_PAGE_SIZE]; // 注：AT24C04时，AT24CX_PAGE_SIZE=8
uint8_t test_array2[3 * AT24CX_PAGE_SIZE]; //     AT24C04时，一个页面有24个字节


#define TEST_NUM

#ifdef TEST_NUM
void at24c04_test_num(void)
{
	uint16_t i;
	uint16_t j;

	for (i = 0; i < 3 * AT24CX_PAGE_SIZE; i++)
	{
		if (i >= 256)
			j = i - 256; // test_array1[256---383] 单元初始化数值 = 1---128
		else if (i >= 128)
			j = i - 128; // test_array1[128---255] 单元初始化数值 = 1---128
		else
			j = i; // test_array1[0---127] 单元初始化数值 = 1---128
		test_array1[i] = j + 1;
	}

	memset(test_array2, 0x00, 3 * AT24CX_PAGE_SIZE);

	if (at24cx_checkok() == 1) // 如果检测到I2C器件存在
	{
		at24cx_writebytes(test_array1, 80, 3 * AT24CX_PAGE_SIZE); // 从I2C的地址80处开始写3页字节（测试跨页连续写）
		at24cx_readbytes(test_array2, 80, 3 * AT24CX_PAGE_SIZE);  // 从I2C的地址80处开始读3页字节（测试跨页连续读）
	}

	for (i = 0; i < sizeof(test_array2); ++i)
	{
		printf("%d, ", test_array2[i]);
	}
}
#endif

#ifdef TEST_STRING
void at24c04_test_string(void)
{
	uint8_t data[13] = "hello world!"; //{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
	uint8_t res[13] = { 0 };

	if (at24cv_checkok() == 1)
	{
		at24cx_writebytes(data, 80, sizeof(data));
		at24cx_readbytes(res, 80, sizeof(data));
	}

	for(int i=0;i<sizeof(data);++i)
	{
		printf("%c", res[i]);
	}
}
#endif

int main(void)
{
	board_lowlevel_init();
	bl_delay_init();
	bl_led_init();

	uart_init();

	ctl_at24cxx_i2c_init();

	printf("test begin\r\n");
	//at24c04_test_string();
    at24c04_test_num();

	/*if (ee_Test() == 1)
	{
		bl_led_on(GPIO_Pin_5);
		printf("\r\n SUCCESS \r\n");
	}
	else
	{
		bl_led_on(GPIO_Pin_6);
		printf("\r\n ERROR!!! \r\n");
	}*/

	/*while (1)
	{
		bl_led_on(GPIO_Pin_5);
		bl_delay_ms(500);
		bl_led_on(GPIO_Pin_6);
		bl_delay_ms(500);
		bl_led_off(GPIO_Pin_5);
		bl_delay_ms(500);
		bl_led_off(GPIO_Pin_6);
		bl_delay_ms(500);
	}*/

	return 0;
}
