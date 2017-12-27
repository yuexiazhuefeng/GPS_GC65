/***************************************************************************
 * @Filename        -   usart.c
 * 
 * Copyright ?zzg Technology Ltd, 2005. All rights reserved.
 * 
 * @Brief           - 
 *      This file contains the implementation of the stm32f103 UART driver
 *      for UARTS 1 ,2,3,4,5.
 * 
 * 											   
 * @Revision        - 1.1.1.1 $
 * @Date            - 2012/05/11 11:50:35 $
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
#include "main.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "led.h"


/***********************************************************************
 * Public user header files (#include)
 **********************************************************************/

/***********************************************************************
 * Private user header files (#include)
 **********************************************************************/

/***********************************************************************
 * Constant definitions (#define)
 **********************************************************************/


#if 0
 const    unsigned char sirf4_cmd00[] = {"$PSRF103,00,00,00,01*24\r\n"};    //$PSRF103,00,00,00,01*24
 const    unsigned char sirf4_cmd01[] = {"$PSRF103,01,00,00,01*25\r\n"};    //$PSRF103,01,00,00,01*25
 const    unsigned char sirf4_cmd02[] = {"$PSRF103,02,00,00,01*26\r\n"};    //$PSRF103,02,00,00,01*26
 const    unsigned char sirf4_cmd03[] = {"$PSRF103,03,00,00,01*27\r\n"};    //$PSRF103,03,00,00,01*27
 const    unsigned char sirf4_cmd04[] = {"$PSRF103,04,00,00,01*20\r\n"};    //$PSRF103,04,00,00,01*20
 const    unsigned char sirf4_cmd05[] = {"$PSRF103,05,00,00,01*21\r\n"};    //$PSRF103,05,00,00,01*21
 const    unsigned char sirf4_cmd06[] = {"$PSRF103,06,00,00,01*22\r\n"};    //$PSRF103,06,00,00,01*22
#else
// UBLOX 设置命令
 const unsigned char ubx_cmd01[] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x01,0x00,0x01,0x01,0x01,0x01,0x09,0x56};//VTG
 const unsigned char ubx_cmd02[] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x01,0x00,0x01,0x01,0x01,0x01,0x04,0x33};//GGA
 const unsigned char ubx_cmd03[] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x01,0x00,0x01,0x01,0x01,0x01,0x06,0x41};//GSA
 const unsigned char ubx_cmd04[] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x01,0x00,0x01,0x01,0x01,0x01,0x07,0x48};//GSV
 const unsigned char ubx_cmd05[] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x05,0x3A};//GLL
 const unsigned char ubx_cmd06[] = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x5B};//ZDA
 const unsigned char ubx_cmd07[] = {0xB5,0x62,0x06,0x16,0x08,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x26,0x97};//DIS_SBAS
 const unsigned char ubx_cmd08[] = {0xB5,0x62,0x06,0x24,0x24,0x00,0xFF,0xFF,0x04,0x03,0x00,0x00,0x00,0x00,0x10,0x27,
										  0x00,0x00,0x05,0x00,0xFA,0x00,0xFA,0x00,0x32,0x00,0x2C,0x01,0x64,0x00,0x00,0x00,
										  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x46,0x8C};//CFG_NAV5
#endif
/***********************************************************************
 * Macro definitions (#define)
 **********************************************************************/
#define GPRS_FLAG    0x7e
/***********************************************************************
 * Data declarations of global data imported.  
 **********************************************************************/
//进入A-GPS状态
extern u8 in_agps;
//LJW,20161124,这个数组目前只是用来进行参数设置用的了。extern  unsigned char g_ucGPRSCmdBuffback[GPRS_BUFF_SIZE];
extern  unsigned char g_ucGPRSCmdBuffback[GPRS_BACK_BUFF_SIZE];                                          
/***********************************************************************
 * Function prototypes of global functions imported.  
 **********************************************************************/
volatile unsigned char ManualRx0 = 0; 
//wei,for AT+QISTAT   u8 gUart0RxCnt = 0;
u16 gUart0RxCnt = 0;//wei,for AT+QISTAT 
u8 gps_reclen = 0;

uart_ctl_t UART1_ctl_t;
uart_ctl_t UART2_ctl_t;

//ljw,20161126for um220 unsigned char GPRSCmdCount = 0;
u16 GPRSCmdCount = 0;//ljw,20161126for um220 
//检测到数据头标志　0X7E
unsigned char volatile proGPRSCmdFlag = 0;


u16 agps_len_to_send = 0;
//AGPS　超时
u32 AGPS_ov=0;

/***********************************************************************
 * Data definitions of global data exported.  
 **********************************************************************/

//AGPS 桢头
u32  fB5 = 0;
u8 UART1_tbuf[UART1_tbuf_len];
u8 UART2_rbuf[UART2_rbuf_len]; 
u32  fB5s = 0; 
  //u8 UART2_tbuf[UART2_tbuf_len];


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


/*
**-----------------------------------------------------------------------------
**
**	??:
**		GPS????.
**
**	??:
**		?
**
**	??:
**		?
**
**-----------------------------------------------------------------------------
*/
u8 GPS_BaudRate_FLAG = 0;//ljw,20160826 x05a,GPS波特率正确
////ljw,20160826 x05a void UbxConfig(void)
u8 UbxConfig(void)
{
    u8 i;
#if 0//ljw.20160826,atgm332d
	//return;
	//DebugOut("\r\n************* Config Ublox ************\r\n\r\n",0);
//zzgtest
#if 0
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)sirf4_cmd00,strlen(sirf4_cmd00)); 
	delayms(50);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)sirf4_cmd01,strlen(sirf4_cmd01));
	delayms(50);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)sirf4_cmd02,strlen(sirf4_cmd02));
	delayms(50);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)sirf4_cmd03,strlen(sirf4_cmd03)); 
	delayms(50);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	//UART6_SendData(&UART2_ctl_t,(unsigned char *)sirf4_cmd04,strlen(sirf4_cmd04)); SoftDelay_ms(30);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)sirf4_cmd05,strlen(sirf4_cmd05)); 
	delayms(50);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)sirf4_cmd06,strlen(sirf4_cmd06)); 
	delayms(50);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
