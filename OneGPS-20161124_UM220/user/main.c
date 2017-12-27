/***************************************************************************
 * @Filename        -  
 * 
 * Copyright ?zzg Technology Ltd, 2015. All rights reserved.
 * 
 * @Brief           - 
 *      This file 
 *      
 * 
 * 											   
 * @Revision        - 1.1.1.1 $
 * @Date            - 2015/03/11 11:50:35 $
 * @Author          - zzg $
 * @Project         - 
 *
 *
 * @Revision History- 20150514 12:00发送位置的改好了其它短信的处理还要继续搞，短信的接收，解析
 * @Log             - 　20150515: at first LED all flash .
 *                              20150531: 串口设置参数的也写好了，但都还没有调试，还有定时唤醒和休眠没有做啊。
 *                                             要能够设置24小时的，使用RTC, 取当前时间加上需要演示延时的时间安
 * // 20150630>12.00: 加速度芯片换了地址也不能读出WHO AM I，怀疑芯片坏了 
 * // 20150702 今天把ADC调好
 * // 20150721,发现了UART0_ReceiveData还没有调试好,开机时要处理未接的短信，现在没看到
 * // 20150724:低功耗测试，定时唤醒使用RTC，工作持续时间使用普通定时器或者系统TICK定时器
 * // 20150725: gprs通讯时，为了保链路，需要定时发送，不然会有问题澹″！!!
 * // 现在试下来，定时唤醒总是几秒就唤醒了！？ 看看是不是看门狗的原因 ，关了看门狗也不行
 * // 现在不进入休眠看看,不经休眠的话，时间是准的。看看是不是不能在中断服务程序中进入休眠
 * // 20150808:中文指令的gprs ID由0x0f00->0x0f88
 * // 20151106: 增加gsm 120秒都收不到服务器的消息,就重启GSM.  休眠的时,要把数据复位,防止唤醒后立即重启gsm.
***************************************************************************/
/***************************************************************************
 * Copyright  ?zzg Technology Ltd. 2015.  All rights reserved.
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
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "led.h"
#include "SmsCmd.h"
/***********************************************************************
 * Public user header files (#include)
 **********************************************************************/
//#include <includes.h> 
/***********************************************************************
 * Private user header files (#include)
 **********************************************************************/
 
/***********************************************************************
 * Constant definitions (#define)
 **********************************************************************/
//device ID 就是作为协议中传送所需的手机号
 const u8 DEVICEID[]={0x01,0x56,0x56,0x04,0x93,0x32,0x00};
//ljw,20161129const u8 DEVICEID[]={0x01,0x85,0x51,0x09,0x00,0x21,0x00};
const u8 PASSWD[]= "123456";
//wie  const u8 IPADDR[]=	"220.178.0.57";  //"218.22.32.154"//	"220.178.0.58"
const u8 IPADDR[]=	"220.178.0.54";  //wei"218.22.32.154"//	"220.178.0.58"wei
const u8 IPPORT[]=	"9668";  //"4095" (公司老协议 ) //"6000" //	9668(部标协议)

//wei  const unsigned char  g_ucVERSION[18] = "900MTS001U-160129";         	// 系统模式 影子寄存器
const unsigned char  g_ucVERSION[18] = "900MTS001U-161220";         	// 系统模式 影子寄存器

//灵敏度从低到高
//ljw,20160914 注释掉使用测试的，需恢复 const u8 acceleration_para[3]={0x4a,0x3a,0x1a};
//ljw,20160914 注释掉使用测试的，需恢复 const u8 impact_time_para[3]={0x5,0x5,3};
//ljw,20160919测完恢复const u8 acceleration_para[3]={0x4a,0x3a,0x22}; //ljw,20160914.   2a  24  20
//ljw,20160919测完恢复const u8 impact_time_para[3]={0x7,0x5,3};

const u8 acceleration_para[3]={0x42,0x32,0x2a}; //ljw,20160914.测完要删 2a  24  20
const u8 impact_time_para[3]={0x03,0x03,0x03}; //ljw,20160914.测完要删
/***********************************************************************
 * Macro definitions (#define)
 **********************************************************************/
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
#define FLASH_PAGE_SIZE         ((uint32_t)0x00000400)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((uint32_t)0x08006000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ((uint32_t)0x08007000)   /* End @ of user Flash area */


/***********************************************************************
 * Data declarations of global data imported.  
 **********************************************************************/
//zzg test 20140425
extern __IO uint32_t TimingDelay;
extern _smsdata smsdatareg;
extern u8 Impact_flag ;

extern  unsigned short timegpserror;

extern u8 in_agps;
// 未定位计时
//wei,20160823放到main.h里面  extern  u32 NOLocalCount;

/***********************************************************************
 * Function prototypes of global functions imported.  
 **********************************************************************/

/***********************************************************************
 * Data definitions of global data exported.  
 **********************************************************************/
SYSTEM_PARA  system_para;
unsigned char g_ucSysMode;  						// 0: SMS   1: SMS & GPRS
//unsigned char g_ucSMSMode;                                          // 0:关闭自动短信回复；1，打开自动短信回复
//unsigned char g_ucSysInitialized		= SYS_UNINITI;		// 系统初始化标志
unsigned char g_ucSim900Status			= IDLE;			// SIM900模块状态 
//ljw,20160924 unsigned char g_ucCurrentNum[11]		= "00000000000";	// 当前操作设备的号码
unsigned char g_ucCurrentNum[PHNUM_LEN]		= {0};	// 当前操作设备的号码
unsigned char g_ucUART6RXUsed			= 0;			// 串口6当前状态
unsigned char g_ucTaskList			= 0;			// 任务表
unsigned char g_ucCurrentSMSNum[2]	        = {0,0};		// 当前短信存储序号
unsigned char volatile g_ucInitSettingFlag	= 0;			// 初始化设置标志位
volatile unsigned char g_ucChargingFlag 	= 0;			// 充电状态标志位
volatile unsigned char g_ucUpdateFlag 		= 0;			// 系统状态更新标志
volatile unsigned char g_ucLowPowerFlag		= 0;			// 电量低
volatile unsigned char g_ucFStatus		= 0;			//各种状态
volatile unsigned char g_ucHeartUpdateFlag = 0;            // 心跳需要上传的标志
volatile unsigned char g_ucACCFlag = 0;            //wei,20160524 ACC状态标志0:ACC没有   0x5a:ACC为上电
// GPRS 工作方式标志位
//volatile unsigned char g_ucGPRSMode 			= GPRS_MODE_WAKE; //GPRS_MODE_PERIOD;	// 分时段模式标志位

// 当前时间
//unsigned short g_usCurrentTime;
// 当前速度
unsigned short g_usCurrentSpeed = 0;
// 叫醒时刻
//unsigned short g_usWakeTime;
// 叫醒模式下 数据上传标志 

// 叫醒工作时间
//unsigned short g_usGPRSWakeUpTime = 5*60;   				// 默认: 5 分钟
u8 entersleepflag=0;
//rtc在休眠的唤醒次数
u16 rtc_wakeup_timer=0;
//定时器提醒要进入SLEEP
u8 g_ucGotoSleepFlag=0;
u8 REGNUMbeSET=0;
volatile unsigned short TimeUpLdCnt = 20;
// 经纬度数据
//double g_dCurrentLat = 0, g_dCurrentLon = 0;  	// 当前纬度 经度
//double g_dMaxLat     = 0, g_dMaxLon     = 0;  	// 最大纬度 经度
//double g_dMinLat     = 0, g_dMinLon     = 0;  	// 最小纬度 经度
//double g_dMetersPLonDegree = 0; 				// 当前经度下 经度每改变一度地表变化的距离

// GPRS 命令执行任务标志位
unsigned char volatile g_ucGPRSCmdMissionFlag = 0;  // 1 -- 有未执行命令 0 -- 无未执行命令
// SMS 命令执行任务标志位
unsigned char volatile g_ucSMSCmdMissionFlag = 0;  	// 1 -- 有未执行命令 0 -- 无未执行命令
//振动或ACC 触发报警
unsigned char volatile g_ucACC = 0;
// 速度限制报警标志
unsigned char volatile g_ucSpeedLimitAlarm = 0;
//紧急报警状态
unsigned char volatile g_ucAlarm = 0;
//功耗模式
//unsigned char volatile g_ucENERGY;
//进入低功耗模式的标志
unsigned char volatile g_FlagENERGY = 0;
//定时唤醒时间间隔
//unsigned  int  WtmCount;
//ACC 状态变化持续时间
unsigned short int volatile ACCCount=0;
//unsigned char  volatile ACCflag=0;
//振动检测信号变化的持续时间
unsigned short int volatile ShakerCount=0;
//允许初始化的标志
volatile unsigned char  EN_initialize=1;
//低电量持续时间
unsigned short volatile LOWPOWERCount=0;
//外部中断触发系统唤醒
unsigned char intflag=0;
unsigned char g_ucSim900Buff[SIM900_BUFF_SIZE];	// GSM  输出缓冲区
unsigned char g_ucNeo6QBuff[85];//[75];		// GPS  输出缓冲区
unsigned char backup_g_ucNeo6QBuff[85];//[75];	 // 最后一次有效定位
unsigned char g_ucGPRSDataBuff[150];	// GPRS 数据缓冲区
//unsigned char g_ucGPRSDataBuff_hex[170];	// GPRS 数据缓冲区
unsigned char g_ucGPRSCmdBuff[GPRS_BUFF_SIZE];	// GPRS 命令缓冲区
//LJW,20161124,这个数组目前只是用来进行参数设置用的了。 unsigned char g_ucGPRSCmdBuffback[GPRS_BUFF_SIZE];	// GPRS 命令缓冲区
unsigned char g_ucGPRSCmdBuffback[GPRS_BACK_BUFF_SIZE];	// GPRS 命令缓冲区//LJW,20161124,这个数组目前只是用来进行参数设置用的了。
//ljw,20160906for space unsigned char volatile GPRSCmdFlag;
u16 voltage;  //外接电源电压
u8 shakeon;  //震动状态
//振铃的状态
u8  ring_sms;
volatile unsigned char  bit_flag = 0;//wei.20160530,位0,1==1,则在请求AGPS;  位2,3==1, 则需要重新初始化震动芯片3DH
BITFlag BitFlag;//wei,20160603,定义位区
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
u8 SHACK_Init_Flag_Time = 0; //wei20160603,震动脚低电平计时器，超时重新配置
#endif
__IO uint16_t CCR3_Val = 13654;
__IO uint16_t CCR4_Val = 6826;
uint16_t PrescalerValue = 0;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
static __IO uint32_t TimingDelay;

