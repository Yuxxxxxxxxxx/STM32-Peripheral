#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>


typedef struct
{
	uint8_t Rxlen;
	char RxBuf[20];
	uint8_t Txlen;
	char TxBuf[20];
}PRINT_MODS_T;

extern PRINT_MODS_T g_tPrint;

void bl_delay_init(void);
void bl_delay_ms(uint32_t ms);
uint32_t bl_now(void);

#endif /* __MAIN_H */
