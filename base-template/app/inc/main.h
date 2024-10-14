#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>


void bsp_delay_init(void);
void bsp_delay_ms(uint32_t ms);
uint32_t bsp_now(void);

#endif /* __MAIN_H */
