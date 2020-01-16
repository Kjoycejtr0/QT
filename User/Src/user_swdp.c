#include "main.h"
#include "stm32f1xx_hal.h"
#include	"user_main.h"
#include "user_delay.h"
#include "user_gpio.h"
#include "user_flash.h"
#include "stdio.h"
#include "SWD_host.h"
#include "SWD_flash.h"
#include "user_swdp.h" 
#include "TEA.h"
#include "flash_blob.c"
#include "string.h"
#include "oled.h"

#define Main_Flash_Start 1024
unsigned int *TEA_KEY = (unsigned int *)"This_is_Key_1057";

uint32_t Main_Flash_Len=0;
uint8_t File_Name_Str[17];
u8 USART_RX_BUF[265];

extern UART_HandleTypeDef huart2;


static void uint8touint32(uint8_t *destin,uint32_t *source,uint32_t uint8_len)
{
	uint32_t cnt=0;
	
	for(cnt=0;cnt<uint8_len/4;cnt++)
	{
		source[cnt]=destin[cnt*4+0]<<24	|	destin[cnt*4+1]<<16	|	destin[cnt*4+2]<<8	|	destin[cnt*4+3];
	}
}

//0-15 byte : File Name
//16-19 byte : flash_code length
//20-23 byte : flash_algo length
//64-575 byte : flash code max 512 byte
//768-811 byte : flash algo (44 byte)
//1020-1023 byte : main flash length 
//1k byte -max byte : main flash
void Upgrade_Target_Flash(void)
{
	uint8_t Read_Flash_Buff[1024]={0};
	uint32_t Flash_Code_Buff[512];
	uint32_t Flash_Algo_Buff[44];
	uint16_t cnt=0;
	
	uint32_t flash_code_len=0;
	uint32_t flash_algo_len=0;
	
	/*************Read Flash*************/
	SPI_FLASH_BufferRead(Read_Flash_Buff,0x00,1024);
	memcpy(File_Name_Str,Read_Flash_Buff,16);
	File_Name_Str[16]='\0';
	OLED_ShowString(0,0,File_Name_Str);
	
	/*************flash code length and flash algo length****************/
	flash_code_len=Read_Flash_Buff[16]<<24 |	Read_Flash_Buff[17]<<16 |	Read_Flash_Buff[18]<<8 |	Read_Flash_Buff[19];
	if(flash_code_len>512)	flash_code_len=512;
	
	flash_algo_len=Read_Flash_Buff[20]<<24 |	Read_Flash_Buff[21]<<16 |	Read_Flash_Buff[22]<<8 |	Read_Flash_Buff[23];
	if(flash_algo_len>44)	flash_algo_len=44;
	/************Flash Code************/
	uint8touint32(Read_Flash_Buff+64,Flash_Code_Buff,flash_code_len);
	memcpy(flash_code,Flash_Code_Buff,flash_code_len);
	
	/************Flash Algo************/
	uint8touint32(Read_Flash_Buff+768,Flash_Algo_Buff,flash_algo_len);
	cnt=768;
	
	flash_algo.init=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.uninit=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.erase_chip=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.erase_sector=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.program_page=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.sys_call_s.breakpoint=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.sys_call_s.static_base=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.sys_call_s.stack_pointer=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.program_buffer=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.algo_start=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	cnt+=1;
	
	flash_algo.algo_size=flash_code_len;
	cnt+=1;
	
	flash_algo.algo_blob=flash_code;
	
	flash_algo.program_buffer_size=Read_Flash_Buff[cnt]<<24 |	Read_Flash_Buff[cnt+1]<<16 |	Read_Flash_Buff[cnt+2]<<8 |	Read_Flash_Buff[cnt+3];
	
	
	/************main flash length*************/
	Main_Flash_Len=Read_Flash_Buff[1020]<<24 |	Read_Flash_Buff[1021]<<16 |	Read_Flash_Buff[1022]<<8 |	Read_Flash_Buff[1023];
}

/***********************************************************
AT+WS DATA CRC16 0x0D 0x0A //Start Write Flash
AT+WE DATA CRC16 0x0D 0x0A //End Write Flash
***********************************************************/
static uint8_t WE[5]={0x41, 0x54, 0x2B, 0x57, 0x45 };
static uint8_t WS[5]={0x41, 0x54, 0x2B, 0x57, 0x53 };
uint32_t Write_cnt=0;

