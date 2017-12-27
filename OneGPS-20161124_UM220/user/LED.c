#include <stdio.h>
#include "main.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "led.h"

LED_STRUCT LED_yellow;
LED_STRUCT LED_red;
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN,LED3_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK,LED3_GPIO_CLK};



/**
  * @brief  Configure PA0 in interrupt mode
  * @param  None
  * @retval None
  */
void RING_EXTI0_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  /* Enable GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GSM_RING_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GSM_RING_PORT, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void IO_Init(void)
{

      GPIO_InitTypeDef GPIO_InitStructure;
   	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */    

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC, ENABLE);
	//led init	PA4 PB12 PB13
//ljw,20160903	GPIO_InitStructure.GPIO_Pin =  LED1_PIN |OIL_BREAK_PIN  ;
//wei,20160618    GPIO_InitStructure.GPIO_Pin =  OIL_BREAK_PIN  ;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|LED1_PIN |OIL_BREAK_PIN  ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
//wei,20160618,<将断油脚使用上拉    
//    GPIO_InitStructure.GPIO_Pin =  OIL_BREAK_PIN  ;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//wei,20160618>
#if 0//ljw,20160903 for space 与上面合并<
	//PA0 GSM POWER 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PA1 GPS POWER
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif ////ljw,20160903for space与上面合并>

	GPIO_InitStructure.GPIO_Pin =  ACC_PIN| ALARM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//ljw,20161111 ACC有异常GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ljw,20161111 ACC有异常
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


//ljw,20160903 for space	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 ;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
#if 0 //ljw,20160903 for space 与上面合并<

	/* Configure I2C pins: SDA  */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif

	/* Configure I2C pins: SDA  */
	GPIO_InitStructure.GPIO_Pin =  LED2_PIN |LED3_PIN|GSM_POWERKEY_PIN;//|DTR_PIN; wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin  =  GSM_RING_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#if 0//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
    GPIO_InitStructure.GPIO_Pin  =  DTR_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DTR_PORT, &GPIO_InitStructure);
	// low is no in sleep
	GPIO_ResetBits(DTR_PORT,DTR_PIN);
#endif //wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电

  	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
// acc	
	// 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);
	/* Connect Button EXTI Line to Button GPIO Pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource12);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  // EXTI_Trigger_Falling acc 开关必须打开，关的话延时2分钟才振动起作用
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


// PA8  ARALM
// #define  ALARM_PIN  GPIO_Pin_8
// #define ALARM_PORT  GPIOA
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}



void set_GPS_POWERON(bool status)
{
	if (status)
		GPIO_SetBits(GPS_POWERON_PORT,GPS_POWERON_PIN);
	else
		GPIO_ResetBits(GPS_POWERON_PORT,GPS_POWERON_PIN);
}

/******wei20160601****************************************************************
//说明: 发送15个AT，判断MCU有没有和GSM模块通信成功。
//参数: 无
//返回: 大于0，成功；  等于0，失败
**********************************************************************/
u8 Check_AT(void)
{
	u8 i =8;//wei8;
	while(i)
	{
		i--;
		ddi_uart_Write(&UART1_ctl_t,"AT\r\n",4); 
		if(0 == WaitGSMOK1())
		{
            ddi_uart_Write(&UART1_ctl_t,"ATE0\r\n",strlen("ATE0\r\n"));
            WaitGSMOK1();//ljw   WaitGSMOK();
//wei,20160826          ddi_uart_Write(&UART1_ctl_t,"AT+IPR=0\r\n",strlen("AT+IPR=0\r\n"));//wei,20160826,配置为自适应波特率
//            WaitGSMOK1();//ljw   WaitGSMOK();
            break;			
		}
		if(i==0) break;
		delayms(500); //8000，释放PWRKEY脚后还要再等2秒
	}
	return i;
}

