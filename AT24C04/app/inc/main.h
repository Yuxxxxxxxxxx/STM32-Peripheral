/******************************************************************************
 * @file main.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-22
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef __MAIN_H
#define __MAIN_H

#include <stddef.h>
#include <stdint.h>


void bl_delay_init(void);
void bl_delay_us(uint32_t us);
void bl_delay_ms(uint32_t ms);
uint32_t bl_now(void);

#endif /* __MAIN_H */
