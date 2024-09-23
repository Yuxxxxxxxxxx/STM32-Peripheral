/******************************************************************************
 * @file at24c.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-23
 * @note 实现AT24xx系列AT24CXPROM的读写操作。写操作采用页写模式提高写入效率。
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include "ctl_i2c.h"
#include "at24c.h"

 
/******************************************************************************
 * @brief  判断串行EERPOM是否正常
 * 
 * @return uint8_t : 1 表示正常， 0 表示不正常
 * 
******************************************************************************/
uint8_t at24cx_checkok(void)
{
	if (ctl_i2c_checkdevice(AT24CX_DEV_ADDR) == 0)
	{
		return 1;
	}
	else
	{
		// 失败后，切记发送I2C总线停止信号
		ctl_i2c_stop();
		return 0;
	}
}
 
/******************************************************************************
 * @brief      从串行EEPROM指定地址处开始读取若干数据
 * 
 * @param[in]  readuf    :    起始地址  
 * @param[in]  address   :    数据长度，单位为字节
 * @param[in]  size      :    存放读到的数据的缓冲区指针
 * 
 * @return     uint8_t   :    0 表示失败，1表示成功
 * 
******************************************************************************/
uint8_t at24cx_readbytes(uint8_t *readbuf, uint16_t address, uint16_t size)
{
	uint16_t i;
 
	/**
	 * 采用串行AT24CXPROM随即读取指令序列，连续读取若干字节
	 */ 
 
	// 第1步：发起I2C总线启动信号
	ctl_i2c_start();
 
	// 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
#if AT24CX_ADDR_A8 == 1
	ctl_i2c_sendbyte(AT24CX_DEV_ADDR | I2C_WR | ((address >> 7) & 0x0E));	// 写指令
#else
	ctl_i2c_sendbyte(AT24CX_DEV_ADDR | I2C_WR);	 //写指令
#endif
 
	// 第3步：发送ACK
	if (ctl_i2c_waitack() != 0)
	{
		goto cmd_fail;	// AT24CXPROM器件无应答
	}
 
	// 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址
	if (AT24CX_ADDR_BYTES == 1)
	{
		ctl_i2c_sendbyte((uint8_t)address);
		if (ctl_i2c_waitack() != 0)
		{
			goto cmd_fail;	// AT24CXPROM器件无应答
		}
	}
	else
	{
		ctl_i2c_sendbyte(address >> 8);
		if (ctl_i2c_waitack() != 0)
		{
			goto cmd_fail;	// AT24CXPROM器件无应答
		}
 
		ctl_i2c_sendbyte(address);
		if (ctl_i2c_waitack() != 0)
		{
			goto cmd_fail;	// AT24CXPROM器件无应答
		}
	}
 
	// 第5步：重新启动I2C总线。下面开始读取数据
	ctl_i2c_start();
 
	// 第6步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
#if AT24CX_ADDR_A8 == 1
	ctl_i2c_sendbyte(AT24CX_DEV_ADDR | I2C_RD | ((address >> 7) & 0x0E));	// 写指令
#else		
	ctl_i2c_sendbyte(AT24CX_DEV_ADDR | I2C_RD);  // 此处是写指令
#endif	
 
	// 第7步：发送ACK 
	if (ctl_i2c_waitack() != 0)
	{
		goto cmd_fail;	// AT24CXPROM器件无应答 
	}
 
	// 第8步：循环读取数据 
	for (i = 0; i < size; i++)
	{
		readbuf[i] = ctl_i2c_readbyte();	// 读1个字节
 
		// 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack
		if (i != size - 1)
		{
			ctl_i2c_ack();	// 中间字节读完后，CPU产生ACK信号(驱动SDA = 0)
		}
		else
		{
			ctl_i2c_nack();	// 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) 
		}
	}

	// 发送I2C总线停止信号
	ctl_i2c_stop();
	return 1;	// 执行成功
 
	// 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备
cmd_fail: 
	// 发送I2C总线停止信号
	ctl_i2c_stop();
	return 0;
}
 
