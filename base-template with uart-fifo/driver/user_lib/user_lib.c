/******************************************************************************
 * @file user_lib.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief   This file contains the function prototypes of the user library.
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/

#include "user_lib.h"

// CRC 高位字节值表
static const uint8_t s_crc_high[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

// CRC 低位字节值表
const uint8_t s_crc_low[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/******************************************************************************
 * @brief      计算字符串长度 \0 是结束符
 * 
 * @param[in]  _str    :     字符串
 * 
 * @return int 
 * 
******************************************************************************/
int str_len(char *_str)
{
	int len = 0;

	while (*_str++) len++;
	return len;
}

/******************************************************************************
 * @brief 	   复制字符串
 * 
 * @param[in]  _tar    :    目标缓冲区
 * @param[in]  _src    :    源缓冲区
 * 
 * @return    none
 * 
******************************************************************************/
void str_cpy(char *_tar, char *_src)
{
	do
	{
		*_tar++ = *_src;
	}
	while (*_src++);
}

/******************************************************************************
 * @brief      字符串比较
 * 
 * @param[in]  s1    :    字符串1
 * @param[in]  s2    :    字符串2
 * 
 * @return     int   :    0 表示相等， 非0 表示不相等
 * 
******************************************************************************/
int str_cmp(char * s1, char * s2)
{
	while ((*s1!=0) && (*s2!=0) && (*s1==*s2))
	{
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

/******************************************************************************
 * @brief      设置缓冲区内容
 * 
 * @param[in]  _tar    :    目标缓冲区
 * @param[in]  _data   :    带设置的数据
 * @param[in]  _len    :    要设置缓冲区的长度
 * 
 * @return     none
 * 
******************************************************************************/
void mem_set(char *_tar, char _data, int _len)
{
	while (_len--)
	{
		*_tar++ = _data;
	}
}

/******************************************************************************
 * @brief      整形转字符串
 * 
 * @param[in]  _iNumber   :    数字   
 * @param[in]  _pBuf      :    目标缓冲区，存放字符串
 * @param[in]  _len       :    字符串长度
 * 
 * @return     none
 * 
******************************************************************************/
void int_to_str(int _iNumber, char *_pBuf, unsigned char _len)
{
	unsigned char i;
	int iTemp;

	if (_iNumber < 0)	/* 负数 */
	{
		iTemp = -_iNumber;	/* 转为正数 */
	}
	else
	{
		iTemp = _iNumber;
	}

	mem_set(_pBuf, ' ',_len);

	/* 将整数转换为ASCII字符串 */
	for (i = 0; i < _len; i++)
	{
		_pBuf[_len - 1 - i] = (iTemp % 10) + '0';
		iTemp = iTemp / 10;
		if (iTemp == 0)
		{
			break;
		}
	}
	_pBuf[_len] = 0;

	if (_iNumber < 0)	/* 负数 */
	{
		for (i = 0; i < _len; i++)
		{
			if ((_pBuf[i] == ' ') && (_pBuf[i + 1] != ' '))
			{
				_pBuf[i] = '-';
				break;
			}
		}
	}
}

/******************************************************************************
 * @brief     将ASCII码字符串转换成整数。 遇到小数点自动越过。
 * 
 * @param[in]  _pStr    :    待转换的ASCII码串. 为非0-9的字符。可以以逗号，#或0结束。
 * 
 * @return     int      :    二进制整数值
 * 
******************************************************************************/
int str_to_int(char *_pStr)
{
	unsigned char flag;
	char *p;
	int ulInt;
	unsigned char  i;
	unsigned char  ucTemp;

	p = _pStr;
	if (*p == '-')
	{
		flag = 1;	/* 负数 */
		p++;
	}
	else
	{
		flag = 0;
	}

	ulInt = 0;
	for (i = 0; i < 15; i++)
	{
		ucTemp = *p;
		if (ucTemp == '.')	/* 遇到小数点，自动跳过1个字节 */
		{
			p++;
			ucTemp = *p;
		}
		if ((ucTemp >= '0') && (ucTemp <= '9'))
		{
			ulInt = ulInt * 10 + (ucTemp - '0');
			p++;
		}
		else
		{
			break;
		}
	}

	if (flag == 1)
	{
		return -ulInt;
	}
	return ulInt;
}

/******************************************************************************
 * @brief      将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
 * 
 * @param[in]  _pBuf    :    数组
 * 
 * @return     uint16_t :    16位整数值
 * 
******************************************************************************/
uint16_t be_buf_to_uint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

/******************************************************************************
 * @brief     将2字节数组(小端Little Endian，低字节在前)转换为16位整数
 * 
 * @param[in]  _pBuf    :    数组
 * 
 * @return     uint16_t :    16位整数值 
 * 
******************************************************************************/
uint16_t le_buf_to_uint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[1] << 8) | _pBuf[0]);
}

