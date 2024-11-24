/******************************************************************************
 * @file user_lib.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  This file contains the function prototypes of the user library.
 * @version 0.1
 * @date 2024-10-14  移植了 segger 和 easylogger 两个组件以及带有 FIFO 的 UART
 * 
 * @copyright Copyright (c) 2024
 *
 * @note      使用 easylogger 时，要注意在 Project->Options for Target->C/C++
 *            ->Preprocessor Symbols->Define 添加 DEBUG，否则不会打印结果
 *            要注意，log_x 不能和 printf 同时使用
 * 
******************************************************************************/
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
#include "SEGGER_RTT.h"

extern void board_lowlevel_init(void);

volatile int _Cnt;
volatile int _Delay;
__IO static uint8_t test_state = 0;


static void test1()
{
	int r;
  int CancelOp;

  do {
    _Cnt = 0;

    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
    SEGGER_RTT_WriteString(0, "Press <1> to continue in blocking mode (Application waits if necessary, no data lost)\r\n");
    SEGGER_RTT_WriteString(0, "Press <2> to continue in non-blocking mode (Application does not wait, data lost if fifo full)\r\n");
    do {
      r = SEGGER_RTT_WaitKey();
    } while ((r != '1') && (r != '2'));
    if (r == '1') {
      SEGGER_RTT_WriteString(0, "\r\nSelected <1>. Configuring RTT and starting...\r\n");
      SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    } else {
      SEGGER_RTT_WriteString(0, "\r\nSelected <2>. Configuring RTT and starting...\r\n");
      SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    }
    CancelOp = 0;
    do {
      //for (_Delay = 0; _Delay < 10000; _Delay++);
      SEGGER_RTT_printf(0, "Count: %d. Press <Space> to get back to menu.\r\n", _Cnt++);
      r = SEGGER_RTT_HasKey();
      if (r) {
        CancelOp = (SEGGER_RTT_GetKey() == ' ') ? 1 : 0;
      }
      //
      // Check if user selected to cancel the current operation
      //
      if (CancelOp) {
        SEGGER_RTT_WriteString(0, "Operation cancelled, going back to menu...\r\n");
        break;
      }
    } while (1);
    SEGGER_RTT_GetKey();
    SEGGER_RTT_WriteString(0, "\r\n");
  } while (1);
}

int main(void)
{
	board_lowlevel_init();
	bsp_uart_init();
	
	bsp_led_init();
	bsp_button_init();
	
	bsp_timer_init();

#if 0
	test1();
#endif
	
#if 0
	uint8_t ucKeyCode;
	uint8_t read;
	const char buf1[] = "接收到串口命令1\r\n";
	const char buf2[] = "接收到串口命令2\r\n";
	const char buf3[] = "接收到串口命令3\r\n";
	const char buf4[] = "接收到串口命令4\r\n";
	
	//printf("Test dddsfadffd asg df: \r\n");

	while (1)
	{
		if (com_get_char(COM1, &read))
		{
			switch(read)
			{
				case '1': 
					com_send_buf(COM1, (uint8_t *)buf1, strlen(buf1));
					break;
				case '2':
					com_send_buf(COM1, (uint8_t *)buf2, strlen(buf2));
					break;
				case '3':
					com_send_buf(COM1, (uint8_t *)buf3, strlen(buf3));
					break;
				case '4':
					com_send_buf(COM1, (uint8_t *)buf4, strlen(buf4));
					break;
				default:
					printf("Error Code!\r\n");
					break;
			}
		}
		
		if (bsp_button_pressed())
		{             
			test_state = 1;
			bsp_led_toggle(GPIO_Pin_5);
			SEGGER_RTT_printf(0,"hello world!");
		}
	}
#endif

#if 0
	while(1)
	{
		if (test_state >= 100) test_state = 0;
		bsp_delay_ms(10);
		test_state += 1;
		printf("%d\r\n", test_state);
	}
#endif

#if 0
	while (1)
	{
		SEGGER_RTT_SetTerminal(0);
		SEGGER_RTT_printf(0, RTT_CTRL_TEXT_BRIGHT_CYAN"[CH0]: Hello RTT\r\n");
		SEGGER_RTT_SetTerminal(1);
		SEGGER_RTT_printf(0, RTT_CTRL_TEXT_MAGENTA"[CH1]: Hello RTT\r\n");
		SEGGER_RTT_SetTerminal(2);
		SEGGER_RTT_printf(0, RTT_CTRL_BG_BRIGHT_BLUE"[CH2]: Hello RTT\r\n");
		bsp_delay_ms(1000);
	}
#endif

#if 1
	while (1)
	{
		printf("Hello RTT\r\n");
		bsp_delay_ms(100);
	}
#endif
}

int fputc(int ch, FILE *f)
{ 	
    SEGGER_RTT_PutChar(0, ch);
	return ch;
}
