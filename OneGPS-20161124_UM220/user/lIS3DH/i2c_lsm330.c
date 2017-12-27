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
//IO�ڵ�ַӳ��
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
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

 
 //IO��������
//#define lsm330_SDA_IN()  {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=8<<24;}
//#define lsm330_SDA_OUT() {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=3<<24;}

//IO��������	 
//#define IIC_lsm330_SCL    PDout(15) //SCL
//#define IIC_lsm330_SDA    PCout(6) //SDA	 
#define READ_LSM330_SDA    GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7) //PCin(6)  //����SDA 

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
//IIC���в�������
void IIC_lsm330_Init(void);                //��ʼ��IIC��IO��				 
void IIC_lsm330_Start(void);				//����IIC��ʼ�ź�
void IIC_lsm330_Stop(void);	  			//����IICֹͣ�ź�
void IIC_lsm330_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_lsm330_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_lsm330_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_lsm330_Ack(void);					//IIC����ACK�ź�
void IIC_lsm330_NAck(void);				//IIC������ACK�ź�

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
//wei,20160824 for ����һ���Ż�
//������ʱ����
u32 volatile TimingDelay = 0;
void delayms(u32 nTime)
{
    IWDG_ReloadCounter();
	TimingDelay = nTime;
	while(TimingDelay != 0);
    IWDG_ReloadCounter();
}

#if 0//wei,20160824 for ����һ���Ż�
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
 //IO��������
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
//IO��������	 

//#define IIC_lsm330_SDA    PFout(7) //SDA	 

//#define READ_LSM330_SDA   PFin(7)  //����SDA 


//��ʼ��IIC
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
//����IIC��ʼ�ź�
void IIC_lsm330_Start(void)
{
	lsm330_SDA_OUT();     //sda�����
	GPIO_SetBits(GPIOF,GPIO_Pin_7);	  	  
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	delayus(4);
 	GPIO_ResetBits(GPIOF,GPIO_Pin_7);//START:when CLK is high,DATA change form high to low 
	delayus(4);
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_lsm330_Stop(void)
{
	lsm330_SDA_OUT();//sda�����
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);
	GPIO_ResetBits(GPIOF,GPIO_Pin_7);//STOP:when CLK is high DATA change form low to high
 	delayus(4);
	GPIO_SetBits(GPIOF,GPIO_Pin_6); 
	GPIO_SetBits(GPIOF,GPIO_Pin_7);//����I2C���߽����ź�
	delayus(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_lsm330_Wait_Ack(void)
{
	u8 ucErrTime=0;
	lsm330_SDA_IN();      //SDA����Ϊ����  
	GPIO_SetBits(GPIOF,GPIO_Pin_7);
	delayus(1);	   
	GPIO_SetBits(GPIOF,GPIO_Pin_6);//Ӧ���ź���SCLʱ���ź�Ϊ��ʱ�����豸����SDA��
	delayus(1);	 
	while(READ_LSM330_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
#if 1	//zzg test	
			IIC_lsm330_Stop();
#else
		GPIO_ResetBits(GPIOF,GPIO_Pin_6);//ʱ�����0 	   
#endif
			return 1;
		}
	}
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_lsm330_Send_Byte(u8 txd)
{                        
    u8 t;   
	  lsm330_SDA_OUT(); //��PF7�Ĺܽ���Ϊ���������	    
    GPIO_ResetBits(GPIOF,GPIO_Pin_6);//����ʱ�ӿ�ʼ���ݴ���
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
	delayus(2);   //��TEA5767��������ʱ���Ǳ����
	GPIO_SetBits(GPIOF,GPIO_Pin_6);
	delayus(2); 
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);	
	delayus(2);
    }
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_lsm330_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	lsm330_SDA_IN();//SDA����Ϊ����
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
        IIC_lsm330_NAck();//����nACK
    else
        IIC_lsm330_Ack(); //����ACK   
    return receive;
}

#if 0


//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	//	IIC_Send_Byte(0XA0); 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delayms(10);	 
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
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
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
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









