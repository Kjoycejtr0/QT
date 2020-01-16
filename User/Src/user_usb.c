#include "main.h"
#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "string.h"	
#include "stdarg.h"	
#include	"user_delay.h"
#include	"user_usb.h"



u8 USB_USART_RX_BUF[USB_USART_REC_LEN]; 
u8 USB_USART_RX_STA=0;       				//接收状态标记	 

//usb虚拟串口,printf 函数
void usb_printf(char* fmt,...)  
{  
	u16 i;
	u8	USART_PRINTF_Buffer[512];
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART_PRINTF_Buffer,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART_PRINTF_Buffer);//此次发送数据的长度

	CDC_Transmit_FS(USART_PRINTF_Buffer,i);
	
	delay_ms(1);
} 

//USB虚拟串口传输数据
void USB_TX_HEX(uint8_t *Data)
{
	CDC_Transmit_FS(Data,59);
}



