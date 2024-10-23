/******************************************************************************
 * @file uart_fifo.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  采用串口中断+FIFO模式实现多个串口的同时访问
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdio.h>
#include "uart_fifo.h"
#include "user_lib.h"


/* 串口1的GPIO */
#if UART1_FIFO_EN == 1
	#define UART1_TX_PORT      GPIOA
	#define UART1_TX_PIN       GPIO_Pin_9
	#define UART1_TX_CLK       RCC_AHB1Periph_GPIOA
	#define UART1_TX_SOURCE    GPIO_PinSource9

	#define UART1_RX_PORT      GPIOA
	#define UART1_RX_PIN       GPIO_Pin_10
	#define UART1_RX_CLK       RCC_AHB1Periph_GPIOA
	#define UART1_RX_SOURCE    GPIO_PinSource10
#endif

/* 串口2的GPIO */
#if UART2_FIFO_EN == 1
	#define UART2_TX_PORT      GPIOA
	#define UART2_TX_PIN       GPIO_Pin_2
	#define UART2_TX_CLK       RCC_AHB1Periph_GPIOA
	#define UART2_TX_SOURCE    GPIO_PinSource2

	#define UART2_RX_PORT      GPIOA
	#define UART2_RX_PIN       GPIO_Pin_3
	#define UART2_RX_CLK       RCC_AHB1Periph_GPIOA
	#define UART2_RX_SOURCE    GPIO_PinSource3
#endif

/* 串口3的GPIO */
#if UART3_FIFO_EN == 1
	#define UART3_TX_PORT      GPIOB
	#define UART3_TX_PIN       GPIO_Pin_10
	#define UART3_TX_CLK       RCC_AHB1Periph_GPIOB
	#define UART3_TX_SOURCE    GPIO_PinSource10

	#define UART3_RX_PORT      GPIOB
	#define UART3_RX_PIN       GPIO_Pin_11
	#define UART3_RX_CLK       RCC_AHB1Periph_GPIOB
	#define UART3_RX_SOURCE    GPIO_PinSource11
#endif

/* 串口4的GPIO */
#if UART4_FIFO_EN == 1
	#define UART4_TX_PORT      GPIOC
	#define UART4_TX_PIN       GPIO_Pin_10
	#define UART4_TX_CLK       RCC_AHB1Periph_GPIOC
	#define UART4_TX_SOURCE    GPIO_PinSource10

	#define UART4_RX_PORT      GPIOC
	#define UART4_RX_PIN       GPIO_Pin_11
	#define UART4_RX_CLK       RCC_AHB1Periph_GPIOC
	#define UART4_RX_SOURCE    GPIO_PinSource11
#endif

/* 串口5的GPIO */
#if UART5_FIFO_EN == 1
	#define UART5_TX_PORT      GPIOC
	#define UART5_TX_PIN       GPIO_Pin_12
	#define UART5_TX_CLK       RCC_AHB1Periph_GPIOC
	#define UART5_TX_SOURCE    GPIO_PinSource12

	#define UART5_RX_PORT      GPIOD
	#define UART5_RX_PIN       GPIO_Pin_2
	#define UART5_RX_CLK       RCC_AHB1Periph_GPIOD
	#define UART5_RX_SOURCE    GPIO_PinSource2
#endif

