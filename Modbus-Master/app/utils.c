/******************************************************************************
 * @file utils.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  封装一些工具函数
 * @version 0.1
 * @date 2024-08-25
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"


static uint32_t ticks; // 时钟滴答数


/******************************************************************************
 * @brief 初始化Systick定时器
 * 
 * @return 无
 * 
******************************************************************************/
void bl_delay_init(void)
{
    SysTick_Config(SystemCoreClock / 1000); // 1ms定时器
}

/******************************************************************************
 * @brief 毫秒级延时函数
 * 
 * @param[in]  ms    :  延时毫秒数
 * 
 * @return 无
 * 
******************************************************************************/
void bl_delay_ms(uint32_t ms)
{
    uint32_t start = ticks;
    while (ticks - start < ms);
}

/******************************************************************************
 * @brief 返回系统从开机以来的毫秒数
 * 
 * @return uint32_t 
 * 
******************************************************************************/
uint32_t bl_now(void)
{
    return ticks;
}

/******************************************************************************
 * @brief 系统时钟中断处理函数
 * 
 * @return 无
 * 
******************************************************************************/
/*void SysTick_Handler(void)
{
    ticks++;
}*/