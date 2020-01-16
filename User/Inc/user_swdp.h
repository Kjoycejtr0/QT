#ifndef __USER_SWDP_H__
#define __USER_SWDP_H__

#include "main.h"

#define Flash_Start_Addr 0x08000000

uint8_t Program_Target(void);
void Write_Flash(uint8_t *Buff);
void Upgrade_Target_Flash(void);
uint16_t CRC16(const uint8_t* pDataIn, uint16_t	iLenIn);

#endif