//ljw,20160908 for space u8 testshake=0;


__IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0;

//低电压计数器
u32 lowbattcount=0;
u8 chinesesmsflag=0;
u8 startchinesesms=0;

//因为要保留最后一次有效定位数据，所以在数据处理时不许往gps缓冲区天数据
u8 gps_gate= 1; // 1:可以往GPS缓存g_ucNeo6QBuff里面写刚获得的GPS数据, 0:正在操作g_ucNeo6QBuff缓存中的数据，不能往里写新的数据
u8 gps_copy=0;
//确认是低电压要报警了
u8 lowbatt_flag = 0;
//服务器有应答,一直没有就会重启GSM
u8 server_ack_flag=0;
// 没响应就一秒加一次,加到120次就重启模块
u16 server_noack_counter=0;
//重启GSM模块标志
u8 rst_gsm_flag= 0;


mileage_ctl_t wzw_mileage;
u16 mileage_tmp=0;

//当前SIM的运营伤　１是移动，二是联通
u32 MobileCompany=0;
/***********************************************************************
 * Function prototypes of functions requiring global scope
 * not included elsewhere.  
 **********************************************************************/
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
extern void IIC_lsm330_Init(void);
extern void LMS330DLC_init(void);
#endif
extern void delayms(u32 count);


extern void ADC1_Config(void);
extern u16 GetVoltage(void);
extern void Cmd_CXCS(void);
/***********************************************************************
 * Static function prototypes. (Forward declarations)
 **********************************************************************/
/* Forward declarations of private methods */


/***********************************************************************
 * Static variable definition (file scope).  
 **********************************************************************/
void Delay(__IO uint32_t nTime);
void SysTick_Configuration(void);
void RTC_Config(void);
void SysTick_Configuration(void);

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
 


/******************************************************************************************************/





void RTC_Config(void)
{
  __IO uint32_t index = 0;
  //GPIO_InitTypeDef GPIO_InitStructure;
  RTC_InitTypeDef   RTC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;
  RTC_TimeTypeDef   RTC_TimeStructure;
  RTC_AlarmTypeDef  RTC_AlarmStructure;
 
  NVIC_InitTypeDef NVIC_InitStructure;
    


  /* RTC Configuration **********************************************************/
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

	/* Reset RTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  
  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();  



    /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* NVIC configuration */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;     //  2 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv = 0x0138;
  
  if (RTC_Init(&RTC_InitStructure) == ERROR)
  {
   //   GSM_Power(FALSE); //wei,20160608看门狗复位前，给GSM断电
//ljw,20160908提高效率       while(1); //等看门狗复位系统
      for(;;);
  }
    

  /* Set the alarm X+5s */
  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x01;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x00;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 10;
  RTC_AlarmStructure.RTC_AlarmDateWeekDay = 31; //0x31;
  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

    /* Enable the RTC Alarm A interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	
  /* Enable the alarm */
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
  

  
  /* Set the time to 01h 00mn 00s AM */
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  RTC_TimeStructure.RTC_Hours   = 0x01;
  RTC_TimeStructure.RTC_Minutes = 0x00;
  RTC_TimeStructure.RTC_Seconds = 0x00;  
  
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
  
  /* Clear the Alarm A Pending Bit */
  RTC_ClearITPendingBit(RTC_IT_ALRA);  
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
void SYSCLKConfig_STOP(void)
{  
  /* After wake-up from STOP reconfigure the system clock */
  /* At this stage the HSI is already enabled */

  /* Enable Prefetch Buffer and set Flash Latency */
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
 
  /* HCLK = SYSCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
  /* PCLK = HCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

  /* PLL configuration = (HSI/2) * 12 = ~48 MHz */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
  RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL12); // RCC_CFGR_PLLMULL12
            
  /* Enable PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
  }

  /* Select PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
  {
  }
}


/**
  * @brief  Configures the SysTick to generate an interrupt each 250 millisecond.
  * @param  None
  * @retval None
  */
void SysTick_Configuration(void)
{
  /* SysTick interrupt each 250 ms */
  if (SysTick_Config((SystemCoreClock/8) / 4))
  { 
    /* Capture error */ 
    while (1);
  }

  /* Select AHB clock(HCLK) divided by 8 as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

  /* Set SysTick Preemption Priority to 1 */
  NVIC_SetPriority(SysTick_IRQn, 0x04);
}


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, with a base of 250 milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);

}

#if 1 
void 	energy(u8 power) //进入低功耗模式
{
	NVIC_InitTypeDef   NVIC_InitStructure;
      GPIO_InitTypeDef GPIO_InitStructure;
	u16 i;
	g_ucGotoSleepFlag = 0;
	
	WaitGSMOK1();//ljw   WaitGSMOK();
	// 休眠的时,要把数据复位,防止唤醒后立即重启gsm.
	rst_gsm_flag = 0;
	server_noack_counter = 0;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;		//
	NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
	USART_Cmd(USART1, DISABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;		//
	NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
	USART_Cmd(USART2, DISABLE);  
	//led all off
	SHAKELedOff();
	GPSLedOff();
	GSMLedOff();
//WEI20160607	if(power == EXTRA_LOW_POWER_MODE) 
//WEI20160607	{
		GSM_Power(FALSE);
		GPSOff();
//WEI20160607	}	
//WEI20160607	else if(power == LOW_POWER_MODE)
//WEI20160607		set_GSM_SLEEP(TRUE);
	

	//led init	PA4 PB12 PB13
//ljw,20160903	GPIO_InitStructure.GPIO_Pin =  LED1_PIN |OIL_BREAK_PIN  ;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|LED1_PIN |OIL_BREAK_PIN  ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//PA0 GSM POWER 
//WEI20160607	if(power == EXTRA_LOW_POWER_MODE) 
//WEI20160607	{
#if 0 //ljw,20160903与上面合并了
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//PA1 GPS POWER
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif 
//WEI20160607	}

//ljw,20160903	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 ;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

#if 0 //ljw,20160903与上面合并了
	/* Configure I2C pins: SDA  */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
	/* Configure I2C pins: SDA  */
//ljw	GPIO_InitStructure.GPIO_Pin =  LED2_PIN |LED3_PIN|GSM_POWERKEY_PIN;
    GPIO_InitStructure.GPIO_Pin =  LED2_PIN |LED3_PIN|GSM_POWERKEY_PIN|GSM_RING_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	


//WEI20160607	if(power == EXTRA_LOW_POWER_MODE) 
//WEI20160607	{
//		GPIO_InitStructure.GPIO_Pin  =  GSM_RING_PIN;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
//		//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOB, &GPIO_InitStructure);

#if 0//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
		GPIO_InitStructure.GPIO_Pin  =  DTR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(DTR_PORT, &GPIO_InitStructure);
#endif //wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
//WEI20160607	}
	IWDG_ReloadCounter();

	 //while(RTC_ALARM_timer < (system_para.g_usSleepTime*6))
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
	 SHACK_Init_Flag_Time = 0;//wei,20160604,
#endif
	 BitFlag.SHACK_Init_Flag = 0;  
	 entersleepflag = 0x5a;
	 RTC_ALARM_timer = 0;
	 rtc_wakeup_timer =  0;
	 while(rtc_wakeup_timer < (system_para.g_usSleepTime*6))
	 {
	 	i++;
		i--;
	    IWDG_Feed();
		RTC_Config();

		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		while(RTC_ALARM_timer == 0)
		{
			i++;
            if(rtc_wakeup_timer >= (system_para.g_usSleepTime*6))
                break ;
		//IWDG_Feed();
		}		
		IWDG_Feed();
		RTC_ALARM_timer = 0;
			    /* Disable the RTC Alarm interrupt */
		RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
		SYSCLKConfig_STOP();
		for(i=0; i<0x1fff;i++)
            IWDG_Feed(); 
		rtc_wakeup_timer ++;
		//if (system_para.g_bACCon &&(get_ACC_CHECK_status()))
		{
		//	rtc_wakeup_timer = system_para.g_usSleepTime*6 + 2;
		}
	 }
    IWDG_Feed();//ljw,20161109 for 5min reset
    
	 //退出休眠，进入正常工作时间
	// else 
	

	RTC_ALARM_timer = 0;

	entersleepflag = 0;
	//if(power == LOW_POWER_MODE) 
//WEI20160607	set_GSM_SLEEP(FALSE);
	IO_Init();
    SHAKELedOn();
#if 0
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//
	NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
	USART_Cmd(USART1, ENABLE);


	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//
	NVIC_Init(&NVIC_InitStructure);	//??NVIC_InitStruct???????????NVIC???USART1
	USART_Cmd(USART2, ENABLE);  
#endif 	
//WEI20160607	if(power == EXTRA_LOW_POWER_MODE)
//WEI20160607	{

		//不定位延时5分钟计数器复位。
		timegpserror = 0;
		GPSOn();
        delayms(30);//ljw,20161110GPS先上电后打开串口
        ddi_uart2_init(ENABLE); //ljw,20161019
        delayms(10);//ljw,20161019,让串口稳定一下
//WEI20160607	GSM_reset();
		if(GSM_Power(TRUE))//直接给机器上电
		{
			RING_EXTI0_Config();			
		}
		dial_status = 0;
		i=0;//wei20160607
		while(dial_status == 0)
		{
			StartGPRS();	
			i++;//wei20160607
			if((i%2==0)||(CPIN_READY_flag == 0))//wei20160607//连续两次没起来，断电重启GSM
			{
	                 GSM_reset();////wei20160607重启GSM
			}
		}
	 	// 开启模块休眠功能
#if 0//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
		ddi_uart_Write(&UART1_ctl_t,"AT+QSCLK=1\r\n",strlen("AT+QSCLK=1\r\n"));
		WaitGSMOK1();//ljw   WaitGSMOK();
#endif //wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
             //　定位过标志，保证唤醒后确认是否定位过。
		dingweied_flag = 0;
		//set_GSM_SLEEP(FALSE);
//WEI20160607	}
//WEI20160607	else
//WEI20160607	if(power == LOW_POWER_MODE) 
//WEI20160607	{
		if(dial_status == 1)
			GSMLedOn();
//WEI20160607	}
//WEI20160607	set_GSM_SLEEP(FALSE);

	ring_sms = SMS;
	//发一条GPRS 
	g_ucUpdateFlag = 1;
	delayms(5000);
	if((send_alarm_flag)&&(system_para.g_bAUTOSMSon))
	{
		send_alarm_flag = 0;
		for(i=0 ; i<2 ; i++)
		{
            if((system_para.g_ucRegNum[i][0] ==  FULL)&&(system_para.g_ucRegNum[i][1] == strlen((u8 *)(&system_para.g_ucRegNum[i][2]))))
            {
                SendALARMSMS1((char *)(system_para.g_ucRegNum[i]+2));
            }
//ljw,20160928 for wulian<
//			if((system_para.g_ucRegNum[i][0] ==  FULL)&&(0 != strncmp((char *)(&system_para.g_ucRegNum[i][4]),"00000000000",11)))//ljw20160902
//			{
//			   	//只发送前两个号码		          	
//                SendALARMSMS1((char *)(system_para.g_ucRegNum[i]+4));
//				delayms(3500);	//发送短信后，不等三秒就发送GPRS 会失败			   	
//			}
//ljw,20160928 for wulian>
		}
	}
	// 防止一出休眠就重启GSM
    ACC_GPS_flag = 0x00;
    ACC_GSM_flag = 0x00;
	rst_gsm_flag = 0;
	server_ack_flag = 0X5A;
	server_noack_counter = 0;
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
	SHACK_Init_Flag_Time = 0;//wei20160604防止一起来就初始化震动芯片
#endif
	BitFlag.SHACK_Init_Flag = 0; //wei20160604
}
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure the TIM IRQ Handler.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
	/* -----------------------------------------------------------------------
		TIM3 Configuration: Output Compare Timing Mode:
		
		In this example TIM3 input clock (TIM3CLK) is set to APB1 clock (PCLK1),  
			=> TIM3CLK = PCLK1 = SystemCoreClock = 48 MHz
					
		To get TIM3 counter clock at 6 MHz, the prescaler is computed as follows:
			 Prescaler = (TIM3CLK / TIM3 counter clock) - 1
			 Prescaler = (PCLK1 /6 MHz) - 1
																									
		CC3 update rate = TIM3 counter clock / CCR3_Val = 439.4 Hz
		==> Toggling frequency = 219.7 Hz
		
		CC4 update rate = TIM3 counter clock / CCR4_Val = 878.9 Hz
		==> Toggling frequency = 439.4 Hz

		Note: 
		 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
		 Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
		 function to update SystemCoreClock variable value. Otherwise, any configuration
		 based on this variable will be incorrect.  
         
    wei,zhu,20160711,
    SystemCoreClock=48000000,
    In this example TIM3 input clock (TIM3CLK) is set to APB1 clock (PCLK1),  
			=> TIM3CLK = PCLK1 = SystemCoreClock = 48 MHz
    To get TIM3 counter clock at 6 MHz, the prescaler is computed as follows:
			 Prescaler = (TIM3CLK / TIM3 counter clock) - 1
			 Prescaler = (PCLK1 /6 MHz) - 1
    ARR=CNT/6M
	----------------------------------------------------------------------- */   

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock  / 6000000) - 1;

	/* Time base configuration */
