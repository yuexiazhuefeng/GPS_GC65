/***************************************************************************
 * @Filename        -  
 * 
 * Copyright ?zzg Technology Ltd, 2005. All rights reserved.
 * 
 * @Brief           - 
 *      This file contains the implementation of the stm32f0xx iic driver
 *      for iic2.
 * 
 * 											   
 * @Revision        - 1.1.1.1 $
 * @Date            - 2013/04/11 11:50:35 $
 * @Author          - zzg $
 * @Project         - 
 *
 *
 * @Revision History- 
 * @Log             - $
 * 
***************************************************************************/
/***************************************************************************
 * Copyright  ?zzg Technology Ltd. 2005.  All rights reserved.
 ***************************************************************************/
/*
 * NOTE:
 * Comment banners infra can be edited or deleted as applicable.
 * They are included as ordinal reminders.
 *
 * Function banners are required.
 */
/***********************************************************************
 * Library header files (#include)
 **********************************************************************/
#include "stm32f0xx_i2c.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "main.h"
/***********************************************************************
 * Public user header files (#include)
 **********************************************************************/
//#include <includes.h> 
/***********************************************************************
 * Private user header files (#include)
 **********************************************************************/
//U24(28)-U8(63)  I2C-SDA    PC6  
//U24(24)-U8(62)  I2C-SCL    PD15   
/***********************************************************************
 * Constant definitions (#define)
 **********************************************************************/
 #define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

 
 //IO方向设置
//#define lsm330_SDA_IN()  {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=8<<24;}
//#define lsm330_SDA_OUT() {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=3<<24;}

//IO操作函数	 
//#define IIC_lsm330_SCL    PDout(15) //SCL
//#define IIC_lsm330_SDA    PCout(6) //SDA	 
#define READ_LSM330_SDA    GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7) //PCin(6)  //输入SDA 

/***********************************************************************
 * Macro definitions (#define)
 **********************************************************************/

/***********************************************************************
 * Data declarations of global data imported.  
 **********************************************************************/
   	   		   

/***********************************************************************
 * Function prototypes of global functions imported.  
 **********************************************************************/

/***********************************************************************
 * Data definitions of global data exported.  
 **********************************************************************/


/***********************************************************************
 * Function prototypes of functions requiring global scope
 * not included elsewhere.  
 **********************************************************************/

/***********************************************************************
 * Static function prototypes. (Forward declarations)
 **********************************************************************/
/* Forward declarations of private methods */


/***********************************************************************
 * Static variable definition (file scope).  
 **********************************************************************/

/***********************************************************************
 * Note: Function banner and body below are repeated for each function.
 **********************************************************************/
//IIC所有操作函数
void IIC_lsm330_Init(void);                //初始化IIC的IO口				 
void IIC_lsm330_Start(void);				//发送IIC开始信号
void IIC_lsm330_Stop(void);	  			//发送IIC停止信号
void IIC_lsm330_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_lsm330_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_lsm330_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_lsm330_Ack(void);					//IIC发送ACK信号
void IIC_lsm330_NAck(void);				//IIC不发送ACK信号

void IIC_lsm330_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_lsm330_Read_One_Byte(u8 daddr,u8 addr);	 

/***********************************************************************
 * Function banner
 *     |
 *     |
 *     v
 **********************************************************************/


/**************************************************************************/
/* 
 * @func      - 
 * @return    - n/a
 * @param     - n/a
 * @param     - n/a
 * @brief     - 
 * @notes:    - [if any]
 *              (1) ...
 *              (2) ...
 *
 */
/**************************************************************************/
/***********************************************************************
 * Function body (definition)  
 **********************************************************************/
/* static void funcname (void * data)
 * {
 *      etc...
 * }
 */
 
void delayus(u32 count)
{
	volatile u16 i;
	for (;count>0; count--)
	{
		for(i=0; i<1;i++)
		{
			i ++;
			i --;
		}
	}

}
//	#define soft_delay
//wei,20160824 for 采用一级优化
//毫秒延时函数
u32 volatile TimingDelay = 0;
void delayms(u32 nTime)
{
    IWDG_ReloadCounter();
	TimingDelay = nTime;
	while(TimingDelay != 0);
    IWDG_ReloadCounter();
}

#if 0//wei,20160824 for 采用一级优化
void delayms(u32 count)
{
	volatile u16 i;
       volatile u32 back_delayms_counter;
	volatile u32  tmp;
#ifdef soft_delay	
	IWDG_ReloadCounter();
	for (;count>0; count--)
	{
		delayus(1000);
		
	}
	IWDG_ReloadCounter();
#else
	if((u32)count > 0)
	{
		IWDG_ReloadCounter();
	    back_delayms_counter = delayms_counter;
 		tmp = delayms_counter;

		while((tmp-back_delayms_counter) < (u32)count)
		{
			i = delayms_counter;
	        i ++;
			i --;
			tmp = delayms_counter;
			tmp ++;
			tmp --;
		}
		IWDG_ReloadCounter();
	}
#endif
}
#endif
 //IO方向设置