/* 定义每个串口结构体变量 */
#if UART1_FIFO_EN == 1
	static uart_t g_uart1;
	static uint8_t g_tx_buf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_rx_buf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART2_FIFO_EN == 1
	static uart_t g_uart2;
	static uint8_t g_tx_buf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_rx_buf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART3_FIFO_EN == 1
	static uart_t g_uart3;
	static uint8_t g_tx_buf3[UART3_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_rx_buf3[UART3_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART4_FIFO_EN == 1
	static uart_t g_uart4;
	static uint8_t g_tx_buf4[UART4_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_rx_buf4[UART4_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART5_FIFO_EN == 1
	static uart_t g_uart5;
	static uint8_t g_tx_buf5[UART5_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_rx_buf5[UART5_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

static void uart_var_init(void);
static void uart_lowlevel_init(void);
static void uart_irq_init(void);

static void uart_send(uart_t *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t uart_get_char(uart_t *_pUart, uint8_t *_pByte);
static void uart_irq_handler(uart_t *_pUart);

#ifdef RS485
void rs485_init_txe(void);
#endif


/******************************************************************************
 * @brief  初始化串口硬件，并对全局变量赋初值.
 * 
 * @return none
 * 
******************************************************************************/
void bsp_uart_init(void)
{
	uart_var_init();		/* 必须先初始化全局变量,再配置硬件 */

	uart_lowlevel_init();		/* 配置串口的硬件参数(波特率等) */

#ifdef RS485
	rs485_init_txe();	/* 配置RS485芯片的发送使能硬件，配置为推挽输出 */
#endif

	uart_irq_init();	/* 配置串口中断 */
}

/******************************************************************************
 * @brief     将COM端口号转换为UART指针
 * 
 * @param[in]  _port    :    端口号(COM1 - COM5)
 * 
 * @return     uart_t*    :    uart指针
 * 
******************************************************************************/
static uart_t *com_to_uart(com_port_t _port)
{
	if (_port == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_uart1;
		#else
			return 0;
		#endif
	}
	else if (_port == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_uart2;
		#else
			return 0;
		#endif
	}
	else if (_port == COM3)
	{
		#if UART3_FIFO_EN == 1
			return &g_uart3;
		#else
			return 0;
		#endif
	}
	else if (_port == COM4)
	{
		#if UART4_FIFO_EN == 1
			return &g_uart4;
		#else
			return 0;
		#endif
	}
	else if (_port == COM5)
	{
		#if UART5_FIFO_EN == 1
			return &g_uart5;
		#else
			return 0;
		#endif
	}
	else
	{
		/* 不做任何处理 */
		return 0;
	}
}

/******************************************************************************
 * @brief      将COM端口号转换为 USART_TypeDef* USARTx
 * 
 * @param[in]  _port    :     端口号(COM1 - COM5)
 * 
 * @return     USART_TypeDef* : USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5
 * 
******************************************************************************/
static USART_TypeDef *com_to_USARTx(com_port_t _port)
{
	if (_port == COM1)
	{
		#if UART1_FIFO_EN == 1
			return USART1;
		#else
			return 0;
		#endif
	}
	else if (_port == COM2)
	{
		#if UART2_FIFO_EN == 1
			return USART2;
		#else
			return 0;
		#endif
	}
	else if (_port == COM3)
	{
		#if UART3_FIFO_EN == 1
			return USART3;
		#else
			return 0;
		#endif
	}
	else if (_port == COM4)
	{
		#if UART4_FIFO_EN == 1
			return UART4;
		#else
			return 0;
		#endif
	}
	else if (_port == COM5)
	{
		#if UART5_FIFO_EN == 1
			return UART5;
		#else
			return 0;
		#endif
	}
	else
	{
		/* 不做任何处理 */
		return 0;
	}
}

/******************************************************************************
 * @brief      向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
 * 
 * @param[in]  _port    :    端口号(COM1 - COM5)
 * @param[in]  _buf     :    待发送的数据缓冲区
 * @param[in]  _size    :    数据长度
 * 
 * @return 	   none
 * 
******************************************************************************/
void com_send_buf(com_port_t _port, uint8_t *_buf, uint16_t _size)
{
	uart_t *uart_tmp;

	uart_tmp = com_to_uart(_port);
	if (uart_tmp == 0)
	{
		return;
	}

	if (uart_tmp->uart_ops.send_before != 0)
	{
		uart_tmp->uart_ops.send_before();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
	}

	uart_send(uart_tmp, _buf, _size);
}

/******************************************************************************
 * @brief      向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
 * 
 * @param[in]  _port    :    端口号(COM1 - COM5)
 * @param[in]  _byte    :    待发送的数据
 * 
 * @return     none
 * 
******************************************************************************/
void com_send_char(com_port_t _port, uint8_t _byte)
{
	com_send_buf(_port, &_byte, 1);
}

/******************************************************************************
 * @brief      从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
 * 
 * @param[in]  _port    :    端口号(COM1 - COM5)
 * @param[in]  _byte    :    接收到的数据存放在这个地址
 * 
 * @return     uint8_t  :    0 表示无数据, 1 表示读取到有效字节
 * 
******************************************************************************/
uint8_t com_get_char(com_port_t _port, uint8_t *_byte)
{
	uart_t *uart_tmp;

	uart_tmp = com_to_uart(_port);
	if (uart_tmp == 0)
	{
		return 0;
	}

	return uart_get_char(uart_tmp, _byte);
}

/******************************************************************************
 * @brief 	  清零串口发送缓冲区
 * 
 * @param[in]  _ucPort    :    端口号(COM1 - COM5)
 * 
 * @return 	   none
 * 
******************************************************************************/
void com_clear_tx_fifo(com_port_t _ucPort)
{
	uart_t *uart_tmp;

	uart_tmp = com_to_uart(_ucPort);
	if (uart_tmp == 0)
	{
		return;
	}
	
	uart_tmp->ctrl_tx.tx_read_idx  = 0;
	uart_tmp->ctrl_tx.tx_write_idx = 0;
	uart_tmp->ctrl_tx.tx_count     = 0;
}

/******************************************************************************
 * @brief 	   清零串口接收缓冲区
 * 
 * @param[in]  _port    :    端口号(COM1 - COM5)
 * 
 * @return 	   none
 * 
******************************************************************************/
void com_clear_rx_fifo(com_port_t _port)
{
	uart_t *uart_tmp;

	uart_tmp = com_to_uart(_port);
	if (uart_tmp == 0)
	{
		return;
	}

	uart_tmp->ctrl_rx.rx_read_idx  = 0;
	uart_tmp->ctrl_rx.rx_write_idx = 0;
	uart_tmp->ctrl_rx.rx_count     = 0;
}

/******************************************************************************
 * @brief      设置串口的波特率
 * 
 * @param[in]  _port       :    端口号(COM1 - COM5)
 * @param[in]  _baudrate   :    波特率，0-4500000， 最大4.5Mbps
 * 
 * @return     none
 * 
******************************************************************************/
void com_set_baud(com_port_t _port, uint32_t _baudrate)
{
	USART_TypeDef* USARTx;
	
	USARTx = com_to_USARTx(_port);
	if (USARTx == 0)
	{
		return;
	}
	
	usart_set_baudrate(USARTx, _baudrate);
}

/******************************************************************************
 * @brief      修改波特率寄存器，不更改其他设置。如果使用 USART_Init函数, 
 *             则会修改硬件流控参数和RX,TX配置。根据固件库中 USART_Init函数，
 * 			   将其中配置波特率的部分单独提出来封装为一个函数
 * 
 * @param[in]  USARTx      :    USART1, USART2, USART3, UART4, UART5
 * @param[in]  BaudRate    :    波特率，取值 0 - 4500000
 * 
 * @return     none
 * 
******************************************************************************/
void usart_set_baudrate(USART_TypeDef* USARTx, uint32_t BaudRate)
{
	uint32_t tmpreg = 0x00, apbclock = 0x00;
	uint32_t integerdivider = 0x00;
	uint32_t fractionaldivider = 0x00;
	RCC_ClocksTypeDef RCC_ClocksStatus;

	/* Check the parameters */
	assert_param(IS_USART_ALL_PERIPH(USARTx));
	assert_param(IS_USART_BAUDRATE(BaudRate));  

	/*---------------------------- USART BRR Configuration -----------------------*/
	/* Configure the USART Baud Rate */
	RCC_GetClocksFreq(&RCC_ClocksStatus);

	if (USARTx == USART1)
	{
		apbclock = RCC_ClocksStatus.PCLK2_Frequency;
	}
	else
	{
		apbclock = RCC_ClocksStatus.PCLK1_Frequency;
	}

	/* Determine the integer part */
	if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
	{
		/* Integer part computing in case Oversampling mode is 8 Samples */
		integerdivider = ((25 * apbclock) / (2 * (BaudRate)));    
	}
	else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
	{
		/* Integer part computing in case Oversampling mode is 16 Samples */
		integerdivider = ((25 * apbclock) / (4 * (BaudRate)));    
	}
	tmpreg = (integerdivider / 100) << 4;

	/* Determine the fractional part */
	fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

	/* Implement the fractional part in the register */
	if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
	{
		tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
	}
	else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
	{
		tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
	}

	/* Write to USART BRR register */
	USARTx->BRR = (uint16_t)tmpreg;
}


#ifdef RS485
/* 如果是RS485通信，请按如下格式编写函数， 这里以 USART3 作为 RS485 通信端口 */

/******************************************************************************
 * @brief  配置RS485发送使能口线 TXE
 * 
 * @return none
 * 
 * @note   本例使用的 STM32F407VE 核心板并没有板载 485 芯片，
 * 		   所以外接了一个 TTL 转 485 模块，这里无需使能 TXE 口
 * 
******************************************************************************/
void rs485_init_txe(void)
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_RS485_TXEN, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 无上拉电阻 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
	GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
	GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
#endif
}

/******************************************************************************
 * @brief      修改485串口的波特率。
 * 
 * @param[in]  _baudrate    :   波特率.0-4500000
 * 
 * @return     none
 * 
******************************************************************************/
void rs485_set_baud(uint32_t _baudrate)
{
	com_set_baud(COM3, _baudrate);
}

/******************************************************************************
 * @brief      通过RS485芯片发送一串数据。注意，本函数不等待发送完毕。
 * 
 * @param[in]  _buf     :     数据缓冲区
 * @param[in]  _size    :     数据长度
 * 
 * @return     none
 * 
******************************************************************************/
void rs485_send_buf(uint8_t *_buf, uint16_t _size)
{
	com_send_buf(COM3, _buf, _size);
}

/******************************************************************************
 * @brief     向485总线发送一个字符串 \0结束。
 * 
 * @param[in]  _buf    :    要发送的字符串
 * 
 * @return     none
 * 
******************************************************************************/
void rs485_send_str(char *_buf)
{
	rs485_send_buf((uint8_t *)_buf, strlen(_buf));
}

/******************************************************************************
 * @brief  发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
 *		   并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
 * 
 * @return none
 * 
 * @note   板载模块自动收发
 * 
******************************************************************************/
void rs485_send_before(void)
{
	// RS485_TX_EN();	/* 切换RS485收发芯片为发送模式 */
}

/******************************************************************************
 * @brief     接收到新的数据
 * 
 * @param[in]  _byte    :   接收到的新数据
 * 
 * @return    none
 * 
******************************************************************************/
extern void modh_receive_new(uint8_t _data);
void rs485_receive_new(uint8_t _byte)
{
	modh_receive_new(_byte);
}

/******************************************************************************
 * @brief  发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
 *		   并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
 *
 * @return none
 * 
 * @note   板载模块自动收发
 * 
******************************************************************************/
void rs485_send_over(void)
{
	// RS485_RX_EN();	/* 切换RS485收发芯片为接收模式 */
}
#endif

/******************************************************************************
 * @brief  初始化串口相关的变量
 * 
 * @return none
 * 
******************************************************************************/
static void uart_var_init(void)
{
#if UART1_FIFO_EN == 1
	g_uart1.uart = USART1;						       /* STM32 串口设备 */
	
	/* 接收控制器参数 */
	g_uart1.ctrl_rx.rx_buf       = g_rx_buf1;		   /* 接收缓冲区指针 */
	g_uart1.ctrl_rx.rx_buf_size  = UART1_RX_BUF_SIZE;  /* 接收缓冲区大小 */
	g_uart1.ctrl_rx.rx_read_idx  = 0;				   /* 接收FIFO读索引 */
	g_uart1.ctrl_rx.rx_write_idx = 0;				   /* 接收FIFO写索引 */
	g_uart1.ctrl_rx.rx_count     = 0;                  /* 接收到的新数据个数 */

	/* 发送控制器参数 */
	g_uart1.ctrl_tx.tx_buf       = g_tx_buf1;		   /* 发送缓冲区指针 */
	g_uart1.ctrl_tx.tx_buf_size  = UART1_TX_BUF_SIZE;  /* 发送缓冲区大小 */
	g_uart1.ctrl_tx.tx_read_idx  = 0;				   /* 发送FIFO读索引 */
	g_uart1.ctrl_tx.tx_write_idx = 0;				   /* 发送FIFO写索引 */
	g_uart1.ctrl_tx.tx_count     = 0;                  /* 待发送的数据个数 */
	
	/* 串口操作函数设置 */
	g_uart1.uart_ops.send_before = 0;				   /* 发送数据前的回调函数 */
	g_uart1.uart_ops.recive_new  = 0;				   /* 接收到新数据后的回调函数 */
	g_uart1.uart_ops.send_over   = 0;				   /* 发送完毕后的回调函数 */
#endif

#if UART2_FIFO_EN == 1
	g_uart2.uart = USART2;						       /* STM32 串口设备 */
	
	/* 接收控制器参数 */
	g_uart2.ctrl_rx.rx_buf       = g_rx_buf2;		   /* 接收缓冲区指针 */
	g_uart2.ctrl_rx.rx_buf_size  = UART2_RX_BUF_SIZE;  /* 接收缓冲区大小 */
	g_uart2.ctrl_rx.rx_read_idx  = 0;				   /* 接收FIFO读索引 */
	g_uart2.ctrl_rx.rx_write_idx = 0;				   /* 接收FIFO写索引 */
	g_uart2.ctrl_rx.rx_count     = 0;                  /* 接收到的新数据个数 */

	/* 发送控制器参数 */
	g_uart2.ctrl_tx.tx_buf       = g_tx_buf2;		   /* 发送缓冲区指针 */
	g_uart2.ctrl_tx.tx_buf_size  = UART2_TX_BUF_SIZE;  /* 发送缓冲区大小 */
	g_uart2.ctrl_tx.tx_read_idx  = 0;				   /* 发送FIFO读索引 */
	g_uart2.ctrl_tx.tx_write_idx = 0;				   /* 发送FIFO写索引 */
	g_uart2.ctrl_tx.tx_count     = 0;                  /* 待发送的数据个数 */
	
	/* 串口操作函数设置 */
	g_uart2.uart_ops.send_before = 0;				   /* 发送数据前的回调函数 */
	g_uart2.uart_ops.recive_new  = 0;				   /* 接收到新数据后的回调函数 */
	g_uart2.uart_ops.send_over   = 0;				   /* 发送完毕后的回调函数 */
#endif

#if UART3_FIFO_EN == 1
	g_uart3.uart = USART3;						       /* STM32 串口设备 */
	
	/* 接收控制器参数 */
	g_uart3.ctrl_rx.rx_buf       = g_rx_buf3;		   /* 接收缓冲区指针 */
	g_uart3.ctrl_rx.rx_buf_size  = UART3_RX_BUF_SIZE;  /* 接收缓冲区大小 */
	g_uart3.ctrl_rx.rx_read_idx  = 0;				   /* 接收FIFO读索引 */
	g_uart3.ctrl_rx.rx_write_idx = 0;				   /* 接收FIFO写索引 */
	g_uart3.ctrl_rx.rx_count     = 0;                  /* 接收到的新数据个数 */

	/* 发送控制器参数 */
	g_uart3.ctrl_tx.tx_buf       = g_tx_buf3;		   /* 发送缓冲区指针 */
	g_uart3.ctrl_tx.tx_buf_size  = UART3_TX_BUF_SIZE;  /* 发送缓冲区大小 */
	g_uart3.ctrl_tx.tx_read_idx  = 0;				   /* 发送FIFO读索引 */
	g_uart3.ctrl_tx.tx_write_idx = 0;				   /* 发送FIFO写索引 */
	g_uart3.ctrl_tx.tx_count     = 0;                  /* 待发送的数据个数 */
	
	/* 串口操作函数设置 */
	g_uart3.uart_ops.send_before = rs485_send_before;  /* 发送数据前的回调函数 */
	g_uart3.uart_ops.recive_new  = rs485_receive_new;  /* 接收到新数据后的回调函数 */
	g_uart3.uart_ops.send_over   = rs485_send_over;	   /* 发送完毕后的回调函数 */
#endif

#if UART4_FIFO_EN == 1
	g_uart4.uart = UART4;						       /* STM32 串口设备 */
	
	/* 接收控制器参数 */
	g_uart4.ctrl_rx.rx_buf       = g_rx_buf4;		   /* 接收缓冲区指针 */
	g_uart4.ctrl_rx.rx_buf_size  = UART4_RX_BUF_SIZE;  /* 接收缓冲区大小 */
	g_uart4.ctrl_rx.rx_read_idx  = 0;				   /* 接收FIFO读索引 */
	g_uart4.ctrl_rx.rx_write_idx = 0;				   /* 接收FIFO写索引 */
	g_uart4.ctrl_rx.rx_count     = 0;                  /* 接收到的新数据个数 */

	/* 发送控制器参数 */
	g_uart4.ctrl_tx.tx_buf       = g_tx_buf4;		   /* 发送缓冲区指针 */
	g_uart4.ctrl_tx.tx_buf_size  = UART4_TX_BUF_SIZE;  /* 发送缓冲区大小 */
	g_uart4.ctrl_tx.tx_read_idx  = 0;				   /* 发送FIFO读索引 */
	g_uart4.ctrl_tx.tx_write_idx = 0;				   /* 发送FIFO写索引 */
	g_uart4.ctrl_tx.tx_count     = 0;                  /* 待发送的数据个数 */
	
	/* 串口操作函数设置 */
	g_uart4.uart_ops.send_before = 0;				   /* 发送数据前的回调函数 */
	g_uart4.uart_ops.recive_new  = 0;				   /* 接收到新数据后的回调函数 */
	g_uart4.uart_ops.send_over   = 0;				   /* 发送完毕后的回调函数 */
#endif

#if UART5_FIFO_EN == 1
	g_uart5.uart = UART5;						       /* STM32 串口设备 */
	
	/* 接收控制器参数 */
	g_uart5.ctrl_rx.rx_buf       = g_rx_buf5;		   /* 接收缓冲区指针 */
	g_uart5.ctrl_rx.rx_buf_size  = UART5_RX_BUF_SIZE;  /* 接收缓冲区大小 */
	g_uart5.ctrl_rx.rx_read_idx  = 0;				   /* 接收FIFO读索引 */
	g_uart5.ctrl_rx.rx_write_idx = 0;				   /* 接收FIFO写索引 */
	g_uart5.ctrl_rx.rx_count     = 0;                  /* 接收到的新数据个数 */

	/* 发送控制器参数 */
	g_uart5.ctrl_tx.tx_buf       = g_tx_buf5;		   /* 发送缓冲区指针 */
	g_uart5.ctrl_tx.tx_buf_size  = UART5_TX_BUF_SIZE;  /* 发送缓冲区大小 */
	g_uart5.ctrl_tx.tx_read_idx  = 0;				   /* 发送FIFO读索引 */
	g_uart5.ctrl_tx.tx_write_idx = 0;				   /* 发送FIFO写索引 */
	g_uart5.ctrl_tx.tx_count     = 0;                  /* 待发送的数据个数 */
	
	/* 串口操作函数设置 */
	g_uart5.uart_ops.send_before = 0;				   /* 发送数据前的回调函数 */
	g_uart5.uart_ops.recive_new  = 0;				   /* 接收到新数据后的回调函数 */
	g_uart5.uart_ops.send_over   = 0;				   /* 发送完毕后的回调函数 */
#endif
}

/******************************************************************************
 * @brief  配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-F4开发板
 * 
 * @return none
 * 
******************************************************************************/
static void uart_lowlevel_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

#if UART1_FIFO_EN == 1		/* 串口1 TX = PA9   RX = PA10 或 TX = PB6   RX = PB7 */

	/* 第1步： 配置GPIO */
	#if 1	/* TX = PA9   RX = PA10 */
		/* 打开 GPIO 时钟 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* 打开 UART 时钟 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		/* 将 PA9 映射为 USART1_TX */
		GPIO_PinAFConfig(UART1_TX_PORT, UART1_TX_SOURCE, GPIO_AF_USART1);

		/* 将 PA10 映射为 USART1_RX */
		GPIO_PinAFConfig(UART1_TX_PORT, UART1_RX_SOURCE, GPIO_AF_USART1);

		/* 配置 USART Tx/Rx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;	/* 复用模式 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStructure.GPIO_Pin = UART1_TX_PIN;
		GPIO_Init(UART1_TX_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = UART1_RX_PIN;
		GPIO_Init(UART1_RX_PORT, &GPIO_InitStructure);
	#else	/* TX = PB6   RX = PB7  */
		/* 打开 GPIO 时钟 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		/* 打开 UART 时钟 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		/* 将 PB6 映射为 USART1_TX */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

		/* 将 PB7 映射为 USART1_RX */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;	/* 复用模式 */

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* 配置 USART Rx 为复用功能 */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif

	/* 第2步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate   	      = UART1_BAUD;
	USART_InitStructure.USART_WordLength 	      = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   	      = USART_StopBits_1;
	USART_InitStructure.USART_Parity     	      = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 				  = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(USART1, ENABLE);		/* 使能串口 */

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif

#if UART2_FIFO_EN == 1		/* 串口2 TX = PD5   RX = PD6 或  TX = PA2， RX = PA3  */
	/* 第1步： 配置GPIO */
	#if 0	/* 串口2 TX = PD5   RX = PD6 */
		/* 打开 GPIO 时钟 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		/* 打开 UART 时钟 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		/* 将 PD5 映射为 USART2_TX */
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);

		/* 将 PD6 映射为 USART2_RX */
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* 配置 USART Rx 为复用功能 */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

	#else	/* 串口2   TX = PA2， RX = PA3 */
		/* 打开 GPIO 时钟 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* 打开 UART 时钟 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		/* 将 PA2 映射为 USART2_TX */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

		/* 将 PA3 映射为 USART2_RX */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;	/* 复用模式 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* 配置 USART Rx 为复用功能 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif

	/* 第2步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate 		      = UART2_BAUD;	
	USART_InitStructure.USART_WordLength 		  = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 			  = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(USART2, ENABLE);		/* 使能串口 */

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif

#if UART3_FIFO_EN == 1			/* 串口3 TX = PB10   RX = PB11 */

	/* 配置 PB2为推挽输出，用于切换 RS485芯片的收发状态 */
	#if 0
	{
		RCC_AHB1PeriphClockCmd(RCC_RS485_TXEN, ENABLE);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */

		GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
		GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
	}
	#endif
	/* 打开 GPIO 时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* 打开 UART 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* 将 PB10 映射为 USART3_TX */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);

	/* 将 PB11 映射为 USART3_RX */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	/* 配置 USART Tx 为复用功能 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

	/* 配置 USART Tx 为复用功能 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 配置 USART Rx 为复用功能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 第2步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate  		  = UART3_BAUD;
	USART_InitStructure.USART_WordLength 		  = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 			  = USART_StopBits_1;
	USART_InitStructure.USART_Parity 		      = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 				  = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(USART3, ENABLE);		/* 使能串口 */

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif

#if UART4_FIFO_EN == 1			/* 串口4 TX = PC10   RX = PC11 */
	/* 第1步： 配置GPIO */

	/* 打开 GPIO 时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* 打开 UART 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	/* 将 PC10 映射为 UART4_TX */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART1);

	/* 将 PC11 映射为 UART4_RX */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART1);

	/* 配置 USART Tx 为复用功能 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;	/* 复用模式 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* 配置 USART Rx 为复用功能 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* 第2步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate 			  = UART1_BAUD;
	USART_InitStructure.USART_WordLength 		  = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 			  = USART_StopBits_1;
	USART_InitStructure.USART_Parity 			  = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode   		      = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);

	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(UART4, ENABLE);		/* 使能串口 */

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(UART4, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif

#if UART5_FIFO_EN == 1			/* 串口5 TX = PC12   RX = PD2 */
	/* 第1步： 配置GPIO */

	/* 打开 GPIO 时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);

	/* 打开 UART 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	/* 将 PC12 映射为 UART5_TX */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);

	/* 将 PD2 映射为 UART5_RX */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

	/* 配置 UART Tx 为复用功能 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;	/* 复用模式 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* 配置 UART Rx 为复用功能 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* 第2步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate 			  = UART5_BAUD;	/* 波特率 */
	USART_InitStructure.USART_WordLength 		  = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 			  = USART_StopBits_1;
	USART_InitStructure.USART_Parity 			  = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 				  = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);

	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(UART5, ENABLE);		/* 使能串口 */

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(UART5, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif
}

/******************************************************************************
 * @brief  配置串口硬件中断
 * 
 * @return none
 * 
******************************************************************************/
static void uart_irq_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

#if UART1_FIFO_EN == 1
	/* 使能串口1中断 */
	NVIC_InitStructure.NVIC_IRQChannel  	      = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 	      = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART2_FIFO_EN == 1
	/* 使能串口2中断 */
	NVIC_InitStructure.NVIC_IRQChannel 			  = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd		  = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART3_FIFO_EN == 1
	/* 使能串口3中断t */
	NVIC_InitStructure.NVIC_IRQChannel 			  = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd 		  = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART4_FIFO_EN == 1
	/* 使能串口4中断t */
	NVIC_InitStructure.NVIC_IRQChannel 			  = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd 		  = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART5_FIFO_EN == 1
	/* 使能串口5中断t */
	NVIC_InitStructure.NVIC_IRQChannel 			  = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd 		  = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

/******************************************************************************
 * @brief     填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
 * 
 * @param[in]  _uart    :    uart1~5 
 * @param[in]  _buf     :    待发送的数据缓冲区
 * @param[in]  _size    :    待发送的数据长度
 * 
 * @return     none
 * 
******************************************************************************/
static void uart_send(uart_t *_uart, uint8_t *_buf, uint16_t _size)
{
	uint16_t i;

	for (i = 0; i < _size; ++i)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
	#if 0
		/*
			在调试GPRS例程时，下面的代码出现死机，while 死循环
			原因： 发送第1个字节时 _pUart->usTxWrite = 1；_pUart->usTxRead = 0;
			将导致while(1) 无法退出
		*/
		while (1)
		{
			uint16_t usRead;

			DISABLE_INT();
			usRead = _pUart->usTxRead;
			ENABLE_INT();

			if (++usRead >= _pUart->usTxBufSize)
			{
				usRead = 0;
			}

			if (usRead != _pUart->usTxWrite)
			{
				break;
			}
		}
	#else
		/* 当 _pUart->usTxBufSize == 1 时, 下面的函数会死掉(待完善) */
		while (1)
		{
			__IO uint16_t count;

			DISABLE_INT();
			count = _uart->ctrl_tx.tx_count;
			ENABLE_INT();

			if (count < _uart->ctrl_tx.tx_buf_size)
			{
				break;
			}
		}
	#endif

		/* 将新数据填入发送缓冲区 */
		_uart->ctrl_tx.tx_buf[_uart->ctrl_tx.tx_write_idx] = _buf[i];

		DISABLE_INT();
		if (++_uart->ctrl_tx.tx_write_idx >= _uart->ctrl_tx.tx_buf_size)
		{
			_uart->ctrl_tx.tx_write_idx = 0;
		}
		++_uart->ctrl_tx.tx_count;
		ENABLE_INT();
	}

	USART_ITConfig(_uart->uart, USART_IT_TXE, ENABLE);
}