void Write_Flash(uint8_t *Buff)
{
	uint8_t AT_CMD[5];
	uint8_t Verify[256];
	uint8_t Write_Buff[256];
	
	memcpy(AT_CMD,Buff,5);
	memset(Write_Buff,0xFF,256);
	memcpy(Write_Buff,Buff+5,256);
	
	if((Write_cnt%16)==0)	
	{
		SPI_FLASH_SectorErase(Write_cnt*256);
	}
	if(memcmp(AT_CMD,WS,5)==0)
	{
		SPI_FLASH_PageWrite(Write_Buff,0x00+Write_cnt*256,256);
		SPI_FLASH_BufferRead(Verify,Write_cnt*256,256);
		if(memcmp(Verify,Write_Buff,256)==0)
		{
			HAL_UART_Transmit(&huart2,WS,5,0xFFFF);
			Write_cnt+=1;
		}
		else 
		{
			printf("%d\n\r",Write_cnt);
			Write_cnt=0;
			printf("AT+EE\r\n");
		}
	}
	else if(memcmp(AT_CMD,WE,5)==0)
	{
		SPI_FLASH_PageWrite(Write_Buff,0x00+Write_cnt*256,256);
		SPI_FLASH_BufferRead(Verify,Write_cnt*256,256);
		if(memcmp(Verify,Write_Buff,256)==0)
		{
			HAL_UART_Transmit(&huart2,WE,5,0xFFFF);
			Write_cnt=0;
		}
		else 
		{
			Write_cnt=0;
			printf("AT+EE\n\r");
		}
		Upgrade_Target_Flash();
		Buzzer_Success();
	}
	else printf("AT CMD No Wirte \r\n");
	
	
}



uint8_t Program_Target(void)
{
	uint8_t Progarm_Buff[1024],Check_Data[1024];
	uint32_t Pro_Len=0;
	
	OLED_Clear_Line(2);
	OLED_Clear_Line(3);
	OLED_Clear_Line(4);
	OLED_Clear_Line(5);
	OLED_Clear_Line(6);

	if((Main_Flash_Len%1024)==0)	Pro_Len=Main_Flash_Len/1024;
	else Pro_Len=(Main_Flash_Len/1024)+1;
	
	
	SPI_Switch(1);
	printf("Start Progarm\r\n");
	if(target_flash_init(Flash_Start_Addr)!=ERROR_SUCCESS)
	{
		printf("Flash Init Fault\r\n");
		OLED_ShowString(0,1,"Init    Fault");
		return	1;
	}
	OLED_ShowString(0,1,"Init    OK");//目标Flash初始化
		
	if(target_flash_erase_chip()!=ERROR_SUCCESS)
	{
		printf("Erase Fault\r\n");
		OLED_ShowString(0,2,"Erase   Fault");
		return	1;
	}
	OLED_ShowString(0,2,"Erase   OK");//擦除目标Flash
	
	for(uint32_t n=0;n<Pro_Len;n++)
	{
		SPI_FLASH_BufferRead(Progarm_Buff,Main_Flash_Start+n*1024,1024);
		DecryptBuffer(Progarm_Buff,1024,TEA_KEY);
		if(target_flash_program_page(Flash_Start_Addr+n*1024, Progarm_Buff, 1024)!=ERROR_SUCCESS)
		{
			printf("Program Fault\r\n");
			OLED_ShowString(0,3,"Program Fault");
			return	1;
		}
		swd_read_memory(0x08000000+n*1024,Check_Data,1024);
		if(memcmp(Check_Data,Progarm_Buff,1024)!=0)
		{
			printf("Verify Flash Fault %d\r\n",n*1024);
			OLED_ShowString(0,4,"Verify  Fault");
			return 1;
		}
		memset(Progarm_Buff,0xFF,1024);
		memset(Check_Data,0xFF,1024);
	}
	OLED_ShowString(0,3,"Program OK");
	OLED_ShowString(0,4,"Verify  OK");
	
	return 0;
}


static uint16_t const wCRC16Table[256] = 
{
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,     
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,     
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,      
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,     
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,       
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,     
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,     
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,     
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,     
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,        
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,     
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,     
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,     
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,     
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,        
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,     
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,     
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,     
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,     
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,        
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,     
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,     
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,     
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,     
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,       
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,     
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,     
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,     
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,     
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,       
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,     
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};   
	
	

uint16_t CRC16(const uint8_t* pDataIn, uint16_t	iLenIn)     
{
	uint16_t wResult = 0;     
  uint16_t wTableNo = 0;     
  int i = 0;
  for( i = 0; i < iLenIn; i++)     
  {
		wTableNo = ((wResult & 0xff) ^ (pDataIn[i] & 0xff));     
    wResult = ((wResult >> 8) & 0xff) ^ wCRC16Table[wTableNo];     
  } 
	
	return wResult;     
}  


