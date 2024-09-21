/******************************************************************************
 * @file w25q.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-20
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "ctl_spi.h"


/******************************************************************************
 * @brief  SPI GPIO初始化
 * 
 * @return none
 * 
******************************************************************************/
static void spi_pin_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitStructure));

    // CS
    GPIO_InitStructure.GPIO_Mode   =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType  =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd   =  GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed  =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin    =  GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIO_SetBits(GPIOA, GPIO_Pin_4);

    // SCK MISO MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 引脚复用
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
}

/******************************************************************************
 * @brief  SPI初始化
 * 
 * @return none
 * 
******************************************************************************/
static void spi_lowlevel_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    memset(&SPI_InitStructure, 0, sizeof(SPI_InitStructure));

    SPI_InitStructure.SPI_Direction          =   SPI_Direction_2Lines_FullDuplex; // 双线全双工
    SPI_InitStructure.SPI_BaudRatePrescaler  =   SPI_BaudRatePrescaler_256;       // 波特率预分频值为256
    SPI_InitStructure.SPI_CPHA               =   SPI_CPHA_2Edge;                  // 同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_CPOL               =   SPI_CPOL_High;                   // 同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CRCPolynomial      =   7;                               // CRC计算的多项式
    SPI_InitStructure.SPI_DataSize           =   SPI_DataSize_8b;                 // 8位帧数据结构
    SPI_InitStructure.SPI_FirstBit           =   SPI_FirstBit_MSB;                // 数据传输从MSB位开始
    SPI_InitStructure.SPI_Mode               =   SPI_Mode_Master;                 // 主机模式
    SPI_InitStructure.SPI_NSS                =   SPI_NSS_Soft;                    // NSS 信号由硬件(NSS管脚)还是软件(使用 SSI位)管理:内部NSS信号由SSI位控制 
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}

/******************************************************************************
 * @brief  初始化spi
 * 
 * @return none
 * 
******************************************************************************/
void spi_init(void)
{
    spi_pin_init();
    spi_lowlevel_init();
}

/******************************************************************************
 * @brief      SPI 数据读写函数
 * 
 * @param[in]  tx_data    :    要发送的数据
 * 
 * @return     uint8_t    :    接收到的数据
 * 
******************************************************************************/
uint8_t spi_read_write_byte(uint8_t tx_data)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)  // 等待发送区空
    { }

    SPI_I2S_SendData(SPI1, tx_data);  // SPIx发送一个 byte 数据

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)  // 等待接收完一个 byte
    { }

    return SPI_I2S_ReceiveData(SPI1);  // 返回接收的数据
}


