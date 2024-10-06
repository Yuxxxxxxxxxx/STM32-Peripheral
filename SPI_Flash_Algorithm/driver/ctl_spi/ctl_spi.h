/******************************************************************************
 * @file w25q.h
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-20
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#ifndef __CTL_SPI_H
#define __CTL_SPI_H


void spi_init(void);
uint8_t spi_read_write_byte(uint8_t tx_data);


#endif /* __CTL_SPI_H */  