//wei,20160711 adjusting TIM3 interrupt to 1ms	TIM_TimeBaseStructure.TIM_Period = 4800; // ms, 重载值
	TIM_TimeBaseStructure.TIM_Period = 6000; // ms, 重载值//wei,20160711 adjusting TIM3 interrupt to 1ms
    TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//PSC预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//计数器对齐方式,向上计算

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

//		/* Output Compare Timing Mode configuration: Channel1 */
//		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
//		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

//		/* Output Compare Timing Mode configuration: Channel3 */
//		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//		TIM_OCInitStructure.TIM_Pulse = CCR3_Val; //CCR3_Val

//		TIM_OC3Init(TIM3, &TIM_OCInitStructure);

//		TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);

//		/* Output Compare Timing Mode configuration: Channel4 */
//		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//		TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

//		TIM_OC4Init(TIM3, &TIM_OCInitStructure);

//		TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
		 
		/* TIM Interrupts enable */
		TIM_ITConfig(TIM3, TIM_IT_Update|TIM_IT_Trigger , ENABLE);

		/* TIM3 enable counter */
		TIM_Cmd(TIM3, ENABLE);//开始定时

}

//wei 处理参数存储的函数，在这里调用
u8 Store_System_Para(void)
{
	u8  i = 0;
	for(i=0;i<3;i++)
	{
		if(!Set_Store_System_Para()) //成功返回 0，否则返回非零
			break ;
	}
	if(i == 3) //上面的存储失败了
	 FLASH_ErasePage_Complete((u32 )system_para_ADDRESS);//擦出这个页
	for(i=0;i<3;i++)
	{
		if(!Backup_Store_System_Para())  //成功返回 0，否则返回非零
			break ;
	}
	if(i == 3) //上面的存储失败了
	 FLASH_ErasePage_Complete((u32 )backup_system_para_ADDRESS);//擦出这个页;//擦出这个页
}