#else
//ublox 6m
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)ubx_cmd01,sizeof(ubx_cmd01)); 
	delayms(500);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)ubx_cmd02,sizeof(ubx_cmd02));
	delayms(500);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)ubx_cmd03,sizeof(ubx_cmd03));
	delayms(500);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)ubx_cmd04,sizeof(ubx_cmd04)); 
	delayms(500);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	//UART6_SendData(&UART2_ctl_t,(unsigned char *)sirf4_cmd04,strlen(sirf4_cmd04)); SoftDelay_ms(30);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)ubx_cmd05,sizeof(ubx_cmd05)); 
	delayms(500);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)ubx_cmd06,sizeof(ubx_cmd06)); 
	delayms(500);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)ubx_cmd07,sizeof(ubx_cmd07)); 
	delayms(500);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
	ddi_uart_Write(&UART2_ctl_t,(unsigned char *)ubx_cmd08,sizeof(ubx_cmd08));
	delayms(500);//DebugOut("\r  Ubx_cmd01 was Executed!\r\n",0);
#endif
	//DebugOut("\r\n***************** Done! ***************\r\n\r\n",0);
#endif //ljw,20160826 atgm332d

#ifdef ZHONGKEWEI_GPS //ljw,20161125,中科微的GPS模块  
//wei,20160826 test<
    UART2_ctl_t.Parity = Parity_No;
    ddi_uart_open(&UART2_ctl_t,9600);
//    ddi_uart_open(&UART2_ctl_t,115200);
    delayms(10);
    ddi_uart_Write(&UART2_ctl_t,"$PCAS01,5*19\r\n",strlen("$PCAS01,5*19\r\n")); //ljw,20161130  115200-"$PCAS01,5*19\r\n"; 9600-"$PCAS01,1*1D\r\n"
    ddi_uart_Write(&UART2_ctl_t,"$PCAS00*01\r\n",strlen("$PCAS00*01\r\n")); 
    delayms(10);
    UART2_ctl_t.Parity = Parity_No;
    ddi_uart_open(&UART2_ctl_t,115200);
    
    GPS_BaudRate_FLAG = 0x00;
    for(i=0;i<60;i++)
    {
        delayms(50);
        if(GPS_BaudRate_FLAG == 0x5a)
        {
//            GPS_Reset();
            ddi_uart_Write(&UART2_ctl_t,"$PCAS03,1,0,0,0,1,0,0,0*02\r\n",strlen("$PCAS03,1,0,0,0,1,0,0,0*02\r\n"));            
            return GPS_BaudRate_FLAG;
        }
    }
    if(i>=60)//ljw,20160928 115200波特率失败切会9600
    {
        UART2_ctl_t.Parity = Parity_No;
        ddi_uart_open(&UART2_ctl_t,9600);
        delayms(10);
    }
    return 0;
#endif
#ifdef UM220_GPS  //LJW,20161128
    UART2_ctl_t.Parity = Parity_No;
//    ddi_uart_open(&UART2_ctl_t,9600);
    ddi_uart_open(&UART2_ctl_t,115200);
    delayms(10);
    ddi_uart_Write(&UART2_ctl_t,"$PCAS01,1*1D\r\n",strlen("$PCAS01,1*1D\r\n")); //ljw,20161130  115200-"$PCAS01,5*19\r\n"; 9600-"$PCAS01,1*1D\r\n"
//    delayms(10);
    ddi_uart_Write(&UART2_ctl_t,"$PCAS00*01\r\n",strlen("$PCAS00*01\r\n"));
    delayms(10);
//    ddi_uart_Write(&UART2_ctl_t,"$CFGPRT,,0,115200,,*0F\r\n",strlen("$CFGPRT,,0,115200,,*0F\r\n"));//$CFGPRT,,,115200,*E3\r\n  $CFGPRT,,,9600,,*A7
    ddi_uart_Write(&UART2_ctl_t,"$CFGPRT,,0,9600,,*07\r\n",strlen("$CFGPRT,,0,9600,,*07\r\n"));
    delayms(10);
    UART2_ctl_t.Parity = Parity_No;
    ddi_uart_open(&UART2_ctl_t,9600);
    
    GPS_BaudRate_FLAG = 0x00;
    for(i=0;i<60;i++)
    {
        delayms(50);
        if(GPS_BaudRate_FLAG == 0x5a)
        {
//            GPS_Reset();
            ddi_uart_Write(&UART2_ctl_t,"$PCAS03,1,0,0,0,1,0,0,0*02\r\n",strlen("$PCAS03,1,0,0,0,1,0,0,0*02\r\n"));
            //ddi_uart_Write(&UART2_ctl_t,"$PDTINFO,*62\r\n",strlen("$PDTINFO,*62\r\n"));
            return GPS_BaudRate_FLAG;
        }
    }
    return 0;
//    if(i>=60)//ljw,20160928 115200波特率失败切会9600
//    {
//        UART2_ctl_t.Parity = Parity_No;
//        ddi_uart_open(&UART2_ctl_t,9600);
//        delayms(10);
//    }
//    ddi_uart_Write(&UART2_ctl_t,"$PDTINFO,*62\r\n",strlen("$PDTINFO,*62\r\n"));
//    delayms(1000);
#endif
//wei,20160826 test>
}

u8 in_gpsModule = 0;//
u8 Get_GPS_module(void)
{
    u8 i =0 ;
    u8 j =0;
    in_gpsModule = 0x5a;
    for(i=0;i<3;i++)
    {
        ddi_uart_Write(&UART2_ctl_t,"$PDTINFO,*62\r\n",strlen("$PDTINFO,*62\r\n"));
        delayms(50);
        ddi_uart_Write(&UART2_ctl_t,"$PCAS06,0*1B\r\n",strlen("$PCAS06,0*1B\r\n"));
        for(j=0;j<200;j++)
        {
            if(GPS_module)
            {
                in_gpsModule = 0x00;
               return 1;
            }
            delayms(5);
        }        
        
        //delayms(1000);
    }
    in_gpsModule = 0x00;
    return 0;
}

