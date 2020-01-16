#ifndef __USER_RTC_H
#define __USER_RTC_H	  

#include "main.h"

extern RTC_HandleTypeDef RTC_Handler;  //RTC���
												    
//ʱ��ṹ��
typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;			
	//������������
	uint16_t w_year;
	uint8_t  w_month;
	uint8_t  w_date;
	uint8_t  week;	
}_calendar_obj;					 
extern _calendar_obj calendar;				//�����ṹ��

u8 RTC_Init(void);        					//��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u8 Is_Leap_Year(u16 year);					//ƽ��,�����ж�
u8 RTC_Get(void);         					//��ȡʱ��   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);		//����ʱ��	
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);	//��������	
#endif



















