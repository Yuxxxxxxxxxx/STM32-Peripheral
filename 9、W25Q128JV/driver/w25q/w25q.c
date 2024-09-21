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
#include <stdio.h>
#include "stm32f4xx.h"
#include "w25q.h"
#include "ctl_spi.h"
#include "main.h"
#include "console_uart.h"

// 方便移植，实现如下几个函数
#define w25qxx_cs_high()   GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define w25qxx_cs_low()    GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define w25qxx_r_w_byte(n) spi_read_write_byte(n)
#define w25qxx_spi_init()  spi_init()
#define w25qxx_delay_us(n) bl_delay_us(n)

w25qxx_device_t w25q32_dev = {
    .init = w25qxx_init,
    .wr = w25qxx_write,
    .rd = w25qxx_read,
    .type = 0x00,
};

void w25qxx_init(void)
{
    w25qxx_spi_init();
    w25q32_dev.type = w25qxx_readid();
}

// 读取芯片ID W25X的 ID:0XEF17
uint16_t w25qxx_readid(void)
{
    uint16_t temp = 0;
    //uint16_t temp1 = 0, temp2 = 0, temp3 = 0;
    w25qxx_cs_low();

    w25qxx_r_w_byte(W25X_ManufactDeviceID); // 发送读取ID命令

    w25qxx_r_w_byte(0x00);
    w25qxx_r_w_byte(0x00);
    w25qxx_r_w_byte(0x00);

    temp |= (w25qxx_r_w_byte(0xFF) << 8);
    temp |= w25qxx_r_w_byte(0xFF);

    w25qxx_cs_high();

    return temp;
}

/******************************************************************************
 * @brief      读取SPI_FLASH的状态寄存器
 * 
 * @return     uint8_t    :    状态寄存器的值
 * 
 * @note       BIT7  6   5   4   3   2   1   0
 *             SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *             SPR: 默认0,状态寄存器保护位,配合WP使用
 *             TB,BP2,BP1,BP0: FLASH区域写保护设置
 *             WEL:写使能锁定
 *             BUSY:忙标记位(1,忙;0,空闲)
 *             默认: 0x00
 * 
******************************************************************************/
uint8_t w25qxx_readsr(void)
{
    uint8_t byte = 0;
    w25qxx_cs_low();                     // 使能器件
    w25qxx_r_w_byte(W25X_ReadStatusReg); // 发送读取状态寄存器命令
    byte = w25qxx_r_w_byte(0xff);        // 读取一个字节
    w25qxx_cs_high();                    // 取消片选
    return byte;
}

/******************************************************************************
 * @brief      写SPI_FLASH状态寄存器
 * 
 * @param[in]  sr    :    要写入的状态寄存器的值
 * 
 * @return     none
 * 
 * @note       只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写
 * 
******************************************************************************/
void w25qxx_write_sr(uint8_t sr)
{
    w25qxx_cs_low();                      // 使能器件
    w25qxx_r_w_byte(W25X_WriteStatusReg); // 发送写取状态寄存器命令
    w25qxx_r_w_byte(sr);                  // 写入一个字节
    w25qxx_cs_high();                     // 取消片选
}

/******************************************************************************
 * @brief  SPI_FLASH写使能(将WEL置位)
 * 
 * @return none
 * 
******************************************************************************/
void w25qxx_write_enable(void)
{
    w25qxx_cs_low();                   // 使能器件
    w25qxx_r_w_byte(W25X_WriteEnable); // 发送写使能
    w25qxx_cs_high();                  // 取消片选
}

/******************************************************************************
 * @brief  SPI_FLASH写禁止(将WEL清零)
 * 
 * @return none
 * 
******************************************************************************/
void w25qxx_write_disable(void)
{
    w25qxx_cs_low();                    // 使能器件
    w25qxx_r_w_byte(W25X_WriteDisable); // 发送写禁止指令
    w25qxx_cs_high();                   // 取消片选
}

