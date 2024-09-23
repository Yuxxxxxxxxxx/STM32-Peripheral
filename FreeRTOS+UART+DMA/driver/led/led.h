#ifndef __LED_H
#define __LED_H


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef  NULL
#define NULL (void*)0
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>




#if defined (STM32F40_41xxx)

#include "stm32f4xx.h"

typedef    GPIO_TypeDef*  LED_GPIO_Port_t;

#else

#error led.h: No processor defined!

#endif

typedef struct led_t
{
    LED_GPIO_Port_t     port;             //��ʹ�õĶ˿�
    uint32_t            pin;              //��ʹ�õ�����
    bool                status;           //led״̬��Ĭ����  status == false ��    status == true ��
    bool                init;             //��ʼ����־
}LED_t;


int    LED_Init(LED_t *led, LED_GPIO_Port_t port, uint32_t pin);//IO��ʼ��
int    LED_On(LED_t *led);
int    LED_Off(LED_t *led);

#endif
