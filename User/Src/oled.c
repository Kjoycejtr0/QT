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
	@brief			IIC写入命令
	@param			IIC_Command：写入的命令
	@retval			无
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
	@brief			IIC写入数据
	@param			IIC_Data：数据
	@retval			无
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
	@brief			对OLED写入一个字节
	@param			dat：数据
					cmd：1，写诶数据；0，写入命令
	@retval			无
 */
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
	if(cmd) 
	  {
       Write_IIC_Data(dat); //写入数据
	  }
	else {
       Write_IIC_Command(dat); //写入命令
	}
}


/*
	@brief			设置数据写入的起始行、列
	@param			x： 列的起始低地址与起始高地址；0x00~0x0f：设置起始列低地址（在页寻址模式）；
						0x10~0x1f：设置起始列高地址（在页寻址模式）
					y：起始页地址 0~7
	@retval			无
 */
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);//写入页地址
	OLED_WR_Byte((x&0x0f),OLED_CMD);  //写入列的地址  低半字节
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);//写入列的地址 高半字节
}   	     	  


/*
	@brief			开显示
	@param			无
	@retval			无
 */ 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //设置OLED电荷泵
	OLED_WR_Byte(0X14,OLED_CMD);  //使能，开
	OLED_WR_Byte(0XAF,OLED_CMD);  //开显示
}


/*
	@brief			关显示
	@param			无
	@retval			无
 */  
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0XAE,OLED_CMD);  //关显示
	OLED_WR_Byte(0X8D,OLED_CMD);  //设置OLED电荷泵
	OLED_WR_Byte(0X10,OLED_CMD);  //失能，关
}		   			 


/*
	@brief			清屏
	@param			无
	@retval			无
 */	  
void OLED_Clear(void)  
{  
	unsigned char i,n;		    //定义变量
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //从0~7页依次写入
		OLED_WR_Byte (0x00,OLED_CMD);      //列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //列高地址  
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); //写入 0 清屏
	}
}

void OLED_Clear_Line(uint8_t Line)
{
	uint8_t n;
	OLED_WR_Byte (0xb0+Line,OLED_CMD);    //从0~7页依次写入
	OLED_WR_Byte (0x00,OLED_CMD);      //列低地址
	OLED_WR_Byte (0x10,OLED_CMD);      //列高地址  
	for(n=0;n<128;n++)
	{
		OLED_WR_Byte(0,OLED_DATA); //写入 0 清屏
	}
}


/*
	@brief			显示一个字符
	@param			x：起始列
					y：起始页，SIZE = 16占两页；SIZE = 12占1页
					chr：字符
	@retval			无
 */
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' '; //获取字符的偏移量	
		if(x>Max_Column-1){x=0;y=y+2;} //如果列数超出了范围，就从下2页的第0列开始
		OLED_Set_Pos(x,y+1); //一页就可以画完
		for(i=0;i<6;i++) //循环6次 ，占6列
		{
				OLED_WR_Byte(F6x8[c][i],OLED_DATA); //把字符画完
		}
			
}


/*
	@brief			计算m^n
	@param			m：输入的一个数
					n：输入数的次方
	@retval			result：一个数的n次方
 */
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/*
	@brief			在指定的位置，显示一个指定长度大小的数
	@param			x：起始列
					y：起始页
					num：数字
					len：数字的长度
					size：显示数字的大小
	@retval			无
 */		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size)
{         	
	unsigned char t,temp;  //定义变量
	unsigned char enshow=0;		//定义变量

	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;//取出输入数的每个位，由高到低
		if(enshow==0&&t<(len-1)) //enshow：是否为第一个数；t<(len-1)：判断是否为最后一个数
		{
			if(temp==0) //如果该数为0 
			{
				OLED_ShowChar(x+(size/2)*t,y,temp+'0');
//				OLED_ShowChar(x+(size/2)*t,y,' ');//显示 0 ；x+(size2/2)*t根据字体大小偏移的列数（8）
				continue; //跳过剩下语句，继续重复循环（避免重复显示）
			}else enshow=1; 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); //显示一个位；x+(size2/2)*t根据字体大小偏移的列数（8）
	}
} 


/*
	@brief			显示字符串
	@param			x：起始列
					y：起始页
					*chr：第一个字符首地址
	@retval			无
 */
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned char j=0; //定义变量

	while (chr[j]!='\0') //如果不是最后一个字符
	{		
		OLED_ShowChar(x,y,chr[j]); //显示字符
		x+=8; //列数加8 ，一个字符的列数占8
		if(x>=128){x=0;y+=2;} //如果x大于等于128，切换页，从该页的第一列显示
		j++; //下一个字符
	}
}


