/******************************************************************************
 * @file at24c.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  实现AT24xx系列AT24CXPROM的读写操作，使用STM32的硬件I2C接口。
 * @version 0.1
 * @date 2024-11-11
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include "ctl_i2c.h"
#include "at24c.h"


static uint8_t at24cx_i2c_wait_eeprom_standby_state(uint16_t addr);
 
/******************************************************************************
 * @brief  写一个字节到EEPROM中
 *
 * @return none 
******************************************************************************/
uint8_t at24cx_write_byte(uint8_t *wbuf, uint16_t write_addr)
{
	/* 产生I2C起始信号 */
    ctl_i2c_start();

    /* 发送EEPROM设备地址 */
	ctl_i2c_send_address((0xA0 | ((write_addr >> 7) & 0x0E)), I2C_WR);

    /* 发送要写入的EEPROM内部地址(即EEPROM内部存储器的地址) */
	ctl_i2c_send_byte((uint8_t)write_addr);

    /* 发送一字节要写入的数据 */
	ctl_i2c_send_byte(*wbuf);

    /* 发送停止信号 */
	ctl_i2c_stop();

    return 1;
} 

/******************************************************************************
 * @brief      写多个字节到EEPROM
 * 
 * @param[in]  wbuf         :     要写入的数据
 * @param[in]  write_addr   :     要写入的地址
 * @param[in]  size         :     写入数据的大小
 * @return uint32_t 
******************************************************************************/
uint8_t at24cx_write_bytes(uint8_t *wbuf, uint16_t write_addr, uint16_t size)
{
	uint16_t i;
    uint8_t res;

    /*每写一个字节调用一次I2C_EE_ByteWrite函数*/
    for (i = 0; i < size; i++) {
        /*等待EEPROM准备完毕*/
        at24cx_i2c_wait_eeprom_standby_state(write_addr);
        /*按字节写入数据*/
        res = at24cx_write_byte(wbuf++, write_addr++);
    }
    return res;
}

/******************************************************************************
 * @brief      将缓冲区中的数据写到EEPROM中，采用单字节写入的方式，速度比页写入慢
 * 
 * @param[in]  wbuf         :      要写入的数据
 * @param[in]  write_addr   :      要写入的地址
 * @param[in]  size         :      写入数据的大小
 * @return     uint8_t 
 * 
 * @note       一次写入的字节数不能超过EEPROM页的大小，AT24C04每页有16个字节
 *             超过字节会覆盖前面的内容，例如想要一次写入24个字节，当写到第17个字节时，
 *             会重新从地址0开始写入，会导致前面的内容会被覆盖掉！！！！！！
******************************************************************************/
uint8_t at24cx_write_page(uint8_t *wbuf, uint16_t write_addr, uint16_t size)
{
	/*总线忙时等待 */
	ctl_i2c_wait_busy();

    /* 产生I2C起始信号 */
    ctl_i2c_start();

    /* 发送EEPROM设备地址  */
	ctl_i2c_send_address((0xA0 | ((write_addr >> 7) & 0x0E)), I2C_WR);

    /* 发送要写入的EEPROM内部地址(即EEPROM内部存储器的地址) */
	ctl_i2c_send_byte((uint8_t)write_addr);

    /* 循环发送NumByteToWrite个数据 */
    while (size--) 
	{
        /* 发送缓冲区中的数据 */
		ctl_i2c_send_byte(*wbuf);

        /* 指向缓冲区中的下一个数据 */
        wbuf++;
    }

    /* 发送停止信号 */
    ctl_i2c_stop();

    return 1;
}

