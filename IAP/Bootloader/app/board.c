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
#include "console_uart.h"
#include <stdio.h>

void board_lowlevel_init(void)
{
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

void board_lowlevel_deinit(void)
{
	bl_uart_deinit();

	SysTick->CTRL = 0;

	// __disable_irq();
}

void start_printf(void)
{
	printf("###############################################################\r\n");
	printf("## ______             _   _                     _            ##\r\n");
	printf("## | ___ \\           | | | |                   | |           ##\r\n");
	printf("## | |_/ / ___   ___ | |_| |     ___   __ _  __| | ___ _ __  ##\r\n");
	printf("## | ___ \\/ _ \\ / _ \\| __| |    / _ \\ / _` |/ _` |/ _ \\ '__| ##\r\n");
	printf("## | |_/ / (_) | (_) | |_| |___| (_) | (_| | (_| |  __/ |    ##\r\n");
	printf("## \\____/ \\___/ \\___/ \\__\\_____/\\___/ \\__,_|\\__,_|\\___|_|    ##\r\n");
	printf("###############################################################\r\n");
	printf("\r\n");
	printf("###############################################################\r\n");
	printf("## _    _  _____         _____ _______                       ##\r\n");
	printf("##| |  | |/ ____|  /\\   |  __ \\__   __|                      ##\r\n");
	printf("##| |  | | (___   /  \\  | |__) | | |                         ##\r\n");
	printf("##| |  | |\\___ \\ / /\\ \\ |  _  /  | |                         ##\r\n");
	printf("##| |__| |____) / ____ \\| | \\ \\  | |                         ##\r\n");
	printf("## \\____/|_____/_/    \\_\\_|  \\_\\ |_|                         ##\r\n");
	printf("###############################################################\r\n");
	printf("\r\n");
	printf("[user] Version 1.0                                               \r\n");
	printf("[user] Last change 2023-03-28                                    \r\n");
	printf("[user] BootLoader starting . . . . . .\r\n");
	printf("[user] BootLoader  code Flash addr from 0x8000000 to 0x8010000 | size  64K bytes \r\n");
	printf("[user] Application code Flash addr from 0x8010000 to 0x8100000 | size 960K bytes \r\n");
	printf("[user] BootLoader start finished\r\n");
	printf("[user] 欢迎进入BootLoader引导程序，本程序适用于STM32F407ZGT6\r\r\n");
	printf("是否进入BootLoader程序？（请输入yes）\r 否则10s后开始运行应用程序\r\n");
}
