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
 * @brief     初始化LED
 *
 * @param[in]  led   :  led结构体指针
 * @param[in]  port  :  引脚端口
 * @param[in]  pin   :  引脚
 *
 * @return     0, 表示初始化成功, 其他值表示失败
 *
 ******************************************************************************/
int LED_Init(LED_t *led, LED_GPIO_Port_t port, uint32_t pin) //IO初始化
{
    if (!led)
        return -1;

    //配置引脚，默认输出
#if  defined  (STM32F40_41xxx)

    assert_param(IS_GPIO_ALL_PERIPH(port));

    if (port  == GPIOA)        { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); }
    else if (port  == GPIOB)   { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); }
    else if (port  == GPIOC)   { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); }
    else if (port  == GPIOD)   { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); }
    else if (port  == GPIOE)   { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); }
    else if (port  == GPIOF)   { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); }
    else if (port  == GPIOG)   { RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); }
    else return -1;;

#endif

    led->port = port;
    led->pin = pin;
    led->init = true;
    led->status = false;

    __LED_CONFIG_IO_OUTPUT(led->port, led->pin);
    LED_Off(led);

    return 0;
}




/******************************************************************************
 * @brief      打开led
 *
 * @param[in]  led   :  led结构体指针
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
int LED_On(LED_t *led)
{
    if (!led || !led->init)
        return -1;  //初始化引脚之后

    __LED_IO_SET(led->port, led->pin, LED_PIN_ON);   //低电平亮
    led->status = true;

    return 0;
}




/******************************************************************************
 * @brief      关闭led
 *
 * @param[in]  led   :  led结构体指针
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
int LED_Off(LED_t *led)
{
    if (!led || !led->init)
        return -1;  //初始化引脚之后
    
    __LED_IO_SET(led->port, led->pin, LED_PIN_OFF);   //高电平灭
    led->status = false;

    return 0;
}
