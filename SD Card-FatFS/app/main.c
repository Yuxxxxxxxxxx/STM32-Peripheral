#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "console_uart.h"
#include "sdio_test.h"
#include "button.h"
#include "main.h"
#include "ff.h"

FATFS fs;                    /* FatFs文件系统对象 */
FIL fnew;                    /* 文件对象 */
FRESULT res_sd;              /* 文件操作结果 */
UINT fnum;                   /* 文件成功读写数量 */
BYTE ReadBuffer[1024] = {0}; /* 读缓冲区 */
BYTE WriteBuffer[] = "欢迎使用野火STM32 F407开发板 今天是个好日子，新建文件系统测试文件\r\n";

static char m_path_buf[256];
 
int scan_files(char *dir_path)
{
	FILINFO m_file_info;
	
	if (!dir_path) {
		return -1;
	}
	
	DIR dir;             //定义目录对象
	FRESULT res;         //定义结果对象
	int ret = 0;
 
	res = f_opendir(&dir, dir_path); //打开目录，返回状态 和 目录对象的指针
	if(res != FR_OK) {
		printf("opendir 失败  %d", res);
		return -2;
	}
	
	
	for(;;)	{
		res = f_readdir(&dir, &m_file_info);  //读取目录，返回状态 和 文件信息的指针
		if(res != FR_OK || m_file_info.fname[0] == 0) 
			break; //若打开失败 或 到结尾，则退出
 
		if (!(m_file_info.fattrib & AM_DIR)) {
			printf("是文件 %s/%s\n", dir_path, m_file_info.fname);
			++ret;
		}
		else {
			printf("是目录 %s\n", dir_path);
			sprintf(m_path_buf, "%s/%s", dir_path, m_file_info.fname); //将新目录添加在原目录后面
			res = scan_files(dir_path); //将新目录进行递归调用
			if(res != FR_OK) 
				break;
		}
	}
 
	f_closedir(&dir);
	return ret;
}

int main(void)
{
    bl_delay_init();

    /* 初始化LED灯 */
    bl_led_init();
    /* 初始化独立按键 */
    bl_button_init();

    /*初始化USART1*/
    uart_init();

    bl_led_on(GPIO_Pin_5);

    /* 初始化调试串口，一般为串口1 */
    printf("\r\n****** 这是一个SD卡 文件系统实验 ******\r\n");

    // 在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
    res_sd = f_mount(&fs, "0:", 1);

    /*----------------------- 格式化测试 ---------------------------*/
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if (res_sd == FR_NO_FILESYSTEM)
    {
        printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
        /* 格式化 */
        res_sd = f_mkfs("0:", 0, 0);

        if (res_sd == FR_OK)
        {
            printf("》SD卡已成功格式化文件系统。\r\n");
            /* 格式化后，先取消挂载 */
            res_sd = f_mount(NULL, "0:", 1);
            /* 重新挂载	*/
            res_sd = f_mount(&fs, "0:", 1);
        }
        else
        {
            bl_led_on(GPIO_Pin_6);
            printf("《《格式化失败。》》\r\n");
            while (1)
                ;
        }
    }
    else if (res_sd != FR_OK)
    {
        printf("！！SD卡挂载文件系统失败。(%d)\r\n", res_sd);
        printf("！！可能原因：SD卡初始化不成功。\r\n");
        while (1)
            ;
    }
    else
    {
        printf("》文件系统挂载成功，可以进行读写测试\r\n");
    }

    /*----------------------- 文件系统测试：写测试 -----------------------------*/
    /* 打开文件，如果文件不存在则创建它 */
    printf("\r\n****** 即将进行文件写入测试... ******\r\n");
    res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_sd == FR_OK)
    {
        printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
        if (res_sd == FR_OK)
        {
            printf("》文件写入成功，写入字节数据：%d\n", fnum);
            printf("》向文件写入的数据为：\r\n%s\r\n", WriteBuffer);
        }
        else
        {
            printf("！！文件写入失败：(%d)\n", res_sd);
        }
        /* 不再读写，关闭文件 */
        f_close(&fnew);
    }
    else
    {
        bl_led_off(GPIO_Pin_5);
        printf("！！打开/创建文件失败。\r\n");
    }

    /*------------------- 文件系统测试：读测试 ------------------------------------*/
    printf("****** 即将进行文件读取测试... ******\r\n");
    res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ);
    if (res_sd == FR_OK)
    {
        bl_led_on(GPIO_Pin_6);
        printf("》打开文件成功。\r\n");
        res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (res_sd == FR_OK)
        {
            printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
            printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
        }
        else
        {
            printf("！！文件读取失败：(%d)\n", res_sd);
        }
    }
    else
    {
        bl_led_off(GPIO_Pin_5);
        printf("！！打开文件失败。\r\n");
    }
    /* 不再读写，关闭文件 */
    f_close(&fnew);

    scan_files("0:testdir");

    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL, "0:", 1);

    /* 操作完成，停机 */
    while (1)
    {
    }
}
