#ifndef __USER_DATA_H__
#define __USER_DATA_H__
#include "main.h"

void USB_Data_Process(void);
uint8_t Save_Data(uint8_t *Data,uint32_t cnt);
uint8_t Load_Flash_Algo(void);
uint8_t Test_FLASH(void);

#endif