/******************************************************************************
 * @brief     从串口接收缓冲区读取1字节数据 （用于主程序调用）
 * 
 * @param[in]  _uart    :   uart1~5
 * @param[in]  _byte    :   存放读取数据的指针
 * 
 * @return     uint8_t   :   0 表示无数据  1表示读取到数据
 * 
******************************************************************************/
static uint8_t uart_get_char(uart_t *_uart, uint8_t *_byte)
{
	uint16_t count;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	DISABLE_INT();
	count = _uart->ctrl_rx.rx_count;
	ENABLE_INT();

	/* 如果读和写索引相同，则返回0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (count == 0)	/* 已经没有数据 */
	{
		return 0;
	}
	else
	{
		*_byte = _uart->ctrl_rx.rx_buf[_uart->ctrl_rx.rx_read_idx];		/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
		DISABLE_INT();
		if (++_uart->ctrl_rx.rx_read_idx >= _uart->ctrl_rx.rx_buf_size)
		{
			_uart->ctrl_rx.rx_read_idx = 0;
		}
		--_uart->ctrl_rx.rx_count;
		ENABLE_INT();
	
		return 1;
	}
}

/******************************************************************************
 * @brief     供中断服务程序调用，通用串口中断处理函数
 * 
 * @param[in]  _uart    :    uart1~5
 * 
 * @return     none
 * 
******************************************************************************/
static void uart_irq_handler(uart_t *_uart)
{
	/* 处理接收中断  */
	if (USART_GetITStatus(_uart->uart, USART_IT_RXNE) != RESET)
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		uint8_t ch;

		ch = USART_ReceiveData(_uart->uart);

		_uart->ctrl_rx.rx_buf[_uart->ctrl_rx.rx_write_idx] = ch;
		if (++_uart->ctrl_rx.rx_write_idx >= _uart->ctrl_rx.rx_buf_size)
		{
			_uart->ctrl_rx.rx_write_idx = 0;
		}
		if (_uart->ctrl_rx.rx_count < _uart->ctrl_rx.rx_buf_size)
		{
			++_uart->ctrl_rx.rx_count;
		}

		/* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_uart->uart_ops.recive_new)
			{
				_uart->uart_ops.recive_new(ch);
			}
		}

		//printf("处理接收中断\r\n");
	}

	/* 处理发送缓冲区空中断 */
	if (USART_GetITStatus(_uart->uart, USART_IT_TXE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_uart->ctrl_tx.tx_count == 0)
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			USART_ITConfig(_uart->uart, USART_IT_TXE, DISABLE);

			/* 使能数据发送完毕中断 */
			USART_ITConfig(_uart->uart, USART_IT_TC, ENABLE);
		}
		else
		{
			/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
			USART_SendData(_uart->uart, _uart->ctrl_tx.tx_buf[_uart->ctrl_tx.tx_read_idx]);
			if (++_uart->ctrl_tx.tx_read_idx >= _uart->ctrl_tx.tx_buf_size)
			{
				_uart->ctrl_tx.tx_read_idx = 0;
			}
			--_uart->ctrl_tx.tx_count;
		}

		//printf("处理发送缓冲区空中断\r\n");
	}
	/* 数据bit位全部发送完毕的中断 */
	else if (USART_GetITStatus(_uart->uart, USART_IT_TC) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_uart->ctrl_tx.tx_count == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			USART_ITConfig(_uart->uart, USART_IT_TC, DISABLE);

			/* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
			if (_uart->uart_ops.send_over)
			{
				_uart->uart_ops.send_over();
			}
		}
		else
		{
			/* 正常情况下，不会进入此分支 */

			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			USART_SendData(_uart->uart, _uart->ctrl_tx.tx_buf[_uart->ctrl_tx.tx_read_idx]);
			if (++_uart->ctrl_tx.tx_read_idx >= _uart->ctrl_tx.tx_buf_size)
			{
				_uart->ctrl_tx.tx_read_idx = 0;
			}
			_uart->ctrl_tx.tx_count--;
		}

		//printf("数据bit位全部发送完毕的中断\r\n");
	}
}

