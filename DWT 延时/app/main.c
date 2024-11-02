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
#include "dwt.h"

#if DEBUG
#define LOG_TAG "mn" // 定义日志标签，用于下面的 log_d() 函数
#define LOG_LVL ELOG_LVL_INFO
#include "elog.h"
#endif

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

	bsp_dwt_init();
	
	uint8_t ucKeyCode;
	uint8_t read;
	const char buf1[] = "接收到串口命令1\r\n";
	const char buf2[] = "接收到串口命令2\r\n";
	const char buf3[] = "接收到串口命令3\r\n";
	const char buf4[] = "接收到串口命令4\r\n";
	
	//printf("Test dddsfadffd asg df: \r\n");
	
	/* 使用PC0测试时间 */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
			
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	/* 复用模式 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);		
		
	}

	while (1)
	{
		//com_send_buf(COM1, (uint8_t *)buf1, strlen(buf1));
		//USART_SendData(USART1, 'A');
		GPIOC->BSRR = (uint32_t)GPIO_Pin_0;
		bsp_delay_US(1);
		GPIOC->BSRR = (uint32_t)GPIO_Pin_0 << 16;
		bsp_delay_US(1);
	}
}
