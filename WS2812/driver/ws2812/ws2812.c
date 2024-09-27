/******************************************************************************
 * @file ws2812.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-27
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "ws2812.h"
#include "main.h"


// Some Static Colors
const RGBColor_TypeDef RED     = { 255, 0, 0 };
const RGBColor_TypeDef GREEN   = { 0, 255, 0 };
const RGBColor_TypeDef BLUE    = { 0, 0, 255 };
const RGBColor_TypeDef SKY     = { 0, 255, 255 };
const RGBColor_TypeDef MAGENTA = { 255, 0, 255 };
const RGBColor_TypeDef YELLOW  = { 255, 255, 0 };
const RGBColor_TypeDef ORANGE  = { 127, 106, 0 };
const RGBColor_TypeDef BLACK   = { 0, 0, 0 };
const RGBColor_TypeDef WHITE   = { 255, 255, 255 };
const RGBColor_TypeDef PURPLE  = { 65, 105, 225 };

static uint8_t pixelBuffer[WS2812_NUM][24]; // 灯珠

/******************************************************************************
 * @brief  初始化 WS2812 GPIO 设置
 * 
 * @return none
 * 
******************************************************************************/
static void ws2812_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin    =  GPIO_Pin_15;           // PB15复用功能输出
    GPIO_InitStructure.GPIO_Mode   =  GPIO_Mode_AF;          // 复用功能
    GPIO_InitStructure.GPIO_OType  =  GPIO_OType_PP;         // 推挽输出
    GPIO_InitStructure.GPIO_Speed  =  GPIO_Speed_50MHz;      // 100MHz
    GPIO_InitStructure.GPIO_PuPd   =  GPIO_PuPd_UP;          // 上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);                   // 初始化

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2); // PB15复用为 SPI2
}

/******************************************************************************
 * @brief  初始化 WS2812 SPI 设置
 * 
 * @return none
 * 
******************************************************************************/
static void ws2812_spi_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;  // 设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                  // 设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                  // 设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;                     // 串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                   // 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                     // NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;          // 42M/8=5.25M
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                 // 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                                // CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);                                         // 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI2, ENABLE); // 使能SPI外设
}

/******************************************************************************
 * @brief  初始化 WS2812 DMA 设置
 * 
 * @return none
 * 
******************************************************************************/
static void ws2812_dma_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Stream4);

    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE)
    {
    } // 等待DMA可配置

    DMA_InitStructure.DMA_Channel            = DMA_Channel_0;               // 通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;              // DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr    = (u32)pixelBuffer;            // DMA 存储器0地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;  // 存储器到外设模式
    DMA_InitStructure.DMA_BufferSize         = WS2812_NUM * 24;             // 数据传输量
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设非增量模式
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;     // 存储器数据长度:8位
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;             // 使用普通模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;         // 中等优先级
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;        // 禁用FIFO模式
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;      // 传输FIFO阈值:满
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;      // 存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;  // 外设突发单次传输
    DMA_Init(DMA1_Stream4, &DMA_InitStructure);                             // 初始化DMA Stream

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);   // 使能SPI2的DMA发送
    DMA_Cmd(DMA1_Stream4, DISABLE);                    // 关闭DMA传输
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE)  // 确保DMA可以被设置
    {
    }                                                 

    DMA_SetCurrDataCounter(DMA1_Stream4, WS2812_NUM * 24); // 数据传输量
    DMA_Cmd(DMA1_Stream4, ENABLE);
}

/******************************************************************************
 * @brief  初始化 WS2812 驱动
 * 
 * @return none
 * 
******************************************************************************/
void ws2812_init(void)
{
    ws2812_gpio_init();
    ws2812_spi_init();
    ws2812_dma_init();

    bl_delay_ms(1);
    RGB_BLACK(WS2812_NUM); // RGB RESET
    bl_delay_ms(1);
}

/******************************************************************************
 * @brief      设置颜色
 * 
 * @param[in]  led_id    :    第几颗灯珠  
 * @param[in]  color     :    要设置的颜色
 * 
 * @return     none
 * 
******************************************************************************/
static void rgb_set_color(uint16_t led_id, RGBColor_TypeDef color)
{
    if (led_id > WS2812_NUM)
    {
        printf("Error: Out of Range!\r\n");
        return;
    }

    uint16_t i;

    {
        for (i = 0; i <=7; ++i)
        {
            pixelBuffer[led_id][i] = ( (color.r & ( 1 << ( 7 - i ) )) ? (CODE1) : (CODE0) );
        }

        for (i = 8; i <=15; ++i)
        {
            pixelBuffer[led_id][i] = ( (color.g & ( 1 << ( 15 - i ) )) ? (CODE1) : (CODE0) );
        }

        for (i = 16; i <=23; ++i)
        {
            pixelBuffer[led_id][i] = ( (color.b & ( 1 << ( 23 - i ) )) ? (CODE1) : (CODE0) );
        }
    }
}

