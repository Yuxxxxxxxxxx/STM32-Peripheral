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
#include "cm_backtrace.h"

#define LOG_TAG "mn" // 定义日志标签，用于下面的 log_d() 函数
#define LOG_LVL ELOG_LVL_INFO
#include "elog.h"

extern void board_lowlevel_init(void);

#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"

extern void fault_test_by_unalign(void);
extern void fault_test_by_div0(void);

int test3(int a) {
	volatile int * SCB_CCR = (volatile int *) 0xE000ED14; // SCB->CCR
	*SCB_CCR |= (1 << 4); /* bit4: DIV_0_TRP. */
	
	int x;
	x = 0;
    int res = a / x;
	
	return res;
}
 
int test2(int arg) {
	int res = test3(arg) * 2;
	return res;
}
 
int test1(int a, int b) {
	int a2 = test2(a);
	return a2 + b;
}

int main(void)
{
	board_lowlevel_init();
	
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
	bsp_uart_init();
	bsp_timer_init();
	
	cm_backtrace_init("CmBacktrace", HARDWARE_VERSION, SOFTWARE_VERSION);
	
	//fault_test_by_div0(); // test1
	test1(1, 2);            // test2
 
    uint8_t _continue = 1;
    while(_continue == 1) {
        bsp_led_on(GPIO_Pin_5);
        bsp_delay_ms(1000);
        bsp_led_off(GPIO_Pin_5);
        bsp_delay_ms(1000);
    }
}