//#define lsm330_SDA_IN()  {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=8<<24;}
void lsm330_SDA_IN(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);				 

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

}
//#define lsm330_SDA_OUT() {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=3<<24;}
void lsm330_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);					 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

}
//IO操作函数	 

//#define IIC_lsm330_SDA    PFout(7) //SDA	 

//#define READ_LSM330_SDA   PFin(7)  //输入SDA 


//初始化IIC
void IIC_lsm330_Init(void)
{	
#if 0
	
#else
	
	
	
	GPIO_InitTypeDef  GPIO_InitStructure; 	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF,ENABLE);	
	/* Configure I2C pins: SCL  */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
  
	delayus(100);
		/* Configure I2C pins: SDA  */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
//	while(1)
//	{
//		GPIOA->BSRR = GPIO_Pin_1;
//		delayms(100);
//		GPIOA->BRR = GPIO_Pin_1;
//		delayms(900);
//	}		
  

#endif	 
	delayus(100);	
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	GPIO_SetBits(GPIOF,GPIO_Pin_7);

	
//	GPIO_SetBits(GPIOD,GPIO_Pin_0);

}
//产生IIC起始信号
void IIC_lsm330_Start(void)
{
	lsm330_SDA_OUT();     //sda线输出
	GPIO_SetBits(GPIOF,GPIO_Pin_7);	  	  
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	delayus(4);
 	GPIO_ResetBits(GPIOF,GPIO_Pin_7);//START:when CLK is high,DATA change form high to low 
	delayus(4);
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_lsm330_Stop(void)
{
	lsm330_SDA_OUT();//sda线输出
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);
	GPIO_ResetBits(GPIOF,GPIO_Pin_7);//STOP:when CLK is high DATA change form low to high
 	delayus(4);
	GPIO_SetBits(GPIOF,GPIO_Pin_6); 
	GPIO_SetBits(GPIOF,GPIO_Pin_7);//发送I2C总线结束信号
	delayus(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_lsm330_Wait_Ack(void)
{
	u8 ucErrTime=0;
	lsm330_SDA_IN();      //SDA设置为输入  
	GPIO_SetBits(GPIOF,GPIO_Pin_7);
	delayus(1);	   
	GPIO_SetBits(GPIOF,GPIO_Pin_6);//应答信号是SCL时钟信号为高时，从设备拉低SDA线
	delayus(1);	 
	while(READ_LSM330_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
#if 1	//zzg test	
			IIC_lsm330_Stop();
#else
		GPIO_ResetBits(GPIOF,GPIO_Pin_6);//时钟输出0 	   
#endif
			return 1;
		}
	}
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_lsm330_Ack(void)
{
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);
	lsm330_SDA_OUT();
	GPIO_ResetBits(GPIOF,GPIO_Pin_7);
	delayus(2);
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	delayus(2);
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);
}
//不产生ACK应答		    
void IIC_lsm330_NAck(void)
{
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);
	lsm330_SDA_OUT();
	GPIO_SetBits(GPIOF,GPIO_Pin_7);
	delayus(2);
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	delayus(2);
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_lsm330_Send_Byte(u8 txd)
{                        
    u8 t;   
	  lsm330_SDA_OUT(); //将PF7的管脚设为推挽输出。	    
    GPIO_ResetBits(GPIOF,GPIO_Pin_6);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
       // IIC_lsm330_SDA=(txd&0x80)>>7;
	 if (0x80 == (txd&0x80))
	 {
		//IIC_lsm330_SDA = 1;
		GPIO_SetBits(GPIOF,GPIO_Pin_7);
		txd ++;
		txd --;
		 
	 }
	 else
	 {
		GPIO_ResetBits(GPIOF,GPIO_Pin_7);
     }
			
    txd<<=1; 	  
	delayus(2);   //对TEA5767这三个延时都是必须的
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	delayus(2); 
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);	
	delayus(2);
    }
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_lsm330_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	lsm330_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        GPIO_ResetBits(GPIOF,GPIO_Pin_6); 
        delayus(2);
		GPIO_SetBits(GPIOF,GPIO_Pin_6);
        receive<<=1;
        if(READ_LSM330_SDA)receive++;   
		delayus(1); 
    }					 
    if (!ack)
        IIC_lsm330_NAck();//发送nACK
    else
        IIC_lsm330_Ack(); //发送ACK   
    return receive;
}

#if 0


//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC_Init();
}
//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
		IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	//	IIC_Send_Byte(0XA0); 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delayms(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
 

#endif









