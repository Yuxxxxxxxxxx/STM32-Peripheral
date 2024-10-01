#include "delay.h"
#include "led.h"
#include <stdio.h>



/******************************************************************************
 * @brief     通用定时器4中断初始化
 *            定时器溢出时间计算方法:Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *            Ft = 定时器工作频率, 单位: Mhz
 *
 * @param[in]  arr   :  自动重装值
 * @param[in]  psc   :  时钟预分频数
 *
 * @return     无
 *
 ******************************************************************************/
void TIM4_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                  //使能TIM4时钟

    TIM_TimeBaseInitStructure.TIM_Prescaler      = psc;                   //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode    = TIM_CounterMode_Up;    //向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period         = arr;                   //自动重装载值
    TIM_TimeBaseInitStructure.TIM_ClockDivision  = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);                   //初始化定时器4

    TIM_ITConfig(TIM4,TIM_IT_Update, ENABLE);                             //允许定时器4更新中断
    TIM_Cmd(TIM4, ENABLE);                                                //使能定时器4

    NVIC_InitStructure.NVIC_IRQChannel                    = TIM4_IRQn;    //定时器4中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x02;         //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x03;         //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}




//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)  //溢出中断
    {
		bl_led_toggle(GPIO_Pin_5);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //清除中断标志位
    }  
	printf("timer 4\r\n");
}
