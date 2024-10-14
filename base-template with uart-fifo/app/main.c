/******************************************************************************
 * @file user_lib.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  This file contains the function prototypes of the user library.
 * @version 0.1
 * @date 2024-10-14  移植了 segger 和 easylogger 两个组件以及带有 FIFO 的 UART
 * 
 * @copyright Copyright (c) 2024
 *
 * @note      使用 easylogger 时，要注意在 Project->Options for Target->C/C++
 *            ->Preprocessor Symbols->Define 添加 DEBUG，否则不会打印结果
 *            要注意，log_x 不能和 printf 同时使用
 * 
******************************************************************************/
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

#define LOG_TAG "mn" // 定义日志标签，用于下面的 log_d() 函数
#define LOG_LVL ELOG_LVL_INFO
#include "elog.h"

extern void board_lowlevel_init(void);


int main(void)
{
	board_lowlevel_init();
	bsp_uart_init();
	
#if DEBUG 
	elog_init();
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
	elog_start();
#endif
	
	bsp_led_init();
	bsp_button_init();
	
	bsp_timer_init();

	log_a("a_Hello EasyLogger!");
    log_e("e_Hello EasyLogger!");
    log_w("w_Hello EasyLogger!");
    log_i("i_Hello EasyLogger!");
    log_d("d_Hello EasyLogger!");
    log_v("v_Hello EasyLogger!");
	
	uint8_t ucKeyCode;
	uint8_t read;
	const char buf1[] = "接收到串口命令1\r\n";
	const char buf2[] = "接收到串口命令2\r\n";
	const char buf3[] = "接收到串口命令3\r\n";
	const char buf4[] = "接收到串口命令4\r\n";
	
	//printf("Test dddsfadffd asg df: \r\n");
	
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
			bsp_led_toggle(GPIO_Pin_5);
			log_i("Button clicked!!!");
		}
	}
}
