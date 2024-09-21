#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static void blink_led(void *args)
{
	const LED_t *led = (LED_t *)args;
	LED_Init((LED_t *)led);
	
	while (1) {
		LED_On((LED_t *)led);
		vTaskDelay(200);
		// delay();
		LED_Off((LED_t *)led);
		vTaskDelay(200);
	}
}

int main(void)
{
	xTaskCreate(blink_led, "blink-led", 256, (void *)&led0, 1, NULL);  
	xTaskCreate(blink_led, "blink-led", 256, (void *)&led1, 1, NULL);
	xTaskCreate(blink_led, "blink-led", 256, (void *)&led2, 1, NULL);
	
	vTaskStartScheduler();
	
	return 0;
}
