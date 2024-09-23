/******************************************************************************
 * @file ctl_i2c.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-23
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H
 
#include "stm32f4xx.h"
 
 
#define I2C_WR	0		// 写控制bit
#define I2C_RD	1		// 读控制bit
 
#define RCC_AT24CXX_I2C_PORT 			RCC_AHB1Periph_GPIOB		// GPIO端口时钟
#define GPIO_AT24CXX_I2C_PORT			GPIOB						// GPIO端口
#define GPIO_AT24CXX_I2C_SCL_Pin		GPIO_Pin_8					// 连接到SCL时钟线的GPIO
#define GPIO_AT24CXX_I2C_SDA_Pin		GPIO_Pin_9					// 连接到SDA数据线的GPIO
 
#define I2C_SCL_H()  	 GPIO_SetBits(GPIO_AT24CXX_I2C_PORT, GPIO_AT24CXX_I2C_SCL_Pin) 		      // SCL = 1
#define I2C_SCL_L()  	 GPIO_ResetBits(GPIO_AT24CXX_I2C_PORT, GPIO_AT24CXX_I2C_SCL_Pin) 		  // SCL = 0
#define I2C_SDA_H()  	 GPIO_SetBits(GPIO_AT24CXX_I2C_PORT, GPIO_AT24CXX_I2C_SDA_Pin) 		      // SDA = 1
#define I2C_SDA_L()  	 GPIO_ResetBits(GPIO_AT24CXX_I2C_PORT, GPIO_AT24CXX_I2C_SDA_Pin) 		  // SDA = 0
#define I2C_SDA_RD()     GPIO_ReadInputDataBit(GPIO_AT24CXX_I2C_PORT, GPIO_AT24CXX_I2C_SDA_Pin)   // 读SDA口线状态
#define I2C_SCL_RD()     GPIO_ReadInputDataBit(GPIO_AT24CXX_I2C_PORT, GPIO_AT24CXX_I2C_SCL_Pin)   // 读SCL口线状态


void ctl_at24cxx_i2c_init(void);
void ctl_i2c_start(void);
void ctl_i2c_stop(void);
void ctl_i2c_sendbyte(uint8_t byte);
void ctl_i2c_ack(void);
void ctl_i2c_nack(void);
uint8_t ctl_i2c_waitack(void);
uint8_t ctl_i2c_readbyte(void);
uint8_t ctl_i2c_checkdevice(uint8_t address);
 
#endif
