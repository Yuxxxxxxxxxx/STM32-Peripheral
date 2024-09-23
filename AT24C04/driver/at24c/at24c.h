/******************************************************************************
 * @file at24c.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-23
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef __AT24C_H
#define	__AT24C_H
 
#include "stm32f4xx.h"
 
/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0xA0
 * 1 0 1 0 0  0  0  1 = 0xA1 
 */
 
/* AT24C01/02每页有8个字节 
 * AT24C04/08A/16A每页有16个字节 、
 */
	
#define AT24C04
 
 
 
#ifdef AT24C01
	#define AT24CX_MODEL_NAME		"AT24C01"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		8			/* 页面大小(字节) */
	#define AT24CX_SIZE				128			/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		1			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			0			/* 地址字节的高8bit不在首字节 */
#endif
 
 
 
#ifdef AT24C02
	#define AT24CX_MODEL_NAME		"AT24C02"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		8			/* 页面大小(字节) */
	#define AT24CX_SIZE				256			/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		1			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			0			/* 地址字节的高8bit不在首字节 */
#endif
 
 
 
#ifdef AT24C04
	#define AT24CX_MODEL_NAME		"AT24C04"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		8			/* 页面大小(字节) */
	#define AT24CX_SIZE				512			/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		1			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			1			/* 地址字节的高8bit在首字节 */
#endif
 
 
 
#ifdef AT24C08
	#define AT24CX_MODEL_NAME		"AT24C08"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		16			/* 页面大小(字节) */
	#define AT24CX_SIZE				(16*64)		/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		2			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			1			/* 地址字节的高8bit在首字节 */
#endif
 
 
 
 
#ifdef AT24C16
	#define AT24CX_MODEL_NAME		"AT24C16"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		16			/* 页面大小(字节) */
	#define AT24CX_SIZE				(128*16)	/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		2			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			1			/* 地址字节的高8bit在首字节 */
#endif
 
 
 
#ifdef AT24C32
	#define AT24CX_MODEL_NAME		"AT24C32"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		32			/* 页面大小(字节) */
	#define AT24CX_SIZE				(128*32)	/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		2			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			1			/* 地址字节的高8bit在首字节 */
#endif
 
 
#ifdef AT24C64
	#define AT24CX_MODEL_NAME		"AT24C64"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		32			/* 页面大小(字节) */
	#define AT24CX_SIZE				(256*32)	/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		2			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			1			/* 地址字节的高8bit在首字节 */
#endif
 
 
 
#ifdef AT24C128
	#define AT24CX_MODEL_NAME		"AT24C128"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		64			/* 页面大小(字节) */
	#define AT24CX_SIZE				(256*64)	/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		2			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			0			/* 地址字节的高8bit不在首字节 */
#endif
 
 
 
#ifdef AT24C256
	#define AT24CX_MODEL_NAME		"AT24C256"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		64			/* 页面大小(字节) */
	#define AT24CX_SIZE				(512*64)	/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		2			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			0			/* 地址字节的高8bit不在首字节 */
#endif
 
 
 
#ifdef AT24C512
	#define AT24CX_MODEL_NAME		"AT24C512"
	#define AT24CX_DEV_ADDR			0xA0		/* 设备地址 */
	#define AT24CX_PAGE_SIZE		128			/* 页面大小(字节) */
	#define AT24CX_SIZE				(512*128)	/* 总容量(字节) */
	#define AT24CX_ADDR_BYTES		2			/* 地址字节个数 */
	#define AT24CX_ADDR_A8			0			/* 地址字节的高8bit不在首字节 */
#endif
 

uint8_t at24cx_checkok(void);
uint8_t at24cx_readbytes(uint8_t *readbuf, uint16_t address, uint16_t size);
uint8_t at24cx_writebytes(uint8_t *writebuf, uint16_t address, uint16_t size);
 
#endif /* __AT24CH */

 