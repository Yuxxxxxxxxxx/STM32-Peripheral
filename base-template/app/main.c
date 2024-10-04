#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "main.h"
#include "led.h"
#include "button.h"
#include "console_uart.h"

extern void board_lowlevel_init(void);

int main(void)
{
	board_lowlevel_init();
	
	bl_delay_init();
	bl_led_init();
	bl_button_init();
	bl_uart_init();
	
	while (1)
	{
	}
}
