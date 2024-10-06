#ifndef __BL_LED_H
#define __BL_LED_H


#include <stdbool.h>


void bl_led_init(void);
void bl_led_set(bool on);
void bl_led_on(void);
void bl_led_off(void);
void bl_led_toggle(void);


#endif /* __BL_LED_H */
