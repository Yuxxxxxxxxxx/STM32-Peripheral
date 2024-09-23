/******************************************************************************
 * @file ctl_i2c.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  
 * @version 0.1
 * @date 2024-09-22
 * @note 在访问I2C设备前，请先调用 ctl_i2c_checkdevice() 检测I2C设备是否正常，该函数会配置GPIO
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/ 
#include "stm32f4xx.h"
#include "ctl_i2c.h"
 
 
/******************************************************************************
 * @brief  初始化I2C总线的GPIO
 * 
 * @return none
 * 
 * @note   采用模拟IO的方式实现
 * 
******************************************************************************/
void ctl_at24cxx_i2c_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		// 设为输出口 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		// 设为开漏模式 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	// 上下拉电阻不使能 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	// IO口最大速度
	GPIO_InitStructure.GPIO_Pin = GPIO_AT24CXX_I2C_SCL_Pin | GPIO_AT24CXX_I2C_SDA_Pin;
	GPIO_Init(GPIO_AT24CXX_I2C_PORT, &GPIO_InitStructure);
 
	// 给一个停止信号, 复位I2C总线上的所有设备到待机模式
	ctl_i2c_stop();
}
 
/******************************************************************************
 * @brief  I2C总线位延迟，最快400KHz
 * 
 * @return none
 * 
******************************************************************************/
static void i2c_delay(void)
{
	uint8_t i;
 
	/**　
	 *	CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
	 *	循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
     *	循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
	 *	循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us
	 *	上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us
	 *	实际应用选择400KHz左右的速率即可
	 */
	for (i = 0; i < 30; i++)
	{
		__NOP();
		__NOP();
	}
}
 
/******************************************************************************
 * @brief  CPU发起I2C总线启动信号
 * 
 * @return none
 * 
 * @note   当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
 * 
******************************************************************************/
void ctl_i2c_start(void)
{
	// 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
	I2C_SDA_H();
	I2C_SCL_H();
	i2c_delay();
	I2C_SDA_L();
	i2c_delay();
	I2C_SCL_L();
	i2c_delay();
}
 
/******************************************************************************
 * @brief  CPU发起I2C总线停止信号
 * 
 * @return none
 * 
 * @note   当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
 * 
******************************************************************************/
void ctl_i2c_stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	I2C_SDA_L();
	I2C_SCL_H();
	i2c_delay();
	I2C_SDA_H();
	i2c_delay();
}
 
/******************************************************************************
 * @brief      CPU向I2C总线设备发送8bit数据
 * 
 * @param[in]  byte    :     等待发送的1个字节数据
 * 
 * @return     none
 * 
 * @note       SDA 上的数据变化只能在 SCL 低电平期间发生
 * 
******************************************************************************/
void ctl_i2c_sendbyte(uint8_t byte)
{
	uint8_t i;
 
	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{
		if (byte & 0x80)
		{
			I2C_SDA_H();
		}
		else
		{
			I2C_SDA_L();
		}
		i2c_delay();
		I2C_SCL_H();  // SCL高电平有效，发送一位数据
		i2c_delay();
		I2C_SCL_L();  // SCL低电平，准备下一位数据

		// 若是最后一位数据，释放SDA总线，表示数据传输结束
		if (i == 7)
		{
			 I2C_SDA_H(); // 释放总线
		}

		// 数据左移，准备下一位数据（高位先到
		byte <<= 1;
		i2c_delay();
	}
} 
 
/******************************************************************************
 * @brief  CPU从I2C总线设备读取8bit数据
 * 
 * @return uint8_t 
 * 
******************************************************************************/
uint8_t ctl_i2c_readbyte(void)
{
	uint8_t i;
	uint8_t value = 0;
 
	/* 读到第1个bit为数据的bit7 */
	for (i = 0; i < 8; i++)
	{
		value <<= 1;

		I2C_SCL_H();  // 将SCL拉高，准备接收数据
		i2c_delay();

		// 判断EEPROM发送过来的是1还是0
		if (I2C_SDA_RD())
		{
			value++;
		}

		I2C_SCL_L();  // 让EEPROM准备下一位数据
		i2c_delay();
	}

	return value;
}

/******************************************************************************
 * @brief  CPU产生一个时钟，并读取器件的ACK应答信号
 * 
 * @return uint8_t 
 * 
******************************************************************************/
uint8_t ctl_i2c_waitack(void)
{
	uint8_t re;
 
	I2C_SDA_H();// 自动释放SDA总线，将控制权交给EEPROM
	i2c_delay();
	I2C_SCL_H();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_delay();
	if (I2C_SDA_RD())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_L();
	i2c_delay();
	return re;
}
 
/******************************************************************************
 * @brief  CPU产生一个ACK信号
 * 
 * @return none
 * 
******************************************************************************/
void ctl_i2c_ack(void)
{
	I2C_SDA_L();	// SCL低电平期间，SDA 为低电平，表示应答信号
	i2c_delay();
	I2C_SCL_H();	// CPU产生1个时钟
	i2c_delay();
	I2C_SCL_L();
	i2c_delay();
	I2C_SDA_H();	// 应答完成释放SDA总线，否则接收到的数据全是0
}
 
/******************************************************************************
 * @brief  CPU产生1个NACK信号
 * 
 * @return none
 * 
******************************************************************************/
void ctl_i2c_nack(void)
{
	I2C_SDA_H();  // CPU驱动SDA = 1
	i2c_delay();
	I2C_SCL_H();  // SCL 高电平期间，SDA 为高电平，表示非应答信号
	i2c_delay();
	I2C_SCL_L();
	i2c_delay();
}
 
/******************************************************************************
 * @brief      检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
 * 
 * @param[in]  address    :     设备地址
 * 
 * @return     uint8_t    :     0 表示成功检测到设备; 返回1表示未探测到
 * 
******************************************************************************/
uint8_t ctl_i2c_checkdevice(uint8_t _Address)
{
	uint8_t ucAck;
 
	if (I2C_SDA_RD() && I2C_SCL_RD())
	{
		ctl_i2c_start();  // 发送启动信号
 
		// 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传
		ctl_i2c_sendbyte(_Address | I2C_WR);
		ucAck = ctl_i2c_waitack();	// 检测设备的ACK应答
 
		ctl_i2c_stop();  // 发送停止信号
 
		return ucAck;
	}
	return 1;  // I2C总线异常
}
