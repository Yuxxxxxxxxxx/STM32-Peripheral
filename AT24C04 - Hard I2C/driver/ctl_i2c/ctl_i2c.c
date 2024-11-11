/******************************************************************************
 * @file ctl_i2c.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  实现I2C总线的基本操作，使用STM32的硬件I2C接口。
 * @version 0.1
 * @date 2024-11-11
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdio.h>
#include "stm32f4xx.h"
#include "ctl_i2c.h"
 
 
static at24cx_err_callback_t err_callback;

const char *err_str[] = {
	[I2C_ERR_OK] = "everything is ok! \r\n",
	[I2C_ERR_START] = "send the start is timeout \r\n",
	[I2C_ERR_DEVICE_ADDR] = "send the address of device is timeout \r\n",
	[I2C_ERR_WRITE_ADDR] = "send the address of write buffer timeout \r\n",
	[I2C_ERR_WRITE_DATA] = "send the write data timeout \r\n",
	[I2C_ERR_STOP] = "send the stop bit timeout \r\n",
	[I2C_ERR_WAIT_BUSY] = "wait for the i2c bus timeout \r\n",
	[I2C_ERR_READ] = "send the read wrong \r\n",
};

/******************************************************************************
 * @brief  GPIO 初始化
 * 
 * @return none
******************************************************************************/
static void at24cxx_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		// 设为复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		// 设为开漏模式 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	// 上下拉电阻不使能 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// IO口最大速度
	GPIO_InitStructure.GPIO_Pin = GPIO_AT24CXX_I2C_SCL_Pin | GPIO_AT24CXX_I2C_SDA_Pin;
	GPIO_Init(GPIO_AT24CXX_I2C_PORT, &GPIO_InitStructure);
	
	// 配置引脚复用模式
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);  // SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);  // SDA
}
 
/******************************************************************************
 * @brief  I2C 初始化
 * 
 * @return none
******************************************************************************/
static void at24cxx_i2c_init(void)
{
	I2C_InitTypeDef I2C_InitStructure;

	// 配置 I2C
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                 // I2C 模式
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                         // 占空比
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                // 使能响应
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  // 7 位地址
	I2C_InitStructure.I2C_OwnAddress1 = 0x0A;  // STM32自身的I2C地址，这个地址只要与STM32外挂的I2C器件地址不一样即可
	I2C_InitStructure.I2C_ClockSpeed = 100000;                                 // 设置通信速度，这里设置为 100kHz
	
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

/******************************************************************************
 * @brief  初始化I2C总线的GPIO
 * 
 * @return none
 * 
 * @note   采用硬件I2C的方式实现
 *           SDA - PB9
 *           SCL - PB8
******************************************************************************/
void ctl_at24cxx_i2c_init(void)
{
	at24cxx_gpio_init();
	at24cxx_i2c_init();
}
 
/******************************************************************************
 * @brief  CPU发起I2C总线启动信号
 * 
 * @return none
 * 
 * @note   当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
******************************************************************************/
uint8_t ctl_i2c_start(void)
{
	uint16_t timeout = I2CT_FLAG_TIMEOUT;

	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		if ((timeout--) == 0) {
			return err_callback(I2C_ERR_START);
		}
	}
	
	return 1;
}
 
/******************************************************************************
 * @brief  CPU发起I2C总线停止信号
 * 
 * @return none
 * 
 * @note   当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
******************************************************************************/
void ctl_i2c_stop(void)
{
	//uint16_t timeout = I2CT_FLAG_TIMEOUT;

	I2C_GenerateSTOP(I2C1, ENABLE);
	/*while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)) {
		if ((timeout--) == 0) {
			I2C_TIMEOUT_UserCallback(1);
			return;
		}
	}*/
}

/******************************************************************************
 * @brief      CPU向I2C总线发送设备地址
 * 
 * @param[in]  addr    :    EEPROM设备地址
 * @param[in]  rw_flag :    读写标志位，0表示写模式，1表示读模式
 * @return     none
******************************************************************************/
uint8_t ctl_i2c_send_address(uint8_t addr, uint8_t rw_flag)
{
	uint16_t timeout = I2CT_FLAG_TIMEOUT;
	uint32_t event;
	
	if (rw_flag == I2C_WR) {  /*!< 写模式 */
		I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
		event = I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED;
	}
	else {  /*!< 读模式 */
		I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Receiver);
		event = I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED;
	}
	
	while (!I2C_CheckEvent(I2C1, event)) {
		if ((timeout--) == 0) {
			return err_callback(I2C_ERR_DEVICE_ADDR);
		}
	}
	
	return 1;
}

/******************************************************************************
 * @brief      CPU向I2C总线设备发送8bit数据
 * 
 * @param[in]  byte    :     等待发送的1个字节数据
 * @return     none
 * 
 * @note       SDA 上的数据变化只能在 SCL 低电平期间发生
******************************************************************************/
uint8_t ctl_i2c_send_byte(uint8_t byte)
{
	uint16_t timeout = I2CT_FLAG_TIMEOUT;
	
	I2C_SendData(I2C1, byte);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if ((timeout--) == 0) {
			return err_callback(I2C_ERR_WRITE_DATA);
		}
	}
	
	return 1;
}

/******************************************************************************
 * @brief  CPU从I2C总线设备读取8bit数据
 * 
 * @return uint8_t 
 * 
******************************************************************************/
uint8_t ctl_i2c_read_byte(void)
{
	uint8_t value = 0;
	
	uint16_t timeout = I2CT_LONG_TIMEOUT;
	while (!I2C_GetFlagStatus(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
		if ((timeout--) == 0) {
			return err_callback(I2C_ERR_READ);
		}
	}
	
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) ;
	value = I2C_ReceiveData(I2C1);

	return value;
}

/******************************************************************************
 * @brief      I2C 总线忙等待
 * 
 * @param[in]  address    :     设备地址
 * 
 * @return     uint8_t    :     0 表示成功检测到设备; 返回1表示未探测到
 * 
******************************************************************************/
uint8_t ctl_i2c_wait_busy(void)
{
	uint16_t timeout = I2CT_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
		if ((timeout--) == 0) {
			return err_callback(I2C_ERR_WAIT_BUSY);
		}
	}
	
	return 1;
}

void ctl_i2c_ack(void)
{
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

void ctl_i2c_nack(void)
{
	I2C_AcknowledgeConfig(I2C1, DISABLE);
}

/******************************************************************************
 * @brief      I2C 错误处理回调函数注册
 * 
 * @param[in]  cb    :    回调函数
 * @return 	   none
******************************************************************************/
void at24cx_i2c_err_callback_register(at24cx_err_callback_t cb)
{
	err_callback = cb;
}

uint8_t at24cx_err_callback(uint8_t err_code)
{
	return err_callback(err_code);
}
