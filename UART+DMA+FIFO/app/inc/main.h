#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>


void bl_delay_init(void);
void bl_delay_ms(uint32_t ms);
uint32_t bl_now(void);

#endif /* __MAIN_H */
