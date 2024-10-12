/******************************************************************************
 * @file fifo.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-08
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "stm32f4xx.h"
#include "fifo.h"
#include "main.h"


uint32_t fifo_get_total_size(_fifo_t *pfifo);
uint32_t fifo_get_occupy_size(_fifo_t *pfifo);
uint32_t fifo_get_free_size(_fifo_t *pfifo);

/******************************************************************************
 * @brief      注册 fifo
 * 
 * @param[in]  pfifo       :    fifo 结构体指针
 * @param[in]  pfifo_buf   :    fifo 内存块
 * @param[in]  size        :    内存块大小
 * @param[in]  lock        :    上锁回调函数
 * @param[in]  unlock      :    解锁回调函数
 * 
 * @return     none
 * 
******************************************************************************/
void fifo_register(_fifo_t *pfifo, uint8_t *pfifo_buf, uint32_t size,
                            lock_fun lock, lock_fun unlock)
{
    pfifo->buf          =  pfifo_buf;
    pfifo->buf_size     =  size;
    pfifo->occupy_size  =  0;
    pfifo->pwrite       =  pfifo->buf;
    pfifo->pread        =  pfifo->buf; 
    pfifo->lock         =  lock;
    pfifo->unlock       =  unlock;
}

/******************************************************************************
 * @brief      释放 fifo
 * 
 * @param[in]  pfifo    :     fifo 结构体指针
 * 
 * @return     none
 * 
******************************************************************************/
void fifo_release(_fifo_t *pfifo)
{
    pfifo->buf          =  NULL;
    pfifo->buf_size     =  0;
    pfifo->occupy_size  =  0;
    pfifo->pwrite       =  0;
    pfifo->pread        =  0; 
    pfifo->lock         =  NULL;
    pfifo->unlock       =  NULL;
}

/******************************************************************************
 * @brief      向 fifo 写数据
 * 
 * @param[in]  pfifo    :    fifo 结构体指针
 * @param[in]  pbuf     :    待写数据
 * @param[in]  size     :    待写数据长度
 * 
 * @return     uint32_t :    实际写数据长度
 * 
******************************************************************************/
uint32_t fifo_write(_fifo_t *pfifo, const uint8_t *pbuf, uint32_t size)
{
	uint32_t w_size = 0, free_size = 0;

	if ((size == 0) || (pfifo == NULL) || (pbuf == NULL))
	{
	 return 0;
	}

	free_size = fifo_get_free_size(pfifo);
	if (free_size == 0)
	{
		return 0;
	}

	if (free_size < size)
	{
		size = free_size;
	}

	w_size = size;

	if (pfifo->lock != NULL)
		pfifo->lock();
	/*********************上锁*********************/
	while (w_size-- > 0)
	{
		*pfifo->pwrite++ = *pbuf++;
		if (pfifo->pwrite >= (pfifo->buf + pfifo->buf_size))
		{
			pfifo->pwrite = pfifo->buf;
		}
		pfifo->occupy_size++;
	}
	/*********************解锁*********************/
	if (pfifo->unlock != NULL)
		pfifo->unlock();
	
	return size;
}

/******************************************************************************
 * @brief      从 fifo 读数据
 * 
 * @param[in]  pfifo    :    fifo 结构体指针
 * @param[in]  pbuf     :    待读数据缓存
 * @param[in]  size     :    待读数据长度
 * 
 * @return     uint32_t :    实际读数据长度
 * 
******************************************************************************/
uint32_t fifo_read(_fifo_t *pfifo, uint8_t *pbuf, uint32_t size)
{
	uint32_t r_size = 0, occupy_size = 0;

	if ((size == 0) || (pfifo == NULL) || (pbuf == NULL))
	{
		return 0;
	}

	occupy_size = fifo_get_occupy_size(pfifo);
	if (occupy_size == 0)
	{
		return 0;
	}

	if (occupy_size < size)
	{
		size = occupy_size;
	}
	
	if (pfifo->lock != NULL)
		pfifo->lock();
	/*********************上锁*********************/
	r_size = size;

	while (r_size-- > 0)
	{
		*pbuf++ = *pfifo->pread++;
		if (pfifo->pread >= (pfifo->buf + pfifo->buf_size))
		{
			pfifo->pread = pfifo->buf;
		}
		pfifo->occupy_size--;
	}
	/*********************解锁*********************/
	if (pfifo->unlock != NULL)
		pfifo->unlock();

	return size;
}  

/******************************************************************************
 * @brief      获取 fifo 空间大小
 * 
 * @param[in]  pfifo    :    fifo 结构体指针
 * 
 * @return     uint32_t :    fifo大小 
 * 
******************************************************************************/
static uint32_t fifo_get_total_size(_fifo_t *pfifo)
{
    if (pfifo == NULL)
        return 0;

    return pfifo->buf_size;
}

/******************************************************************************
 * @brief      获取 fifo 已用空间大小
 * 
 * @param[in]  pfifo    :    fifo 结构体指针
 * 
 * @return     uint32_t :    fifo已用大小
 * 
******************************************************************************/
static uint32_t fifo_get_occupy_size(_fifo_t *pfifo)
{
    if (pfifo == NULL)
        return 0;

    return pfifo->occupy_size;
}

/******************************************************************************
 * @brief      获取 fifo 空闲空间大小
 * 
 * @param[in]  pfifo    :    fifo 结构体指针
 * 
 * @return     uint32_t :    fifo空闲大小
 * 
******************************************************************************/
static uint32_t fifo_get_free_size(_fifo_t *pfifo)
{
    if (pfifo == NULL)
        return 0;

    uint32_t size = 0;

    size = pfifo->buf_size - fifo_get_occupy_size(pfifo);

    return size;
}