//wei,20160517,擦除一个页
//擦除正确，返回0 ， 擦除错误返回0xff
u8  FLASH_ErasePage_Complete(u32 addr)
{
 
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
//wei,20160829这句把	if (FLASH_ErasePage(backup_system_para_ADDRESS)!= FLASH_COMPLETE)
    if (FLASH_ErasePage(addr)!= FLASH_COMPLETE)
	{
	 /* Error occurred while sector erase. 
	     User can add here some code to deal with this error  */
	  return 0xff;
	}
	FLASH_Lock(); 
	return 0;
}
//存放系统变量的备份，放到800FC00放完后校验，成功返回0，其他的非零
//backup_system_para_ADDRESS
u8 Backup_Store_System_Para(void)
{
	uint32_t EraseCounter = 0x00, Address = 0x00;

	 FLASH_Status FLASHStatus = FLASH_COMPLETE;
	 TestStatus MemoryProgramStatus = PASSED;
	volatile u32 *ptmp=(u32*)&system_para;
	volatile u32 *ptmp_system_para;
	volatile u8 i;
	
	 /* Unlock the Flash to enable the flash control register access *************/ 
	FLASH_Unlock();

	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 

  /* Erase the FLASH pages */
	if (FLASH_ErasePage(backup_system_para_ADDRESS)!= FLASH_COMPLETE)
	{
	 /* Error occurred while sector erase. 
	     User can add here some code to deal with this error  */
	  return 0xff;
	}
EraseCounter = sizeof(SYSTEM_PARA);  //wei test,2016.05.17
  /* Program the user Flash area word by word */
	   Address = backup_system_para_ADDRESS;

	  while (Address  <  (backup_system_para_ADDRESS+sizeof(SYSTEM_PARA)))
	  {
	    if (FLASH_ProgramWord(Address, *ptmp++) == FLASH_COMPLETE)
	    {
	      Address +=4;
		// ptmp ++;
	    }
	    else
	    { 
	      /* Error occurred while writing data in Flash memory. 
	         User can add here some code to deal with this error */
		return 0xff; 
	    }
	  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
       FLASH_Lock(); 


  /* Check if the programmed data is OK 
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
	ptmp=(u32*)&system_para;
      ptmp_system_para = (u32 *)backup_system_para_ADDRESS;
	for(i=0;i<sizeof(SYSTEM_PARA); )
	{
		if (*ptmp !=  *ptmp_system_para)
		{
			return 0xff;  //直接推出，然后在写依次，　直到写成功
		}
		i +=4;
//wei		ptmp +=4;
//wei		ptmp_system_para += 4;
		ptmp += 1;  //wei
   	ptmp_system_para +=1; //wei

  }
	return 0;
}

//存放系统变量，放到８００ｆ８００，放完后校验，成功返回０，其它的非０
//wei  u8 Store_System_Para(void)
u8 Set_Store_System_Para(void)   //wei,在0x800F800处存储参数，总共有两个地方存储，另一个地方是备份区0x800f600
{
	uint32_t EraseCounter = 0x00, Address = 0x00;

	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	TestStatus MemoryProgramStatus = PASSED;
	volatile u32 *ptmp=(u32*)&system_para;
	volatile u32 *ptmp_system_para;
	volatile u8 i;
	
	 /* Unlock the Flash to enable the flash control register access *************/ 
	FLASH_Unlock();

	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 

  /* Erase the FLASH pages */
	if (FLASH_ErasePage(system_para_ADDRESS)!= FLASH_COMPLETE)
	{
	 /* Error occurred while sector erase. 
	     User can add here some code to deal with this error  */
	  return 0xff;
	}

  /* Program the user Flash area word by word */
	   Address = system_para_ADDRESS;

	  while (Address  <  (system_para_ADDRESS+sizeof(SYSTEM_PARA)))
	  {
	    if (FLASH_ProgramWord(Address, *ptmp++) == FLASH_COMPLETE)
	    {
	      Address +=4;
		// ptmp ++;
	    }
	    else
	    { 
	      /* Error occurred while writing data in Flash memory. 
	         User can add here some code to deal with this error */
		return 0xff; 
	    }
	  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
       FLASH_Lock(); 


  /* Check if the programmed data is OK 
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
	ptmp=(u32*)&system_para;
      ptmp_system_para = (u32 *)system_para_ADDRESS;
	for(i=0;i<sizeof(SYSTEM_PARA); )
	{
		if (*ptmp !=  *ptmp_system_para)
		{
			return 0xff;  //直接推出，然后在写依次，　直到写成功
		}
		i +=4;
//wei,20160518		ptmp +=4;
//wei,20160518			ptmp_system_para += 4;
		ptmp +=1; //wei,20160518
		ptmp_system_para += 1;//wei,20160518

  }
	return 0;
}


void RestoreSystem(void)
{
	//SYSTEM_PARA  backUP_system_para;
	u8 tmp_mid[6];
	u8 i;
	memcpy(tmp_mid,system_para.g_ucDeviceID,6);
	// 系统参数初始化
	memset((u8*)&system_para,0,sizeof(system_para));// id 保留上一次的
	
	system_para.g_ucSysInitialized = SYS_INITIED;
	//system_para.g_ucSysMode_S 	  	= MODE;         	// 系统模式 影子寄存器
	//system_para.g_ucPowerMode               = IPOWERMODE; //系统的功耗模式
	system_para.g_ucTrackMode		= TRACK_MODE_GPRS;	// 定位模式
	// IP 地址
	if (FHTIP != system_para.g_ucSysMode_S)
	{
		memcpy(system_para.g_ucIPAddress,IPADDR, strlen(IPADDR) );
		//system_para.g_ucUDPPort[5] 	   	= IPPORT;		// UDP 端口
		memcpy(system_para.g_ucUDPPort,IPPORT, strlen(IPPORT) );
	}	
	//system_para.g_ucDeviceID[6] 	   	= DEVICEID;		// 设备ID
	memcpy(system_para.g_ucDeviceID,tmp_mid, sizeof(tmp_mid));
	//system_para.g_ucNetUser[6] 	   	= "GPRS";		// 设备ID
	memcpy(system_para.g_ucNetUser,"GPRS", strlen("GPRS") );
	//system_para.g_ucNetPsw[8] 	   	= "GPRS";		// 设备ID
	memcpy(system_para.g_ucNetPsw,"GPRS", strlen("GPRS") );
	//system_para.g_ucNetAPN[20] 	   	= "CMNET";		// 设备APN
	memcpy(system_para.g_ucNetAPN,"CMNET", strlen("CMNET") );

	//system_para.g_ucPassWord[6] 	   	= PASSWD;  		// 系统密码
	memcpy(system_para.g_ucPassWord,PASSWD, strlen(PASSWD) );

	system_para.g_usGPRSUpLoadTime  = default_g_usGPRSUpLoadTime;		// 上传持续时间 单位 分钟
	system_para.g_usGPRSUpLoadAternationTime = UPLOADTIME;		// 上传间隔
	system_para.g_usSleepTime = default_g_usSleepTime ;
	system_para.g_bAUTOSMSon = 0;
	system_para.g_break = 0;
	system_para.REGNUMbeSET = 0;
	system_para.g_ucPowerMode = ALLONLINE_MODE;
	//
	system_para.HeartAternationTime = default_HeartAternationTime;
	system_para.acceleration = acceleration_para[2]; //ljw,20160919 system_para.acceleration = acceleration_para[2];
    system_para.impact_time = impact_time_para[2];

	for(i=0; i<2; i++)
	{
//ljw,20160926for wulian	  	strncpy((char *)system_para.g_ucRegNum[i],"e   00000000000",15);        // 删除内存中授权号码
        strncpy((char *)system_para.g_ucRegNum[i],"e",1);        // 删除内存中授权号码
	}
	mileage_flash_init();
}

//初始化过了的，读出数据放到，结构体中，没有初始化的放入默认值
//wei,20160517
void sys_Init(void)
{
 	SYSTEM_PARA *tmp;
	//若已经初始化过，读出在flash中存放的内容，
	tmp = (SYSTEM_PARA *)system_para_ADDRESS;
	if (tmp->g_ucSysInitialized == SYS_INITIED)
	{
		memcpy((u8*)&system_para,(u8*)tmp,sizeof(system_para));
		tmp = (SYSTEM_PARA *)backup_system_para_ADDRESS;//wei,准备检查备份区
		if (tmp->g_ucSysInitialized != SYS_INITIED) //备份区错误,再次写备份区
		{
				if(Backup_Store_System_Para())//备份区没写成功，再写一遍，总共写两边
					Backup_Store_System_Para();
		}
		if(system_para.g_break ==1)
		{
			set_OIL_BREAK(TRUE);
		}
		else
		if (system_para.g_break == 0)
		{
			set_OIL_BREAK(FALSE);
		}
	}
	else
	{
		tmp = (SYSTEM_PARA *)backup_system_para_ADDRESS;
		if (tmp->g_ucSysInitialized == SYS_INITIED)
		{
			memcpy((u8*)&system_para,(u8*)tmp,sizeof(system_para));
//wei,20160622			if(Backup_Store_System_Para())//备份区是正确的，正常的存储区是错误的，重写一遍存储区，如果失败再写一遍
//wei,20160622					Backup_Store_System_Para(); //上步写失败了，再写一遍，如果还失败，就不写了。
            if(Set_Store_System_Para())//备份区是正确的，正常的存储区是错误的，重写一遍存储区，如果失败再写一遍
                Set_Store_System_Para();

            if(system_para.g_break ==1)
			{
				set_OIL_BREAK(TRUE);
			}
			else
			if (system_para.g_break == 0)
			{
				set_OIL_BREAK(FALSE);
			}
		}
		else  //如果两部分存储区都不对
		{
			RestoreSystem();//给机子的一些参数设为默认值		
            memcpy(system_para.g_ucDeviceID,DEVICEID, sizeof(DEVICEID));
//ljw,20161129不应该用这个 strlen(DEVICEID)			memcpy(system_para.g_ucDeviceID,DEVICEID, strlen(DEVICEID) );
			
		}
	}
		

}	
#if 0  //wei
//初始化过了的，读出数据放到，结构体中，没有初始化的放入默认值
void sys_Init(void)
{
 	SYSTEM_PARA *tmp;
	//若已经初始化过，读出在flash中存放的内容，
	tmp = (SYSTEM_PARA *)system_para_ADDRESS;
	if (tmp->g_ucSysInitialized == SYS_INITIED)
	{
		memcpy((u8*)&system_para,(u8*)tmp,sizeof(system_para));

		if(system_para.g_break ==1)
		{
			set_OIL_BREAK(TRUE);
		}
		else
		if (system_para.g_break == 0)
		{
			set_OIL_BREAK(FALSE);
		}
	}
	else
	{

		RestoreSystem();//给机子的一些参数设为默认值		
		memcpy(system_para.g_ucDeviceID,DEVICEID, strlen(DEVICEID) );
		
	}	

}	
#endif  //wei

uint8_t readio=0xff;
//#define TESTLED
// #define test_adc
extern u8  test_a_gps(void);

//wei20160608,<用来看门重启之前的延时
void delayms_nodog(u32 count)
{	
	for (;count>0; count--)
	{
		delayus(1000);		
	}
}
//wei20160608,>用来看门重启之前的延时

//ljw,20160924使用分号内提取的手机号，不做长度限制，只要不撑爆缓存就行
/*
AT+CMGR=1

+CMGR: "REC UNREAD","1064899150042",,"16/09/25,15:42:12+32"
fhtnewnum123456@13900001111

OK
*/
void Deal_SMS(void)
{
    volatile u32 i;
	volatile u32 j;
    u8 *pStart;
	u8 *pEnd;
    u8 temp_len;
    u8 smssendbuf[30]="AT+CMGR=";
    ring_sms = NO_USE;
    if(system_para.g_ucSysInitialized == SYS_INITIED)	// 如果系统未初始化 将不接受任何短信命令
    {
        for(j=0; j<5; j++)
        {
            if (g_ucSMSCmdMissionFlag) // 防止新的短信又进来
            {
                j=0;
                g_ucSMSCmdMissionFlag = 0;
            }

            delayms(10);
            memset(smssendbuf ,0,sizeof(smssendbuf));
            //ddi_uart_Write(&UART1_ctl_t,"AT+CMGL=\"REC UNREAD\"\r\n",22);
            memcpy(smssendbuf,"AT+CMGR=",8);
            smssendbuf[8] = j+'0'+1;
            smssendbuf[9] = 0x0d;
            smssendbuf[10] = 0x0a;
            ddi_uart_Write(&UART1_ctl_t,smssendbuf,strlen(smssendbuf));
            gUart0RxCnt = 0;
            ManualRx0 = 1;
            UART0_ReceiveData(g_ucSim900Buff,0);  //0	// 串口中断方式接收短信内容
            //+CMGR: "REC UNREAD","1064899150042",,"16/09/24,08:41:55+32"
            if (NULL == strstr((char *)g_ucSim900Buff,"+CMGR:"))
            continue;
            // 寻找短信息发送者号码
            pStart = (u8*) strstr(g_ucSim900Buff,"\"");   		// 第一个‘，’
            pStart = (u8*) strstr((pStart+1),",\"");         		//找号码前面的 ,"
            pEnd = (u8*) strstr((pStart+1),"\",");         		// 找号码后面的",
            temp_len =pEnd-pStart-2;//ljw,2016092提取号码长度
            if(temp_len >= PHNUM_LEN-3)
                continue;
//            if('1'  ==  *(char*)(pStart + 2))
//            pStart += 2;
//            else
//            pStart += 5;
//            strncpy((char *)g_ucCurrentNum,(char *)(pStart),11); 	//some has +86,someone no	// 指向发送号码
            pStart += 2;
            memset(g_ucCurrentNum,0x00,sizeof(g_ucCurrentNum));//ljw,20160925先清空，再存放号码
            strncpy((char *)g_ucCurrentNum,(char *)(pStart),temp_len); 	//some has +86,someone no	// 指向发送号码
//ljw,20160925for wulian<
//            for(i =0; i<11; i++)
//            for(i =0; i<temp_len; i++)
//            {
//                if((*(pStart+i)>'9')||(*(pStart+i)<'0'))
//                {
//                    break;
//                }
//            }
//            if(i >10)
//ljw,20160925for wulian >
//LJW,20160928不要做字符判断            if(isdigtit_len(pStart,temp_len))
//            {
//                memcpy(&(smsdatareg.mobilenum[smsdatareg.count]),g_ucCurrentNum,11);
                memset((u8 *)(&(smsdatareg.mobilenum[smsdatareg.count])),0x00,PHNUM_LEN);//ljw,20160925先清空再用。
                memcpy((u8 *)(&(smsdatareg.mobilenum[smsdatareg.count])),g_ucCurrentNum,temp_len);
                analysis_sms(pStart);
                setSMS();
 //           }
            //20131105: 号码错误时，也要删除信息 
            // else
            //  continue;

            delayms(100);  //1000
            memset(smssendbuf ,0,sizeof(smssendbuf));
            memcpy(smssendbuf,"AT+CMGD=1,0\r\n",strlen("AT+CMGD=1,0\r\n"));
            smssendbuf[8] = j+1+'0';
            //smssendbuf[8] = j+'0';

            ddi_uart_Write(&UART1_ctl_t,smssendbuf,strlen(smssendbuf));
            gUart0RxCnt = 0;
            ManualRx0 = 1;
            //UART0_ReceiveData(smssendbuf,2);  //0	// 串口中断方式接收短信内容
            //20150721: 这里感觉也没处理收到的结果
            UART0_ReceiveData(g_ucSim900Buff,0); 
            delayms(1000);
            // 如果这时有GPRS数据就赶紧处理GPRS
            if(g_ucGPRSCmdMissionFlag || chinesesmsflag)
            {
                break;
//ljw,20160908                break;
            }
        }
        delayms(2000);  //wei,20160630,延时2秒，再发判断是不是要发删短信的动作
        if((g_ucSMSCmdMissionFlag == 0)&&(g_ucGPRSCmdMissionFlag == 0)&&(chinesesmsflag==0))
        {
            //wei,20160630                delayms(2000);  // 2000
            if((0x5a !=ring_start_flag)&&(ring_sms !=SMS))
            {
                ddi_uart_Write(&UART1_ctl_t,"AT+CMGD=1,4\r\n",13);
                delayms(500);  // 2000
            }
        }
    }// 结束始化检测

    g_ucSMSCmdMissionFlag = 0;
    //防止短信处理时间长导致GSM复位
    rst_gsm_flag = 0;
    server_ack_flag = 0X5A;
    server_noack_counter = 0;
}

/*短信处理，改变手机号码的长度
void Deal_SMS(void)
{
    volatile u32 i;
	volatile u32 j;
    u8 *pStart;
	u8 *pEnd;	
    u8 smssendbuf[30]="AT+CMGR=";
//wei,20160825测试短信上报3和4的问题    测完要删
//    ddi_uart_Write(&UART1_ctl_t,"AT+CNMI?\r",strlen("AT+CNMI?\r"));
//    WaitGSMOK2(250);
//    ddi_uart_Write(&UART1_ctl_t,"AT+CPMS=\"SM\"\r",strlen("AT+CPMS=\"SM\"\r"));  	// //优选短信存储器为SIM卡
////DebugOut("* Set SMS Format: TEXT.\r\n",0);
//    WaitGSMOK();
//    
//    ddi_uart_Write(&UART1_ctl_t,"AT+CPMS?\r",strlen("AT+CPMS?\r"));  	// //优选短信存储器为SIM卡
////DebugOut("* Set SMS Format: TEXT.\r\n",0);
//    WaitGSMOK();
//wei,20160825测试短信上报3和4的问题    测完要删
    ring_sms = NO_USE;
    if(system_para.g_ucSysInitialized == SYS_INITIED)	// 如果系统未初始化 将不接受任何短信命令
    {
        for(j=0; j<5; j++)
        {
            if (g_ucSMSCmdMissionFlag) // 防止新的短信又进来
            {
                j=0;
                g_ucSMSCmdMissionFlag = 0;
            }

            delayms(10);
            memset(smssendbuf ,0,sizeof(smssendbuf));
            //ddi_uart_Write(&UART1_ctl_t,"AT+CMGL=\"REC UNREAD\"\r\n",22);
            memcpy(smssendbuf,"AT+CMGR=",8);
            smssendbuf[8] = j+'0'+1;
            smssendbuf[9] = 0x0d;
            smssendbuf[10] = 0x0a;
            ddi_uart_Write(&UART1_ctl_t,smssendbuf,strlen(smssendbuf));

            gUart0RxCnt = 0;
            ManualRx0 = 1;
            UART0_ReceiveData(g_ucSim900Buff,0);  //0	// 串口中断方式接收短信内容
            if (NULL == strstr((char *)g_ucSim900Buff,"+CMGR:"))
            continue;
            // 寻找短信息发送者号码
            pStart = (u8*) strstr(g_ucSim900Buff,"\"");   		// 第一个‘，’
            pStart = (u8*) strstr((pStart+1),",\"");         		// 第二个‘，’
            if('1'  ==  *(char*)(pStart + 2))
            pStart += 2;
            else
            pStart += 5;
            strncpy((char *)g_ucCurrentNum,(char *)(pStart),11); 	//some has +86,someone no	// 指向发送号码

            for(i =0; i<11; i++)
            {
                if ((*(pStart+i)>'9') ||(*(pStart+i)<'0'))
                {
                    break;
                }
            }
            if(i >10)
            {
                memcpy(&(smsdatareg.mobilenum[smsdatareg.count]),g_ucCurrentNum,11);
                analysis_sms(pStart);
                setSMS();
            }
            //20131105: 号码错误时，也要删除信息 
            // else
            //  continue;

            delayms(100);  //1000
            memset(smssendbuf ,0,sizeof(smssendbuf));
            memcpy(smssendbuf,"AT+CMGD=1,0\r\n",strlen("AT+CMGD=1,0\r\n"));
            smssendbuf[8] = j+1+'0';
            //smssendbuf[8] = j+'0';

            ddi_uart_Write(&UART1_ctl_t,smssendbuf,strlen(smssendbuf));
            gUart0RxCnt = 0;
            ManualRx0 = 1;
            //UART0_ReceiveData(smssendbuf,2);  //0	// 串口中断方式接收短信内容
            //20150721: 这里感觉也没处理收到的结果
            UART0_ReceiveData(g_ucSim900Buff,0); 
            delayms(1000);
            // 如果这时有GPRS数据就赶紧处理GPRS
            if (g_ucGPRSCmdMissionFlag || chinesesmsflag)
            {
                break;
//ljw,20160908                break;
            }
        }
        delayms(2000);  //wei,20160630,延时2秒，再发判断是不是要发删短信的动作
        if((g_ucSMSCmdMissionFlag == 0)&&(g_ucGPRSCmdMissionFlag == 0)&&(chinesesmsflag==0))
        {
            //wei,20160630                delayms(2000);  // 2000
            if((0x5a !=ring_start_flag)&&(ring_sms !=SMS))
            {
                ddi_uart_Write(&UART1_ctl_t,"AT+CMGD=1,4\r\n",13);
                delayms(500);  // 2000
            }
        }
    }// 结束始化检测

    g_ucSMSCmdMissionFlag = 0;
    //防止短信处理时间长导致GSM复位
    rst_gsm_flag = 0;
    server_ack_flag = 0X5A;
    server_noack_counter = 0;
}
*/

//ljw,20160908判断休眠条返回 0x05,休眠  0x0a不休眠
u8 check_sleep_state(void)
{
    if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))//满足休眠条件，要休眠
    {
        if((g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE)) //wei,20160709,满足睡眠条件，不要在继续拨号了
            return 0x05;
    }
    return 0x0a;
}

u32 alarm_interval_timer = 0;//wei,20160712,紧急报警的时间间隔，需要与总时间轴做减法
u8 alarm_count = 0;//wei,20160712,紧急报警的次数
//u8 test_LIS3DH_timer; //ljw,20160913,测试震动 测完要删
int main(void)
{
    volatile u32 i;
	volatile u32 j;
	u16 u16tmp=0;
	u8* ptmp = NULL;
	u8* ptmp1 = NULL;

	u8 *pStart;
	u8 *pEnd;
    u8 smssendbuf[30]="AT+CMGR=";
	NVIC_InitTypeDef   NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    memset((u8 *)&BitFlag,0x00,sizeof(BITFlag));//wei,20160603,清空位区。
    IWDG_Init(IWDG_Prescaler_256,0x0c30);//wei,20160805,把看门狗放到最前面   实测15--20秒// 20 miao,
	IO_Init();

//wei	set_GPS_POWERON (FALSE);
    set_GPS_POWERON (TRUE);
    GPIO_ResetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);//wei,20160805 保持GSM为低电平
		//开始时亮一下就灭
//ljw	GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN);
//ljw	GPIO_ResetBits(LED2_GPIO_PORT,LED2_PIN);
//ljw	GPIO_ResetBits(LED3_GPIO_PORT,LED3_PIN);

//wei,20160805 将看门狗放到前面去	IWDG_Init(IWDG_Prescaler_256,0x0c30);//wei20160606,实测15--20秒// 20 miao, 

	sys_Init();
//wei,160517,测试备份区
//		smssendbuf[0] =Store_System_Para();//wei
//	smssendbuf[2] = Set_Store_System_Para();//wei
//	smssendbuf[0] =Backup_Store_System_Para();//wei
//	mileage_flash_init();//wei
//	smssendbuf[1] =FLASH_ErasePage_Complete((u32 *)backup_system_para_ADDRESS);//wei
//wei,160517,测试备份区

	/* TIM Configuration */
	TIM_Config();

//WEI,20160802 for space	ddi_uart_init();
    ddi_uart1_init(ENABLE);
    ddi_uart2_init(ENABLE);

	UART1_ctl_t.Parity = Parity_No;
	ddi_uart_open(&UART1_ctl_t,9600);

	UART2_ctl_t.Parity = Parity_No;
	ddi_uart_open(&UART2_ctl_t,9600);
	delayms( 10);
	
//wei,20160805 防止异常复位，保持GSM为断电状态	GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);//保持PWRKEY为上拉
//wei,20160805 防止异常复位，保持GSM为断电状态	GPIO_SetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);//给GSM模块上电
//wei	delayms( 10);
//wei,20160805 防止异常复位，保持GSM为断电状态	delayms(30);//wei
//wei,20160805 防止异常复位，保持GSM为断电状态	GPIO_SetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);	//拉低PWRKEY脚

	//开始时亮一下就灭
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
	GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);
