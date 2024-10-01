#include <stdio.h>
#include "iap.h"
#include "norflash.h"
#include "stm32f4xx.h"
#include "console_uart.h"


uint32_t iapbuf[512];

/******************************************************************************
 * @brief      向Flash写入应用程序  
 * 
 * @param[in]  addr    :     
 * @param[in]  buf     :  
 * @param[in]  size    :  
 * 
 * @return     none
 * 
******************************************************************************/
void bl_iap_write_app_bin(uint32_t addr, u8 *buf, uint32_t size)
{
    uint32_t t;
    u16 i = 0;
    uint32_t fwaddr = addr; // 当前写入的地址
    
    for (t = 0; t < size; t += 4)
    {
        iapbuf[i++] = (uint32_t)(buf[t + 3] << 24) |
                      (uint32_t)(buf[t +2] << 16)  | 
                      (uint32_t)(buf[t + 1] << 8)  | 
                      (uint32_t)(buf[t]);
        
        if (i == 512)
        {
            i = 0;
            bl_norflash_write(fwaddr, iapbuf, 512);
            fwaddr += 2048; // 偏移2048  512*4=2048
        }
    }
    if (i)
        bl_norflash_write(fwaddr, iapbuf, i); // 将最后的一些内容字节写进去.
}

extern void board_lowlevel_deinit(void);

/******************************************************************************
 * @brief      跳转到应用程序段
 * 
 * @param[in]  addr    :    用户代码起始地址
 * 
 * @return     none
 * 
******************************************************************************/
void bl_iap_load_app(uint32_t addr)
{
    if ( ( ( *(volatile uint32_t *)addr ) & 0x2FFE0000 ) != 0x20000000 ) // 检查栈顶地址是否合法.
    {
        printf("Stack pointer is not valid!\r\n");
        return;
    }

    uint32_t _sp = *(volatile uint32_t*)(addr + 0);
    uint32_t _pc = *(volatile uint32_t*)(addr + 4);

    entry_t app_entry = (entry_t)_pc;           // 用户代码区第二个字为程序开始地址(复位地址)
    MSR_MSP(_sp);                               // 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
    
    board_lowlevel_deinit();                  
    app_entry();                                // 跳转到APP.
}

__asm void MSR_MSP(uint32_t addr)
{
    MSR MSP, r0;
    BX r14;
}
