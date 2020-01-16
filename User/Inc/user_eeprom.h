#ifndef __USER_EEPROM_H
#define __USER_EEPROM_H

#include "stm32f1xx_hal.h"
#include "main.h"

//IO��������


//IO��������	 
#define IIC_SCL(x)    PBOut(8,x) //SCL
#define IIC_SDA(x)    PBOut(9,x) //SDA	 
#define READ_SDA   PBin(9)  //����SDA 

void SDA_IN(void);
void SDA_OUT(void);


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  


#endif

