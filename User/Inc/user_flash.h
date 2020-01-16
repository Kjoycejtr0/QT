#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#include "stm32f1xx_hal.h"
#include "main.h"

//#define FLASH_SIZE 0x00A00000

#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128 	0XEF17 
 
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
//#define  SPI_FLASH_ID                       0xEF3015     //W25X16   2MB
//#define  SPI_FLASH_ID                       0xEF4015	   //W25Q16   4MB
//#define  SPI_FLASH_ID                       0XEF4017     //W25Q64   8MB
#define  SPI_FLASH_ID                       W25Q128     //W25Q128  16MB YS-F1Pro开发默认使用

#define FLASH_SPIx                                 SPI2
#define FLASH_SPIx_RCC_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define FLASH_SPIx_RCC_CLK_DISABLE()               __HAL_RCC_SPI2_CLK_DISABLE()

#define FLASH_SPI_GPIO_ClK_ENABLE()                __HAL_RCC_GPIOB_CLK_ENABLE() 
#define FLASH_SPI_GPIO_PORT                        GPIOB
#define FLASH_SPI_SCK_PIN                          GPIO_PIN_13
#define FLASH_SPI_MISO_PIN                         GPIO_PIN_14
#define FLASH_SPI_MOSI_PIN                         GPIO_PIN_15

#define FLASH_SPI_CS_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()    
#define FLASH_SPI_CS_PORT                          GPIOB
#define FLASH_SPI_CS_PIN                           GPIO_PIN_12

#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET)

/* 扩展变量 ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspiflash;

/* 函数声明 ------------------------------------------------------------------*/

void SPI_Flash_Init(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint16_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
u8 SPI1_ReadWriteByte(u8 TxData);
uint8_t SPI_TXRX(uint8_t *TXBuff,uint8_t *RXBuff,uint8_t cnt);
uint8_t SPI_TX(uint8_t *TXBuff,uint8_t cnt);
uint8_t SPI_TX7(uint8_t *TXBuff);
uint8_t SPI_TX4(uint8_t *TXBuff);
uint8_t SPI_TX2(uint8_t *TXBuff);   

#endif