/******************************************************************************
 * @brief      读取SPI FLASH
 * 
 * @param[in]  pbuffer            :    数据存储区
 * @param[in]  read_addr          :    开始读取的地址(24bit)
 * @param[in]  num_byte_to_read   :    要读取的字节数(最大65535)
 * 
 * @return     none
 * 
******************************************************************************/
void w25qxx_read(uint8_t *pbuffer, uint32_t read_addr, uint16_t num_byte_to_read)
{
    uint16_t i;

    w25qxx_cs_low();
    w25qxx_r_w_byte(W25X_ReadData);                // 发送读取命令
    w25qxx_r_w_byte((uint8_t)((read_addr) >> 16)); // 发送24bit地址
    w25qxx_r_w_byte((uint8_t)((read_addr) >> 8));
    w25qxx_r_w_byte((uint8_t)read_addr);
    for (i = 0; i < num_byte_to_read; i++)
    {
        pbuffer[i] = w25qxx_r_w_byte(0XFF); // 循环读数
    }
    w25qxx_cs_high();
}


/******************************************************************************
 * @brief      在指定地址开始写入最大256字节的数据
 * 
 * @param[in]  pbuffer             :    数据存储区
 * @param[in]  write_addr          :    开始写入的地址(24bit)
 * @param[in]  num_byte_to_write   :    要写入的字节数(最大256),该数不应该超过该页的剩余字节数
 * 
 * @return     none
 * 
******************************************************************************/
static void w25qxx_write_page(uint8_t* pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    uint16_t i;

    w25qxx_write_enable();                          //SET WEL 
    w25qxx_cs_low();  
    w25qxx_r_w_byte(W25X_PageProgram);              //发送写页命令   
    w25qxx_r_w_byte((uint8_t)((write_addr) >> 16)); //发送24bit地址    
    w25qxx_r_w_byte((uint8_t)((write_addr) >> 8));   
    w25qxx_r_w_byte((uint8_t)write_addr);
    for(i = 0; i < num_byte_to_write; i++)
        w25qxx_r_w_byte(pbuffer[i]);                //循环写数  
    w25qxx_cs_high();

    w25qxx_wait_busy();                             //等待写入结束
}

/******************************************************************************
 * @brief      在指定地址开始写入指定长度的数据,不检查数据是否为0XFF(具有自动换页功能)
 * 
 * @param[in]  pbuffer              :    数据存储区
 * @param[in]  write_addr           :    开始写入的地址(24bit)
 * @param[in]  num_byte_to_write    :    要写入的字节数(最大65535)
 * 
 * @return     none
 * 
 * @note       必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败
 * 
******************************************************************************/
static void w25qxx_write_nocheck(uint8_t* pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)   
{                    
    uint16_t pageremain;

    pageremain = 256 - write_addr % 256;                            //单页剩余的字节数              
    if(num_byte_to_write <= pageremain)
        pageremain = num_byte_to_write;                             //不大于256个字节
    while(1)
    {      
        w25qxx_write_page(pbuffer, write_addr, pageremain);
        if(num_byte_to_write == pageremain)                         //写入结束了
            break;                      
        else                                            //num_byte_to_write>pageremain
        {
            pbuffer += pageremain;
            write_addr += pageremain;   

            num_byte_to_write -= pageremain;            //减去已经写入了的字节数
            if(num_byte_to_write > 256)
                pageremain = 256;                       //一次可以写入256个字节
            else 
                pageremain = num_byte_to_write;         //不够256个字节了
        }
    };      
}

uint8_t W25QXX_BUFFER[4096];

