/******************************************************************************
 * @file timer.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  定时器模块(通用定时器 TIM2~TIM5、SysTick)
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>


/*
	在此定义若干个软件定时器全局变量
	注意，必须增加__IO 即 volatile，因为这个变量在中断和主程序中同时被访问，有可能造成编译器错误优化。
*/
#define TMR_COUNT	4		/* 软件定时器的个数 （定时器ID范围 0 - 3) */

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum
{
	TMR_ONCE_MODE = 0,  /* 一次工作模式 */
	TMR_AUTO_MODE = 1	/* 自动定时工作模式 */
} timer_mode_t;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
	volatile uint8_t mode;		/* 计数器模式，1次性 */
	volatile uint8_t flag;		/* 定时到达标志  */
	volatile uint32_t count;	/* 计数器 */
	volatile uint32_t preload;	/* 计数器预装值 */
} soft_timer_t;


void bsp_timer_init(void);
void bsp_timer_start(uint8_t _id, uint32_t _period);
void bsp_timer_start_auto(uint8_t _id, uint32_t _period);
void bsp_timer_stop(uint8_t _id);

void bsp_delay_ms(uint32_t n);
void bsp_delay_us(uint32_t n);

uint8_t bsp_timer_check(uint8_t _id);
int32_t bsp_get_run_time(void);
int32_t bsp_check_run_time(int32_t _LastTime);

void bsp_hard_timer_init(void);
void bsp_hard_timer_start(uint8_t _cc, uint32_t _timout, void * _callback);

#endif /* __TIMER_H */

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
