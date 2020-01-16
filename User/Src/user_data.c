#include "main.h"
#include "stm32f1xx_hal.h"
#include	"user_main.h"
#include "user_delay.h"
#include "user_gpio.h"
#include "user_flash.h"
#include "user_usb.h"
#include "usbd_cdc_if.h"
#include "SWD_host.h"
#include "SWD_flash.h"
#include "user_swdp.h"
#include 	"flash_blob.c"

uint32_t Code_Length=0;

extern program_target_t flash_algo;
extern uint32_t flash_code[128];

extern uint8_t USB_USART_RX_BUF[USB_USART_REC_LEN]; 
extern uint8_t USB_USART_RX_STA;
extern TIM_HandleTypeDef htim1;

extern uint8_t STM32F1_demo_code[3604];

uint8_t Save_Data(uint8_t *Data,uint32_t cnt)
{
	static uint16_t Page_cnt=0;
	static uint8_t Flash_Write_Buff[256]={0xFF};
	
	memcpy(Flash_Write_Buff+cnt*32,Data+4,32);

	if((cnt%8)==0)
	{
		SPI_FLASH_PageWrite(Flash_Write_Buff,Page_cnt*256,256);
		Page_cnt+=1;
	}
	
	if(Data[3]==0x01)	
	{
		Send_Command(0x54);
		HAL_TIM_Base_Start_IT(&htim1);//开启定时器，启动超时检测
	}
	else
	{
		Page_cnt=0;
		Send_Command(0x06);
		Buzzer(0);
		delay_ms(100);
		Buzzer(1);
		delay_ms(100);
		Refresh_Parameters();//刷新参数
	}
	
	return 0;
}

void USB_Data_Process(void)
{
	uint8_t USB_RX_Buff[39]={0};
	static uint32_t cnt=0;
	
	memcpy(USB_RX_Buff,USB_USART_RX_BUF,39);//复制USB接收缓存的数据
	
	if(USB_RX_Buff[36]==CRC8(USB_USART_RX_BUF,36))//计算CRC校验值是否正确
	{
		Save_Data(USB_RX_Buff,cnt);//接收到数据传输指令，处理接收到的数据并应答
		if(USB_RX_Buff[3]==0x01) cnt+=1;//下一次还有数据传输，请做好接收准备
		else cnt=0;
	}
	
	memset(USB_USART_RX_BUF,0x00,64);
	USB_USART_RX_STA=0;
}

uint8_t Load_Flash_Algo(void)
{
	uint8_t	Read_Buff[4]={0};
	uint8_t	Flash_Algo_Length_Buff[4]={0};
	uint32_t Flash_Algo_Length=0;
	uint8_t Code_Buff[4]={0};
	
	uint8_t	Read_Buff_Par[48]={0};
	uint16_t	cnt=0;
	
	memset(Flash_Algo_Length_Buff,0xFF,4);

	SPI_FLASH_BufferRead(Flash_Algo_Length_Buff,17,4);
	Flash_Algo_Length=(Flash_Algo_Length_Buff[0]<<24	|	Flash_Algo_Length_Buff[1]<<16	|	Flash_Algo_Length_Buff[2]<<8	|Flash_Algo_Length_Buff[3]);//Flash算法长度包括Flash_Blob/Flash/Device_Flash
	
	memset(flash_code,0xFF,Flash_Algo_Length);
	
	/*******************************************************************************************************************************************************
	-----------------------------------------------------------------加载Flash算法，位置无关代码-------------------------------------------------------------
	*******************************************************************************************************************************************************/
	for(cnt=0;cnt<Flash_Algo_Length;cnt++)
	{
		memset(Read_Buff,0x00,4);
		SPI_FLASH_BufferRead(Read_Buff,21+cnt*4,4);
		flash_code[cnt]=	(
													Read_Buff[0]<<24	|	Read_Buff[1]<<16	|
													Read_Buff[2]<<8	|	Read_Buff[3]		);
	}

	/*******************************************************************************************************************************************************
	------------------------------------------------------------------------加载Flash参数表-----------------------------------------------------------------
	*******************************************************************************************************************************************************/
	
	SPI_FLASH_BufferRead(Read_Buff_Par,533,4*11);
	
	cnt=0;
	flash_algo.init=											(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);
	
	cnt+=1;
	flash_algo.uninit=										(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);
								
	cnt+=1;							
	flash_algo.erase_chip=								(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);									
										
	cnt+=1;							
	flash_algo.erase_sector=							(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);
										
	cnt+=1;							
	flash_algo.program_page=							(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);
					
	cnt+=1;	
	flash_algo.sys_call_s.breakpoint=		(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);	

  cnt+=1;	
	flash_algo.sys_call_s.static_base=		(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);		
															 
	cnt+=1;	
	flash_algo.sys_call_s.stack_pointer=	(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);

  cnt+=1;							
	flash_algo.program_buffer=						(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);
											
	cnt+=1;							
	flash_algo.algo_start=								(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);
																
	flash_algo.algo_size=									sizeof(flash_code);

	flash_algo.algo_blob=									flash_code;
	
	
	cnt+=1;							
	flash_algo.program_buffer_size=			(
																		Read_Buff_Par[4*cnt+0]<<24|	Read_Buff_Par[4*cnt+1]<<16|
																		Read_Buff_Par[4*cnt+2]<<8	|	Read_Buff_Par[4*cnt+3]		);
																		
	
	SPI_FLASH_BufferRead(Code_Buff,768,4);
	Code_Length=(Code_Buff[0]<<24	|	Code_Buff[1]<<16	|	Code_Buff[2]<<8	|Code_Buff[3]);
	return 0;
}

