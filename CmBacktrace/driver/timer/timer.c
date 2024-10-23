/******************************************************************************
 * @file timer.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  配置systick定时器作为系统滴答定时器。缺省定时周期为1ms。
 *         实现了多个软件定时器供主程序使用(精度1ms)， 可以通过修改 TMR_COUNT 增减定时器个数
 *		   实现了ms级别延迟函数（精度1ms） 和us级延迟函数
 *		   实现了系统运行时间函数（1ms单位）
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "timer.h"         
#include "user_lib.h"


/**
 *	定义用于硬件定时器的TIM， 可以使 TIM2 - TIM5
 *	TIM3 和 TIM4 是16位
 *	TIM2 和 TIM5 是32位 （使用时，还存在BUG）
 */
#define USE_TIM2
//#define USE_TIM3
//#define USE_TIM4
//#define USE_TIM5

#ifdef USE_TIM2
	#define TIM_HARD		TIM2
	#define TIM_HARD_IRQn	TIM2_IRQn
	#define TIM_HARD_RCC	RCC_APB1Periph_TIM2
#endif

#ifdef USE_TIM3
	#define TIM_HARD		TIM3
	#define TIM_HARD_IRQn	TIM3_IRQn
	#define TIM_HARD_RCC	RCC_APB1Periph_TIM3
#endif

#ifdef USE_TIM4
	#define TIM_HARD		TIM4
	#define TIM_HARD_IRQn	TIM4_IRQn
	#define TIM_HARD_RCC	RCC_APB1Periph_TIM4
#endif

#ifdef USE_TIM5
	#define TIM_HARD		TIM5
	#define TIM_HARD_IRQn	TIM5_IRQn
	#define TIM_HARD_RCC	RCC_APB1Periph_TIM5
#endif


/* 这2个全局变量转用于 bsp_DelayMS() 函数 */
static volatile uint32_t s_delay_count = 0;
static volatile uint8_t s_timeout_flag = 0;

/* 定于软件定时器结构体变量 */
static soft_timer_t s_timer[TMR_COUNT];

/*
	全局运行时间，单位1ms
	最长可以表示 24.85天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题
*/
__IO int32_t g_run_time = 0;

static void bsp_soft_timer_dec(soft_timer_t *_tmr);

/* 保存 TIM定时中断到后执行的回调函数指针 */
static void (*s_timer_callback1)(void);
static void (*s_timer_callback2)(void);
static void (*s_timer_callback3)(void);
static void (*s_timer_callback4)(void);


/******************************************************************************
 * @brief  配置systick中断，并初始化软件定时器变量
 * 
 * @return none
 * 
******************************************************************************/
void bsp_timer_init(void)
{
	uint8_t i;

	/* 清零所有的软件定时器 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_timer[i].count = 0;
		s_timer[i].preload = 0;
		s_timer[i].flag = 0;
		s_timer[i].mode = TMR_ONCE_MODE;	/* 缺省是1次性工作模式 */
	}

	SystemCoreClockUpdate();	/* 根据PLL配置更新系统时钟频率变量 SystemCoreClock */
	
	/**
	 *	配置systic中断周期为1ms，并启动systick中断。
	 *
     *	SystemCoreClock 是固件中定义的系统内核时钟，对于STM32F4XX,一般为 168MHz
	 *
     *	SysTick_Config() 函数的形参表示内核时钟多少个周期后触发一次Systick定时中断.
	 *    	-- SystemCoreClock / 1000  表示定时频率为 1000Hz， 也就是定时周期为  1ms
	 *   	-- SystemCoreClock / 500   表示定时频率为 500Hz，  也就是定时周期为  2ms
	 *   	-- SystemCoreClock / 2000  表示定时频率为 2000Hz， 也就是定时周期为  500us
	 * 
     *	对于常规的应用，我们一般取定时周期1ms。对于低速CPU或者低功耗应用，可以设置定时周期为 10ms
     */
	SysTick_Config(SystemCoreClock / 1000);
	
	bsp_hard_timer_init();
}

