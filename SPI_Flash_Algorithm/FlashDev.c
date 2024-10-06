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
   FLASH_DRV_VERS,                  /* �����㷨���� MDK �ƶ����� */
   "Yux_STM32F407VE_SPI_W25Q128",   /* �㷨���� */ 
   EXTSPI,                          /* �豸���ͣ�����չ SPI-Flash */
   SPI_FLASH_MEM_ADDR,              /* Flash ��ʼ��ַ */
   0x01000000,                      /* Flash ��С��16MB */
   4096,                            /* ���ҳ��С */
   0,                               /* ����������Ϊ 0 */
   0xFF,                            /* ���������ֵ */
   3000,                            /* ҳ��̵ȴ�ʱ�� */
   3000,                            /* ���������ȴ�ʱ�� */
   
   0x001000, 0x000000,              /* ������С��������ַ */
   SECTOR_END
};
