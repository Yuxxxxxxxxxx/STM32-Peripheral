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
 

typedef uint8_t (*at24cx_err_callback_t)(uint8_t err_code);

typedef enum 
{
	I2C_ERR_OK,             /*!< 正常 */
	I2C_ERR_START,          /*!< 启动时序错误 */
	I2C_ERR_DEVICE_ADDR,    /*!< 设备地址错误 */
	I2C_ERR_WRITE_ADDR,     /*!< 写地址错误 */
	I2C_ERR_WRITE_DATA,     /*!< 写数据错误 */
	I2C_ERR_STOP,           /*!< 停止时序错误 */
	I2C_ERR_WAIT_BUSY,      /*!< 等待总线忙错误 */
	I2C_ERR_READ,           /*!< 读数据错误 */
	I2C_ERR_TIMEOUT,        /*!< 超时 */
	
	BL_ERR_UNKNOWN = 0xff,  /*!< 未知错误 */
} i2c_err_t;

extern const char *err_str[];

#define I2C_WR	0		// 写控制bit
#define I2C_RD	1		// 读控制bit
 
#define RCC_AT24CXX_I2C_PORT 			RCC_AHB1Periph_GPIOB		// GPIO端口时钟
#define GPIO_AT24CXX_I2C_PORT			GPIOB						// GPIO端口
#define GPIO_AT24CXX_I2C_SCL_Pin		GPIO_Pin_8					// 连接到SCL时钟线的GPIO
#define GPIO_AT24CXX_I2C_SDA_Pin		GPIO_Pin_9					// 连接到SDA数据线的GPIO
 
/*通讯等待超时时间*/
#define I2CT_FLAG_TIMEOUT   ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT   ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

void ctl_at24cxx_i2c_init(void);
uint8_t ctl_i2c_start(void);
void ctl_i2c_stop(void);
uint8_t ctl_i2c_send_address(uint8_t addr, uint8_t rw_flag);
uint8_t ctl_i2c_send_byte(uint8_t byte);
uint8_t ctl_i2c_wait_busy(void);
uint8_t ctl_i2c_read_byte(void);
void ctl_i2c_ack(void);
void ctl_i2c_nack(void);

void at24cx_i2c_err_callback_register(at24cx_err_callback_t cb);
uint8_t at24cx_err_callback(uint8_t err_code);

#endif
