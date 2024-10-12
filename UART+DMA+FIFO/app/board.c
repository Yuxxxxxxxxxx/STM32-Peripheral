/******************************************************************************
 * @file board.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-22
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include "stm32f4xx.h"
//#include "console_uart.h"
#include <stdio.h>

void board_lowlevel_init(void)
{
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}

/*void board_lowlevel_deinit(void)
{
	bl_uart_deinit();

	SysTick->CTRL = 0;

	// __disable_irq();
}*/
