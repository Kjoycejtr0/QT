#ifndef __USER_MAIN_H
#define __USER_MAIN_H

#include "main.h"
#include "stm32f1xx_hal.h"

int user_main(void);
void Send_Command(uint8_t Command);
void main_Init(void);
void Refresh_Parameters(void);
void USB_Data_Process(void);
void Buzzer_Success(void);
void Buzzer_Fault(void);

#endif

