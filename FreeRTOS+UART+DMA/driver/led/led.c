#include "led.h"



//�ߵ�ƽ�𣬵͵�ƽ��
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
 * @brief     ��ʼ��LED
 *
 * @param[in]  led   :  led�ṹ��ָ��
 * @param[in]  port  :  ���Ŷ˿�
 * @param[in]  pin   :  ����
 *
 * @return     0, ��ʾ��ʼ���ɹ�, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int LED_Init(LED_t *led, LED_GPIO_Port_t port, uint32_t pin) //IO��ʼ��
{
    if (!led)
        return -1;

    //�������ţ�Ĭ�����
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
 * @brief      ��led
 *
 * @param[in]  led   :  led�ṹ��ָ��
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int LED_On(LED_t *led)
{
    if (!led || !led->init)
        return -1;  //��ʼ������֮��

    __LED_IO_SET(led->port, led->pin, LED_PIN_ON);   //�͵�ƽ��
    led->status = true;

    return 0;
}




/******************************************************************************
 * @brief      �ر�led
 *
 * @param[in]  led   :  led�ṹ��ָ��
 *
 * @return     0, ��ʾ����, ����ֵ��ʾʧ��
 *
 ******************************************************************************/
int LED_Off(LED_t *led)
{
    if (!led || !led->init)
        return -1;  //��ʼ������֮��
    
    __LED_IO_SET(led->port, led->pin, LED_PIN_OFF);   //�ߵ�ƽ��
    led->status = false;

    return 0;
}
