#ifndef __USER_RTC_H
#define __USER_RTC_H	  

#include "main.h"

extern RTC_HandleTypeDef RTC_Handler;  //RTC句柄
												    
//时间结构体
typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;			
	//公历日月年周
	uint16_t w_year;
	uint8_t  w_month;
	uint8_t  w_date;
	uint8_t  week;	
}_calendar_obj;					 
extern _calendar_obj calendar;				//日历结构体

u8 RTC_Init(void);        					//初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);					//平年,闰年判断
u8 RTC_Get(void);         					//获取时间   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);		//设置时间	
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);	//设置闹钟	
#endif



