#endif
	GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN);
	GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);
    if (SysTick_Config(SystemCoreClock / 10)) 
    {
        /* Capture error */ 
        while(1);
    }
//	delayms( 1020);
//	GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);	

	EN_initialize = 1;
	//　可以通过串口初始化数据
	InitialSetting();
	EN_initialize = 0;

//wei,20160803,for space  不要 GPS_on();了	GPS_on();
    set_GPS_POWERON (TRUE);//wei,20160803,for space  不要 GPS_on();了

	// gsm 波特率设置成115200
//wei,20160826 用9600测试AGPS	UART1_ctl_t.Parity = Parity_No;     //wei,20160826 用9600测试AGPS
//wei,20160826 用9600测试AGPS	ddi_uart_open(&UART1_ctl_t,115200);

	ddi_uart_Write(&UART1_ctl_t,(u8*)g_ucVERSION,17);//wei,20160603串口1打印一下程序版本号
	ddi_uart_Write(&UART1_ctl_t,"\r\n",2);//wei,20160603串口1打印一下程序版本号
	//send GSM start flag
	//ddi_uart_Write(&UART1_ctl_t,"SET GSM BUAD 115200\r\n",strlen("SET GSM BUAD 115200\r\n"));  
//ljw,20160904	memset(backup_g_ucNeo6QBuff,0,sizeof(backup_g_ucNeo6QBuff));
    memset((u8 *)(&gps_ascii),0,sizeof(gps_ascii));

 	mileage_init();
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
	LMS330DLC_init();
	/**********************************************************************
	//说明:  设置碰撞阀值和碰撞持续时间
	//参数1 accelero:  碰撞阀值，分辨率0.1g 范围0~79
	//参数2 time: 碰撞持续时间，分辨率4ms, 范围0~38
	//返回: 0 为失败；1 为成功
	**********************************************************************/
	//u8 set_Impact(u8 accelero,u8 time)
		//set_Impact(10,5); //0x1a,3      (最灵敏)
						  // 0x3a, 5 
						  //0x 4a ,5  (最钝)
	//set_Impact(0X2A,3);
	set_Impact(system_para.acceleration,system_para.impact_time);
