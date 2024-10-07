#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "button.h"
#include "console_uart.h"

extern void board_lowlevel_init(void);

uint32_t CNT = 0;

int main(void)
{
	SCB->CCR = 0x210;  /* 使能除 0 的错误 */
	
	board_lowlevel_init();
	
	bl_delay_init();
	bl_led_init();
	bl_button_init();
	bl_uart_init();
	
	/* 使能 PendSV 中断 */
	// SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	
	while (1)
	{
		bl_idle();
		
		while ( UART_RX_CNT > CNT)
		{
			printf("UART NUM: %d\r\n", UART_RX_CNT);
			printf("NUM: %d\r\n", CNT);
			switch (UART_RX_BUF_BIN[CNT])
			{   
				/* 非法读取 */
				case '1': 
					{
						volatile unsigned int* p;
						unsigned int n;
						p = (unsigned int*)0xCCCCCCCC;
						n = *p;
						
						(void)n;
					}
					break;
				
				/* 非对齐访问 */
				case '2': 
					{
						float *ptr;
                        float fTest;
                        
                        ptr = (float *)(0x20010002);
                        *ptr = 0.2f;
                        *ptr +=0.1f;
                        
                        fTest = *ptr;
                        (void)fTest;
					}
					break;
				
				/* 非法写入 */
				case '3': 
					{
						volatile unsigned int* p;
                        unsigned int n;
                        p = (unsigned int*)0xCCCCCCCC;
                        *p = 100;
                        
                        (void)n;
					}
					break;
					
				/* 非法函数状态 */
				case '4': 
					{
						typedef void (*t_funcPtr)(void);
                        t_funcPtr MyFunc = (t_funcPtr)(0x0001000 | 0x0); /* LSB不能是0*/                                              
                        MyFunc();
					}
					break; 
                    
                /* 除以0的问题 */
				case '5': 
					{
                        int r;
                        volatile unsigned int a;
                        volatile unsigned int b;
                        
                        a = 1;
                        b = 0;
                        r = a / b;
                        
                        (void)r;
					}
					break;
					
				default: printf("Error Num!\r\n");
			}
			
			CNT += 3;
			break;
		}
	}
}
