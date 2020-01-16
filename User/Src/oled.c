/*includes----------------------------------------------*/
#include "oled.h"
#include "oledfont.h" 
#include 	"main.h"
#include 	"stm32f1xx_hal.h"
#include	"user_delay.h"
#include	"user_gpio.h"
#include 	"user_usb.h"
#include	"stm32f1xx_hal_i2c.h"
#include	"user_eeprom.h"
/*definition--------------------------------------------*/
//extern I2C_HandleTypeDef hi2c1;
/*
	@brief			IICд������
	@param			IIC_Command��д�������
	@retval			��
 */


static void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
  IIC_Send_Byte(0x78);
	IIC_Wait_Ack();
  IIC_Send_Byte(0x00);
	IIC_Wait_Ack();
  IIC_Send_Byte(IIC_Command);
	IIC_Wait_Ack();
  IIC_Stop(); 
}


/*
	@brief			IICд������
	@param			IIC_Data������
	@retval			��
 */
static void Write_IIC_Data(unsigned char IIC_Data)
{
	IIC_Start();
  IIC_Send_Byte(0x78);
	IIC_Wait_Ack();
  IIC_Send_Byte(0x40);	
	IIC_Wait_Ack();
  IIC_Send_Byte(IIC_Data);
	IIC_Wait_Ack();
  IIC_Stop();	
}

/*
	@brief			��OLEDд��һ���ֽ�
	@param			dat������
					cmd��1��д�����ݣ�0��д������
	@retval			��
 */
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
	if(cmd) 
	  {
       Write_IIC_Data(dat); //д������
	  }
	else {
       Write_IIC_Command(dat); //д������
	}
}


/*
	@brief			��������д�����ʼ�С���
	@param			x�� �е���ʼ�͵�ַ����ʼ�ߵ�ַ��0x00~0x0f��������ʼ�е͵�ַ����ҳѰַģʽ����
						0x10~0x1f��������ʼ�иߵ�ַ����ҳѰַģʽ��
					y����ʼҳ��ַ 0~7
	@retval			��
 */
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);//д��ҳ��ַ
	OLED_WR_Byte((x&0x0f),OLED_CMD);  //д���еĵ�ַ  �Ͱ��ֽ�
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);//д���еĵ�ַ �߰��ֽ�
}   	     	  


/*
	@brief			����ʾ
	@param			��
	@retval			��
 */ 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //����OLED��ɱ�
	OLED_WR_Byte(0X14,OLED_CMD);  //ʹ�ܣ���
	OLED_WR_Byte(0XAF,OLED_CMD);  //����ʾ
}


/*
	@brief			����ʾ
	@param			��
	@retval			��
 */  
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0XAE,OLED_CMD);  //����ʾ
	OLED_WR_Byte(0X8D,OLED_CMD);  //����OLED��ɱ�
	OLED_WR_Byte(0X10,OLED_CMD);  //ʧ�ܣ���
}		   			 


/*
	@brief			����
	@param			��
	@retval			��
 */	  
void OLED_Clear(void)  
{  
	unsigned char i,n;		    //�������
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //��0~7ҳ����д��
		OLED_WR_Byte (0x00,OLED_CMD);      //�е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //�иߵ�ַ  
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); //д�� 0 ����
	}
}

void OLED_Clear_Line(uint8_t Line)
{
	uint8_t n;
	OLED_WR_Byte (0xb0+Line,OLED_CMD);    //��0~7ҳ����д��
	OLED_WR_Byte (0x00,OLED_CMD);      //�е͵�ַ
	OLED_WR_Byte (0x10,OLED_CMD);      //�иߵ�ַ  
	for(n=0;n<128;n++)
	{
		OLED_WR_Byte(0,OLED_DATA); //д�� 0 ����
	}
}


/*
	@brief			��ʾһ���ַ�
	@param			x����ʼ��
					y����ʼҳ��SIZE = 16ռ��ҳ��SIZE = 12ռ1ҳ
					chr���ַ�
	@retval			��
 */
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' '; //��ȡ�ַ���ƫ����	
		if(x>Max_Column-1){x=0;y=y+2;} //������������˷�Χ���ʹ���2ҳ�ĵ�0�п�ʼ
		OLED_Set_Pos(x,y+1); //һҳ�Ϳ��Ի���
		for(i=0;i<6;i++) //ѭ��6�� ��ռ6��
		{
				OLED_WR_Byte(F6x8[c][i],OLED_DATA); //���ַ�����
		}
			
}


/*
	@brief			����m^n
	@param			m�������һ����
					n���������Ĵη�
	@retval			result��һ������n�η�
 */
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/*
	@brief			��ָ����λ�ã���ʾһ��ָ�����ȴ�С����
	@param			x����ʼ��
					y����ʼҳ
					num������
					len�����ֵĳ���
					size����ʾ���ֵĴ�С
	@retval			��
 */		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size)
{         	
	unsigned char t,temp;  //�������
	unsigned char enshow=0;		//�������

	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;//ȡ����������ÿ��λ���ɸߵ���
		if(enshow==0&&t<(len-1)) //enshow���Ƿ�Ϊ��һ������t<(len-1)���ж��Ƿ�Ϊ���һ����
		{
			if(temp==0) //�������Ϊ0 
			{
				OLED_ShowChar(x+(size/2)*t,y,temp+'0');
//				OLED_ShowChar(x+(size/2)*t,y,' ');//��ʾ 0 ��x+(size2/2)*t���������Сƫ�Ƶ�������8��
				continue; //����ʣ����䣬�����ظ�ѭ���������ظ���ʾ��
			}else enshow=1; 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); //��ʾһ��λ��x+(size2/2)*t���������Сƫ�Ƶ�������8��
	}
} 