//    LMS330DLC_SetInt1Threshold (system_para.acceleration);//ljw,20160914,测完要删
//    LMS330DLC_SetInt1Duration(0x00);//ljw,20160914,测完要删
//wei20160603<

    SHACK_Init_Flag_Time = 0;
    BitFlag.SHACK_Init_Flag = 0;

//wei20160603>
#endif
	//if (SysTick_Config(SystemCoreClock / 1000))
	// 0.1 s
//wei20160819, 放到设置参数前面做，以便设置参数的定时器使用	if (SysTick_Config(SystemCoreClock / 10)) 
//wei20160819, 放到设置参数前面做，以便设置参数的定时器使用	{ 
		/* Capture error */ 
//wei20160819, 放到设置参数前面做，以便设置参数的定时器使用		while (1);
//wei20160819, 放到设置参数前面做，以便设置参数的定时器使用	}
//wei,20160805 防止异常复位，保持GSM为断电状态	GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);//wei20160601释放PWRKEY
//wei,20160805 防止异常复位，保持GSM为断电状态	delayms(20);//wei20160601
//wei test
//    i=0;
    ddi_uart_Write(&UART1_ctl_t,"run later...\r\n",strlen("run later...\r\n")); //wei

//wei,20160805 防止异常复位，保持GSM为断电状态	if(!Check_AT())//wei20160601
//wei,20160805 防止异常复位，保持GSM为断电状态	{
    ddi_uart1_init(DISABLE); //ljw,20161110
    delayms(500);//ljw,20161110,让串口稳定一下
    if(GSM_Power(TRUE))//上电后等待10ms,后再拉低PWRKEY脚1.02秒
    {
        RING_EXTI0_Config();
        delayms(10);
        Clear_All_GSM_flag();
    }
	else    GSM_reset();//重启GSM

//wei,20160805 防止异常复位，保持GSM为断电状态	}
//wei,20160805 防止异常复位，保持GSM为断电状态	else
//wei,20160805 防止异常复位，保持GSM为断电状态		RING_EXTI0_Config();
	/* User button configuration */  
	// STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	/* Configure ADC1 Channel 11 */
	ADC1_Config();
#if 0//LJW,20160904多余了，上面已经关了
	GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);
	GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN);
	GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);  	
#endif
    
   //ljw,20160912 测完要删
//    while(1)
//    {
//        u8 who_am = 0;
//        u8 ctrl4 = 0;
//        u8 tmpReg = 0;
//        if(Impact_flag)
//        {
//            Impact_flag = 0;
//            get_Pitch();
//        }
//        else if(test_LIS3DH_timer >=2) //ljw,20160913,测试震动 测完要删)
//        {
//            test_LIS3DH_timer = 0;
//            LMS330DLC_GetWHO_AM_I((u8 *)&who_am);
//            LMS330DLC_ReadReg(LMS330DLC_CTRL_REG4,&ctrl4);
//            LMS330DLC_ReadReg( LMS330DLC_CTRL_REG1,&tmpReg);
//            
//            get_Pitch();
//        }
//    }
//ljw,20160912 测完要删 
    
    
    
    
    
    
	delayms(2000);
	GetAvailableGPSInfoV();
	i=0;
	if(dingweied_flag == 0) // 已经定位就不加AGPS
	{
		do{
			StartGPRStoAGPS();	
			if(dial_status==1)//wei,20160621 if Dial-up success ,break
                break;
			i++;
//wei,20160601<		if(i>3) //三次就重启 20151124: 不重起了，安装的时候需要只看　GPS的状态
			if(i>3) //三次就重启 20151124: 不重起了，安装的时候需要只看　GPS的状态
			{
			 //	GSM_Power(FALSE);
			 //	while(1);
				break;
			}
            if((i%2==0)||CPIN_READY_flag == 0)//wei,20160617
                GSM_reset();//wei,20160617重启GSM
			if(!dial_status)//wei,20160601如果拨号没成功，先进来判断一下GPS是否定位，如果定位就不用再去拨号了。
			{
				GetAvailableGPSInfoV_N(2); //wei,20160601
                if (dingweied_flag != 0)
                {
                    break;
                }
				if(!Check_AT())//wei201606001
                    GSM_reset();//重启GSM
			}
		}
		while(dial_status == 0);
//wei201600601<		
//        set_GPS_POWERON (TRUE);//wei,20160803,for space  不要 GPS_on();了
//wei,20160801        GPS_on();//wei,20160801 for power
//wei,20160801        delayms(500);//wei,20160801 for power
		for(i=4;i>1;i--)//执行三次
		{
			GetAvailableGPSInfoV_N(1); //wei,20160601
            if(dingweied_flag)
                break;
			if((1 ==dial_status)&&(dingweied_flag == 0))//wei,20160601AGPS拨号成功，且GPS没定位
			{
			//	delayms(1000);
				if(test_a_gps())
					break;
//				if (!test_a_gps())
//				{
//					
//					if (!test_a_gps())
//					{
//						test_a_gps();
//					}
//				}
			}
			GetAvailableGPSInfoV_N(2);//wei20160601
            if(dingweied_flag)
                break;
            if((dingweied_flag == 0)&&(GPRS_CONNECTED_AGPS != GetGPRSStatus())) //wei20160601
			{
				dial_status =0;//wei20160601
                if((i==2)||CPIN_READY_flag == 0)//wei,20160921
                    GSM_reset();//wei,20160617重启GSM
                StartGPRStoAGPS();//wei20160601
				//if(1<)
			}//针对，信号差的情况下，TCP连接断开了机器却不知道，还在傻傻的发数据
		}
//wei201600601>
		GetAvailableGPSInfoV();
	// 切换到正常的IP
		// gsm 波特率设置成115200
	//	UART1_ctl_t.Parity = Parity_No;
	//	ddi_uart_open(&UART1_ctl_t,115200);
		//send GSM start flag
		//ddi_uart_Write(&UART1_ctl_t,"SET GSM BUAD 115200\r\n",strlen("SET GSM BUAD 115200\r\n"));  
//wei		GSMLedOff();
//wei		GSM_reset();
	}

    GSM_QICLOSED(AGPS_SERVERS);
	i=0;
	dial_status = 0;
	do{
//        if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))//满足休眠条件，要休眠
//        {
//            if((g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE)) //wei,20160709,满足睡眠条件，不要在继续拨号了
//                break ;
//        }
        if(0x05 == check_sleep_state())//ljw,20160908判断休眠条返回 0x05,休眠  0x0a不休眠
            break;
		StartGPRS();	
//wei kill test        dial_status = 0;////wei,20160711 for test
        if(dial_status==1)//wei,20160621 if Dial-up success ,break
            break;
//wei,20160603		i++;
		i++;		
        if(cops_flag==0x5a)//获取运营商成功
        {
            if((secondcounter-cops_flag_timer >0)&&(secondcounter-cops_flag_timer< COPS_WAIT_TIME)&&((ring_sms != SMS)))
            {//获取到运营商后，还未到，能够读取到短信的最小时间
                if((i%2==0)||(CPIN_READY_flag == 0))//连续两次没起来，断电重启GSM
                {
                    i--;
                }
            }
            else //收取短信时间到了。
            {
            //wei,    cops_flag = 0x00;
                cops_flag_timer = secondcounter;
            }
            if(ring_sms == SMS)
            {
                Configuration_TEXT_SMS();//配置英文短信接收
                delayms(200);
                SMS_StartGSM_Flag = 0x5a;
                Deal_SMS();
                SMS_StartGSM_Flag = 0x00;
            }
        }
        if((i%2==0)||(CPIN_READY_flag == 0))//连续两次没起来，断电重启GSM
        {
            GSM_reset();//重启GSM
        }
//wei kill test        dial_status = 0;//wei,20160711 for test
        
        
      //wei,20160603		if(i>3) //三次就重启 20151124: 不重起了，安装的时候需要只看　GPS的状态
//wei,20160603		{
//wei,20160603		 //	GSM_Power(FALSE);
		 //	while(1);
//wei,20160603			break;
//wei,20160603		}
	}
	while(dial_status == 0);

    GetAvailableGPSInfoV_N(3);
#if 0//wei,20160803 for space    
     	// 开启模块休眠功能
	ddi_uart_Write(&UART1_ctl_t,"AT+QSCLK=1\r\n",strlen("AT+QSCLK=1\r\n"));
	WaitGSMOK();//wei,20160803现在的机器已经没有休眠了，只有超低功耗，这个模块休眠功能，也就没必要了     

	GetAvailableGPSInfoV();
	//等短信从基站发下来
	delayms(1000);
	GetAvailableGPSInfoV();
	delayms(1000);
	GetAvailableGPSInfoV();	
	delayms(1000);
	GetAvailableGPSInfoV();	
	delayms(1000);
	GetAvailableGPSInfoV();	
	delayms(1000);
	GetAvailableGPSInfoV();	
	delayms(1000);
	GetAvailableGPSInfoV();
