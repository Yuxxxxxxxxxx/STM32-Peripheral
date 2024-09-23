/******************************************************************************
 * @file task.c
 * @author Yuxxxxxxxxxx (2936481298@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-23
 * 
 * @copyright Copyright (c) 2024
 * 
******************************************************************************/
#include "main.h"


static void misc_task(void *args)
{
    uint32_t i = 0;
    while (1)
    {
        i = i + 1;
    }
}

static void uart_send_task(void *args)
{
    char buff[128];
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        sprintf(buff, "[%lu] %s is running...\r\n", xTaskGetTickCount(), pcTaskGetName(NULL));
        uart_write((uint8_t *)buff, strlen(buff));
    }
}

void test_task(void)
{
    xTaskCreate(uart_send_task, "task1", 512, NULL, 5, NULL);
    xTaskCreate(uart_send_task, "task2", 512, NULL, 6, NULL);
    xTaskCreate(uart_send_task, "task3", 512, NULL, 7, NULL);
    xTaskCreate(misc_task, "misc", 512, NULL, 1, NULL);

    vTaskStartScheduler();
}
