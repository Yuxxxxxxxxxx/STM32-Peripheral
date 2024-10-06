#include <stdbool.h>
#include "stm32f4xx.h"
#include "led.h"


static bool led_state; // 指示led状态


/******************************************************************************
 * @brief led 初始化
 * 
 * @return 无
 * 
******************************************************************************/
void bl_led_init(void)
{
    // PA5
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    //GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    bl_led_off();
}

/******************************************************************************
 * @brief 向 led 写
 * 
 * @param[in]  on    :  写入状态，true/false
 * 
 * @return 无
 * 
******************************************************************************/
void bl_led_set(bool on)
{
    led_state = on;
    GPIO_WriteBit(GPIOE, GPIO_Pin_5, on ? Bit_RESET : Bit_SET);
}

/******************************************************************************
 * @brief 打开 led
 * 
 * @return 无
 * 
******************************************************************************/
void bl_led_on(void)
{
    led_state = true;
    bl_led_set(true);
}

/******************************************************************************
 * @brief 关闭 led
 * 
 * @return 无
 * 
******************************************************************************/
void bl_led_off(void)
{
    led_state = false;
    bl_led_set(false);

}

/******************************************************************************
 * @brief led 灯电平反转
 * 
 * @return 无
 * 
******************************************************************************/
void bl_led_toggle(void)
{
    led_state = !led_state;
    bl_led_set(led_state);
}