#if 0//wei,20160803 for space<
void GPS_on(void)
{
	set_GPS_POWERON (TRUE);	
	//delayms(500);
	//UbxConfig();
}
#endif //wei,20160803 for space>
//wei,20160530<
//参数数明， ENABLE,打开串口, DISABLE,关闭串口
void ddi_uart1_init(FunctionalState New_state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		//USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	if(New_state == ENABLE)
	{		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		UART1_ctl_t.reg = USART1;
		UART1_ctl_t.Parity = 0;

		/* GPIOA Clocks enable */
		RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);

		GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);        
		/*
		*  USART1_TX -> PA9 , USART1_RX ->        PA10
		*/                                
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);   

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
		//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);   
#if TEST_SYSCLK
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_0) ;	
		//mco	  PA.8
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //GPIO_Mode_IN;//GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);  
		/* Output System Clock on MCO pin */
		RCC_MCOConfig(RCC_MCOSource_SYSCLK);  
#endif
		//Usart1 NVIC ??

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//
		NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
		USART_Cmd(USART1, ENABLE);  
	}
	else
	{
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;		//
		NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
		USART_Cmd(USART1, DISABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;                 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//WEI GPIO_Mode_AN;
		//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
	}
}
//wei,20160530>

//wei,20160802 for space<
void ddi_uart2_init(FunctionalState New_state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		//USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;    
    
	if(New_state == ENABLE)
	{		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        UART2_ctl_t.reg = USART2;
        UART2_ctl_t.Parity = 0;
          /* GPIOA Clocks enable */
        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);    
        
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);        
        /*
        *  USART2_TX -> PA2 , USART2_RX ->        PA3
        */                                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
        GPIO_Init(GPIOA, &GPIO_InitStructure);   

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
        //  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
        GPIO_Init(GPIOA, &GPIO_InitStructure);  	

        //Usart2 NVIC ??

        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//
        NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
        USART_Cmd(USART2, ENABLE);
	}
//	else
//	{
//		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;		//
//		NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
//		USART_Cmd(USART2, DISABLE);
//		
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;                 
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//WEI GPIO_Mode_AN;
//		//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//		GPIO_Init(GPIOA, &GPIO_InitStructure); 
//	}
}
//wei,20160802 for space>
#if 0//wei,20160802 for space
void ddi_uart_init(void)
{
	//GPIO????
	GPIO_InitTypeDef GPIO_InitStructure;
	//USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	UART1_ctl_t.reg = USART1;

	UART1_ctl_t.Parity = 0;

	/* GPIOA Clocks enable */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);        
	/*
	*  USART1_TX -> PA9 , USART1_RX ->        PA10
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
#if TEST_SYSCLK
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_0) ;	
	//mco	  PA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //GPIO_Mode_IN;//GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	/* Output System Clock on MCO pin */
	RCC_MCOConfig(RCC_MCOSource_SYSCLK);  
#endif
	//Usart1 NVIC ??

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//
	NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
	USART_Cmd(USART1, ENABLE);  


	UART2_ctl_t.reg = USART2;
	UART2_ctl_t.Parity = 0;
	  /* GPIOA Clocks enable */
//  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);        
	/*
	*  USART2_TX -> PA2 , USART2_RX ->        PA3
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);  	

	//Usart2 NVIC ??

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//
	NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
	USART_Cmd(USART2, ENABLE);
}
#endif //wei,20160802 for space

/**********************************************************************
//??:??COM ???????
//??1:????????
//??2:???
//??3:?????
//??:????????,????NULL
**********************************************************************/
uart_ctl_t* ddi_uart_open(uart_ctl_t *pCOM,u32 BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
//USART ?????

//????
	USART_Cmd(pCOM->reg, DISABLE);   
	//???? 
	USART_ITConfig(pCOM->reg, USART_IT_RXNE, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_WU, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_CM, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_EOB, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_RTO, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_PE, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_TXE, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_TC, DISABLE);//????	
	USART_ITConfig(pCOM->reg, USART_IT_IDLE, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_LBD, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_CTS, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_ERR, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_ORE, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_NE, DISABLE);//????
	USART_ITConfig(pCOM->reg, USART_IT_FE, DISABLE);//????		
	USART_DeInit(pCOM->reg);
	
   
	USART_InitStructure.USART_BaudRate = BaudRate;//pCOM->rate;//?????9600;
	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//pCOM->StopBit;//USART_StopBits_1;

	if (pCOM->Parity == 1)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Odd;//pCOM->Parity;//USART_Parity_No;
	//	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//pCOM->DataBit;//USART_WordLength_8b;
	}
	else	
	if (pCOM->Parity == 2)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Even;//pCOM->Parity;//USART_Parity_No;
	//	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//pCOM->DataBit;//USART_WordLength_8b;
	}
	else
	{
		USART_InitStructure.USART_Parity = USART_Parity_No;//pCOM->Parity;//USART_Parity_No;
	//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//pCOM->DataBit;//USART_WordLength_8b;
	}
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//pCOM->DataBit;//USART_WordLength_8b;	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(pCOM->reg, &USART_InitStructure);


	USART_ITConfig(pCOM->reg, USART_IT_RXNE, ENABLE);//????

	USART_Cmd(pCOM->reg, ENABLE);                    //???? 

	return pCOM;

}