/******************************************************************************
 * @brief  SysTick中断服务程序，每隔1ms进入1次
 * 
 * @return none
 * 
******************************************************************************/
//extern void bsp_RunPer1ms(void);
//extern void bsp_RunPer10ms(void);
void SysTick_ISR(void)
{
	static uint8_t s_count = 0;
	uint8_t i;

	/* 每隔1ms进来1次 （仅用于 bsp_DelayMS） */
	if (s_delay_count > 0)
	{
		if (--s_delay_count == 0)
		{
			s_timeout_flag = 1;
		}
	}

	/* 每隔1ms，对软件定时器的计数器进行减一操作 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		bsp_soft_timer_dec(&s_timer[i]);
	}

	/* 全局运行时间每1ms增1 */
	g_run_time++;
	if (g_run_time == 0x7FFFFFFF)	/* 这个变量是 int32_t 类型，最大数为 0x7FFFFFFF */
	{
		g_run_time = 0;
	}

	//bsp_RunPer1ms();		/* 每隔1ms调用一次此函数，此函数在 bsp.c */

	if (++s_count >= 10)
	{
		s_count = 0;

		//bsp_RunPer10ms();	/* 每隔10ms调用一次此函数，此函数在 bsp.c */
	}
}

/******************************************************************************
 * @brief      每隔1ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
 * 
 * @param[in]  _tmr    :    定时器变量指针
 * 
 * @return     none
 * 
******************************************************************************/
static void bsp_soft_timer_dec(soft_timer_t *_tmr)
{
	if (_tmr->count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--_tmr->count == 0)
		{
			_tmr->flag = 1;

			/* 如果是自动模式，则自动重装计数器 */
			if(_tmr->mode == TMR_AUTO_MODE)
			{
				_tmr->count = _tmr->preload;
			}
		}
	}
}

/******************************************************************************
 * @brief      ms级延迟，延迟精度为正负1ms
 * 
 * @param[in]  n    :     延迟长度，单位1 ms。 n 应大于2
 * 
 * @return     none
 * 
******************************************************************************/
void bsp_delay_ms(uint32_t n)
{
	if (n == 0)
	{
		return;
	}
	else if (n == 1)
	{
		n = 2;
	}

	DISABLE_INT();  			/* 关中断 */

	s_delay_count = n;
	s_timeout_flag = 0;

	ENABLE_INT();  				/* 开中断 */

	while (1)
	{
		bsp_Idle();				/* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

		/*
			等待延迟时间到
			注意：编译器认为 s_ucTimeOutFlag = 0，所以可能优化错误，因此 s_ucTimeOutFlag 变量必须申明为 volatile
		*/
		if (s_timeout_flag == 1)
		{
			break;
		}
	}
}

