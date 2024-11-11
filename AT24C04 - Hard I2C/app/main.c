/******************************************************************************
 * @file main.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  STM32F4 系列驱动AT24CX系列E2PROM测试程序
 * @version 0.1
 * @date 2024-11-11
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

/******************************************************************************
 * @brief      I2C等待事件超时的情况下会调用这个函数来处理
 * 
 * @param[in]  errorCode   :   错误码  
 * @return     uint32_t 
******************************************************************************/
uint8_t i2c_err_callback(uint8_t err_code)
{               
	switch (err_code)
	{
		case I2C_ERR_OK: {
			printf("Error: %s", err_str[I2C_ERR_OK]);
		}
		case I2C_ERR_START: {
			printf("Error: %s", err_str[I2C_ERR_START]);
		}
		case I2C_ERR_DEVICE_ADDR: {
			printf("Error: %s", err_str[I2C_ERR_DEVICE_ADDR]);
		}
		case I2C_ERR_WRITE_ADDR: {
			printf("Error: %s", err_str[I2C_ERR_WRITE_ADDR]);
		}
		case I2C_ERR_WRITE_DATA: {
			printf("Error: %s", err_str[I2C_ERR_WRITE_DATA]);
		}
		case I2C_ERR_STOP: {
			printf("Error: %s", err_str[I2C_ERR_STOP]);
		}
		case I2C_ERR_WAIT_BUSY: {
			printf("Error: %s", err_str[I2C_ERR_WAIT_BUSY]);
		}
		case I2C_ERR_READ: {
			printf("Error: %s", err_str[I2C_ERR_READ]);
		}
	}
    return 0;
}

// 最大值，因为AT24C04的最大容量就是4Kb=512*8，数组为 8*8（页大小）*64（大小）
#define TEST_SIZE 4

uint8_t test_array1[TEST_SIZE * AT24CX_PAGE_SIZE]; // 注：AT24C04时，AT24CX_PAGE_SIZE=8
uint8_t test_array2[TEST_SIZE * AT24CX_PAGE_SIZE]; //     AT24C04时，一个页面有24个字节


#define TEST_NUM 1
//#define TEST_STRING

#ifdef TEST_NUM
void at24c04_test_page_write(void)
{
	uint16_t i;

	for (i = 0; i < 3 * AT24CX_PAGE_SIZE; ++i) {
		test_array1[i] = 100 + i;
	}

	memset(test_array2, 0x00, 4 * AT24CX_PAGE_SIZE);

	/** 一次只能写入16个字节 */
	at24cx_write_page(test_array1, 0, AT24CX_PAGE_SIZE);   
	bl_delay_ms(100);
	at24cx_write_page(test_array1, 16, AT24CX_PAGE_SIZE);
	
	bl_delay_ms(100);
	
	at24cx_read_buf(test_array2, 0, 4 * AT24CX_PAGE_SIZE);

	for (i = 0; i < sizeof(test_array2); ++i)
	{
		printf("%d, ", test_array2[i]);
	}
}

void at24c04_test_bytes_write(void)
{
	uint16_t i;
	//uint16_t j;

	for (i = 0; i < TEST_SIZE * AT24CX_PAGE_SIZE; ++i) {
		test_array1[i] = 10 + i;
	}

	memset(test_array2, 0x00, TEST_SIZE * AT24CX_PAGE_SIZE);

	at24cx_write_bytes(test_array1, 0, TEST_SIZE * AT24CX_PAGE_SIZE);   
	bl_delay_ms(100);
	
	at24cx_read_buf(test_array2, 0, TEST_SIZE * AT24CX_PAGE_SIZE);

	for (i = 0; i < sizeof(test_array2); ++i)
	{
		printf("%d, ", test_array2[i]);
	}
}

void at24c04_test_buffer_write(void)
{
	uint16_t i;
	//uint16_t j;

	for (i = 0; i < TEST_SIZE * AT24CX_PAGE_SIZE; ++i) {
		test_array1[i] = 1 + i;
	}

	memset(test_array2, 0x00, TEST_SIZE * AT24CX_PAGE_SIZE);

	at24cx_write_buffer(test_array1, 0, TEST_SIZE * AT24CX_PAGE_SIZE);   
	bl_delay_ms(100);
	
	at24cx_read_buf(test_array2, 0, TEST_SIZE * AT24CX_PAGE_SIZE);

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

	at24cx_write_buffer(data, 0, sizeof(data));
	at24cx_read_buf(res, 0, sizeof(data));

	for(int i=0;i<sizeof(data);++i)
	{
		printf("%c ", res[i]);
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

	at24cx_i2c_err_callback_register(i2c_err_callback);
	
	printf("test begin\r\n");
#ifdef TEST_STRING
	at24c04_test_string();
#elif  (TEST_NUM == 1)
	at24c04_test_page_write();
	//at24c04_test_buffer_write();
	//at24c04_test_bytes_write();
#endif

	return 0;
}
