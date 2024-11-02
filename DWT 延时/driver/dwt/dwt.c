/******************************************************************************
 * @file dwt.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  DWT 驱动实现 
 * @version 0.1
 * @date 2024-11-02
 * @note 在CM3，CM4中可以有3种跟踪源：ETM, ITM 和DWT，本驱动主要实现
 *       DWT中的时钟周期（CYCCNT）计数功能，此功能非常重要，可以很方便的
 *       计算程序执行的时钟周期个数。
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdint.h>
#include "stm32f4xx.h"
#include "dwt.h"


/** 寄存器定义 */
#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
#define  DWT_CR      *(volatile unsigned int *)0xE0001000
#define  DEM_CR      *(volatile unsigned int *)0xE000EDFC
#define  DBGMCU_CR   *(volatile unsigned int *)0xE0042004

#define  DEM_CR_TRCENA               (1 << 24)
#define  DWT_CR_CYCCNTENA            (1 <<  0)

/******************************************************************************
 * @brief  初始化 DWT
 * @return none
******************************************************************************/
void bsp_dwt_init(void)
{
    DEM_CR         |= (unsigned int)DEM_CR_TRCENA;   /* Enable Cortex-M4's DWT CYCCNT reg.  */
	DWT_CYCCNT      = (unsigned int)0u;
	DWT_CR         |= (unsigned int)DWT_CR_CYCCNTENA;
}

/******************************************************************************
 * @brief      
 * @param[in]  _delay_time    :    延时时间  
 * @return     none
******************************************************************************/
void bsp_dwt_delay(uint32_t _delay_time)
{
	uint32_t cnt, delay_cnt;
	uint32_t start;
		
	cnt = 0;
	delay_cnt = _delay_time;  /* 需要的节拍数 */ 		      
	start = DWT_CYCCNT;       /* 刚进入时的计数器值 */
	
	while(cnt < delay_cnt)
	{
		cnt = DWT_CYCCNT - start; /* 求减过程中，如果发生第一次32位计数器重新计数，依然可以正确计算 */	
	}
}

/******************************************************************************
 * @brief      这里的延时采用CPU的内部计数实现，32位计数器
 *             OSSchedLock(&err);
 *			   bsp_DelayUS(5);
 *			   OSSchedUnlock(&err); 根据实际情况看看是否需要加调度锁或选择关中断
 * @param[in]  _delay_time    :    延迟长度，单位1 us
 * @return     none
 * @note       1. 主频168MHz的情况下，32位计数器计满是2^32/168000000 = 25.565秒
 *                建议使用本函数做延迟的话，延迟在1秒以下。  
 *             2. 实际通过示波器测试，微妙延迟函数比实际设置实际多运行0.25us左右的时间。
 *             下面数据测试条件：
 *             （1）. MDK5.15，优化等级0, 不同的MDK优化等级对其没有影响。
 *             （2）. STM32F407IGT6
 *             （3）. 测试方法：
 *				 GPIOI->BSRRL = GPIO_Pin_8;
 *				 bsp_DelayUS(10);
 *				 GPIOI->BSRRH = GPIO_Pin_8;
 *             -------------------------------------------
 *                测试                 实际执行
 *             bsp_DelayUS(1)          1.2360us
 *             bsp_DelayUS(2)          2.256us
 *             bsp_DelayUS(3)          3.256us
 *             bsp_DelayUS(4)          4.256us
 *             bsp_DelayUS(5)          5.276us
 *             bsp_DelayUS(6)          6.276us
 *             bsp_DelayUS(7)          7.276us
 *             bsp_DelayUS(8)          8.276us
 *             bsp_DelayUS(9)          9.276us
 *             bsp_DelayUS(10)         10.25us
 *            3. 两个32位无符号数相减，获取的结果再赋值给32位无符号数依然可以正确的获取差值。
 *              假如A,B,C都是32位无符号数。
 *              如果A > B  那么A - B = C，这个很好理解，完全没有问题
 *              如果A < B  那么A - B = C， C的数值就是0xFFFFFFFF - B + A + 1。这一点要特别注意，正好用于本函数。
******************************************************************************/
void bsp_delay_US(uint32_t _delay_time)
{
	uint32_t cnt, delay_cnt;
	uint32_t start;
		
	start = DWT_CYCCNT;                                     /* 刚进入时的计数器值 */
	cnt = 0;
	delay_cnt = _delay_time * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */ 		      

	while(cnt < delay_cnt)
	{
		cnt = DWT_CYCCNT - start; /* 求减过程中，如果发生第一次32位计数器重新计数，依然可以正确计算 */	
	}
}

/******************************************************************************
 * @brief      为了让底层驱动在带RTOS和裸机情况下有更好的兼容性
 *             专门制作一个阻塞式的延迟函数，在底层驱动中ms毫秒延迟主要用于初始化，并不会影响实时性。 
 * @param[in]  _delay_time    :    延迟长度，单位1 ms
 * @return     none
******************************************************************************/
void bsp_delay_MS(uint32_t _delay_time)
{
	bsp_delay_US(1000 * _delay_time);
}
