#include "board.h"

const LED_t led0 = {
    .clk_source = RCC_AHB1Periph_GPIOE,
    .port = GPIOE,
    .pin = GPIO_Pin_5,
    
    .status = false,
	.init = true, 
};

const LED_t led1 = {
    .clk_source = RCC_AHB1Periph_GPIOE,
    .port = GPIOE,
    .pin = GPIO_Pin_6,
    
    .status = false,
	.init = true,
};

const LED_t led2 = {
    .clk_source = RCC_AHB1Periph_GPIOC,
    .port = GPIOC,
    .pin = GPIO_Pin_13,

	.status = false,
	.init = true,
};

