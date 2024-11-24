/******************************************************************************
 * @file uart_fifo.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  串口中断+FIFO驱动模块
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "stm32f4xx.h"

/*
	如果需要更改串口对应的管脚，请自行修改 bsp_uart_fifo.c文件中的 static void InitHardUart(void)函数
*/

/* 定义使能的串口, 0 表示不使能（不增加代码大小）， 1表示使能 */
/*
	安富莱STM32-V5 串口分配：
	【串口1】 RS232 芯片第1路。
		PB6/USART1_TX	  --- 打印调试口
		PB7/USART1_RX

	【串口2】 PA2 管脚用于以太网； RX管脚用于接收GPS信号
		PA2/USART2_TX/ETH_MDIO (用于以太网，不做串口发送用)
		PA3/USART2_RX	;接GPS模块输出

	【串口3】 RS485 通信 - TTL 跳线 和 排针
		PB10/USART3_TX
		PB11/USART3_RX

	【串口4】 --- 不做串口用。
	【串口5】 --- 不做串口用。

	【串口6】--- GPRS模块 （硬件流控）
		PC6/USART6_TX
		PC7/USART6_RX
		PG8/USART6_RTS
		PG15/USART6_CTS
*/
#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	0
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0

/* RS485芯片发送使能GPIO, PB2 */
//#define RCC_RS485_TXEN 	RCC_AHB1Periph_GPIOB
//#define PORT_RS485_TXEN  GPIOB
//#define PIN_RS485_TXEN	 GPIO_Pin_2

//#define RS485_RX_EN()	PORT_RS485_TXEN->BSRRH = PIN_RS485_TXEN
//#define RS485_TX_EN()	PORT_RS485_TXEN->BSRRL = PIN_RS485_TXEN

/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1  PA9, PA10 或  PB6, PB7*/
	COM2 = 1,	/* USART2, PD5,PD6 或 PA2, PA3 */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11 */
	COM5 = 4,	/* UART5, PC12, PD2 */
} com_port_t;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD			115200
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD			9600
	#define UART2_TX_BUF_SIZE	1
	#define UART2_RX_BUF_SIZE	2*1024
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			9600
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
	#define RS485
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			115200
	#define UART4_TX_BUF_SIZE	1*1024
	#define UART4_RX_BUF_SIZE	1*1024
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			115200
	#define UART5_TX_BUF_SIZE	1*1024
	#define UART5_RX_BUF_SIZE	1*1024
#endif

typedef struct 
{
	uint8_t *rx_buf;			/* 接收缓冲区 */
	uint16_t rx_buf_size;		/* 接收缓冲区大小 */
	
	/* 必须增加 __IO 修饰,否则优化后会导致串口发送函数死机 */
	__IO uint16_t rx_write_idx;	/* 接收缓冲区写指针 */
	__IO uint16_t rx_read_idx;		/* 接收缓冲区读指针 */
	__IO uint16_t rx_count;	/* 还未读取的新数据个数 */
} uart_rx_t;

typedef struct 
{
	uint8_t *tx_buf;			 /* 发送缓冲区 */
	uint16_t tx_buf_size;		 /* 发送缓冲区大小 */

	/* 必须增加 __IO 修饰,否则优化后会导致串口发送函数死机 */
	__IO uint16_t tx_write_idx;	 /* 发送缓冲区写指针 */
	__IO uint16_t tx_read_idx;   /* 发送缓冲区读指针 */
	__IO uint16_t tx_count;	     /* 等待发送的数据个数 */
} uart_tx_t;

typedef struct
{
	void (*send_before)(void); 			/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*send_over)(void); 			/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*recive_new)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
} uart_operations_t;

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;  	     /* STM32内部串口设备指针 */
	
	uart_rx_t ctrl_rx;  		 /* 接收控制器 */
	uart_tx_t ctrl_tx;   	     /* 发送控制器 */

	uart_operations_t uart_ops;  /* 串口操作函数 */
} uart_t;

void bsp_uart_init(void);
void com_send_buf(com_port_t _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void com_send_char(com_port_t _ucPort, uint8_t _ucByte);
uint8_t com_get_char(com_port_t _ucPort, uint8_t *_pByte);
void com_clear_tx_fifo(com_port_t _ucPort);
void com_clear_rx_fifo(com_port_t _ucPort);
void com_set_baud(com_port_t _ucPort, uint32_t _BaudRate);

void usart_set_baudrate(USART_TypeDef* USARTx, uint32_t BaudRate);

#ifdef RS485
void rs485_send_buf(uint8_t *_ucaBuf, uint16_t _usLen);
void rs485_send_str(char *_pBuf);
void rs485_set_baud(uint32_t _baud);
#endif

#endif /* __USART_FIFO_H */

