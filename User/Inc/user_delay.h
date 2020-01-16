#ifndef __user_delay_H
#define __user_delay_H

#include "stm32f1xx_hal.h"
#include "main.h"

void Delay_Init(u8 Freq);
void delay_us(u32 nus);
void delay_ms(u16 nus);

#endif