/******************************************************************************
 * @brief      在指定地址开始写入指定长度的数据
 * 
 * @param[in]  pbuffer              :    数据存储区
 * @param[in]  write_addr           :    开始写入的地址(24bit)
 * @param[in]  num_byte_to_write    :    要写入的字节数(最大65535)
 * 
 * @return     none
 * 
 * @note       该函数带擦除操作
 * 
******************************************************************************/
void w25qxx_write(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;

    secpos = write_addr / 4096; // 扇区地址 0~511 for w25x16
    secoff = write_addr % 4096; // 在扇区内的偏移
    secremain = 4096 - secoff;  // 扇区剩余空间大小

    if (num_byte_to_write <= secremain)
        secremain = num_byte_to_write; // 不大于4096个字节
    while (1)
    {
        w25qxx_read(W25QXX_BUFFER, secpos * 4096, 4096); // 读出整个扇区的内容
        for (i = 0; i < secremain; i++)                  // 校验数据
        {
            if (W25QXX_BUFFER[secoff + i] != 0XFF)
                break; // 需要擦除
        }

        if (i < secremain) // 需要擦除
        {
            w25qxx_erase_sector(secpos);    // 擦除这个扇区
            for (i = 0; i < secremain; i++) // 复制
            {
                W25QXX_BUFFER[i + secoff] = pbuffer[i];
            }
            w25qxx_write_nocheck(W25QXX_BUFFER, secpos * 4096, 4096); // 写入整个扇区
        }
        else
            w25qxx_write_nocheck(pbuffer, write_addr, secremain); // 写已经擦除了的,直接写入扇区剩余区间.

        if (num_byte_to_write == secremain)
            break; // 写入结束了
        else       // 写入未结束
        {
            secpos++;   // 扇区地址增1
            secoff = 0; // 偏移位置为0

            pbuffer += secremain;           // 指针偏移
            write_addr += secremain;        // 写地址偏移
            num_byte_to_write -= secremain; // 字节数递减
            if (num_byte_to_write > 4096)
                secremain = 4096; // 下一个扇区还是写不完
            else
                secremain = num_byte_to_write; // 下一个扇区可以写完了
        }
    };
}

/******************************************************************************
 * @brief  擦除整个芯片
 * 
 * @return none
 * 
 * @note   整片擦除时间非常长！！
 * 
******************************************************************************/
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable(); // SET WEL
    w25qxx_wait_busy();
    w25qxx_cs_low();                 // 使能器件
    w25qxx_r_w_byte(W25X_ChipErase); // 发送片擦除命令
    w25qxx_cs_high();                // 取消片选
    w25qxx_wait_busy();              // 等待芯片擦除结束
}

/******************************************************************************
 * @brief      擦除一个扇区
 * 
 * @param[in]  dst_addr    :    扇区地址 0~511 for w25x16
 * 
 * @return     none
 * 
 * @note       擦除一个山区的最少时间:150ms
 * 
******************************************************************************/
void w25qxx_erase_sector(uint32_t dst_addr)
{
    dst_addr *= 4096;
    w25qxx_write_enable(); // SET WEL
    w25qxx_wait_busy();
    w25qxx_cs_low();                              // 使能器件
    w25qxx_r_w_byte(W25X_SectorErase);            // 发送扇区擦除指令
    w25qxx_r_w_byte((uint8_t)((dst_addr) >> 16)); // 发送24bit地址
    w25qxx_r_w_byte((uint8_t)((dst_addr) >> 8));
    w25qxx_r_w_byte((uint8_t)dst_addr);
    w25qxx_cs_high();   // 取消片选
    w25qxx_wait_busy(); // 等待擦除完成
}

/******************************************************************************
 * @brief   等待W25QXX芯片忙标志位清空
 * 
 * @return  none
 * 
******************************************************************************/
void w25qxx_wait_busy(void)
{
    while ((w25qxx_readsr() & 0x01) == 0x01)
        ; // 等待BUSY位清空
}

/******************************************************************************
 * @brief  进入掉电模式
 * 
 * @return none
 * 
******************************************************************************/
void w25qxx_powerdown(void)
{
    w25qxx_cs_low();                 // 使能器件
    w25qxx_r_w_byte(W25X_PowerDown); // 发送掉电命令
    w25qxx_cs_high();                // 取消片选
    w25qxx_delay_us(3);              // 等待TPD
}

/******************************************************************************
 * @brief  唤醒
 * 
 * @return none
 * 
******************************************************************************/
void w25qxx_wakeup(void)
{
    w25qxx_cs_low();                        // 使能器件
    w25qxx_r_w_byte(W25X_ReleasePowerDown); // send W25X_PowerDown command 0xAB
    w25qxx_cs_high();                       // 取消片选
    w25qxx_delay_us(3);                     // 等待TRES1
}
