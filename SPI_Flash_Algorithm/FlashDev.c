/**************************************************************************//**
 * @file     FlashDev.c
 * @brief    Flash Device Description for New Device Flash
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


struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,                  /* 驱动算法，由 MDK 制定，勿动 */
   "Yux_STM32F407VE_SPI_W25Q128",   /* 算法名称 */ 
   EXTSPI,                          /* 设备类型，外扩展 SPI-Flash */
   SPI_FLASH_MEM_ADDR,              /* Flash 起始地址 */
   0x01000000,                      /* Flash 大小，16MB */
   4096,                            /* 编程页大小 */
   0,                               /* 保留，必须为 0 */
   0xFF,                            /* 擦除后的数值 */
   3000,                            /* 页编程等待时间 */
   3000,                            /* 扇区擦除等待时间 */
   
   0x001000, 0x000000,              /* 扇区大小，扇区地址 */
   SECTOR_END
};
