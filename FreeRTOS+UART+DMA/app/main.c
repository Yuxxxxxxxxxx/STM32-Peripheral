/******************************************************************************
 * @file main.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-23
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include "main.h"


extern void board_lowlevel_init(void);
extern void test_task(void);


void main(void)
{
    board_lowlevel_init();
    uart_init();

	printf("Start!\r\n");
    test_task();
}