/******************************************************************************
 * @brief      功能同at24cx_write_bytes，通过写入多个页的方式来实现，比
 *             at24cx_write_bytes 要快很多
 * 
 * @param[in]  wbuf         :      要写入的数据
 * @param[in]  write_addr   :      要写入的地址
 * @param[in]  size         :      写入数据的大小
 * @return     uint8_t 
 * 
 * @note       一次写入的字节数不能超过EEPROM页的大小，AT24C04每页有16个字节
 *             超过字节会覆盖前面的内容，例如想要一次写入24个字节，当写到第17个字节时，
 *             会重新从地址0开始写入，会导致前面的内容会被覆盖掉！！！！！！
******************************************************************************/
void at24cx_write_buffer(uint8_t* wbuf, uint16_t write_addr, uint16_t size)
{
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0;

    /* mod运算求余，若writeAddr是I2C_PageSize整数倍，运算结果Addr值为0 */
    addr = write_addr % AT24CX_PAGE_SIZE;

    /* 差count个数据，刚好可以对齐到页地址 */
    count = AT24CX_PAGE_SIZE - addr;
    /*计算出要写多少整数页*/
    num_of_page =  size / AT24CX_PAGE_SIZE;
    /*mod运算求余，计算出剩余不满一页的字节数*/
    num_of_single = size % AT24CX_PAGE_SIZE;

    /* addr=0,则WriteAddr 刚好按页对齐 aligned  */
    if (addr == 0) {
        /* 如果 size < AT24CX_PAGE_SIZE */
        if (num_of_page == 0) {
            at24cx_write_page(wbuf, write_addr, num_of_single);
            at24cx_i2c_wait_eeprom_standby_state(write_addr);
        }
        /* 如果 size > AT24CX_PAGE_SIZE */
        else {
            /*先把整数页都写了*/
            while (num_of_page--) {
                at24cx_write_page(wbuf, write_addr, AT24CX_PAGE_SIZE);
                at24cx_i2c_wait_eeprom_standby_state(write_addr);
                write_addr +=  AT24CX_PAGE_SIZE;
                wbuf += AT24CX_PAGE_SIZE;
            }

            /*若有多余的不满一页的数据，把它写完*/
            if (num_of_single!=0) {
                at24cx_write_page(wbuf, write_addr, num_of_single);
                at24cx_i2c_wait_eeprom_standby_state(write_addr);
            }
        }
    }
    /* 如果 write_addr 不是按 AT24CX_PAGE_SIZE 对齐  */
    else {
        /* 如果 size < AT24CX_PAGE_SIZE */
        if (num_of_page== 0) {
            at24cx_write_page(wbuf, write_addr, num_of_single);
            at24cx_i2c_wait_eeprom_standby_state(write_addr);
        }
        /* 如果 size > AT24CX_PAGE_SIZE */
        else {
            /*地址不对齐多出的count分开处理，不加入这个运算*/
            size -= count;
            num_of_page =  size / AT24CX_PAGE_SIZE;
            num_of_single = size % AT24CX_PAGE_SIZE;

            /* 先把WriteAddr所在页的剩余字节写了 */
            if (count != 0) {
                at24cx_write_page(wbuf, write_addr, count);
                at24cx_i2c_wait_eeprom_standby_state(write_addr);

                /* WriteAddr加上count后，地址就对齐到页了 */
                write_addr += count;
                wbuf += count;
        	}
            /*把整数页都写了*/
            while (num_of_page--) {
                at24cx_write_page(wbuf, write_addr, AT24CX_PAGE_SIZE);
                at24cx_i2c_wait_eeprom_standby_state(write_addr);
                write_addr +=  AT24CX_PAGE_SIZE;
                wbuf += AT24CX_PAGE_SIZE;
            }
            /*若有多余的不满一页的数据，把它写完*/
            if (num_of_single != 0) {
                at24cx_write_page(wbuf, write_addr, num_of_single);
                at24cx_i2c_wait_eeprom_standby_state(write_addr);
            }
        }
    }
}

//等待Standby状态的最大次数
#define MAX_TRIAL_NUMBER 300

