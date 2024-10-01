#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "norflash.h"
#include "main.h"

typedef struct
{
	uint32_t sector;
	uint32_t size;
} sector_desc_t;

// stm32f4 每个分区的大小描述
static const sector_desc_t sector_descs[] =
{
	{FLASH_Sector_0, 16 * 1024},
	{FLASH_Sector_1, 16 * 1024},
	{FLASH_Sector_2, 16 * 1024},
	{FLASH_Sector_3, 16 * 1024},
	{FLASH_Sector_4, 64 * 1024},
	{FLASH_Sector_5, 128 * 1024},
	{FLASH_Sector_6, 128 * 1024},
	{FLASH_Sector_7, 128 * 1024},
	{FLASH_Sector_8, 128 * 1024},
	{FLASH_Sector_9, 128 * 1024},
	{FLASH_Sector_10, 128 * 1024},
	{FLASH_Sector_11, 128 * 1024},
};

uint32_t bl_norflash_read_word(uint32_t faddr)
{
	return *(volatile uint32_t *)faddr;
}

/******************************************************************************
 * @brief      获取某个地址所在的flash扇区
 * 
 * @param[in]  addr     :   flash地址
 * 
 * @return     uint16_t :   0~11,即addr所在的扇区
 * 
******************************************************************************/
static uint16_t bl_norflash_get_flash_sector(uint32_t addr)
{
	uint32_t address = STM32_FLASH_BASE;
	for (uint16_t sector = 0; sector < sizeof(sector_descs) / sizeof(sector_desc_t); ++sector)
    {
		if (addr < address + sector_descs[sector].size) {
			return sector_descs[sector].sector;
		}
		address += sector_descs[sector].size;
	}

	printf("Flash sector not found!");
	return FLASH_Sector_11;
}

/******************************************************************************
 * @brief 	   从指定地址开始写入指定长度的数据
 * 
 * @param[in]  write_addr    :    起始地址(此地址必须为4的倍数!!)
 * @param[in]  data  	     :    要写入的数据
 * @param[in]  size    		 : 	  写入数据的大小
 * 
 * @return     none
 * 
 * @note       1. 该函数对OTP区域也有效!可以用来写OTP区(0X1FFF7800~0X1FFF7A0F)!
 * 			   2. 因为STM32F4的扇区太大了,没办法本地保存扇区数据,所以本函数
 *                写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
 *                写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
 *                没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写.
 *    
******************************************************************************/
void bl_norflash_write(uint32_t write_addr, uint32_t *data, uint32_t size)
{
	if (write_addr < STM32_FLASH_BASE || write_addr % 4) {  // 非法地址
		printf("Please check the WriteAddr!");
		return;
	}					 

	FLASH_Status status = FLASH_COMPLETE;
	uint32_t addr_begin = 0;
	uint32_t addr_end = 0;
	
	FLASH_Unlock();				 // 解锁
	FLASH_DataCacheCmd(DISABLE); // FLASH擦除期间,必须禁止数据缓存
	/*****************************************************************************/

	addr_begin = write_addr;					  // 写入的起始地址
	addr_end = write_addr + size * 4; 	  // 写入的结束地址
	if (addr_begin < 0X1FFF0000)				  // 只有主存储区,才需要执行擦除操作!!
	{
		while (addr_begin < addr_end) // 扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if (bl_norflash_read_word(addr_begin) != 0XFFFFFFFF) // 有非0XFFFFFFFF的地方,要擦除这个扇区
			{
				status = FLASH_EraseSector(bl_norflash_get_flash_sector(addr_begin), VoltageRange_3); // VCC=2.7~3.6V之间!!
				if (status != FLASH_COMPLETE) {
					printf("Flash erase error!");
					break; // 发生错误了
				}
			}
			else
				addr_begin += 4;
		}
	}

	if (status == FLASH_COMPLETE)
	{
		while (write_addr < addr_end) // 写数据
		{
			if (FLASH_ProgramWord(write_addr, *data) != FLASH_COMPLETE) // 写入数据
			{
				printf("Flash write error!");
				break; // 写入异常
			}
			write_addr += 4;
			data++;
		}
	}
	/*****************************************************************************/
	FLASH_DataCacheCmd(ENABLE); // FLASH擦除结束,开启数据缓存
	FLASH_Lock();				// 上锁
}

/******************************************************************************
 * @brief 	   从指定地址开始读出指定长度的数据
 * 
 * @param[in]  read_addr    :    起始地址  
 * @param[in]  data  	    :    存放读取数据
 * @param[in]  size    		:    要读取数据的大小
 * 
 * @return     none
 * 
******************************************************************************/
void bl_norflash_read(uint32_t read_addr, uint32_t *data, uint32_t size)
{
	if (read_addr < STM32_FLASH_BASE || data == NULL || size == 0)
	{
		printf("Please check the ReadAddr or the size!");
		return;
	}

	uint32_t i;
	for (i = 0; i < size; i++)
	{
		data[i] = bl_norflash_read_word(read_addr); // 读取4个字节.
		read_addr += 4;							  // 偏移4个字节.
	}
}
