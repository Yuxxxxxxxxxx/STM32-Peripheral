#ifndef __IAP_H
#define __IAP_H


#include <stdint.h>

typedef  int (*entry_t)(void);				//定义一个函数类型的参数.   
#define FLASH_APP1_ADDR		0x08010000

void bl_iap_load_app(uint32_t appxaddr);			//跳转到APP程序执行
void bl_iap_write_app_bin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);
void MSR_MSP(uint32_t addr);

#endif
