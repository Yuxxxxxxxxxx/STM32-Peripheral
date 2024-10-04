/******************************************************************************
 * @file button.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  按键配置
 * @version 0.1
 * @date 2024-08-25
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"


/******************************************************************************
 * @brief 按键初始化
 * 
 * @return 无
 * 
******************************************************************************/
void bl_button_init(void)
{
    // PA6
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/******************************************************************************
 * @brief 按下按键触发函数
 * 
 * @return 是否按下按键 true/false
 * 
******************************************************************************/
bool bl_button_pressed(void)
{
    return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == Bit_RESET;
}
