#ifndef __user_usb_H
#define __user_usb_H

#include "stm32f1xx_hal.h"
#include "main.h"

#define USB_USART_REC_LEN	 	64		//USB���ڽ��ջ���������ֽ���

void usb_printf(char* fmt,...); 
void USB_TX_HEX(uint8_t *Data);

#endif