/******************************************************************************
 * @brief      向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
 * 
 * @param[in]  writeBuf   :   起始地址  
 * @param[in]  address    :   数据长度，单位为字节
 * @param[in]  size       :   存放读到的数据的缓冲区指针
 * 
 * @return     uint8_t    :   0 表示失败，1表示成功
 * 
******************************************************************************/
uint8_t at24cx_writebytes(uint8_t *writebuf, uint16_t address, uint16_t size)
{
	uint16_t i, m;
	uint16_t addr;
 
	/**
	 *	写串行AT24CXPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
	 *	对于24xx02，page size = 8
	 *	简单的处理方法为：按字节写操作模式，每写1个字节，都发送地址
	 *	为了提高连续写的效率: 本函数采用page wirte操作。
	 */
 
	addr = address;
	for (i = 0; i < size; i++)
	{
		// 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址
		if ((i == 0) || (addr & (AT24CX_PAGE_SIZE - 1)) == 0)
		{
			//　第0步：发停止信号，启动内部写操作
			ctl_i2c_stop();
 
			/**
			 *  通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
			 *	CLK频率为200KHz时，查询次数为30次左右
			 */
			for (m = 0; m < 1000; m++)
			{
				// 第1步：发起I2C总线启动信号
				ctl_i2c_start();
 
				// 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
			#if AT24CX_ADDR_A8 == 1
				ctl_i2c_sendbyte(AT24CX_DEV_ADDR | I2C_WR | ((address >> 7) & 0x0E));  // 此处是写指令
			#else				
				ctl_i2c_sendbyte(AT24CX_DEV_ADDR | I2C_WR);
			#endif
 
				// 第3步：发送一个时钟，判断器件是否正确应答
				if (ctl_i2c_waitack() == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail;	// AT24CXPROM器件写超时
			}
 
			// 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址
			if (AT24CX_ADDR_BYTES == 1)
			{
				ctl_i2c_sendbyte((uint8_t)addr);
				if (ctl_i2c_waitack() != 0)
				{
					goto cmd_fail;	// AT24CXPROM器件无应答
				}
			}
			else
			{
				ctl_i2c_sendbyte(addr >> 8);
				if (ctl_i2c_waitack() != 0)
				{
					goto cmd_fail;	// AT24CXPROM器件无应答
				}
 
				ctl_i2c_sendbyte(addr);
				if (ctl_i2c_waitack() != 0)
				{
					goto cmd_fail;	// AT24CXPROM器件无应答
				}
			}
		}
 
		// 第5步：开始写入数据 
		ctl_i2c_sendbyte(writebuf[i]);
 
		// 第6步：发送ACK
		if (ctl_i2c_waitack() != 0)
		{
			goto cmd_fail;	// AT24CXPROM器件无应答
		}
 
		addr++;  // 地址增1
	}
 
	// 命令执行成功，发送I2C总线停止信号
	ctl_i2c_stop();
 
	/**
	 *  通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
	 *	CLK频率为200KHz时，查询次数为30次左右
	 */
	for (m = 0; m < 1000; m++)
	{
		// 第1步：发起I2C总线启动信号
		ctl_i2c_start();
 
		// 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
	#if AT24CX_ADDR_A8 == 1
		ctl_i2c_sendbyte(AT24CX_DEV_ADDR | I2C_WR | ((address >> 7) & 0x0E));  // 此处是写指令
	#else		
		ctl_i2c_sendbyte(AT24CX_DEV_ADDR | I2C_WR);	 // 此处是写指令
	#endif
 
		// 第3步：发送一个时钟，判断器件是否正确应答 
		if (ctl_i2c_waitack() == 0)
		{
			break;
		}
	}
	if (m  == 1000)
	{
		goto cmd_fail;	// AT24CXPROM器件写超时
	}
 
	// 命令执行成功，发送I2C总线停止信号
	ctl_i2c_stop();	
 
	return 1;
 
	// 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备
cmd_fail: 
	// 发送I2C总线停止信号
	ctl_i2c_stop();
	return 0;
}
 
 