/******************************************************************************
 * @brief      等待EEPROM到准备状态
 * 
 * @param[in]  addr    :    要写入的地址
 * @return     uint8_t 
******************************************************************************/
static uint8_t at24cx_i2c_wait_eeprom_standby_state(uint16_t addr)
{
    __IO uint16_t tmpSR1 = 0;
    __IO uint32_t EETrials = 0;

    /*总线忙时等待 */
    ctl_i2c_wait_busy();

    /* 等待从机应答，最多等待300次 */
    while (1)
    {
        /*开始信号 */
        ctl_i2c_start();

        /* 发送EEPROM设备地址 */
		I2C_Send7bitAddress(I2C1, (0xA0 | ((addr >> 7) & 0x0E)), I2C_Direction_Transmitter);

        /* 等待ADDR标志 */
        uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;
        do {
            /* 获取SR1寄存器状态 */
            tmpSR1 = I2C1->SR1;

            if ((I2CTimeout--) == 0) return at24cx_err_callback(I2C_ERR_TIMEOUT); //I2C_TIMEOUT_UserCallback(22);
        }
        /* 一直等待直到addr及af标志为1 */
        while ((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);

        /*检查addr标志是否为1 */
        if (tmpSR1 & I2C_SR1_ADDR) {
            /* 清除addr标志该标志通过读SR1及SR2清除 */
            (void)I2C1->SR2;

            /*产生停止信号 */
            ctl_i2c_stop();

            /* 退出函数 */
            return 1;
        }
        else {
            /*清除af标志 */
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);
        }

        /*检查等待次数*/
        if (EETrials++ == MAX_TRIAL_NUMBER) {
            /* 等待MAX_TRIAL_NUMBER次都还没准备好，退出等待 */
            at24cx_err_callback(I2C_ERR_TIMEOUT);
			//return I2C_TIMEOUT_UserCallback(23);
        }
    }
}

/******************************************************************************
 * @brief      从EEPROM里面读取一块数据
 * 
 * @param[in]  rbuf        :     存放读取的数据
 * @param[in]  read_addr   :     要读取数据的地址
 * @param[in]  size        :     要读取数据的大小
 * @return     uint8_t 
 *
 * @note       注意，当接收完最后一个数据之后收到的是 NACK
******************************************************************************/
uint8_t at24cx_read_buf(uint8_t *rbuf, uint16_t read_addr, uint16_t size)
{
	/** 总线忙时等待 */
	ctl_i2c_wait_busy();

    /* 产生I2C起始信号 */
    ctl_i2c_start();

    /* 发送EEPROM设备地址 */
	ctl_i2c_send_address((0xA0 | ((read_addr >> 7) & 0x0E)), I2C_WR);
    
    /*通过重新设置PE位清除EV6事件 */
    I2C_Cmd(I2C1, ENABLE);

    /* 发送要读取的EEPROM内部地址(即EEPROM内部存储器的地址) */
	ctl_i2c_send_byte(read_addr);
    
    /* 产生第二次I2C起始信号 */
    ctl_i2c_start();
	
    /* 发送EEPROM设备地址 */
    ctl_i2c_send_address((0xA0 | ((read_addr >> 7) & 0x0E)), I2C_RD); 
	
    /* 读取size个数据*/
    while (size)
    {
        /** 若size=1，表示已经接收到最后一个数据了，
            发送非应答信号，结束传输*/
        if (size == 1)
        {
            ctl_i2c_nack();  /*!< 发送非应答信号 */

            ctl_i2c_stop();  /*!< 发送停止信号 */
        }

		/** 通过I2C，从设备中读取一个字节的数据 */
		*rbuf = ctl_i2c_read_byte();
        
		{                     
            rbuf++;  /*!< 存储数据的指针指向下一个地址 */            
            size--;  /*!< 接收数据自减 */
        }
    }

    /** 使能应答，方便下一次I2C传输 */
    ctl_i2c_ack();
    return 1;
}
