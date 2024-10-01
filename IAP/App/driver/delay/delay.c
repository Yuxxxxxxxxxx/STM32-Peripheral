#include "delay.h"
#include "led.h"
#include <stdio.h>



/******************************************************************************
 * @brief     ͨ�ö�ʱ��4�жϳ�ʼ��
 *            ��ʱ�����ʱ����㷽��:Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *            Ft = ��ʱ������Ƶ��, ��λ: Mhz
 *
 * @param[in]  arr   :  �Զ���װֵ
 * @param[in]  psc   :  ʱ��Ԥ��Ƶ��
 *
 * @return     ��
 *
 ******************************************************************************/
void TIM4_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                  //ʹ��TIM4ʱ��

    TIM_TimeBaseInitStructure.TIM_Prescaler      = psc;                   //��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode    = TIM_CounterMode_Up;    //���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_Period         = arr;                   //�Զ���װ��ֵ
    TIM_TimeBaseInitStructure.TIM_ClockDivision  = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);                   //��ʼ����ʱ��4

    TIM_ITConfig(TIM4,TIM_IT_Update, ENABLE);                             //����ʱ��4�����ж�
    TIM_Cmd(TIM4, ENABLE);                                                //ʹ�ܶ�ʱ��4

    NVIC_InitStructure.NVIC_IRQChannel                    = TIM4_IRQn;    //��ʱ��4�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x02;         //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x03;         //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}




//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)  //����ж�
    {
		bl_led_toggle(GPIO_Pin_5);
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //����жϱ�־λ
    }  
	printf("timer 4\r\n");
}
