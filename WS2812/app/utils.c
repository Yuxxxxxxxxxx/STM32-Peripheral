/******************************************************************************
 * @file utils.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-20
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "main.h"


static uint32_t msTicks;
static uint32_t usTicks;


/******************************************************************************
 * @brief 
 * 
 * @return none
 * 
******************************************************************************/
void bl_delay_init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

/******************************************************************************
 * @brief      微妙级延时
 * 
 * @param[in]  us    :  
 * 
 * @return     none
 * 
******************************************************************************/
void bl_delay_us(uint32_t us)
{
    uint32_t start_time = msTicks * 1000 + usTicks;     // 记录开始时间 
    while (msTicks * 1000 + usTicks - start_time < us); // 等待延时结束
}

/******************************************************************************
 * @brief      毫秒级延时
 * 
 * @param[in]  ms    :  
 * 
******************************************************************************/
void bl_delay_ms(uint32_t ms)
{
    uint32_t start = msTicks;
    while (msTicks - start < ms);
}

/******************************************************************************
 * @brief 
 * 
 * @return uint32_t 
 * 
******************************************************************************/
uint32_t bl_now(void)
{
    return msTicks;
}

/******************************************************************************
 * @brief 
 * 
 * @return none
 * 
******************************************************************************/
void SysTick_Handler(void)
{
    msTicks++;
    usTicks++;
}