#if 1//copy
void USART1_IRQHandler(void)                	//??1??????
{
	u8 fUBLOXavailable = 0;  
    static u8 first=0;

	unsigned char turechar;    
	u8 RXB0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		RXB0 = USART_ReceiveData(USART1);
#if 1//lij
        if (1 == EN_initialize)
        {
            if((RXB0 == '%') ||(RXB0 == '<')) 	// 接受到状态同步字符
            {
                if (!g_ucInitSettingFlag)
                {
                    g_ucInitSettingFlag = 1;
                }
                else
                {
                    if (g_ucGPRSCmdMissionFlag)
                    GPRSCmdCount = 0;
                }
//LJW,20161124,这个数组目前只是用来进行参数设置用的了。                if(GPRSCmdCount >= GPRS_BUFF_SIZE)
                if(GPRSCmdCount >= GPRS_BACK_BUFF_SIZE)
                    GPRSCmdCount = 0;		 	
                    //  g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
                g_ucGPRSCmdBuffback[GPRSCmdCount++] = RXB0;
            }
            else
            {
//LJW,20161124,这个数组目前只是用来进行参数设置用的了。                if (GPRSCmdCount>=GPRS_BUFF_SIZE)
                if (GPRSCmdCount>=GPRS_BACK_BUFF_SIZE)
                    GPRSCmdCount = 0;		 	
                // g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
                g_ucGPRSCmdBuffback[GPRSCmdCount++] = RXB0;
            }
        }
#endif //ljw
#if 1//ljw
#if 1
#ifdef ZHONGKEWEI_GPS //ljw,20161125,中科微的GPS模块    
		else
		if(0x5a == in_agps)    //进入接收A GPS 的状态，等B562
		{
			g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
			//做个超时标志，有数，有置位，定时器１秒测一次，没数据就说明结束了
			AGPS_ov = 0;
			if((GPRSCmdCount>1) && (g_ucGPRSCmdBuff[GPRSCmdCount-2] == 0xBA) && ((g_ucGPRSCmdBuff[GPRSCmdCount-1]) == 0xCE))
			{
                // if( (fB5&(1<<1))  != (1<<1)) // 刚开始的，还没有数据
                fB5  ++;
                fB5  --;	
                fB5  ++;
                 
                if (first == 0)
                {
                    memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
                    g_ucGPRSCmdBuff[0] = 0xba;
                    g_ucGPRSCmdBuff[1] = 0xce;
                    GPRSCmdCount = 2;
                    //fB5 |= (1<<1);
                    first = 0x5a;
                }
                else
                {
                 // if((fB5&(1<<5) != (1<<5)))
                    if((fB5 >= 2))
                    {
                        agps_len_to_send = GPRSCmdCount-2;
//LJW,20161124,这个数组目前只是用来进行参数设置用的了。                        memset(g_ucGPRSCmdBuffback,0,GPRS_BUFF_SIZE);
                        memset(g_ucGPRSCmdBuffback,0,GPRS_BACK_BUFF_SIZE);
                        memcpy(g_ucGPRSCmdBuffback,g_ucGPRSCmdBuff,agps_len_to_send);
                        memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
                        g_ucGPRSCmdBuff[0] = 0xba;
                        g_ucGPRSCmdBuff[1] = 0xce;
                        GPRSCmdCount = 2;
                        //fB5 |= (1<<1);	
                    }
                }
			}
		}
#endif
#ifdef UM220_GPS  //ljw,20161125,UM220
        else
		if(0x5a == in_agps)    //进入接收A GPS 的状态，等B562
		{
			g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
            AGPS_rec_len++;
			//做个超时标志，有数，有置位，定时器１秒测一次，没数据就说明结束了
            if((GPRSCmdCount > 6)&&(fB5 == 0))//判断  +RECEIVE:
            {
                if(NULL !=strstr((char *)g_ucGPRSCmdBuff,"+RECEIVE:"))
                {
                    memcpy(g_ucGPRSCmdBuff,"+RECEIVE:",strlen("+RECEIVE:"));
                    GPRSCmdCount = strlen("+RECEIVE:");
                    AGPS_rec_len = GPRSCmdCount;
                    AGPS_ov = 0;
                    fB5 = GC65_RECE;
                }
#if 0//ljw,20161202,这个原来是使用不主动上报数据的
                if(NULL !=strstr((char *)g_ucGPRSCmdBuff,"+QIRD:"))
                {
                    memcpy(g_ucGPRSCmdBuff,"\r\n+QIRD:",strlen("\r\n+QIRD:"));
                    GPRSCmdCount = strlen("\r\n+QIRD:");
                    AGPS_rec_len = GPRSCmdCount;
                    AGPS_ov = 0;
                    fB5 = GC65_RECE;
                }
#endif
#if 0 //ljw,20161128不要，什么时候检测到QIRD，直接用上面的方式处理，较为包容。
                else if(GPRSCmdCount>100)//ljw,20161128,这里加个兼容，防止QIRD:在靠后面才接收到
                {
                    GPRSCmdCount =0;
                    AGPS_rec_len = 0;
                }
#endif
            }
//            if((fB5 == GC65_RECE)&&(AGPS_rec_len >=1500))
//                fB5 = AGPS_START_SEND;
            if(GPRSCmdCount > 380)
                GPRSCmdCount =0;
			AGPS_ov = 0;

#if 0//ljw,20161125 注释掉中科微模块的函数
			if((GPRSCmdCount>1) && (g_ucGPRSCmdBuff[GPRSCmdCount-2] == 0xBA) && ((g_ucGPRSCmdBuff[GPRSCmdCount-1]) == 0xCE))
			{
                // if( (fB5&(1<<1))  != (1<<1)) // 刚开始的，还没有数据
                fB5  ++;
                fB5  --;	
                fB5  ++;
                 
                if (first == 0)
                {
                    memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
                    g_ucGPRSCmdBuff[0] = 0xba;
                    g_ucGPRSCmdBuff[1] = 0xce;
                    GPRSCmdCount = 2;
                    //fB5 |= (1<<1);
                    first = 0x5a;
                }
                else
                {
                 // if((fB5&(1<<5) != (1<<5)))
                    if((fB5 >= 2))
                    {
                        agps_len_to_send = GPRSCmdCount-2;
//LJW,20161124,这个数组目前只是用来进行参数设置用的了。                        memset(g_ucGPRSCmdBuffback,0,GPRS_BUFF_SIZE);
                        memset(g_ucGPRSCmdBuffback,0,GPRS_BACK_BUFF_SIZE);
                        memcpy(g_ucGPRSCmdBuffback,g_ucGPRSCmdBuff,agps_len_to_send);
                        memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
                        g_ucGPRSCmdBuff[0] = 0xba;
                        g_ucGPRSCmdBuff[1] = 0xce;
                        GPRSCmdCount = 2;
                        //fB5 |= (1<<1);	
                    }
                }
			}
#endif
		}
#endif
#endif	
#endif //ljw  
#if 1    //ljw       
		else
		{
 		    if(ManualRx0 != 1)
			{
			    if( proGPRSCmdFlag==0) //没检测到濉　   0x7E
			    {
					if(RXB0 == GPRS_FLAG)
					{
						proGPRSCmdFlag = 1;
						//保存到缓冲区
						GPRSCmdCount = 0;
						g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
					}
                    #if 0//ljw,20160906这里不应该接收，收也不会去处理
					else
					{
						if(gUart0RxCnt>=SIM900_BUFF_SIZE)
						   gUart0RxCnt = 0;		
						g_ucSim900Buff[gUart0RxCnt++] = RXB0;
					    // 还有其它的提示信息也需要处理，像'LOW POWER'之类的，模块自动发出来的，也要处理
	                    //或者是电话，短信之类的
					}
                    #endif//ljw,20160906这里不应该接收，收也不会去处理
				}
				else //剩下的数据也要收到缓冲区,当收到下一个GPRS_FLAG就认为一条数据收全了
				{
					if((RXB0 == GPRS_FLAG) && (g_ucGPRSCmdMissionFlag == 0))
					{
						proGPRSCmdFlag = 0;
						//保存到缓冲区
						 if (GPRSCmdCount>=GPRS_BUFF_SIZE)
						   	GPRSCmdCount = 0;
						g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
						//通知主任务处理GPRS数据
						g_ucGPRSCmdMissionFlag = 1;
					}
					else
					{
						 if (GPRSCmdCount>=GPRS_BUFF_SIZE)
						   	GPRSCmdCount = 0;
						g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
					}
					#if 0
						g_usACCOFFcounter =  0; //工作延时5 minutes
						//asciitohex(g_ucGPRSCmdBuff,strlen(g_ucGPRSCmdBuff));
			            //  memcpy(g_ucGPRSCmdBuff,tmpgprsbuf,(strlen(g_ucGPRSCmdBuff))/2);
			            EI();   
						SetGPRS();
						g_ucGPRSCmdMissionFlag = 0;	
					#endif
                }
			}
//ljw,20160906			else 
            else if(proGPRSCmdFlag==0)
#endif //lijw
			{
				 if (gUart0RxCnt>=SIM900_BUFF_SIZE)
				     gUart0RxCnt = 0;		
				 g_ucSim900Buff[gUart0RxCnt++] = RXB0;
	                    
			}
		}  		 
  }

