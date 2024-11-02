#ifndef __DWT_H
#define __DWT_H

#include <stdint.h>

void bsp_dwt_init(void);
void bsp_dwt_delay(uint32_t _delay_time);
void bsp_delay_US(uint32_t _delay_time);
void bsp_delay_MS(uint32_t _delay_time);

#endif /* __DWT_H */
