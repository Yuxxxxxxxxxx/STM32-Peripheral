/******************************************************************************
 * @file main.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-27
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "console_uart.h"
#include "ws2812.h"
#include "main.h"


extern void board_lowlevel_init(void);


int main(void)
{
	board_lowlevel_init();
	uart_init();
	bl_delay_init();
	
	printf("test begin");
	
	ws2812_init();
	
	while (1)
		rainbow_cycle(240);
	
	//rainbowCycle(240);
	// while (1)            
	// {
	// 	RGB_GREEN(240);  //8个灯全绿
	// 	bl_delay_ms(500);
	// 	RGB_YELLOW(240);	//8个灯全黄
	// 	bl_delay_ms(500);
	// 	RGB_RED(240);		//8个灯全红
	// 	bl_delay_ms(500);
	// 	RGB_BLUE(240);	//8个灯全蓝
	// }
}