/******************************************************************************
 * @brief      us级延迟。 必须在systick定时器启动后才能调用此函数。
 * 
 * @param[in]  n    :    延迟长度，单位1 us
 * 
 * @return     none
 * 
******************************************************************************/
void bsp_delay_us(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
	reload = SysTick->LOAD;                
    ticks = n * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* 刚进入时的计数器值 */

    while (1)
    {
        tnow = SysTick->VAL;    
        if (tnow != told)
        {    
            /* SYSTICK是一个递减的计数器 */    
            if (tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* 重新装载递减 */
            else
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;

            /* 时间超过/等于要延迟的时间,则退出 */
            if (tcnt >= ticks)
            {
            	break;
            }
        }  
    }
} 

/******************************************************************************
 * @brief      启动一个定时器，并设置定时周期。
 * 
 * @param[in]  _id       :    定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
 * @param[in]  _period   :    定时周期，单位1ms
 * 
 * @return     none
 * 
******************************************************************************/
void bsp_timer_start(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  			/* 关中断 */

	s_timer[_id].count = _period;		/* 实时计数器初值 */
	s_timer[_id].preload = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_timer[_id].flag = 0;				/* 定时时间到标志 */
	s_timer[_id].mode = TMR_ONCE_MODE;	/* 1次性工作模式 */

	ENABLE_INT();  				/* 开中断 */
}

/******************************************************************************
 * @brief      启动一个自动定时器，并设置定时周期。
 * 
 * @param[in]  _id       :   定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
 * @param[in]  _period   :   定时周期，单位10ms
 * 
 * @return     none
 * 
******************************************************************************/
void bsp_timer_start_auto(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  		/* 关中断 */

	s_timer[_id].count = _period;	    /* 实时计数器初值 */
	s_timer[_id].preload = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_timer[_id].flag = 0;				/* 定时时间到标志 */
	s_timer[_id].mode = TMR_AUTO_MODE;	/* 自动工作模式 */

	ENABLE_INT();  			/* 开中断 */
}

/******************************************************************************
 * @brief      停止一个定时器
 * 
 * @param[in]  _id    :   定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
 * 
 * @return     none
 * 
******************************************************************************/
void bsp_timer_stop(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  	/* 关中断 */

	s_timer[_id].count = 0;				/* 实时计数器初值 */
	s_timer[_id].flag = 0;				/* 定时时间到标志 */
	s_timer[_id].mode = TMR_ONCE_MODE;	/* 自动工作模式 */

	ENABLE_INT();  		/* 开中断 */
}

/******************************************************************************
 * @brief      检测定时器是否超时
 * 
 * @param[in]  _id    :   定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
 * 
 * @return    uint8_t :   返回 0 表示定时未到， 1表示定时到
 * 
******************************************************************************/
uint8_t bsp_timer_check(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (s_timer[_id].flag == 1)
	{
		s_timer[_id].flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************
 * @brief  获取CPU运行时间，单位1ms。最长可以表示 24.85天
 * 
 * @return int32_t  :  CPU运行时间，单位1ms
 * 
 * @note   如果你的产品连续运行时间超过 24.85 天，则必须考虑溢出问题
 *
******************************************************************************/
int32_t bsp_get_run_time(void)
{
	int32_t runtime;

	DISABLE_INT();  	/* 关中断 */

	runtime = g_run_time;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

	ENABLE_INT();  		/* 开中断 */

	return runtime;
}

/******************************************************************************
 * @brief      计算当前运行时间和给定时刻之间的差值。处理了计数器循环。
 * 
 * @param[in]  _LastTime    :   上个时刻
 * 
 * @return     int32_t      :   当前时间和过去时间的差值，单位1ms
 * 
******************************************************************************/
int32_t bsp_check_run_time(int32_t _LastTime)
{
	int32_t now_time;
	int32_t time_diff;

	DISABLE_INT();  	/* 关中断 */

	now_time = g_run_time;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

	ENABLE_INT();  		/* 开中断 */
	
	if (now_time >= _LastTime)
	{
		time_diff = now_time - _LastTime;
	}
	else
	{
		time_diff = 0x7FFFFFFF - _LastTime + now_time;
	}

	return time_diff;
}

/******************************************************************************
 * @brief  系统嘀嗒定时器中断服务程序。启动文件中引用了该函数。
 * 
 * @return none
 * 
******************************************************************************/
void SysTick_Handler(void)
{
	SysTick_ISR();
}

/******************************************************************************
 * @brief  配置 TIMx，用于us级别硬件定时。TIMx将自由运行，永不停止.
 *		   TIMx可以用TIM2 - TIM5 之间的TIM, 这些TIM有4个通道, 挂在 APB1 上，输入时钟=SystemCoreClock / 2
 * 
 * @return none
 * 
******************************************************************************/
void bsp_hard_timer_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint32_t period;
	uint16_t prescaler;
	uint32_t timx_clk;

  	/* 使能TIM时钟 */
	RCC_APB1PeriphClockCmd(TIM_HARD_RCC, ENABLE);

    /*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	timx_clk = SystemCoreClock / 2;  /* 结果为 84000000 */

	prescaler = timx_clk / 1000000 ;	/* 分频到周期 1us, 结果为 84 */
	//prescaler = 8399;
	
#if defined (USE_TIM2) || defined (USE_TIM5) 
	period = 0xFFFFFFFF; // 9999;
#else
	period = 0xFFFF;
#endif
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM_HARD, &TIM_TimeBaseStructure);

	//TIM_ARRPreloadConfig(TIMx, ENABLE);

	/* TIMx enable counter */
	TIM_Cmd(TIM_HARD, ENABLE);

	/* 配置TIM定时中断 (Update) */
	{
		NVIC_InitTypeDef NVIC_InitStructure;	/* 中断结构体在 misc.h 中定义 */

		NVIC_InitStructure.NVIC_IRQChannel = TIM_HARD_IRQn;

		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	/* 比串口优先级低 */
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
}

/******************************************************************************
 * @brief     使用TIM2-5做单次定时器使用, 定时时间到后执行回调函数。可以同时启动4个定时器，互不干扰。
 *            定时精度正负10us （主要耗费在调用本函数的执行时间，函数内部进行了补偿减小误差）
 *			  TIM2和TIM5 是32位定时器。定时范围很大
 *			  TIM3和TIM4 是16位定时器。
 * 
 * @param[in]  _CC          :    捕获通道，1，2，3, 4
 * @param[in]  _uiTimeOut   :    超时时间, 单位 1us.对于16位定时器，最大 65.5ms; 对于32位定时器，最大 4294秒
 * @param[in]  _pCallBack   :    定时时间到后，被执行的函数
 * 
 * @return     none
 * 
******************************************************************************/
void bsp_hard_timer_start(uint8_t _cc, uint32_t _timeout, void * _callback)
{
    uint32_t cnt_now;
    uint32_t cnt_tar;

    /**
     *   执行下面这个语句，时长 = 18us (通过逻辑分析仪测量IO翻转)
     *   bsp_StartTimer2(3, 500, (void *)test1);
     */
    if (_timeout < 5)
    {
        ;
    }
    else
    {
        _timeout -= 5;
    }

    cnt_now = TIM_GetCounter(TIM_HARD);    	/* 读取当前的计数器值 */
    cnt_tar = cnt_now + _timeout;			/* 计算捕获的计数器值 */
    if (_cc == 1)
    {
        s_timer_callback1 = (void (*)(void))_callback;

        TIM_SetCompare1(TIM_HARD, cnt_tar);      	/* 设置捕获比较计数器CC1 */
        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC1);
		TIM_ITConfig(TIM_HARD, TIM_IT_CC1, ENABLE);	/* 使能CC1中断 */

    }
    else if (_cc == 2)
    {
		s_timer_callback2 = (void (*)(void))_callback;

        TIM_SetCompare2(TIM_HARD, cnt_tar);      	/* 设置捕获比较计数器CC2 */
		TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC2);
		TIM_ITConfig(TIM_HARD, TIM_IT_CC2, ENABLE);	/* 使能CC2中断 */
    }
    else if (_cc == 3)
    {
        s_timer_callback3 = (void (*)(void))_callback;

        TIM_SetCompare3(TIM_HARD, cnt_tar);      	/* 设置捕获比较计数器CC3 */
        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC3);
		TIM_ITConfig(TIM_HARD, TIM_IT_CC3, ENABLE);	/* 使能CC3中断 */
    }
    else if (_cc == 4)
    {
        s_timer_callback4 = (void (*)(void))_callback;

        TIM_SetCompare4(TIM_HARD, cnt_tar);      	/* 设置捕获比较计数器CC4 */
		TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC4);
		TIM_ITConfig(TIM_HARD, TIM_IT_CC4, ENABLE);	/* 使能CC4中断 */
    }
	else
    {
        return;
    }
}