/******************************************************************************
 * @brief 	   将4字节数组(大端Big Endian次序，高字节在前)转换为16位整数
 * 
 * @param[in]  _pBuf    :    数组
 * 
 * @return     uint32_t :    16位整数值 
 * 
******************************************************************************/
uint32_t be_buf_to_uint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[0] << 24) | ((uint32_t)_pBuf[1] << 16) | ((uint32_t)_pBuf[2] << 8) | _pBuf[3]);
}

/******************************************************************************
 * @brief 	  将4字节数组(小端Little Endian，低字节在前)转换为16位整数
 * 
 * @param[in]  _pBuf    :    数组
 * 
 * @return 	   uint32_t :    16位整数值
 * 
******************************************************************************/
uint32_t le_buf_to_uint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[3] << 24) | ((uint32_t)_pBuf[2] << 16) | ((uint32_t)_pBuf[1] << 8) | _pBuf[0]);
}

/******************************************************************************
 * @brief 	   计算CRC。 用于Modbus协议。
 * 
 * @param[in]  _pBuf     :     参与校验的数据
 * @param[in]  _usLen    :     数据长度
 * 
 * @return     uint16_t  :     16位整数值。 对于Modbus ，此结果高字节先传送，低字节后传送。
 * 
 * @note 所有可能的CRC值都被预装在两个数组当中，当计算报文内容时可以简单的索引即可；
 *       一个数组包含有16位CRC域的所有256个可能的高位字节，另一个数组含有低位字节的值；
 *       这种索引访问CRC的方式提供了比对报文缓冲区的每一个新字符都计算新的CRC更快的方法；
 * 
 * 注意：此程序内部执行高/低CRC字节的交换。此函数返回的是已经经过交换的CRC值；也就是说，该函数的返回值可以直接放置
 *        于报文用于发送；
 * 
******************************************************************************/
uint16_t crc16_modbus(uint8_t *_pBuf, uint16_t _usLen)
{
	uint8_t ucCRCHi = 0xFF; /* 高CRC字节初始化 */
	uint8_t ucCRCLo = 0xFF; /* 低CRC 字节初始化 */
	uint16_t usIndex;  /* CRC循环中的索引 */

    while (_usLen--)
    {
		usIndex = ucCRCHi ^ *_pBuf++; /* 计算CRC */
		ucCRCHi = ucCRCLo ^ s_crc_high[usIndex];
		ucCRCLo = s_crc_low[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
}

/******************************************************************************
 * @brief      根据2点直线方程，计算Y值
 * 
 * @param[in]  x1    :   
 * @param[in]  y1    :  
 * @param[in]  x2    :  
 * @param[in]  y2    :  
 * @param[in]  x     :    x输入量
 * 
 * @return    int32_t :   x对应的y值
 * 
******************************************************************************/
int32_t  cacul_two_point(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x)
{
	return y1 + ((int64_t)(y2 - y1) * (x - x1)) / (x2 - x1);
}

/******************************************************************************
 * @brief     将BCD码转为ASCII字符。 比如 0x0A ==> 'A'
 * 
 * @param[in]  _bcd    :    输入的二进制数。必须小于16
 * 
 * @return	   char    :    转换结果
 * 
******************************************************************************/
char bcd_to_char(uint8_t _bcd)
{
	if (_bcd < 10)
	{
		return _bcd + '0';
	}
	else if (_bcd < 16)
	{
		return _bcd + 'A';
	}
	else
	{
		return 0;
	}
}

/******************************************************************************
 * @brief     将二进制数组转换为16进制格式的ASCII字符串。每个2个ASCII字符后保留1个空格。
 *			  0x12 0x34 转化为 0x31 0x32 0x20 0x33 0x34 0x00  即 "1234"
 * 
 * @param[in]  _pHex       :     输入的数据，二进制数组
 * @param[in]  _pAscii     :     存放转换结果, ASCII字符串，0结束。1个二进制对应2个ASCII字符.
 * @param[in]  _BinBytes   :     二进制数组的长度
 * 
 * @return     none
 * 
******************************************************************************/
void hex_to_ascll(uint8_t * _pHex, char *_pAscii, uint16_t _BinBytes)
{
	uint16_t i;
	
	if (_BinBytes == 0)
	{
		_pAscii[0] = 0;
	}
	else
	{
		for (i = 0; i < _BinBytes; i++)
		{
			_pAscii[3 * i] = bcd_to_char(_pHex[i] >> 4);
			_pAscii[3 * i + 1] = bcd_to_char(_pHex[i] & 0x0F);
			_pAscii[3 * i + 2] = ' ';
		}
		_pAscii[3 * (i - 1) + 2] = 0;
	}
}

/******************************************************************************
 * @brief      变长的 ASCII 字符转换为32位整数  ASCII 字符以空格或者0结束 。 支持16进制和10进制输入
 * 
 * @param[in]  pAscii    :    要转换的ASCII码
 * 
 * @return     uint32_t  :    转换得到的整数
 * 
******************************************************************************/
uint32_t ascii_to_uint32(char *pAscii)
{
	char i;
	char bTemp;
	char bIsHex;
	char bLen;
	char bZeroLen;
	uint32_t lResult;
	uint32_t lBitValue;

	/* 判断是否是16进制数 */
	bIsHex = 0;
	if ((pAscii[0] == '0') && ((pAscii[1] == 'x') || (pAscii[1] == 'X')))
	{
		bIsHex=1;
	}

	lResult=0;
	// 最大数值为 4294967295, 10位+2字符"0x" //
	if (bIsHex == 0)
	{ // 十进制 //
		// 求长度 //
		lBitValue=1;

		/* 前导去0 */
		for (i = 0; i < 8; i++)
		{
			bTemp = pAscii[i];
			if (bTemp != '0')
				break;
		}
		bZeroLen = i;

		for (i = 0; i < 10; i++)
		{
			if ((pAscii[i] < '0') || (pAscii[i] > '9'))
				break;
			lBitValue = lBitValue * 10;
		}
		bLen = i;
		lBitValue = lBitValue / 10;
		if (lBitValue == 0)
			lBitValue=1;
		for (i = bZeroLen; i < bLen; i++)
		{
			lResult += (pAscii[i] - '0') * lBitValue;
			lBitValue /= 10;
		}
	}
	else
	{	/* 16进制 */
		/* 求长度 */
		lBitValue=1;

		/* 前导去0 */
		for (i = 0; i < 8; i++)
		{
			bTemp = pAscii[i + 2];
			if(bTemp!='0')
				break;
		}
		bZeroLen = i;
		for (; i < 8; i++)
		{
			bTemp=pAscii[i+2];
			if (((bTemp >= 'A') && (bTemp <= 'F')) ||
				((bTemp>='a')&&(bTemp<='f')) ||
				((bTemp>='0')&&(bTemp<='9')) )
			{
				lBitValue=lBitValue * 16;
			}
			else
			{
				break;
			}
		}
		lBitValue = lBitValue / 16;
		if (lBitValue == 0)
			lBitValue = 1;
		bLen = i;
		for (i = bZeroLen; i < bLen; i++)
		{
			bTemp = pAscii[i + 2];
			if ((bTemp >= 'A') && (bTemp <= 'F'))
			{
				bTemp -= 0x37;
			}
			else if ((bTemp >= 'a') && (bTemp <= 'f'))
			{
				bTemp -= 0x57;
			}
			else if ((bTemp >= '0') && (bTemp <= '9'))
			{
				bTemp -= '0';
			}
			lResult += bTemp*lBitValue;
			lBitValue /= 16;
		}
	}
	return lResult;
}

void bsp_Idle(void)
{
	// 当 CPU 空闲时会执行这个函数
}