//wei,20160531
u8 GSM_Power(bool status)
{
  u8 i=0;
  u32 temp_arry_bar[2]={115200,9600};
//G600
	if(status)
	{
//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电		DTR_PIN_MODE_Contrl(ENABLE); //WEI,20160530关闭串口
//ljw.20161110把串口打开放到后面		ddi_uart1_init(ENABLE); //WEI,20160530
		GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);		
		GPIO_SetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);
		//delayms( 10);//上电后等待10ms, 手册上建议的是30ms
		delayms(30);//WEI20160530 手册上建议的是30ms
        ddi_uart1_init(ENABLE); //ljw,20161019
        delayms(10);//ljw,20161019,让串口稳定一下
		GPIO_SetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);	
		delayms( 1020);//上电等待10ms之后，拉低GSM PWRKEY脚1.02秒	
		GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);
						//释放PWRKEY脚
		delayms(2000); //8000，释放PWRKEY脚后还要再等2秒
		i = 15;
        for(i=0;i<2;i++)
        {
            if(Check_AT()) //
            {
                Deal_GSMRTC(100);//wei,20160812,处理GSM提供的时间基准，做为系统RTC时间	
//wei,20160827,波特率适应
#if 1//ljw,20160906 测完要删
                ddi_uart_Write(&UART1_ctl_t,"AT+IPR=9600&W\r\n",strlen("AT+IPR=9600&W\r\n"));
                WaitGSMOK1();//ljw   WaitGSMOK();
                UART1_ctl_t.Parity = Parity_No;
                ddi_uart_open(&UART1_ctl_t,9600);
                delayms(10);
#else
                ddi_uart_Write(&UART1_ctl_t,"AT+IPR=115200&W\r\n",strlen("AT+IPR=115200&W\r\n"));
                WaitGSMOK1();//ljw   WaitGSMOK();
                UART1_ctl_t.Parity = Parity_No;
                ddi_uart_open(&UART1_ctl_t,115200);
                delayms(10);                
#endif //ljw,20160906 测完要删
//wei,20160827,波特率适应
                ddi_uart_Write(&UART1_ctl_t,"ATS0=0&w\r\n",strlen("ATS0=0&w\r\n")); 
                WaitGSMOK1();//ljw   WaitGSMOK(); 
                ddi_uart_Write(&UART1_ctl_t,"ATI\r\n",strlen("ATI\r\n"));
                WaitGSMOK1();//ljw   WaitGSMOK();
                ddi_uart_Write(&UART1_ctl_t,"AT+QCFG=\"RI/Hopping\",1\r\n",strlen("AT+QCFG=\"RI/Hopping\",1\r\n"));
                WaitGSMOK1();//ljw   WaitGSMOK();
                ddi_uart_Write(&UART1_ctl_t,"AT+QNITZ=1\r",strlen("AT+QNITZ=1\r"));
                WaitGSMOK2(500);
                if(!UbxConfig())
                {
                    UbxConfig();
                }
                if(!Get_GPS_module())
                {
                    Get_GPS_module();
                }
                return 1;
            }
//wei,20160827,波特率适应
            else 
            {
                UART1_ctl_t.Parity = Parity_No;
                ddi_uart_open(&UART1_ctl_t,temp_arry_bar[i]);
                delayms(10);
            }
//wei,20160827,波特率适应
        }
		return 0;
	}
	else
	{
        GPIO_SetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);
		delayms(800);//拉低GSM PWRKEY脚800ms	
		GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);
//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电		DTR_PIN_MODE_Contrl(DISABLE); //WEI,20160530关闭串口
		ddi_uart1_init(DISABLE); //WEI,20160530关闭串口
		
		delayms(2000);//释放GSM PWRKEY脚，让其恢复高电平
		GPIO_ResetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);//
		return 1; //wei,20160631
	}
}
//wei,20160531

#if 0//wei,20160531
void GSM_Power(bool status)
{
//  u8 i=0;
//G600
	if(status)
	{
	
		GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);
		GPIO_SetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);
		delayms( 10);//上电后等待10ms, 手册上建议的是30ms
		GPIO_SetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);	
		delayms( 1020);//上电等待10ms之后，拉低GSM PWRKEY脚1.02秒	
		GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);
						//释放PWRKEY脚

