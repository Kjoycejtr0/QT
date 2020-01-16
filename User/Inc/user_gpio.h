#ifndef __USER_GPIO_H
#define __USER_GPIO_H

#include "stm32f1xx_hal.h"
#include "main.h"



#define GPIO_PIN ((uint16_t)0x0001U)

#define OK	PBOut(12)
#define NG	PBOut(14)
#define Busy	PBOut(15)
#define RST(x)	PBOut(6,x)
#define Buzzer(x) PBOut(1,x)
#define Start_PIN	PAin(4)
#define B1	PAin(5)
#define B2	PAin(6)
#define B3	PAin(7)


#define LED1(x) PBOut(0,x)

uint8_t PAin(uint8_t PIN);
void PAOut(uint8_t PIN,uint8_t DIR);

uint8_t PBin(uint8_t PIN);
void PBOut(uint8_t PIN,uint8_t DIR);

uint8_t PCin(uint8_t PIN);
void PCOut(uint8_t PIN,uint8_t DIR);

uint8_t PDin(uint8_t PIN);
void PDOut(uint8_t PIN,uint8_t DIR);

uint8_t CRC8( uint8_t *ptr, uint8_t len);


#endif