/******************************************************************************
 * @brief  USART中断服务程序
 * 
 * @return none
 * 
******************************************************************************/
#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
	uart_irq_handler(&g_uart1);
}
#endif

#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
	uart_irq_handler(&g_uart2);
}
#endif

#if UART3_FIFO_EN == 1
void USART3_IRQHandler(void)
{
	uart_irq_handler(&g_uart3);
}
#endif

#if UART4_FIFO_EN == 1
void UART4_IRQHandler(void)
{
	uart_irq_handler(&g_uart4);
}
#endif

#if UART5_FIFO_EN == 1
void UART5_IRQHandler(void)
{
	uart_irq_handler(&g_uart5);
}
#endif

/******************************************************************************
 * @brief      重定义putc函数，这样可以使用printf函数从串口1打印输出
 * 
 * @param[in]  ch    :  
 * @param[in]  f    :  
 * 
 * @return int 
 * 
******************************************************************************/
#if 0
int fputc(int ch, FILE *f)
{
#if 1	/* 将需要printf的字符通过串口中断FIFO发送出去，printf函数会立即返回 */
	com_send_char(COM1, ch);

	return ch;
#else	/* 采用阻塞方式发送每个字符,等待数据发送完毕 */
	/* 写一个字节到USART1 */
	USART_SendData(USART1, (uint8_t) ch);

	/* 等待发送结束 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
#endif
}
#endif

/******************************************************************************
 * @brief      重定义getc函数，这样可以使用getchar函数从串口1输入数据
 * 
 * @param[in]  f    :  
 * 
 * @return int 
 * 
******************************************************************************/
int fgetc(FILE *f)
{

#if 1	/* 从串口接收FIFO中取1个数据, 只有取到数据才返回 */
	uint8_t data;

	while(com_get_char(COM1, &data) == 0);

	return data;
#else
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART1);
#endif
}

