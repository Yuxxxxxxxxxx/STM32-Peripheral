#include <stdbool.h>
#include "stm32f4xx.h"
#include "led.h"


static bool led_state_1; // ָʾled״̬
static bool led_state_2;

/******************************************************************************
 * @brief led ��ʼ��
 * 
 * @return ��
 * 
******************************************************************************/
void bl_led_init(void)
{
    // PA5
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    //GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    bl_led_off(GPIO_Pin_5);
    bl_led_off(GPIO_Pin_6);
}

/******************************************************************************
 * @brief �� led д
 * 
 * @param[in]  on    :  д��״̬��true/false
 * 
 * @return ��
 * 
******************************************************************************/
void bl_led_set(bool on, uint32_t pin)
{
    //led_state = on;
    GPIO_WriteBit(GPIOE, pin, on ? Bit_RESET : Bit_SET);
}

/******************************************************************************
 * @brief �� led
 * 
 * @return ��
 * 
******************************************************************************/
void bl_led_on(uint32_t pin)
{
    //led_state = true;
    bl_led_set(true, pin);
}

/******************************************************************************
 * @brief �ر� led
 * 
 * @return ��
 * 
******************************************************************************/
void bl_led_off(uint32_t pin)
{
    //led_state = false;
    bl_led_set(false, pin);
}

/******************************************************************************
 * @brief led �Ƶ�ƽ��ת
 * 
 * @return ��
 * 
******************************************************************************/
void bl_led_toggle(uint32_t pin)
{
    //led_state = !led_state;
    if (pin == GPIO_Pin_5)
	{
		led_state_1 = !led_state_1;
		bl_led_set(led_state_1, pin);
	}
	else if (pin == GPIO_Pin_6)
	{
		led_state_2 = !led_state_2;
		bl_led_set(led_state_2, pin);
	}
	
}
