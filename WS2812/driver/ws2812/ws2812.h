/******************************************************************************
 * @file ws2812.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-27
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef __WX2812_H
#define __WX2812_H


#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGBColor_TypeDef;


extern const RGBColor_TypeDef GREEN;
extern const RGBColor_TypeDef BLUE;
extern const RGBColor_TypeDef SKY;
extern const RGBColor_TypeDef MAGENTA ;
extern const RGBColor_TypeDef YELLOW ;
extern const RGBColor_TypeDef ORANGE;
extern const RGBColor_TypeDef BLACK;
extern const RGBColor_TypeDef WHITE;
extern const RGBColor_TypeDef PURPLE;

#define WS2812_NUM              240

/******************************************************************************
 说明：

 WS2812B编码协议(单位：ns)：
       min     typ     max
bit 0
 T0H:  220      -      420
 T0L:  750      -      1600
 
bit 1 
 T1H:  750      -      1600
 T1L:  220      -      420
 
 
 RESET: time > 300us


8分频APB1，42MHz/8 = 5.25MHz
时钟周期为：1/5.25/1e6 = 1.90e-7=190ns
******************************************************************************/

// 根据不同的SCK适当调整
#define CODE0 0xC0 // 0码, 发送的时间 1100 0000  
#define CODE1 0xFC // 1码, 发送的时间 1111 1100

void ws2812_init(void);

/* Basic Color Effects -------------------------------------------------------*/
void rgb_red(uint16_t Pixel_LEN);
void rgb_green(uint16_t Pixel_LEN);
void rgb_blue(uint16_t Pixel_LEN);
void rgb_yellow(uint16_t Pixel_LEN);
void rgb_magenta(uint16_t Pixel_LEN);
void rgb_black(uint16_t Pixel_LEN);
void rgb_white(uint16_t Pixel_LEN);
void rgb_sky(uint16_t Pixel_LEN);
void rgb_orange(uint16_t Pixel_LEN);
void rgb_purple(uint16_t Pixel_LEN);

/* Complicated Color Effects -------------------------------------------------*/
void rainbow_cycle(uint16_t Pixel_LEN);


#endif /* __WX2812_H */
