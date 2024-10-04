/******************************************************************************
 * @file button.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief  按键配置相关函数声明
 * @version 0.1
 * @date 2024-08-25
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef __BL_BUTTON_H
#define __BL_BUTTON_H


#include <stdbool.h>


void bl_button_init(void);
bool bl_button_pressed(void);
bool button_trap_boot(void);


#endif /* __BL_BUTTON_H */
