#include "led.h"


//高电平灭，低电平亮
#define     LED_PIN_ON                          0
#define     LED_PIN_OFF                         1

#if  defined  (STM32F40_41xxx)



#define    __LED_CONFIG_IO_OUTPUT(port, pin)         {GPIO_InitTypeDef                    GPIO_InitStructure; \
                                                      GPIO_InitStructure.GPIO_Mode     =  GPIO_Mode_OUT; \
                                                      GPIO_InitStructure.GPIO_OType    =  GPIO_OType_PP; \
                                                      GPIO_InitStructure.GPIO_PuPd     =  GPIO_PuPd_UP; \
                                                      GPIO_InitStructure.GPIO_Speed    =  GPIO_Speed_100MHz; \
                                                      GPIO_InitStructure.GPIO_Pin      =  pin ; \
                                                      GPIO_Init(port, &GPIO_InitStructure);}

#define    __LED_IO_SET(port, pin, value)            {if (value == LED_PIN_OFF) \
                                                           GPIO_SetBits(port, pin); \
                                                      else \
                                                           GPIO_ResetBits(port, pin); \
                                                     }
#endif


/******************************************************************************
 * @brief      初始化LED
 * 
 * @param[in]  led    :  led 结构体指针
 * @param[in]  port   :  引脚端口
 * @param[in]  pin    :  引脚
 * 
 * @return int        :  0, 表示初始化成功, 其他值表示失败
 * 
******************************************************************************/
int LED_Init(LED_t *led)
{
    // 使能时钟
	RCC_AHB1PeriphClockCmd(led->clk_source, ENABLE);

    led->init = true;
    led->status = false;

    __LED_CONFIG_IO_OUTPUT(led->port, led->pin); // led 初始化
    LED_Off(led);                                // 关闭 led         
	
	return 0;
}

/******************************************************************************
 * @brief      打开led
 * 
 * @param[in]  led    :  led结构体指针
 * 
 * @return int        :  0, 表示正常, 其他值表示失败
 * 
******************************************************************************/
int LED_On(LED_t *led)
{
    if (!led || !led->init)
        return -1;  // 初始化引脚之后

    __LED_IO_SET(led->port, led->pin, LED_PIN_ON); // 低电平亮
    led->status = true;

    return 0;
}

/******************************************************************************
 * @brief      关闭 led
 * 
 * @param[in]  led    :  led结构体指针
 * 
 * @return int        :  0, 表示正常, 其他值表示失败
 * 
******************************************************************************/
int LED_Off(LED_t *led)
{
    if (!led || !led->init)
        return -1;  // 初始化引脚之后

    __LED_IO_SET(led->port, led->pin, LED_PIN_OFF); // 高电平灭
    led->status = false;

    return 0;
}
