#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "button.h"
#include "uart_fifo.h"
#include "timer.h"
#include "modbus_master.h"
#include "msg.h"


extern void board_lowlevel_init(void);

PRINT_MODS_T g_tPrint;

static void DispMenu(void)
{	
	uint8_t i;
	
	printf("\n\r");
	printf("\33[K");						/* 清除从光标到行尾的内容 */ 
	
	printf(" 发送的命令 : 0x");				/* 打印发送命令 */
	for (i = 0; i < g_tPrint.Txlen; i++)
	{
		printf(" %02X", g_tPrint.TxBuf[i]);
	}

	printf("\n\r");
	printf("\33[K");						/* 清除从光标到行尾的内容 */ 
	
	printf(" 接收的命令 : 0x");				/* 打印接收命令 */
	for (i = 0; i < g_tPrint.Rxlen; i++)
	{
		printf(" %02X", g_tPrint.RxBuf[i]);
	}
	
	printf("\n\r");
	printf("\33[3A");						/* 光标上移3行 */
}

int main(void)
{
	board_lowlevel_init();

	bl_led_init();
	bl_button_init();
	bsp_InitUart();

	bsp_InitTimer();	/* 初始化系统滴答定时器 (此函数会开 systick 中断, tim2-5中断) */
	
	printf("Test Timer2: \r\n");
	
	//bsp_StartHardTimer(3, 1000, (void *)callback);
	
	uint8_t read;
	MSG_T ucMsg;
	
	while (1)
	{
		MODH_Poll();
		
		if (bsp_GetMsg(&ucMsg))
		{
			switch(ucMsg.MsgCode)
			{
				case MSG_MODS:
					DispMenu();
					break;
				default:
					break;
			}
		}
		
		if (comGetChar(COM1, &read))
		{
			bsp_PutMsg(MSG_MODS, 0);
			
			switch(read)
			{
				case '1': 
					if (MODH_ReadParam_01H(REG_D01, 4) == 1) ;else ;
					break;
				case '2':
					if (MODH_ReadParam_03H(REG_P01, 2) == 1) ;else ;
					break;
				case '3':
					{
						uint8_t buf[4];
						
						buf[0] = 0x01;
						buf[1] = 0x02;
						buf[2] = 0x03;
						buf[3] = 0x04;
						if (MODH_WriteParam_10H(REG_P01, 2, buf) == 1) ;else ;
					}
					break;
				case '4':
					if (MODH_WriteParam_06H(REG_P01, 1) == 1) ;else ;
					break;
				case '5':
					if (MODH_WriteParam_05H(REG_D01, 1) == 1) ;else ;
					break;
				case '6':
					if (MODH_WriteParam_05H(REG_D01, 0) == 1) ;else ;
					break;
				case '7':
					if (MODH_ReadParam_02H(REG_T01, 3) == 1) ;else ;
					break;
				case '8':
					if (MODH_ReadParam_04H(REG_A01, 1) == 1) ;else ;	
					break;
				default:
					printf("Error Code!\r\n");
					break;
			}
		}
	}
	
	return 0;
}
