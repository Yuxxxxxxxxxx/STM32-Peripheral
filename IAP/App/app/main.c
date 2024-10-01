#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "console_uart.h"
#include "led.h"
#include "delay.h"

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
void SysTick_Handler(void)
{
    ticks++;
}

int main(void)
{
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
    
	SCB->VTOR = FLASH_BASE | 0x10000;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	uart_init();
	bl_delay_init();
	bl_led_init();
	
	
	printf("\r\n");
	
	bl_led_on(GPIO_Pin_5);
	bl_led_off(GPIO_Pin_6);
	
	TIM4_Init(1000 - 1, 8400 - 1);
	
	/*while (1) 
	{
		printf("welcome to App Data test2!\r\n");
		bl_led_toggle(GPIO_Pin_5);
		bl_delay_ms(1000);
		bl_led_toggle(GPIO_Pin_6);
		bl_delay_ms(1000);
	}*/
	
}