/*
	@brief			��ʾ�ַ���
	@param			x����ʼ��
					y����ʼҳ
					*chr����һ���ַ��׵�ַ
	@retval			��
 */
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned char j=0; //�������

	while (chr[j]!='\0') //����������һ���ַ�
	{		
		OLED_ShowChar(x,y,chr[j]); //��ʾ�ַ�
		x+=8; //������8 ��һ���ַ�������ռ8
		if(x>=128){x=0;y+=2;} //���x���ڵ���128���л�ҳ���Ӹ�ҳ�ĵ�һ����ʾ
		j++; //��һ���ַ�
	}
}


///*
//	@brief			��ʾ����
//	@param			x����ʼ�У�һ������ռ16��
//					y����ʼҳ��һ������ռ��ҳ
//					no����������
//	@retval			��
// */
//void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
//{      			    
//	unsigned char t,adder=0; //�������

//	OLED_Set_Pos(x,y);	//�� x y ��ʼ���㣬�Ȼ���һҳ
//    for(t=0;t<16;t++) //ѭ��16�Σ�����һҳ��16��
//		{
//			OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);//��no������λ�õĵ�һҳ16�еĵ�
//			adder+=1; //�����ַ��1
//     	}	
//		OLED_Set_Pos(x,y+1); //���ڶ�ҳ
//    for(t=0;t<16;t++)//ѭ��16�Σ����ڶ�ҳ��16��
//		{	
//			OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);//��no������λ�õĵڶ�ҳ16�еĵ�
//			adder+=1;//�����ַ��1
//        }					
//}




/*
	@brief			OLED��ʼ������
	@param			��
	@retval			��
 */				    
void OLED_Init(void)
{
	IIC_Init();
	delay_ms(200);	//�ӳ٣����ڵ�Ƭ���ϵ��ʼ����OLED�죬���Ա�������ӳ٣��ȴ�OLED�ϸ�λ���

	OLED_WR_Byte(0xAE,OLED_CMD);	//�ر���ʾ

	OLED_WR_Byte(0x00,OLED_CMD);	//���õ��е�ַ
	OLED_WR_Byte(0x10,OLED_CMD);	//���ø��е�ַ
	OLED_WR_Byte(0x40,OLED_CMD);	//������ʼ�е�ַ
	OLED_WR_Byte(0xB0,OLED_CMD);	//����ҳ��ַ

	OLED_WR_Byte(0x81,OLED_CMD); 	// �Աȶ����ã�����������
	OLED_WR_Byte(0xFF,OLED_CMD);	//  265  

	OLED_WR_Byte(0xA1,OLED_CMD);	//���öΣ�SEG������ʼӳ���ַ��column��127��ַ��SEG0�ĵ�ַ
	OLED_WR_Byte(0xA6,OLED_CMD);	//������ʾ��0xa7����ʾ

	OLED_WR_Byte(0xA8,OLED_CMD);	//��������·����16~64��
	OLED_WR_Byte(0x3F,OLED_CMD);	//64duty
	
	OLED_WR_Byte(0xC8,OLED_CMD);	//��ӳ��ģʽ��COM[N-1]~COM0ɨ��

	OLED_WR_Byte(0xD3,OLED_CMD);	//������ʾƫ��
	OLED_WR_Byte(0x00,OLED_CMD);	//��ƫ��
	
	OLED_WR_Byte(0xD5,OLED_CMD);	//����������Ƶ
	OLED_WR_Byte(0x80,OLED_CMD);	//ʹ��Ĭ��ֵ
	
	OLED_WR_Byte(0xD9,OLED_CMD);	//���� Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);	//ʹ�ùٷ��Ƽ�ֵ
	
	OLED_WR_Byte(0xDA,OLED_CMD);	//���� com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);	//ʹ��Ĭ��ֵ
	
	OLED_WR_Byte(0xDB,OLED_CMD);	//���� Vcomh���ɵ������ȣ�Ĭ�ϣ�
	OLED_WR_Byte(0x40,OLED_CMD);	////ʹ�ùٷ��Ƽ�ֵ
	
	OLED_WR_Byte(0x8D,OLED_CMD);	//����OLED��ɱ�
	OLED_WR_Byte(0x14,OLED_CMD);	//����ʾ
	
	OLED_WR_Byte(0xAF,OLED_CMD);	//����OLED�����ʾ
	OLED_Clear();        //����
	OLED_Set_Pos(0,0); 	 //��������д�����ʼ�С���
}  


/*
	@brief			OLED������������Χ0~1ҳ��ˮƽ����
	@param			��
	@retval			��
 */	
void OLED_Scroll(void)
{
	OLED_WR_Byte(0x2E,OLED_CMD);	//�رչ���
	OLED_WR_Byte(0x27,OLED_CMD);	//ˮƽ�������
	OLED_WR_Byte(0x00,OLED_CMD);	//�����ֽ�
	OLED_WR_Byte(0x00,OLED_CMD);	//��ʼҳ 0
	OLED_WR_Byte(0x00,OLED_CMD);	//����ʱ����
	OLED_WR_Byte(0x01,OLED_CMD);	//��ֹҳ 1
	OLED_WR_Byte(0x00,OLED_CMD);	//�����ֽ�
	OLED_WR_Byte(0xFF,OLED_CMD);	//�����ֽ�
	OLED_WR_Byte(0x2F,OLED_CMD);	//��������
}