#if 1
	USART_ClearITPendingBit(USART1, USART_IT_WU);
	USART_ClearITPendingBit(USART1, USART_IT_CM);
	USART_ClearITPendingBit(USART1, USART_IT_EOB);
	USART_ClearITPendingBit(USART1, USART_IT_RTO);
	USART_ClearITPendingBit(USART1, USART_IT_PE);
	//USART_ClearITPendingBit(USART1, USART_IT_TXE);
	//USART_ClearITPendingBit(USART1, USART_IT_TC);
	USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	USART_ClearITPendingBit(USART1, USART_IT_LBD);
	USART_ClearITPendingBit(USART1, USART_IT_CTS);
	USART_ClearITPendingBit(USART1, USART_IT_ERR);
	USART_ClearITPendingBit(USART1, USART_IT_ORE);
	USART_ClearITPendingBit(USART1, USART_IT_NE);
	USART_ClearITPendingBit(USART1, USART_IT_FE);
#endif
}
#endif
#if 0 //ljw,20160906
//UART TEST 
// 	 if((*ptmp == 0xBA) &&(*(ptmp+1) == 0xCE))
void USART1_IRQHandler(void)                	//??1??????
{
//	u8 Res;
//	u8 err1;
//	u32 len;

	u8 fUBLOXavailable = 0;  
    static u8 first=0;      

	unsigned char turechar;    
	u8 RXB0;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		RXB0 = USART_ReceiveData(USART1);

        if (1 == EN_initialize)
        {
            if((RXB0 == '%') ||(RXB0 == '<')) 	// 接受到状态同步字符
            {
                if (!g_ucInitSettingFlag)
                {
                    g_ucInitSettingFlag = 1;
                }
                else
                {
                    if (g_ucGPRSCmdMissionFlag)
                    GPRSCmdCount = 0;
                }
                if(GPRSCmdCount >= GPRS_BUFF_SIZE)
                    GPRSCmdCount = 0;		 	
                    //  g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
                    g_ucGPRSCmdBuffback[GPRSCmdCount++] = RXB0;
            }
            else
            {
                if (GPRSCmdCount>=GPRS_BUFF_SIZE)
                GPRSCmdCount = 0;		 	
                // g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
                g_ucGPRSCmdBuffback[GPRSCmdCount++] = RXB0;
            }
        }

#if 1
		else
		if(0x5a == in_agps)    //进入接收A GPS 的状态，等B562
		{
			g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;	
			//做个超时标志，有数，有置位，定时器１秒测一次，没数据就说明结束了
			AGPS_ov = 0;
			if((GPRSCmdCount>1) && (g_ucGPRSCmdBuff[GPRSCmdCount-2] == 0xBA) && ((g_ucGPRSCmdBuff[GPRSCmdCount-1]) == 0xCE))
			{
                // if( (fB5&(1<<1))  != (1<<1)) // 刚开始的，还没有数据
                fB5  ++;
                fB5  --;	
                fB5  ++;
                 
                if (first == 0)
                {
                    memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
                    g_ucGPRSCmdBuff[0] = 0xba;
                    g_ucGPRSCmdBuff[1] = 0xce;
                    GPRSCmdCount = 2;
                    //fB5 |= (1<<1);
                    first = 0x5a;
                }
                else
                {
                 // if((fB5&(1<<5) != (1<<5)))
                    if((fB5 >= 2))
                    {
                        agps_len_to_send = GPRSCmdCount-2;
                        memset(g_ucGPRSCmdBuffback,0,GPRS_BUFF_SIZE);
                        memcpy(g_ucGPRSCmdBuffback,g_ucGPRSCmdBuff,agps_len_to_send);
                        memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
                        g_ucGPRSCmdBuff[0] = 0xba;
                        g_ucGPRSCmdBuff[1] = 0xce;
                        GPRSCmdCount = 2;
                        //fB5 |= (1<<1);	
                    }
                }
			}
		}
#endif		
		else
		{
 		    if(ManualRx0 != 1)
			{
			    if( proGPRSCmdFlag==0) //没检测到濉　   0x7E
			    {
//ljw,20160906不严谨					if(RXB0 == GPRS_FLAG)
                    if((RXB0 == GPRS_FLAG)&&(g_ucGPRSCmdMissionFlag == 0))
					{
						proGPRSCmdFlag = 1;
						//保存到缓冲区
						GPRSCmdCount = 0;
						g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
					}
					else
					{
						if (gUart0RxCnt>=SIM900_BUFF_SIZE)
						   gUart0RxCnt = 0;		
						g_ucSim900Buff[gUart0RxCnt++] = RXB0;
									 // 还有其它的提示信息也需要处理，像'LOW POWER'之类的，模块自动发出来的，也要处理
	                         //或者是电话，短信之类的
					}
				}
				else //剩下的数据也要收到缓冲区,当收到下一个GPRS_FLAG就认为一条数据收全了
				{
					if((RXB0 == GPRS_FLAG)&&(g_ucGPRSCmdMissionFlag == 0))
					{
						proGPRSCmdFlag = 0;
						//保存到缓冲区
						 if (GPRSCmdCount>=GPRS_BUFF_SIZE)
						   	GPRSCmdCount = 0;
						g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
						//通知主任务处理GPRS数据
						g_ucGPRSCmdMissionFlag = 1;
					}
					else
					{
						 if (GPRSCmdCount>=GPRS_BUFF_SIZE)
						   	GPRSCmdCount = 0;
						g_ucGPRSCmdBuff[GPRSCmdCount++] = RXB0;
					}
					#if 0
						g_usACCOFFcounter =  0; //工作延时5 minutes
								
						//asciitohex(g_ucGPRSCmdBuff,strlen(g_ucGPRSCmdBuff));
			                        //  memcpy(g_ucGPRSCmdBuff,tmpgprsbuf,(strlen(g_ucGPRSCmdBuff))/2);
			                     EI();   
						SetGPRS();
						g_ucGPRSCmdMissionFlag = 0;	
					#endif
									
                            }
			}
			else
			{
				 if (gUart0RxCnt>=SIM900_BUFF_SIZE)
				     gUart0RxCnt = 0;		
				 g_ucSim900Buff[gUart0RxCnt++] = RXB0;
	                    
			}
		}  		 
  }
