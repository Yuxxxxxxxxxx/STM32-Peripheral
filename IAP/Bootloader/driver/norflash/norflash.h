#ifndef __BL_NORFLASH_H
#define __BL_NORFLASH_H


#include <stdint.h>


//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
 

uint32_t bl_norflash_read_word(uint32_t addr);		  	//读出字  
void bl_norflash_write(uint32_t write_addr,uint32_t *data,uint32_t size);		//从指定地址开始写入指定长度的数据
void bl_norflash_read(uint32_t read_addr,uint32_t *data,uint32_t size);   		//从指定地址开始读出指定长度的数据

#endif /* __BL_NORFLASH_H */