/******************************************************************************
 * @brief  通过 DMA 发送颜色数组
 * 
 * @return none
 * 
******************************************************************************/
static void rgb_send_array(void)
{
    // 等待DMA1_Steam5传输完成
    if (DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4) != RESET)
    {
        DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);      // 清除DMA1_Steam5传输完成标志
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);  // 使能SPI3的DMA发送
        DMA_Cmd(DMA1_Stream4, DISABLE);                   // 关闭DMA传输

        // 确保DMA可以被设置
        while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE)
        { }

        DMA_SetCurrDataCounter(DMA1_Stream4, WS2812_NUM * 24); // 数据传输量
        DMA_Cmd(DMA1_Stream4, ENABLE);                         // 开启DMA传输
    }
}

/******************************************************************************
 * @brief      设置灯珠颜色为红色
 * 
 * @param[in]  Pixel_LEN    :    灯珠数量 
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_red(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; ++i)
    {
        rgb_set_color(i, RED);
    }

    rgb_send_array();
}

/******************************************************************************
 * @brief      设置灯珠颜色为绿色
 * 
 * @param[in]  Pixel_LEN    :    灯珠数量 
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_green(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, GREEN);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      设置灯珠颜色为蓝色
 * 
 * @param[in]  Pixel_LEN    :      灯珠数量
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_blue(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, BLUE);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      设置灯珠颜色为蓝色
 * 
 * @param[in]  Pixel_LEN    :    灯珠数量
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_yellow(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, PURPLE);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      设置灯珠颜色为紫色
 * 
 * @param[in]  Pixel_LEN    :      灯珠数量
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_magenta(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, MAGENTA);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      设置灯珠颜色为黑色
 * 
 * @param[in]  Pixel_LEN    :    灯珠数量
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_black(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, BLACK);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      设置灯珠颜色为白色
 * 
 * @param[in]  Pixel_LEN    :      灯珠数量
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_white(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, WHITE);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      设置灯珠颜色为天蓝色
 * 
 * @param[in]  Pixel_LEN    :      灯珠数量
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_sky(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, SKY);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      设置灯珠颜色为橙色
 * 
 * @param[in]  Pixel_LEN    :    灯珠数量
 * 
 * @return     none
 * 
******************************************************************************/
void rgb_orange(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, ORANGE);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      设置灯珠为粉色
 * 
 * @param[in]  Pixel_LEN    :     灯珠数量
 * 
 * @return     none
 *   
******************************************************************************/
void rgb_purple(uint16_t Pixel_LEN)
{
    uint16_t i;

    for (i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, PURPLE);
    }

    rgb_SendArray();
}

/******************************************************************************
 * @brief      将颜色转换为GRB
 * 
 * @param[in]  wheel_pos    :     颜色数值
 * 
 * @return     RGBColor_TypeDef 
 * 
******************************************************************************/
RGBColor_TypeDef colourful_wheel(uint8_t wheel_pos)
{
    RGBColor_TypeDef color;
    wheel_pos = 255 - wheel_pos;

    if (wheel_pos < 85)
    {
        color.r = 255 - wheel_pos * 3;
        color.g = 0;
        color.b = wheel_pos * 3;
        return color;
    }
    if (wheel_pos < 170)
    {
        wheel_pos -= 85;
        color.r = 0;
        color.g = wheel_pos * 3;
        color.b = 255 - wheel_pos * 3;
        return color;
    }

    wheel_pos -= 170;
    color.r = wheel_pos * 3;
    color.g = 255 - wheel_pos * 3;
    color.b = 0;

    return color;
}

/******************************************************************************
 * @brief      呼吸灯功能
 * 
 * @param[in]  Pixel_LEN    :    灯珠数量
 * 
 * @return     none
 * 
******************************************************************************/
void rainbow_cycle(uint16_t Pixel_LEN)
{
    uint16_t i, j = 0;

    for (j = 0; j < 1023; j++)
    { // 1 cycles of all colors on wheel
        for (i = 0; i < Pixel_LEN; i++)
        {
            rgb_set_color(i, colourful_wheel( ( (i * 256 / Pixel_LEN) + j ) & 255));
        }
        rgb_send_array();
        bl_delay_ms(20);
    }
}

