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
	
	bsp_delay_init();
	bsp_led_init();
	bsp_button_init();
	bsp_uart_init();
	
	while (1)
	{
	}
}