#if 1
	USART_ClearITPendingBit(USART1, USART_IT_WU);
	USART_ClearITPendingBit(USART1, USART_IT_CM);
	USART_ClearITPendingBit(USART1, USART_IT_EOB);
	USART_ClearITPendingBit(USART1, USART_IT_RTO);
	USART_ClearITPendingBit(USART1, USART_IT_PE);
	//USART_ClearITPendingBit(USART1, USART_IT_TXE);
	//USART_ClearITPendingBit(USART1, USART_IT_TC);
	USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	USART_ClearITPendingBit(USART1, USART_IT_LBD);
	USART_ClearITPendingBit(USART1, USART_IT_CTS);
	USART_ClearITPendingBit(USART1, USART_IT_ERR);
	USART_ClearITPendingBit(USART1, USART_IT_ORE);
	USART_ClearITPendingBit(USART1, USART_IT_NE);
	USART_ClearITPendingBit(USART1, USART_IT_FE);
#endif
  
  
}
#endif
//UART TEST 
//把gps的数据中GPRMC数据先保存在某个缓冲区

void USART2_IRQHandler(void)                	//??1??????
{
	u8 Res;
	u8 err1;
    static u8 dollar=0;//是否收到$标志
    static u8 gprmc= 0; //接收到需要的GPS数据头,GPRMC  GNRMC
    static u8 recv_complete_flag = 0;//wei,20160830GPS接收完成标志
    static u8 V_counter =0;//ljw,20160831 不定位计数器
	u8 tmpchar;
	u16 u16tmp=0;//wei.20160523 for mileage
	u8* ptmp = NULL;//wei.20160523 for mileage
	u8* ptmp1 = NULL;//wei.20160523 for mileage
//    u8* pCommard = NULL;//wei.20160907
//    u8* pTempBuf = NULL;//wei.20160907
//    u8  ucOne = 0;//wei.20160907
//    u8  ucTen = 0;//wei.20160907
    
	u8 UART2_rbuf_temp[UART2_rbuf_len]; //wei.20160523 for mileage  

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{
        tmpchar = USART_ReceiveData(USART2);
#if 1		
		if (gps_reclen >= UART2_rbuf_len)
			gps_reclen = 0;
		if((gprmc== 0) &&(dollar ==0) &&(tmpchar == '$'))//判断GPS数据头$
		{
			dollar  = 1;
			gps_reclen = 0;
			memset(UART2_rbuf,0,sizeof(UART2_rbuf));
			UART2_rbuf[gps_reclen++] = tmpchar;        //gps_reclen此时的值应该是1
		}
		else if ((gprmc== 0) &&(dollar == 1) && (gps_reclen<6))//$GPRMC头还没接收完
		{
			UART2_rbuf[gps_reclen++] = tmpchar;
		}
		else if ((gprmc== 0) && (dollar == 1) && (gps_reclen>=5))//$GPRMC头接收完了
		{
            if(in_gpsModule == 0x5a)
            {
                if(strstr((char *)UART2_rbuf,"PDTIN")) //PDTINFO
                {
                    GPS_module = UM220;//UM220模块
                }
                else if(strstr((char *)UART2_rbuf,"GPTXT"))
                {
                    GPS_module = ATGM332D;//ATGM332D模块
                }
            }
			if ((strstr((char *)UART2_rbuf,"GPRMC"))||(strstr((char *)UART2_rbuf,"GNRMC") ))
			{
                GPS_BaudRate_FLAG = 0x5a;//ljw,20160826波特率OK
				gprmc = 1;
				UART2_rbuf[gps_reclen++] = tmpchar;
			}
			else
			{
				dollar = 0;
			}
		}
		else
//wei,用\r\n来判断结尾		 if( (dollar == 1) && (gprmc == 1) && (gps_reclen > 5)&&(gps_reclen<sizeof(g_ucNeo6QBuff)) &&(tmpchar != '*'))
        if((dollar==1)&&(gprmc==1)&&(gps_reclen>5)&&(recv_complete_flag == 0))
        {
            UART2_rbuf[gps_reclen++] = tmpchar;
            if(gps_reclen > sizeof(g_ucNeo6QBuff))
            {
                dollar =0;
                gprmc = 0;
                recv_complete_flag = 0;
            }
            if((UART2_rbuf[gps_reclen-2] == '\r')&&(UART2_rbuf[gps_reclen-1] == '\n')) //ljw,20160830
                recv_complete_flag = 0x5a; //GPS接收完成
        }
//ljw,20160830不要else		 else
		// if( (dollar == 1) && (gprmc == 1) && (gps_reclen>=sizeof(g_ucNeo6QBuff)))
		//结束付好$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
//		 if( (dollar == 1) && (gprmc == 1) && (tmpchar == '*') &&(gps_gate)) 
//wei,用\r\n来判断结尾 			 if((dollar == 1)&&(gprmc == 1)&&(tmpchar == '*') ) 
        if(recv_complete_flag == 0x5a)
		 {  //gps_gate,是g_ucNeo6QBuff是否处于被操作状态 , 1,没有被操作可以写
            if(gps_gate)
            {
                memset(g_ucNeo6QBuff,0,sizeof(g_ucNeo6QBuff));
                memcpy(g_ucNeo6QBuff,UART2_rbuf,sizeof(g_ucNeo6QBuff));
                dollar =0;
                gprmc = 0;
                recv_complete_flag = 0;
            }
	//wei 20160523 for mileage
			
			//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
			// 拷贝时间
//			ptmp = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
			if(strstr((char*)UART2_rbuf,",A,"))
			{
                GPSLedOn();//ljw,20160831 GPS点灯
#if 0//ljw,20160907 for space                
                V_counter = 0;
                memset(UART2_rbuf_temp,0,sizeof(UART2_rbuf_temp));
				memcpy(UART2_rbuf_temp,UART2_rbuf,sizeof(UART2_rbuf_temp));
				pTempBuf = (unsigned char *)strstr((char *)UART2_rbuf_temp,",");
                pTempBuf--;
                do			//此do...while结构中找出$GPRMC中的UTC时间和年月日时间
                {
//ljw               pCommard = (u8*)strchr((const char*)pTempBuf, ',');
                    pCommard = (u8*)strstr((u8 *)pTempBuf, ",");
                    ucTen = pCommard - pTempBuf;
//                    if(0x01 == ucOne) //取GPS的UTC时间（时分秒） HHMMSS.SSS
//                    {;}
                    
                    if(0x07==ucOne)//提取速度信息，20140618办公室无法得到速度值，待测试
                    {
                        if((ucTen > 0)&&(ucTen<7)) //有速度值， 速度值用多少个字节表示不确定。
                        {
                            memcpy((char *)(gps_ascii.Spd),(char *)(pTempBuf),ucTen);//拷贝纬度信息
//                            pCommard = (unsigned char *)strstr((char *)(ptmp+1),".");
//                            ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
//                            ptmp++;	
//                            ptmp1 = ptmp;
//                            ptmp= (unsigned char *)strstr((char *)(ptmp+1),".");
                            
                            //转化成1/10公里
                            u16tmp = asciitospeed(pTempBuf,pCommard-pTempBuf);
        //                    if(u16tmp >3000)//ljw,20160904私家车的速度上限设为300KM/H
        //                        u16tmp = (mileage_tmp/10);//ljw,20160904,速度异常的处理
                            pCommard += 1;
                            // 计算小数点之后的数据
                            u16tmp += ((*pCommard-'0') *1852)/1000;
                            //acc 功能打开，状态是有ACC信号的时候才计算里程
                //wei,20160524,ACC修改			if ((system_para.g_bACCon) &&(get_ACC_CHECK_status()))
                            mileage_tmp = 0;//wei,20160524将里程缓存清0
                            if ((system_para.g_bACCon) &&(g_ucACCFlag))//wei,20160524,ACC修改
                            {
                                mileage_tmp = u16tmp;	
                            }
                        }
                        else break;
                    }
                    pTempBuf = pCommard + 1;
                    ucOne++;
                    if(ucOne >0x09)
                        break;                   
                }while(1);
#endif  //ljw,20160907 for space
#if 1 //ljw,20160907 for space               
                    V_counter = 0;
					memset(UART2_rbuf_temp,0,sizeof(UART2_rbuf_temp));
					memcpy(UART2_rbuf_temp,UART2_rbuf,sizeof(UART2_rbuf_temp));
					ptmp = (unsigned char *)strstr((char *)UART2_rbuf_temp,",");
					ptmp++;
					//memcpy((char *)(tmp_time+6),(char *)(ptmp+1),6);	// 拷贝时间
					ptmp +=6;
					ptmp = (unsigned char *)strstr((char *)ptmp,",");
					// 拷贝GPS信息可用标志
					ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
					// 拷贝 纬度，
					//p1= (unsigned char *)strstr((char *)(i+1),",");
					ptmp++;
					//strncat((char *)smsbuf,i,p1-i);
					ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
					ptmp++;
					//strncat((char *)smsbuf,ptmp,1);
					//strncat((char *)smsbuf,"*WD",3);//JD34.36988N    经度34.36988N 北纬
					ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
					ptmp++;
					// 拷贝 经度，
					//p1= (unsigned char *)strstr((char *)(i+1),",");
					//strncat((char *)smsbuf,i,p1-i);
					ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
					ptmp++;
					//strncat((char *)smsbuf,i,1);
					//strncat((char *)smsbuf,"*SD",3);//SD000          当前速度
					ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
					ptmp++;	
					ptmp1 = ptmp;
					ptmp= (unsigned char *)strstr((char *)(ptmp+1),".");
					//转化成1/10公里
					u16tmp = asciitospeed(ptmp1,ptmp-ptmp1);
//                    if(u16tmp >3000)//ljw,20160904私家车的速度上限设为300KM/H
//                        u16tmp = (mileage_tmp/10);//ljw,20160904,速度异常的处理
					ptmp += 1;
					// 计算小数点之后的数据
					u16tmp += ((*ptmp-'0') *1852)/1000;
                    if(u16tmp > 2200)//ljw,20160928，速度大于220认为无效
                        u16tmp = mileage_tmp;
					//acc 功能打开，状态是有ACC信号的时候才计算里程
		//wei,20160524,ACC修改			if ((system_para.g_bACCon) &&(get_ACC_CHECK_status()))
					mileage_tmp = 0;//wei,20160524将里程缓存清0
					if ((system_para.g_bACCon) &&(g_ucACCFlag))//wei,20160524,ACC修改
					{
						mileage_tmp = u16tmp;
		//wei.20160524				wzw_mileage.mileage_64 += u16tmp;
						
		//				if ((system_para.g_bACCon) &&(get_ACC_CHECK_status()))//wei,20160523,超过2公里就存里程。
		//				{
		//wei.20160524						if(wzw_mileage.mileage_64-backup_mileage_64 >= 7200)  //wei,20160523,超过2公里就存里程。
			//wei.20160524					{
			//wei.20160524						flag_mileage_save = 0x5a;
			//wei.20160524						backup_mileage_64 = wzw_mileage.mileage_64;
			//wei.20160524					}
		//				}
					}
#endif  //ljw,20160907for space
			}
            else if(V_counter++>3) //ljfw,20160831
            {
                V_counter = 0;
                dingwei_flag = 0x00;
                GPSLedOff();//ljfw,20160831
            }
                

//wei 20160523 for mileage

		 }
#else
		if (gps_reclen >= UART2_rbuf_len)
			gps_reclen = 0;
		UART2_rbuf[gps_reclen++] = USART_ReceiveData(USART2);

#endif		 
		//USART_SendData(USART2,Res);
		USART_ClearITPendingBit(USART2, USART_IT_WU);
		USART_ClearITPendingBit(USART2, USART_IT_CM);
		USART_ClearITPendingBit(USART2, USART_IT_EOB);
		USART_ClearITPendingBit(USART2, USART_IT_RTO);
		USART_ClearITPendingBit(USART2, USART_IT_PE);
		//USART_ClearITPendingBit(USART2, USART_IT_TXE);
		//USART_ClearITPendingBit(USART2, USART_IT_TC);
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
		USART_ClearITPendingBit(USART2, USART_IT_LBD);
		USART_ClearITPendingBit(USART2, USART_IT_CTS);
		USART_ClearITPendingBit(USART2, USART_IT_ERR);
		USART_ClearITPendingBit(USART2, USART_IT_ORE);
		USART_ClearITPendingBit(USART2, USART_IT_NE);
		USART_ClearITPendingBit(USART2, USART_IT_FE);
  		 
  }
#if 0
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)  
	{
			USART_SendData(USART2,Res);
			USART_ClearITPendingBit(USART2, USART_IT_TXE);

	}
