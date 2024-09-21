#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "ctl_spi.h"
#include "main.h"
#include "led.h"
#include "console_uart.h"
#include "w25q.h"

extern void board_lowlevel_init(void);

uint8_t i = 0;

void test_number(void)
{	
	uint8_t wr_data[128] = {0};
    uint8_t rd_data[128] = {0};
	
	for (i = 0; i < 128; i++)
	{
		wr_data[i] = i;
	}
	w25q32_dev.wr(wr_data, 0, 128);

	w25q32_dev.rd(rd_data, 0, 128); // 从falsh读取数据并打印
	printf("\r\nread data is :\r\n");
	for (i = 0; i < 128; i++)
	{
		printf("%d, ", rd_data[i]);
	}
}

void test_string(void)
{
	uint8_t data[13] = "hello world!";
	uint8_t rdata[4] = { 0 };
	
	w25q32_dev.wr(data, 0, sizeof(data));
	w25q32_dev.rd(rdata, 0, sizeof(data));
	
	printf("%s", rdata);
}

int main(void)
{
	board_lowlevel_init();
	bl_delay_init();
	bl_led_init();
	uart_init();

	bl_led_on();

	w25q32_dev.init();

	printf("\r\n\r\nw25q128 id is: 0x%x\r\n", w25q32_dev.type);

	printf("detact w25q128 ok!\r\n");
	printf("write data !\r\n"); // 向flash写入数据
	
	
	test_number();
	
	test_string();
	
	return 0;
}