///*
//	@brief			显示中文
//	@param			x：起始列；一个字体占16列
//					y：起始页；一个字体占两页
//					no：字体的序号
//	@retval			无
// */
//void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
//{      			    
//	unsigned char t,adder=0; //定义变量

//	OLED_Set_Pos(x,y);	//从 x y 开始画点，先画第一页
//    for(t=0;t<16;t++) //循环16次，画第一页的16列
//		{
//			OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);//画no在数组位置的第一页16列的点
//			adder+=1; //数组地址加1
//     	}	
//		OLED_Set_Pos(x,y+1); //画第二页
//    for(t=0;t<16;t++)//循环16次，画第二页的16列
//		{	
//			OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);//画no在数组位置的第二页16列的点
//			adder+=1;//数组地址加1
//        }					
//}




/*
	@brief			OLED初始化函数
	@param			无
	@retval			无
 */				    
void OLED_Init(void)
{
	IIC_Init();
	delay_ms(200);	//延迟，由于单片机上电初始化比OLED快，所以必须加上延迟，等待OLED上复位完成

	OLED_WR_Byte(0xAE,OLED_CMD);	//关闭显示

	OLED_WR_Byte(0x00,OLED_CMD);	//设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);	//设置高列地址
	OLED_WR_Byte(0x40,OLED_CMD);	//设置起始行地址
	OLED_WR_Byte(0xB0,OLED_CMD);	//设置页地址

	OLED_WR_Byte(0x81,OLED_CMD); 	// 对比度设置，可设置亮度
	OLED_WR_Byte(0xFF,OLED_CMD);	//  265  

	OLED_WR_Byte(0xA1,OLED_CMD);	//设置段（SEG）的起始映射地址；column的127地址是SEG0的地址
	OLED_WR_Byte(0xA6,OLED_CMD);	//正常显示；0xa7逆显示

	OLED_WR_Byte(0xA8,OLED_CMD);	//设置驱动路数（16~64）
	OLED_WR_Byte(0x3F,OLED_CMD);	//64duty
	
	OLED_WR_Byte(0xC8,OLED_CMD);	//重映射模式，COM[N-1]~COM0扫描

	OLED_WR_Byte(0xD3,OLED_CMD);	//设置显示偏移
	OLED_WR_Byte(0x00,OLED_CMD);	//无偏移
	
	OLED_WR_Byte(0xD5,OLED_CMD);	//设置震荡器分频
	OLED_WR_Byte(0x80,OLED_CMD);	//使用默认值
	
	OLED_WR_Byte(0xD9,OLED_CMD);	//设置 Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);	//使用官方推荐值
	
	OLED_WR_Byte(0xDA,OLED_CMD);	//设置 com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);	//使用默认值
	
	OLED_WR_Byte(0xDB,OLED_CMD);	//设置 Vcomh，可调节亮度（默认）
	OLED_WR_Byte(0x40,OLED_CMD);	////使用官方推荐值
	
	OLED_WR_Byte(0x8D,OLED_CMD);	//设置OLED电荷泵
	OLED_WR_Byte(0x14,OLED_CMD);	//开显示
	
	OLED_WR_Byte(0xAF,OLED_CMD);	//开启OLED面板显示
	OLED_Clear();        //清屏
	OLED_Set_Pos(0,0); 	 //设置数据写入的起始行、列
}  


/*
	@brief			OLED滚屏函数，范围0~1页，水平向左
	@param			无
	@retval			无
 */	
void OLED_Scroll(void)
{
	OLED_WR_Byte(0x2E,OLED_CMD);	//关闭滚动
	OLED_WR_Byte(0x27,OLED_CMD);	//水平向左滚动
	OLED_WR_Byte(0x00,OLED_CMD);	//虚拟字节
	OLED_WR_Byte(0x00,OLED_CMD);	//起始页 0
	OLED_WR_Byte(0x00,OLED_CMD);	//滚动时间间隔
	OLED_WR_Byte(0x01,OLED_CMD);	//终止页 1
	OLED_WR_Byte(0x00,OLED_CMD);	//虚拟字节
	OLED_WR_Byte(0xFF,OLED_CMD);	//虚拟字节
	OLED_WR_Byte(0x2F,OLED_CMD);	//开启滚动
}