#endif	
}

/**********************************************************************
//说明:向端口发送数据
//参数1:端口句柄
//参数2:写数据指针
//参数3:写数据长度
//返回:实际写数据长度
**********************************************************************/
s16 ddi_uart_Write(uart_ctl_t *pCOM,const u8 *pDat,s16 size)
{
	USART_TypeDef * puart=(USART_TypeDef * )(pCOM->reg);
	u8 Res;
	s16 t;
	//u8 tbuf[384];
	volatile u32 i=0;
	//if (size>sizeof(tbuf))
	//	return;
	//memcpy(tbuf,pDat,size);
	for(t=0;t<size;t++)
	{
		USART_SendData(puart,*pDat++);
		//puart->TDR = tbuf[t]; //(*pDat++) ;//& (uint16_t)0x01FF);
		i =0;
		while((puart->ISR & USART_ISR_TXE ) != USART_ISR_TXE)//等待发送结束
		{
			i++;
			if(i > 0x10000)
			break;	
		}
	}
	i ++;
	i --;
	return t;
}


void UART0_ReceiveData( u8* rxbuf, u16 rxnum )
{
	#if 0
	unsigned short  i=0;
	ManualRx0 = 1;
	if(rxnum > 1)  // 固定长度获取串口数据：查询方式
	{
		SRMK0 = 1;
		TM00_Stop();
		while(rxnum--)
		{
			while(SRIF0 == 0)
			{
				i++;
				__delay_10us(10);
				if (i > 10000)
				   break;	
			}
			
			*rxbuf = RXB0;
			rxbuf++; 
			SRIF0 = 0;
		}
	}
	else if(rxnum == 0)  // 不固定长度获取串口数据：中断方式
	{
		gUart0RxCnt = 0;
		SoftDelay_ms(2000);   //(7000); //500
	}
	ManualRx0 = 0;
	#endif
	unsigned short  i=0;
	ManualRx0 = 1;
	if(rxnum > 1)  // 固定长度获取串口数据：查询方式
	{

		delayms(2000); 
	}
	else if(rxnum == 0)  // 不固定长度获取串口数据：中断方式
	{

		gUart0RxCnt = 0;		
		delayms(2000);   //(7000); //500
	}
	ManualRx0 = 0;

}

