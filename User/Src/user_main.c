#include "main.h"
#include "stm32f1xx_hal.h"
#include	"user_main.h"
#include "user_delay.h"
#include "user_gpio.h"
#include "user_flash.h"
#include "stdio.h"
#include "usbd_cdc_if.h"
#include "SWD_host.h"
#include "SWD_flash.h"
#include "user_swdp.h"
#include "DAP.h"
#include "oled.h"

uint8_t Program_Flag=2;//编程标志位
extern TIM_HandleTypeDef htim1;
extern SPI_HandleTypeDef hspi1;
extern u16 USART_RX_STA;
extern u8 USART_RX_BUF[265];
extern UART_HandleTypeDef huart2;

u8 Button=1;
u8 Button_Buff=1;
u8 Start_Ext=1;
u8 Start_Ext_Buff=1;
static uint8_t WS[5]={0x41, 0x54, 0x2B, 0x57, 0x53 };
static uint8_t RS[5]={0x41, 0x54, 0x2B, 0x52, 0x53 };
static uint8_t EE[5]={0x41, 0x54, 0x2B, 0x45, 0x45 };
uint8_t CRC_Flag;
extern uint32_t Write_cnt;
uint8_t UART_RX_Flag=0;
u32 Pro_Succ=0;
extern u8 Start_Pro;


//用户主函数
int user_main(void)
{
	static uint16_t CRC16_Reg=0;
	static const uint8_t CC[5]={0x41, 0x54, 0x2B, 0x43, 0x43 };
	static uint8_t RS_cnt=0;
	
	if(UART_RX_Flag==1)
	{
		HAL_TIM_Base_Stop_IT(&htim1);
		
		CRC16_Reg=(USART_RX_BUF[261]<<8)	|	USART_RX_BUF[262];
		if(CRC16_Reg==CRC16(USART_RX_BUF,261))
		{
			CRC_Flag=1;
			Write_Flash(USART_RX_BUF);
			RS_cnt=0;
		}
		else if(memcmp(CC,USART_RX_BUF,5)	==	0)
		{
			Write_cnt=0;
			HAL_UART_Transmit(&huart2,WS,5,0xFFFF);
		}
		else 
		{
			CRC_Flag=0;
			HAL_UART_Transmit(&huart2,RS,5,0xFFFF);
			RS_cnt+=1;
		}
		
		if(RS_cnt>20)
		{
			HAL_UART_Transmit(&huart2,EE,5,0xFFFF);
			RS_cnt=0;
			Write_cnt=0;
		}
		
		memset(USART_RX_BUF,0xFF,265);		
		USART_RX_STA=0;
		UART_RX_Flag=0;
		
		HAL_TIM_Base_Start_IT(&htim1);
	}
	
	
	
	if((Button!=Button_Buff)	||	(Start_Ext!=Start_Ext_Buff))
	{
		if((Button_Buff==0)	||	(Start_Ext_Buff==0))
		{
			HAL_TIM_Base_Stop_IT(&htim1);
			if(Program_Target()==0)
			{
				Pro_Succ+=1;
				OLED_ShowNum(0,6,Pro_Succ,6,16);
				printf("Download successful number is %d\r\n\r\n",Pro_Succ);
				Buzzer_Success();
			}
			else	Buzzer_Fault();
			PBOut(0,1);
			delay_ms(100);
			PBOut(0,0);
		}
		Button_Buff=Button;
		Start_Ext_Buff=Start_Ext;
		HAL_TIM_Base_Start_IT(&htim1);//在规定时间内接收到数据，关闭定时器
	}
	
	if(Start_Pro==1)
	{
		HAL_TIM_Base_Stop_IT(&htim1);//在规定时间内接收到数据，关闭定时器
		if(Program_Target()==0)
		{
			Pro_Succ+=1;
			OLED_ShowNum(0,6,Pro_Succ,6,16);
			printf("Download successful number is %d\r\n\r\n",Pro_Succ);
//			Buzzer_Success();
		}
		else	Buzzer_Fault();
		Start_Pro=0;
		HAL_TIM_Base_Start_IT(&htim1);//在规定时间内接收到数据，关闭定时器
	}
	return 0;
}


void Buzzer_Success(void)
{
	LED1(1);
	Buzzer(0);
	delay_ms(100);
	Buzzer(1);
	LED1(0);
}

void Buzzer_Fault(void)
{
	Buzzer(0);
	delay_ms(100);
	Buzzer(1);
	delay_ms(100);
	Buzzer(0);
	delay_ms(100);
	Buzzer(1);
	delay_ms(100);
	Buzzer(0);
	delay_ms(100);
	Buzzer(1);
	delay_ms(100);
}



