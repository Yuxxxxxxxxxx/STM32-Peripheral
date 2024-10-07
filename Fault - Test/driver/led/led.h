#ifndef __BL_LED_H
#define __BL_LED_H


#include <stdint.h>
#include <stdbool.h>


void bl_led_init(void);
void bl_led_set(bool on, uint32_t pin);
void bl_led_on(uint32_t pin);
void bl_led_off(uint32_t pin);
void bl_led_toggle(uint32_t pin);


#endif /* __BL_LED_H */
