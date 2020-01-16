#include "main.h"
#include "stm32f1xx_hal.h"
#include	"user_delay.h"
#include	"user_gpio.h"


uint8_t PAin(uint8_t PIN)
{
	uint8_t Value;
	Value	=	HAL_GPIO_ReadPin(GPIOA,(GPIO_PIN<<PIN));
	return Value;
}

//IO输出
void PAOut(uint8_t PIN,uint8_t DIR)
{
	if(DIR	==	0) HAL_GPIO_WritePin(GPIOA,(GPIO_PIN<<PIN),GPIO_PIN_RESET);
	else if(DIR	==	1) HAL_GPIO_WritePin(GPIOA,(GPIO_PIN<<PIN),GPIO_PIN_SET);
}


uint8_t PBin(uint8_t PIN)
{
	uint8_t Value;
	Value	=	HAL_GPIO_ReadPin(GPIOB,(GPIO_PIN<<PIN));
	return Value;
}

//IO输出
void PBOut(uint8_t PIN,uint8_t DIR)
{
	if(DIR	==	0) HAL_GPIO_WritePin(GPIOB,(GPIO_PIN<<PIN),GPIO_PIN_RESET);
	else if(DIR	==	1) HAL_GPIO_WritePin(GPIOB,(GPIO_PIN<<PIN),GPIO_PIN_SET);
}


uint8_t PCin(uint8_t PIN)
{
	return HAL_GPIO_ReadPin(GPIOC,(GPIO_PIN<<PIN));
}

//IO输出
void PCOut(uint8_t PIN,uint8_t DIR)
{
	if(DIR	==	0)  GPIOC->BSRR = (uint32_t)GPIO_PIN<<PIN << 16U;//HAL_GPIO_WritePin(GPIOC,(GPIO_PIN<<PIN),GPIO_PIN_RESET);
	else if(DIR	==	1) GPIOC->BSRR = GPIO_PIN<<PIN; //HAL_GPIO_WritePin(GPIOC,(GPIO_PIN<<PIN),GPIO_PIN_SET);
}


uint8_t PDin(uint8_t PIN)
{
	uint8_t Value;
	Value	=	HAL_GPIO_ReadPin(GPIOD,(GPIO_PIN<<PIN));
	return Value;
}

//IO输出
void PDOut(uint8_t PIN,uint8_t DIR)
{
	if(DIR	==	0) HAL_GPIO_WritePin(GPIOD,(GPIO_PIN<<PIN),GPIO_PIN_RESET);
	else if(DIR	==	1) HAL_GPIO_WritePin(GPIOD,(GPIO_PIN<<PIN),GPIO_PIN_SET);
}

//CRC8校验
uint8_t CRC8( uint8_t *ptr, uint8_t len)
{
 
    unsigned char crc;
    unsigned char i;
    crc = 0;
    while(len--)
    {
       crc ^= *ptr++;
       for(i = 0;i < 8;i++)
       {
           if(crc & 0x80)
           {
               crc = (crc << 1) ^ 0x07;
           }
           else crc <<= 1;
       }
    }
    return crc;
}



