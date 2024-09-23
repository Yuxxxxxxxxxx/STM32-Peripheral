/******************************************************************************
 * @file console_uart.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-23
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include "console_uart.h"
#include "FreeRTOS.h"
#include "semphr.h"


/* 标准库需要的支持函数 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{ 
    x = x;
}

/* 重定义fputc函数 */
int fputc(int ch, FILE *f)
{
    while((USART1->SR & 0X40) == 0); //循环发送,直到发送完毕
    USART1->DR = (uint8_t)ch;
    return ch;
}

static SemaphoreHandle_t uart_tx_done_semphr;
static SemaphoreHandle_t uart_tx_busy_mux;

static void uart_pin_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void uart_lowlevel_init(void)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

static void uart_irq_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void uart_dma_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_InitStructure.DMA_BufferSize           =   0;                               // 缓冲区大小   
    DMA_InitStructure.DMA_Channel              =   DMA_Channel_4;                   // DMA通道4
    DMA_InitStructure.DMA_DIR                  =   DMA_DIR_MemoryToPeripheral;      // 内存到外设
    DMA_InitStructure.DMA_FIFOMode             =   DMA_FIFOMode_Disable;            // 禁用FIFO模式
    DMA_InitStructure.DMA_FIFOThreshold        =   DMA_FIFOThreshold_1QuarterFull;  // FIFO阈值:1/4满
    DMA_InitStructure.DMA_Mode                 =   DMA_Mode_Normal;                 // 正常模式
    DMA_InitStructure.DMA_Memory0BaseAddr      =   0;                               // 内存源地址
    DMA_InitStructure.DMA_MemoryDataSize       =   DMA_MemoryDataSize_Byte;         // 内存数据长度
    DMA_InitStructure.DMA_MemoryBurst          =   DMA_MemoryBurst_Single;          // 单次传输
    DMA_InitStructure.DMA_MemoryInc            =   DMA_MemoryInc_Enable;            // 内存地址自增
    DMA_InitStructure.DMA_PeripheralBaseAddr   =   (uint32_t)&USART1->DR;           // 外设地址
    DMA_InitStructure.DMA_PeripheralDataSize   =   DMA_PeripheralDataSize_Byte;     // 外设数据长度
    DMA_InitStructure.DMA_PeripheralBurst      =   DMA_PeripheralBurst_Single;      // 单次传输
    DMA_InitStructure.DMA_PeripheralInc        =   DMA_PeripheralInc_Disable;       // 外设地址不自增
    DMA_InitStructure.DMA_Priority             =   DMA_Priority_Medium;             // 中等优先级

    DMA_Init(DMA2_Stream7, &DMA_InitStructure);
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA2_Stream7, DISABLE);
}

void uart_init(void)
{
    uart_tx_done_semphr = xSemaphoreCreateBinary();
    configASSERT(uart_tx_done_semphr);

    uart_tx_busy_mux = xSemaphoreCreateMutex();
    configASSERT(uart_tx_busy_mux);

    uart_pin_init();
    uart_lowlevel_init();
    uart_irq_init();
    uart_dma_init();
}

void DMA2_Stream7_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7) != RESET)
    {
        DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
        DMA_Cmd(DMA2_Stream7, DISABLE);

        BaseType_t pxHigherPriorityTaskWoken;
        xSemaphoreGiveFromISR(uart_tx_done_semphr, &pxHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
}

void uart_write(const uint8_t *data, uint32_t length)
{
    xSemaphoreTake(uart_tx_busy_mux, portMAX_DELAY);

    DMA2_Stream7->M0AR = (uint32_t)data;
    DMA2_Stream7->NDTR = length;
    DMA_Cmd(DMA2_Stream7, ENABLE);

    xSemaphoreTake(uart_tx_done_semphr, portMAX_DELAY);

    xSemaphoreGive(uart_tx_busy_mux);
}