//		while(1)
//		{	
//		      i ++;
//			if (i > 50)                  //等待5 秒
//				break;
//			memset(g_ucSim900Buff,0,SIM900_BUFF_SIZE);
//			gUart0RxCnt = 0;
//			ddi_uart_Write(&UART1_ctl_t,"AT\r\n",4); 
//			if (0 == WaitGSMOK1())
//				break;
//		}	
		delayms(2000); //8000，释放PWRKEY脚后还要再等2秒
		UbxConfig();
		ddi_uart_Write(&UART1_ctl_t,"AT\r\n",4); 
		WaitGSMOK();
		ddi_uart_Write(&UART1_ctl_t,"ATS0=0&w\r\n",strlen("ATS0=0&w\r\n")); 
		if (0 == WaitGSMOK1())
				return;
	}
	else
	{
  		GPIO_SetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);	
		delayms(800);//拉低GSM PWRKEY脚800ms
		GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);
		delayms(2000);//释放GSM PWRKEY脚，让其恢复高电平	
		GPIO_ResetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);//
	}

}
#endif //wei,20160531

//wei20160531
//参数说明:ENABL使能DTR,   DISABLE,失能DTR
//
#if 0//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
void DTR_PIN_MODE_Contrl(FunctionalState New_state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(New_state==ENABLE)
	{
		GPIO_InitStructure.GPIO_Pin =  DTR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	}
	else if(New_state==DISABLE)
	{
		GPIO_InitStructure.GPIO_Pin  =  DTR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(DTR_PORT, &GPIO_InitStructure);
	}
}
#endif //wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电

//wei,20160711
//函数功能:  清除所有GSM断电后与GSM模块相关的参数，将其清零，方式硬件断电后，软件的相关标志位却没有同步清零
void Clear_All_GSM_flag(void)
{
    dial_status = 0;//拨号成功标志。
    cops_flag = 0x00;//获取到运营商的标志
    ring_sms = NO_USE;//短信电话标志
}

//wei20160531
//拉低PWRKEY脚,关闭电源,
//保持PWRKEY脚为高,上电
void GSM_reset(void)
{
//ljw,20160908提高效率  	while(1)
   for(;;)
	{
		GSM_Power(FALSE);//拉低PWRKEY脚,然后关闭电源
		//wei	delayms(600);	
		delayms(4000);//wei,断4秒电后，再上电
		if(GSM_Power(TRUE))//上电后等待10ms,后再拉低PWRKEY脚1.02秒
		{
			RING_EXTI0_Config();
            delayms(10);
            Clear_All_GSM_flag();
			return ;
		}
	}
}

#if 0//wei,20160607
void GSM_reset(void)
{
	GSM_Power(FALSE);//拉低PWRKEY脚,然后关闭电源
//wei	delayms(600);	
	delayms(4000);//wei,断4秒电后，再上电
	while(!GSM_Power(TRUE));//上电后等待10ms,后再拉低PWRKEY脚1.02秒
	RING_EXTI0_Config();	
}
#endif //wei,20160607
void set_GSM_POWERON(bool status)
{
	if (status)
		GPIO_SetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);
	else
		GPIO_ResetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);
}

//切断油路的？
void set_OIL_BREAK(bool status)
{
	if (status)
		GPIO_SetBits(OIL_BREAK_PORT,OIL_BREAK_PIN);
	else
		GPIO_ResetBits(OIL_BREAK_PORT,OIL_BREAK_PIN);
}

#if 0//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
void set_GSM_SLEEP(bool status)
{
	if (status)
		GPIO_SetBits(DTR_PORT,DTR_PIN);
	else
		GPIO_ResetBits(DTR_PORT,DTR_PIN);
}
#endif //wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电

bool  get_GSM_RING_status(void)
{
	 if (!GPIO_ReadInputDataBit(GSM_RING_PORT,GSM_RING_PIN))
		return FALSE;
	 else
	 	return TRUE;
}

bool  get_ACC_CHECK_status(void)
{

	 if (GPIO_ReadInputDataBit(ACC_PORT,ACC_PIN))
		return FALSE;
	 else
	 	return TRUE;
}

bool  get_ALARM_status(void)
{
	 if (GPIO_ReadInputDataBit(ALARM_PORT,ALARM_PIN))
		return TRUE;
	 else
	 	return FALSE;
}

bool  get_SHAKE_status(void)
{
	 if (!GPIO_ReadInputDataBit(SHAKE_PORT,SHAKE_PIN))
		return FALSE;
	 else
	 	return TRUE;
}



