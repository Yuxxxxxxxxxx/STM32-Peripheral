#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "console_uart.h"
#include <stdio.h>


//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 
 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;
	return ch;
}
 
#endif


static void uart_pin_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
}

static void uart_lowlevel_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef USART_InitStructure;
    memset(&USART_InitStructure, 0, sizeof(USART_InitTypeDef));

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

static void uart_irq_init(void)
{
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
	
    NVIC_InitTypeDef NVIC_InitStructure;
    memset(&NVIC_InitStructure, 0, sizeof(NVIC_InitTypeDef));

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void uart_init(void)
{
    uart_pin_init();
    uart_lowlevel_init();
    uart_irq_init();
}

void uart_send(uint8_t data)
{
    USART_SendData(USART1, data);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

void uart_sendstring( char *str)
{
    unsigned int k=0;
    do {
        uart_send( *(str + k) );
        k++;
    } while (*(str + k)!='\0');

    /* 等待发送完成 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET) {
    }
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);
		USART_SendData(USART1, data);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

