/******************************************************************************
 * @file user_lib.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  This file contains the function prototypes of the user library.
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef __BSP_USER_LIB_H
#define __BSP_USER_LIB_H

#include <stdint.h>
#include "stm32f4xx.h"


/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 字符串操作 */
int str_len(char *_str);
void str_cpy(char *_tar, char *_src);
int str_cmp(char * s1, char * s2);
void mem_set(char *_tar, char _data, int _len);

/* 数字-字符串转换 */
void int_to_str(int _iNumber, char *_pBuf, unsigned char _len);
int str_to_int(char *_pStr);

/* 字节序转换-uint16_t */
uint16_t be_buf_to_uint16(uint8_t *_pBuf);
uint16_t le_buf_to_uint16(uint8_t *_pBuf);

/* 字节序转换-uint32_t */
uint32_t be_buf_to_uint32(uint8_t *_pBuf);
uint32_t le_buf_to_uint32(uint8_t *_pBuf);

/* CRC16-Modbus */
uint16_t crc16_modbus(uint8_t *_pBuf, uint16_t _usLen) ;

/* 两点距离计算 */
int32_t  cacul_two_point(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);

/* 数字格式转换 */
char bcd_to_char(uint8_t _bcd);
void hex_to_ascll(uint8_t * _pHex, char *_pAscii, uint16_t _BinBytes);
uint32_t ascii_to_uint32(char *pAscii);

/* CPU 空闲任务 */
void bsp_Idle(void);

#endif

