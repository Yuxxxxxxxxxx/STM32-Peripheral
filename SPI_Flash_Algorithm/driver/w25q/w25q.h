/******************************************************************************
 * @file w25q.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-09-20
 *
 * @copyright Copyright (c) 2024
 *
 ******************************************************************************/
#ifndef __W25Q_H
#define __W25Q_H

#include <stdint.h>

// 指令表
#define W25X_WriteEnable        0x06
#define W25X_WriteDisable       0x04
#define W25X_ReadStatusReg      0x05
#define W25X_WriteStatusReg     0x01
#define W25X_ReadData           0x03
#define W25X_FastReadData       0x0B
#define W25X_FastReadDual       0x3B
#define W25X_PageProgram        0x02
#define W25X_BlockErase         0xD8
#define W25X_SectorErase        0x20
#define W25X_ChipErase          0xC7
#define W25X_PowerDown          0xB9
#define W25X_ReleasePowerDown   0xAB
#define W25X_DeviceID           0xAB
#define W25X_ManufactDeviceID   0x90
#define W25X_JedecDeviceID      0x9F

/*typedef struct w25qxx_device_s
{
    void (*init)(void);
    void (*wr)(uint8_t *pbuffer, uint32_t read_addr, uint16_t num_byte_to_read);
    void (*rd)(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write);
    uint16_t type;
} w25qxx_device_t;

extern w25qxx_device_t w25q32_dev; */

void w25qxx_init(void);
uint16_t w25qxx_readid(void);
uint8_t w25qxx_readsr(void);                                                          // 读取状态寄存器
void w25qxx_write_sr(uint8_t sr);                                                     // 写状态寄存器
void w25qxx_write_enable(void);                                                       // 写使能
void w25qxx_write_disable(void);                                                      // 写保护
void w25qxx_read(uint8_t *pbuffer, uint32_t read_addr, uint16_t num_byte_to_read);    // 读取flash
void w25qxx_write(uint8_t *pbuffer, uint32_t write_addr, uint16_t num_byte_to_write); // 写入flash
void w25qxx_erase_chip(void);                                                         // 整片擦除
void w25qxx_erase_sector(uint32_t dst_addr);                                          // 扇区擦除
void w25qxx_wait_busy(void);                                                          // 等待空闲
void w25qxx_powerdown(void);                                                          // 进入掉电模式
void w25qxx_wakeup(void);                                                             // 唤醒  

#endif /* __W25Q_H */
