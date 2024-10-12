#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "bsp_uart.h"
#include "dev_uart.h"

// 加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
// 重定义fputc函数
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0)
        ; // 循环发送,直到发送完毕
    USART1->DR = (u8)ch;
    return ch;
}
#endif

/******************************************************************************
 * @brief  初始化 uart 引脚
 * 
 * @return none
 * 
******************************************************************************/
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
	
	//USART_OverSampling8Cmd(USART1, ENABLE);
}

/******************************************************************************
 * @brief  初始化 USART1
 * 
 * @return none
 * 
******************************************************************************/
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
	
	// 使能 DMA 请求
    USART_DMACmd(USART1, USART_DMAReq_Rx|  USART_DMAReq_Tx, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  // 使能空闲中断
}

/******************************************************************************
 * @brief  配置中断
 * 
 * @return none
 * 
******************************************************************************/
static void uart_irq_init(void)
{
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

    NVIC_InitTypeDef NVIC_InitStructure;
    memset(&NVIC_InitStructure, 0, sizeof(NVIC_InitTypeDef));

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         // 1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************
 * @brief      配置 USART1 DMA 发送和接收
 * 
 * @param[in]  mem_addr    :  
 * @param[in]  mem_size    :  
 * 
 * @return     none
 * 
******************************************************************************/
void uart_dma_init(uint8_t *mem_addr, uint32_t mem_size)
{
    uart_pin_init();
	uart_irq_init();
    uart_lowlevel_init();

    bsp_uart_dmarx_config(mem_addr, mem_size);  
}

/******************************************************************************
 * @brief      配置 USART1 DMA 发送
 * 
 * @param[in]  mem_addr    :    发送缓存区
 * @param[in]  mem_size    :    发送缓存区大小
 * 
 * @return     none
 * 
******************************************************************************/
void bsp_uart_dmatx_config(uint8_t *mem_addr, uint32_t mem_size)
{
    DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA2_Stream7);
	
    // 1. 禁用DMA流
    DMA_Cmd(DMA2_Stream7, DISABLE);
    
    // 2. 配置DMA
    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;               // 选择通道4        
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;  // 数据方向：从内存到外设
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Low;           // 优先级低         
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;             // 普通模式    
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存地址递增     
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据对齐为字节
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;     // 内存数据对齐为字节      
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;        // 禁用FIFO模式
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);     // USART数据寄存器地址
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)mem_addr;          // 内存地址
    DMA_InitStructure.DMA_BufferSize         = mem_size;                    // 数据长度
	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;  // FIFO阈值设置
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;      // 内存突发模式
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;  // 外设突发模式

    // 3. 初始化DMA
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);

    // 4. 清除DMA标志
    DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_FEIF7);

    // 5. 使能DMA传输并启动传输
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC | DMA_IT_TE, ENABLE);  // 使能传输完成中断
    DMA_Cmd(DMA2_Stream7, ENABLE);                  // 启动DMA流
}

/******************************************************************************
 * @brief      配置 USART1 DMA 接收
 * 
 * @param[in]  mem_addr    :    接收缓存区
 * @param[in]  mem_size    :    接收缓存区大小
 * 
 * @return     none
 * 
******************************************************************************/
void bsp_uart_dmarx_config(uint8_t *mem_addr, uint32_t mem_size)
{
    // 配置DMA流
    DMA_InitTypeDef DMA_InitStructure;

    // 重设DMA流                                                                              
    DMA_DeInit(DMA2_Stream2);

    // 先禁用DMA流
    DMA_Cmd(DMA2_Stream2, DISABLE);

    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;                // 选择通道4       
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;   // 数据流向：外设到内存 
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Low;             // 设置优先级为低    
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;            // 设置为循环模式   
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;    // 外设地址不增量     
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;         // 内存地址增量     
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据对齐为字节
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;      // 内存数据对齐为字节 
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         // 禁用FIFO
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);      // USART数据寄存器地址
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)mem_addr;           // 指定内存地址
    DMA_InitStructure.DMA_BufferSize         = mem_size;                     // 设置数据长度
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;   // FIFO阈值设置
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;       // 设置内存突发模式
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;   // 设置外设突发模式

    // 初始化DMA
    DMA_Init(DMA2_Stream2, &DMA_InitStructure);
    
    // 启用DMA中断
    DMA_ITConfig(DMA2_Stream2, DMA_IT_HT | DMA_IT_TC, ENABLE);
    
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2);

    // 启动DMA流
    DMA_Cmd(DMA2_Stream2, ENABLE);
}

/******************************************************************************
 * @brief  获取 DMA 接收缓存区剩余大小
 * 
 * @return uint16_t 
 * 
******************************************************************************/
uint16_t bsp_uart_get_dmarx_buf_remain_size(void)
{
    DMA_GetCurrDataCounter(DMA2_Stream2);
}

/******************************************************************************
 * @brief  USART1 中断处理函数
 * 
 * @return none
 * 
******************************************************************************/
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE)!= RESET)
    {
        uart_dmarx_idle_isr(DEV_UART1);
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
    }
}

/******************************************************************************
 * @brief  DMA2 stream2 中断处理函数
 * 
 * @return none
 * 
******************************************************************************/
void DMA2_Stream2_IRQHandler(void)
{
    // 检查半传输完成中断
    if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_HTIF2))
    {
        uart_dmarx_half_done_isr(DEV_UART1);
        DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_HTIF2);  // 清除半传输完成中断标志
    }

    // 检查传输完成中断
    if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2)) 
    {
        uart_dmarx_done_isr(DEV_UART1);
        DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);  // 清除传输完成中断标志
    }
}

/******************************************************************************
 * @brief  DMA2 stream7 中断处理函数
 *                                                                                                                z
 * @return none
 * 
******************************************************************************/
void DMA2_Stream7_IRQHandler(void)
{
    // 检查传输完成中断
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7)) 
    {
        uart_dmatx_done_isr(DEV_UART1);
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);  // 清除传输完成中断标志
    }
}