#endif //wei,20160803
	ring_sms = SMS; //先处理短信
	g_ucAlarm = 0;
	Impact_flag = 0;
	
	//防止刚开机就复位GSM
	rst_gsm_flag = 0;
	server_ack_flag = 0X5A;
	server_noack_counter = 0;
    
    g_ucUpdateFlag  = 0x5a;//ljw,20160901,开机发一次位置
    back_g_usGPRSUpLoadAternationTime = secondcounter;//ljw,20160901,开机发一次位置
	
	while (1) 
	{
		IWDG_ReloadCounter();
//ljw,20160908提高效率         for(;;);
//ljw,20160924        SendMsg("+8618356957828","+86deduanxing",strlen("+86deduanxing"));
//ljw,测试2小时复位一次   测完要删
//        if(secondcounter >7200)
//        {
//            while(1);
//        }
//ljw,测试2小时复位一次   测完要删
//wei20160601
//    if(test_reset_time>=600)
//		{
//				GSM_Power(FALSE);
//				while(1);
//		}
//wei20160601		
		// 执行GPRS设置命令 
		if(g_ucGPRSCmdMissionFlag == 1)
		{
//ljw,20160906			g_ucGPRSCmdMissionFlag = 0;
			if(system_para.g_usGPRSUpLoadAternationTime > 30)
			{
				//TickPreScale = SYS_STA_UPD_TIME; // 心跳延时
				g_ucUpdateFlag = 0;
			}
			SetGPRS();
			g_ucGPRSCmdMissionFlag = 0;//ljw,20160906调整标志量的位置
			server_ack_flag = 0X5A;
			//delayms(1000);
			// 有数据来,定时时间重新计算,这样减轻GPRS的负荷
			//back_g_usHeartAternationTime = secondcounter;
			//back_g_usGPRSUpLoadAternationTime = secondcounter;			
		}
		else
		if (rst_gsm_flag == 0x5a) //重启GRPS标志
		{
			rst_gsm_flag = 0;
//wei           GSMLedOn();
			GSMLedOff();  //wei
			GSM_reset();
			dial_status = 0;
            i = 0;
			while(dial_status == 0)
			{
				//wei20160603
				StartGPRS();
				i++;
				if((i%2==0)||(CPIN_READY_flag == 0))//连续两次没起来，断电重启GSM
				{
					GSM_reset();//重启GSM
				}
//wei 20160603				StartGPRS();	
			}
		 	// 开启模块休眠功能
#if 0//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
			ddi_uart_Write(&UART1_ctl_t,"AT+QSCLK=1\r\n",strlen("AT+QSCLK=1\r\n"));
			WaitGSMOK();
			set_GSM_SLEEP(FALSE);
#endif //wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
            
			rst_gsm_flag = 0;
			server_ack_flag = 0X5A;
			server_noack_counter = 0;	
		}
		//电话操作	
		if (ring_sms == RING)
		{
			ring_sms = NO_USE;
#if 0//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
			//还在休眠中，这时只是起到唤醒的作用，无法执行平时的功能
			if (entersleepflag == 1)//机器在低功耗模式，不是超低功耗,GSM模块是可以收到电话和短信的，收到后在这里处理
			{
				set_GSM_SLEEP(FALSE);
				entersleepflag = 0;	
				g_FlagENERGY = 0;
				//WtmCount = system_para.g_usSleepTime*150+1; 
				for(i=0; i<10;i++)
				{
					ddi_uart_Write(&UART1_ctl_t,"ATH\r\n",5);  // 挂断电话 
				    	delayms(100);
				}
				// 工作计数器重新计算，开始5分钟					   	 
			}
#endif //wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电
#if 0 //ljw,20160924 说明书里没有，去掉
			ddi_uart_Write(&UART1_ctl_t,"AT+CLCC\r\n",strlen("AT+CLCC\r\n"));
            WaitGSMOK1();//ljw   WaitGSMOK();
			pStart = (u8 *)(strstr(g_ucSim900Buff,"+CLCC:"));
			if (pStart != NULL)
            {
                ManualRx0 = 0;
                strncpy((char *)g_ucCurrentNum,pStart+18,11);
                //  匹配授权号码,如果匹配 根据当前设备模式执行相应操作
                for(i = 0 ; i< 2 ; i++)
                {
                    if(!strncmp((char *)g_ucCurrentNum,(char *)(system_para.g_ucRegNum[i]+4),11))  // 是授权号码
                    {
//ljw,20160908for space                        DebugOut("\r\nSystem is a Tracker:\r\n  Hangup The Phone.\r\n  Send Location SMS.\r\n",0);
                        ddi_uart_Write(&UART1_ctl_t,"ATH\r\n",5);
                        WaitGSMOK1();//ljw   WaitGSMOK();
                        SendLocaSMS1(g_ucCurrentNum);   // 发送位置信息
                             //有时短信还没发完就要进行GPRS数据发送，容易冲突，加点延时
                        delayms(3500);	//发送短信后，不等三秒就发送GPRS 会失败
                        ClrGSMBuff();
                        break;
                    }
                }
                ddi_uart_Write(&UART1_ctl_t,"RING INT\r\n",strlen("RING INT\r\n"));  
                WaitGSMOK1();//ljw   WaitGSMOK();				
                IWDG_ReloadCounter();		
            }
	  		else
	  		{
	  			ManualRx0 = 0;	
			}
#endif //ljw,20160924 说明书里没有，去掉
			//delayms(1000);
		}
		if(ring_sms == SMS)
		{
			ring_sms = NO_USE;
            Deal_SMS();
#if 0
			if(system_para.g_ucSysInitialized == SYS_INITIED)	// 如果系统未初始化 将不接受任何短信命令
			{
			     for(j=0; j<5; j++)
			     {
                    if (g_ucSMSCmdMissionFlag) // 防止新的短信又进来
                    {
                        j=0;
                        g_ucSMSCmdMissionFlag = 0;
                    }
                    delayms(10);
					memset(smssendbuf ,0,sizeof(smssendbuf));
					//ddi_uart_Write(&UART1_ctl_t,"AT+CMGL=\"REC UNREAD\"\r\n",22);
					memcpy(smssendbuf,"AT+CMGR=",8);
					smssendbuf[8] = j+'0'+1;
					smssendbuf[9] = 0x0d;
					smssendbuf[10] = 0x0a;
					ddi_uart_Write(&UART1_ctl_t,smssendbuf,strlen(smssendbuf));
					
					gUart0RxCnt = 0;
		                   ManualRx0 = 1;
					UART0_ReceiveData(g_ucSim900Buff,0);  //0	// 串口中断方式接收短信内容
					if (NULL == strstr((char *)g_ucSim900Buff,"+CMGR:"))
						continue;
					// 寻找短信息发送者号码
					pStart = (u8*) strstr(g_ucSim900Buff,"\"");   		// 第一个‘，’
					pStart = (u8*) strstr((pStart+1),",\"");         		// 第二个‘，’
					if('1'  ==  *(char*)(pStart + 2))
						pStart += 2;
					else
						pStart += 5;
					strncpy((char *)g_ucCurrentNum,(char *)(pStart),11); 	//some has +86,someone no	// 指向发送号码

					 {
			                     for(i =0; i<11; i++)
					        {
								if ((*(pStart+i)>'9') ||(*(pStart+i)<'0'))
								{
									break;
								}
									
						  }  
						 if (i >10)
						 {
							memcpy(&(smsdatareg.mobilenum[smsdatareg.count]),g_ucCurrentNum,11);
						 	analysis_sms(pStart);
							setSMS();
						 } 
						//20131105: 号码错误时，也要删除信息 
						// else
						//  continue;
					 }	 
					delayms(100);  //1000
					memset(smssendbuf ,0,sizeof(smssendbuf));
					memcpy(smssendbuf,"AT+CMGD=1,0\r\n",strlen("AT+CMGD=1,0\r\n"));
					smssendbuf[8] = j+1+'0';
					//smssendbuf[8] = j+'0';

					ddi_uart_Write(&UART1_ctl_t,smssendbuf,strlen(smssendbuf));
					gUart0RxCnt = 0;
		                   ManualRx0 = 1;
					//UART0_ReceiveData(smssendbuf,2);  //0	// 串口中断方式接收短信内容
					//20150721: 这里感觉也没处理收到的结果
					UART0_ReceiveData(g_ucSim900Buff,0); 
					delayms(1000); 
					// 如果这时有GPRS数据就赶紧处理GPRS
					if(g_ucGPRSCmdMissionFlag || chinesesmsflag)
					{
						break;
//ljw,20160908for space						break;
					}
                }
				if((g_ucSMSCmdMissionFlag == 0) && (g_ucGPRSCmdMissionFlag == 0) && chinesesmsflag==0)
				{
					delayms(2000);  // 2000
					ddi_uart_Write(&UART1_ctl_t,"AT+CMGD=1,4\r\n",13);
					delayms(500);  // 2000
				}				 
			}// 结束始化检测

			g_ucSMSCmdMissionFlag = 0;
			//防止短信处理时间长导致GSM复位
			rst_gsm_flag = 0;
			server_ack_flag = 0X5A;
			server_noack_counter = 0;	
#endif
		}
		else
		if ((chinesesmsflag == 0x5a)&&((startchinesesms % 11) == 0))
		{
			cmd_INQUIRE_CHINESE_POSITION();   // 中文短信
//ljw,20160918 22只有两次			if(startchinesesms >= 22)//三次就不要数据了
            if(startchinesesms >= 33)//ljw，20160918
			{
				chinesesmsflag = 0;
				startchinesesms = 0;
				//发送结果给用户
				send_chinese_sms(g_ucCurrentNum,"4e2d65874f4d7f6e83b753d659318d25ff01",strlen("4e2d65874f4d7f6e83b753d659318d25ff01"));
			}
			else
			startchinesesms ++;			
		}
		else
		{
				//if (Impact_flag)    //pb5
				//{	
				//;
					//SHAKELedOff();
					//Impact_flag = 0;
				//}	
            if (g_usCurrentSpeed)
            {
                for(i=0 ; i<2 ; i++)
                {
//ljw,20160928for wulian                    if((system_para.g_ucRegNum[i][0] ==  FULL)&&(0 != strncmp((char *)(&system_para.g_ucRegNum[i][4]),"00000000000",11)))//ljw,20160902
                    if((system_para.g_ucRegNum[i][0] == FULL)&&(system_para.g_ucRegNum[i][1] == strlen((u8 *)(&system_para.g_ucRegNum[i][2]))))//ljw,20160928for wulian
                    {
                        {	//只发送前两个号码

//ljw,20160928for wulian                            Send_speed_ALARMSMS((char *)(system_para.g_ucRegNum[i]+4));
                            Send_speed_ALARMSMS((char *)(system_para.g_ucRegNum[i]+2));
                            delayms(3500);	//发送短信后，不等三秒就发送GPRS 会失败
                        }
                    }
                }
                g_usCurrentSpeed = 0;
            }
			      // 一秒一次
				if(gps_copy == 0x5a)
				{
					gps_copy = 0;
                    if(GPS_BaudRate_FLAG ==0)
                    {
                        if(timegpserror++ >10)
                        {
                            timegpserror = 0;
                            reset_gps();
                        }
                    }
                    else timegpserror = 0;
                    GPS_BaudRate_FLAG = 0x00;
                    RTC_Get();//wei,20160810 测试RTC时间
					//GetAvailableGPSInfoV();
					//201511130　增加里程统计现在没办法作准，在短信操作时有很多的延时，那段时间都没发统计
					
                    if(GetAvailableGPSInfoV())     // 获取一组可用的位置信息
					{
#if 0//wei 20160523 把里程统计放到串口中断里面做
						//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
						// 拷贝时间
						ptmp = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
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
						ptmp += 1;
						// 计算小数点之后的数据
						u16tmp += ((*ptmp-'0') *1852)/1000;
						//acc 功能打开，状态是有ACC信号的时候才计算里程
						if ((system_para.g_bACCon) &&(get_ACC_CHECK_status()))
						{
							wzw_mileage.mileage_64 += u16tmp;
						}
#endif //wei
					}

					if (flag_mileage_save == 0x5a)
					{
						mileage_write();
						flag_mileage_save = 0;
					}
					//ddi_uart_Write(&UART1_ctl_t,"AT+CSQ\r\n",strlen("AT+CSQ\r\n"));
					//WaitGSMOK(); 	
					//GetGPRSStatus();
					voltage = GetVoltage();
					//if ((voltage >= LOW_POWER) || (voltage < power_battary))  //大于9 V 算正常
					if ((voltage >= LOW_POWER))  //大于9 V 算正常
					{
						lowbattcount = 0;
						lowbatt_flag = 0;
					}
					if((lowbattcount>0)  && (lowbattcount%6 == 0))
					{
						lowbatt_flag = 0x5a;
					}
					
					if (get_ALARM_status() && !flag_start_duage_g_ucAlarm)    //pA8
					{
						flag_start_duage_g_ucAlarm = 0x5a;
					}
				}
				//有时长的gprs数据还没收全，或者还没处理就直接发送定时回传的数据，导致收到的数据无法处理
				if (g_ucGPRSCmdMissionFlag==0 && proGPRSCmdFlag==0)	
				{
					// 定时回传
				  	if(g_ucUpdateFlag)
				  	{
				  		g_ucUpdateFlag = 0;
						cmd_0200();//wei,20160823,改上报位置信息，将上报判GPS还是基站放到
                        #if 0//wei,20160823,改上报位置信息，将上报判GPS还是基站放到
					//ljw,20160920zhu luo,等两分钟不好    if(NOLocalCount>=120)
					    if(NOLocalCount>=10)   //wei ,根据超低功耗的程序改的这里
						{
							uploadLAC();
						}
						else
						{
							cmd_0200();
						}
                        #endif
						back_g_usGPRSUpLoadAternationTime = secondcounter;
				  	}
					//心跳
					if(g_ucHeartUpdateFlag)
				  	{
				  		g_ucHeartUpdateFlag = 0;
						cmd_0002();
						back_g_usHeartAternationTime = secondcounter;
				  	}
					//delayms(1000);
				}

//睡眠模式（慢时钟模式）
//模块睡眠功能默认关闭，“AT+QSCLK=1”命令可打开该功能。
//当用“AT+QSCLK=1”设置模块之后，可通过设置DTR 引脚电平来控制模块进入或退出睡眠模式。当
//DTR 引脚置高，且没有中断产生(如GPIO 中断或者数据传递发生在串口)，模块会自动进入到睡眠模式。
//睡眠模式下，模块仍然可以接收来电，短信以及GPRS 下行数据，但是串口不可访问。

// 休眠唤醒
//当模块处于睡眠模式，以下方式可以唤醒模块：
//. 拉低DTR引脚；
//. 语音来电或GPRS下行数据；
//. 收到短信息。				
//wei				if( (system_para.g_bACCon &&(!get_ACC_CHECK_status())) || (!system_para.g_bACCon))
                if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon));  //wei,20160524,ACC修改 ((g_ucACCFlag ==0)&&system_para.g_bACCon) 
				{
						if((g_ucGotoSleepFlag))
						{
							g_ucGotoSleepFlag = 0;
							if((system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE))
							{
								ddi_uart_Write(&UART1_ctl_t,"Enter EXTRA_LOW_POWER_MODE!\r\n",strlen("Enter EXTRA_LOW_POWER_MODE!\r\n"));
								energy(EXTRA_LOW_POWER_MODE);
							}
#if 0 //wei,20160803 for space>
							else if ((system_para.g_ucPowerMode==LOW_POWER_MODE))
							{
								ddi_uart_Write(&UART1_ctl_t,"Enter LOW_POWER_MODE!\r\n",strlen("Enter LOW_POWER_MODE!\r\n"));
								energy(LOW_POWER_MODE);
							}
#endif //wei,20160803 for space>
						}
                }
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
                if(Impact_Alarm_flag ==0x5a)//ljw,20160903,有震动报警
                {
                    Impact_Alarm_flag =0xa5;
                    cmd_0200();//wei,20160823,触发震动报警，位置上发
                }
