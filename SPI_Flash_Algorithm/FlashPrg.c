/**************************************************************************//**
 * @file     FlashPrg.c
 * @brief    Flash Programming Functions adapted for New Device Flash
 * @version  V1.0.0
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "FlashOS.h"        // FlashOS Structures
#include "system_stm32f4xx.h"
#include "w25q.h"

/*
   Mandatory Flash Programming Functions (Called by FlashOS):
                int Init        (unsigned long adr,   // Initialize Flash
                                 unsigned long clk,
                                 unsigned long fnc);
                int UnInit      (unsigned long fnc);  // De-initialize Flash
                int EraseSector (unsigned long adr);  // Erase Sector Function
                int ProgramPage (unsigned long adr,   // Program Page Function
                                 unsigned long sz,
                                 unsigned char *buf);

   Optional  Flash Programming Functions (Called by FlashOS):
                int BlankCheck  (unsigned long adr,   // Blank Check
                                 unsigned long sz,
                                 unsigned char pat);
                int EraseChip   (void);               // Erase complete Device
      unsigned long Verify      (unsigned long adr,   // Verify Function
                                 unsigned long sz,
                                 unsigned char *buf);

       - BlanckCheck  is necessary if Flash space is not mapped into CPU memory space
       - Verify       is necessary if Flash space is not mapped into CPU memory space
       - if EraseChip is not provided than EraseSector for all sectors is called
*/


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int Init(unsigned long adr, unsigned long clk, unsigned long fnc)
{
    SystemInit();   /* 初始化系统和时钟  */

    w25qxx_init();  /* 初始化w25q128 */ 

    /* Add your Code */
    return (0);                                  // Finished without Errors
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit(unsigned long fnc)
{

    /* Add your Code */
    return (0);                                  // Finished without Errors
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip(void)
{
    w25qxx_erase_chip();

    /* Add your Code */
    return (0);                                  // Finished without Errors
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector(unsigned long adr)
{
    uint32_t sector = 0;
    adr -= SPI_FLASH_MEM_ADDR;

    sector = adr / 4096;
    w25qxx_erase_sector(sector);

    /* Add your Code */
    return (0);                                  // Finished without Errors
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
    adr -= SPI_FLASH_MEM_ADDR;
    w25qxx_write(buf, adr, sz);

    /* Add your Code */
    return (0);                                  // Finished without Errors
}

/*
*********************************************************************************************************
*   函 数 名: Verify
*   功能说明: 校验
*   形    参: adr 起始地址
*             sz  数据大小
*             buf 要校验的数据缓冲地址
*   返 回 值: -
*********************************************************************************************************
*/
unsigned char aux_buf[4096];
unsigned long Verify(unsigned long adr, unsigned long sz, unsigned char *buf)
{
    unsigned long remain = sz;  //剩余的字节数
    unsigned long current_add = 0;//当前的地址
    unsigned int index = 0;//用于buf的索引
    current_add = adr - 0xC0000000;

    while (remain >= 4096)
    {
        w25qxx_read(aux_buf, current_add, 4096);
        for (int i = 0; i < 4096; i++)
        {
            if (aux_buf[i] != buf[index + i])
                return adr + index + i;
        }
        current_add += 4096;
        remain -= 4096;
        index += 4096;
    }

    w25qxx_read(aux_buf, current_add, remain);
    for (int i = 0; i < remain; i++)
    {
        if (aux_buf[i] != buf[index + i])
            return adr + index + i;
    }
    return (adr + sz);                      // 校验成功
}

///*
// *  Blank Check Checks if Memory is Blank
// *    Parameter:      adr:  Block Start Address
// *                    sz:   Block Size (in bytes)
// *                    pat:  Block Pattern
// *    Return Value:   0 - OK,  1 - Failed (需要擦除）
// */

int BlankCheck(unsigned long adr, unsigned long sz, unsigned char pat)
{

    return 0;
}