/******************************************************************************
 * @brief  TIM 中断服务程序
 * 
 * @return none
 * 
******************************************************************************/
#ifdef USE_TIM2
void TIM2_IRQHandler(void)
#endif

#ifdef USE_TIM3
void TIM3_IRQHandler(void)
#endif

#ifdef USE_TIM4
void TIM4_IRQHandler(void)
#endif

#ifdef USE_TIM5
void TIM5_IRQHandler(void)
#endif
{
    if (TIM_GetITStatus(TIM_HARD, TIM_IT_CC1))
    {
        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC1);
        TIM_ITConfig(TIM_HARD, TIM_IT_CC1, DISABLE);	/* 禁能CC1中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_timer_callback1();
    }

    if (TIM_GetITStatus(TIM_HARD, TIM_IT_CC2))
    {
        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC2);
        TIM_ITConfig(TIM_HARD, TIM_IT_CC2, DISABLE);	/* 禁能CC2中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_timer_callback2();
    }

    if (TIM_GetITStatus(TIM_HARD, TIM_IT_CC3))
    {
        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC3);
        TIM_ITConfig(TIM_HARD, TIM_IT_CC3, DISABLE);	/* 禁能CC3中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_timer_callback3();
    }

    if (TIM_GetITStatus(TIM_HARD, TIM_IT_CC4))
    {
        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC4);
        TIM_ITConfig(TIM_HARD, TIM_IT_CC4, DISABLE);	/* 禁能CC4中断 */

        /* 先关闭中断，再执行回调函数。因为回调函数可能需要重启定时器 */
        s_timer_callback4();
    }
}