#endif
                if((alarm_in_flag )&&(secondcounter-alarm_interval_timer>=10)&&(alarm_count<=5))
                //if( get_ALARM_status())
                {
                    //delayms(1000);
                    //ddi_uart_Write(&UART1_ctl_t,"Alarm!\r\n",strlen("Alarm!\r\n"));
                    //if (	get_ALARM_status()&&( voltage>power_battary))
                    //if( get_ALARM_status())
                    //if( get_ALARM_status() &&((BatteryStatusGet()>50)))
                    if(BatteryStatusGet() > 50)
                    {
                        if((system_para.g_bAUTOSMSon))
                        {
                            g_ucAlarm = 0x5a;
//wei,20160712                            if((GetAvailableGPSInfoV() )//发送当前位置到前两个授权号码中
//                            if((GetAvailableGPSInfoV())&&(alarm_count ==0))//wei,20160712,alarm_count用来保证短信不会频繁发送
                              if((alarm_count ==0))
                              {
                                j =0;
                                for(i=0 ; i<2 ; i++)
                                {
//ljw,20160928for wulian                                    if((system_para.g_ucRegNum[i][0] ==  FULL)&&(0 != strncmp((char *)(&system_para.g_ucRegNum[i][4]),"00000000000",11)))//ljw,20160902
                                    if((system_para.g_ucRegNum[i][0] ==  FULL)&&(system_para.g_ucRegNum[i][1] == strlen((u8 *)(&system_para.g_ucRegNum[i][2]))))//ljw,20160928for wulian
                                    {
                                       j++;
                                       if (j<3)
                                       {	//只发送前两个号码                                            
//ljw,20160928for wulian                                            SendALARMSMS1((char *)(system_para.g_ucRegNum[i]+4));
                                           SendALARMSMS1((char *)(system_para.g_ucRegNum[i]+2));
                                           delayms(3500);	//发送短信后，不等三秒就发送GPRS 会失败
                                       }
                                    }
                                }
                                //g_ucAlarm = 0;
                            }
                        }
                        //触发一次数据上发                                          
                        if(alarm_count++ <5)
                        {
                            cmd_0200();//wei,20160823,改上报位置信息，将上报判GPS还是基站放到
                            #if 0 //wei,20160823,改上报位置信息，将上报判GPS还是基站放到
                            if(NOLocalCount>=120)
                            {
                                uploadLAC();
                            }
                            else
                            {
                                cmd_0200();
                            }
                            #endif
                            alarm_interval_timer = secondcounter;
                            g_ucUpdateFlag = 0;
                            back_g_usGPRSUpLoadAternationTime = secondcounter;//wei,20160713,位置上传清零                            
                            back_g_usHeartAternationTime = secondcounter;
                        }
                        else alarm_count = 6;                        							
                    }
                    alarm_in_flag = 0;
                }

				if(!g_ucSMSCmdMissionFlag) //ljw,20160906for space && (GPRSCmdFlag == 0))
				{
					if (lowbattcount > 180) //30  // once a  minute 
					{
						if((voltage< LOW_POWER) &&(voltage >= POWEROFF))
						{
							delayms(200); 
							if((voltage < LOW_POWER) &&(voltage >= POWEROFF))
							{
								g_ucLowPowerFlag = 1;
								g_ucFStatus |= (LOWPOWERFLAG);
								if (system_para.g_bAUTOSMSon)
								{
									for(i=0 ; i<2 ; i++)
									{
//ljw,20160928for wulian										if((system_para.g_ucRegNum[i][0] ==  FULL)&&(0 != strncmp((char *)(&system_para.g_ucRegNum[i][4]),"00000000000",11)))//ljw,20160902
										if((system_para.g_ucRegNum[i][0] ==  FULL)&&(system_para.g_ucRegNum[i][1] == strlen((u8 *)(&system_para.g_ucRegNum[i][2]))))//ljw,20160928for wulian
                                        {
										   {	//只发送前两个号码									          	
//ljw,20160928for wulian									          	Send_lowpower_LocaSMS((char *)(system_para.g_ucRegNum[i]+4));
                                               Send_lowpower_LocaSMS((char *)(system_para.g_ucRegNum[i]+2));
                                               delayms(3500);	//发送短信后，不等三秒就发送GPRS 会失败
										   }		
										}	  
									}
								}
							}
						}
						else
						{
                            g_ucLowPowerFlag = 0;
                            LOWPOWERCount = 0;
                            g_ucFStatus &= ~(LOWPOWERFLAG);
						}
						lowbattcount = 0;
					}
				}
//wei,震动芯片异常处理，再次初始化<
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
            if(BitFlag.SHACK_Init_Flag == 0x03)//wei,20160603,要重新初始化震动芯片
            {
                BitFlag.SHACK_Init_Flag = 0x02;//wei,20160604,初始化3DH的时候，中断里面不要接收震动信号，可能是信号不稳产生的干扰
                LMS330DLC_init();
                set_Impact(system_para.acceleration,system_para.impact_time);
                delayms(100);
                SHACK_Init_Flag_Time = 0;
                BitFlag.SHACK_Init_Flag = 0;
            }
#endif
//wei,震动芯片异常处理，再次初始化>
        }
    }
}




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
