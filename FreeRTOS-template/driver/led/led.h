#ifndef __LED_H
#define __LED_H


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#if defined (STM32F40_41xxx)

#include "stm32f4xx.h"

typedef    GPIO_TypeDef*  LED_GPIO_Port_t;

#else

#error led.h: No processor defined!

#endif

typedef struct led_t {
    LED_GPIO_Port_t    port;   // 所使用的端口
    uint32_t           pin;    // 所使用的引脚
	uint32_t           clk_source;
    bool               status; // led状态，默认灭  status == false 灭   status == true 亮
    bool               init;   // 初始化标志
}LED_t;


/* 用户API */

int    LED_Init(LED_t *led);//IO初始化
int    LED_On(LED_t *led);
int    LED_Off(LED_t *led);


#endif /* __LED_H */
