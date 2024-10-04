#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "console_uart.h"
#include "sdio_test.h"
#include "button.h"
#include "main.h"

static bool button_trap_boot(void)
{
    if (bl_button_pressed())
    {
        bl_delay_ms(100);  // 消抖
        return bl_button_pressed();
    }

    return false;
}

int main(void)
{
	bl_delay_init();

	/* 初始化LED灯 */
    bl_led_init();
    /* 初始化独立按键 */
    bl_button_init();

    /*初始化USART1*/
    uart_init();

    printf("\r\n欢迎使用野火  STM32 F407 开发板。\r\n");

    printf("在开始进行SD卡基本测试前，请给开发板插入32G以内的SD卡\r\n");
    printf("本程序会对SD卡进行 非文件系统 方式读写，会删除SD卡的文件系统\r\n");
    printf("实验后可通过电脑格式化或使用SD卡文件系统的例程恢复SD卡文件系统\r\n");
    printf("\r\n 但sd卡内的原文件不可恢复，实验前务必备份SD卡内的原文件！！！\r\n");

    printf("\r\n 若已确认，请按开发板的KEY1按键，开始SD卡测试实验....\r\n");

    /* Infinite loop */
    while (1) {
        /*按下按键开始进行SD卡读写实验，会损坏SD卡原文件*/
        if (button_trap_boot()) {
            printf("\r\n开始进行SD卡读写实验\r\n");
            SD_Test();
        }
    }
}
