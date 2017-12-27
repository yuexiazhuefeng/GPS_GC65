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
 * @Revision History- 20150514 12:00����λ�õĸĺ����������ŵĴ���Ҫ�����㣬���ŵĽ��գ�����
 * @Log             - ��20150515: at first LED all flash .
 *                              20150531: �������ò�����Ҳд���ˣ�������û�е��ԣ����ж�ʱ���Ѻ�����û��������
 *                                             Ҫ�ܹ�����24Сʱ�ģ�ʹ��RTC, ȡ��ǰʱ�������Ҫ��ʾ��ʱ��ʱ�䰲
 * // 20150630>12.00: ���ٶ�оƬ���˵�ַҲ���ܶ���WHO AM I������оƬ���� 
 * // 20150702 �����ADC����
 * // 20150721,������UART0_ReceiveData��û�е��Ժ�,����ʱҪ����δ�ӵĶ��ţ�����û����
 * // 20150724:�͹��Ĳ��ԣ���ʱ����ʹ��RTC����������ʱ��ʹ����ͨ��ʱ������ϵͳTICK��ʱ��
 * // 20150725: gprsͨѶʱ��Ϊ�˱���·����Ҫ��ʱ���ͣ���Ȼ��������壡壡!!
 * // ��������������ʱ�������Ǽ���ͻ����ˣ��� �����ǲ��ǿ��Ź���ԭ�� �����˿��Ź�Ҳ����
 * // ���ڲ��������߿���,�������ߵĻ���ʱ����׼�ġ������ǲ��ǲ������жϷ�������н�������
 * // 20150808:����ָ���gprs ID��0x0f00->0x0f88
 * // 20151106: ����gsm 120�붼�ղ�������������Ϣ,������GSM.  ���ߵ�ʱ,Ҫ�����ݸ�λ,��ֹ���Ѻ���������gsm.
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
//device ID ������ΪЭ���д���������ֻ���
 const u8 DEVICEID[]={0x01,0x56,0x56,0x04,0x93,0x32,0x00};
//ljw,20161129const u8 DEVICEID[]={0x01,0x85,0x51,0x09,0x00,0x21,0x00};
const u8 PASSWD[]= "123456";
//wie  const u8 IPADDR[]=	"220.178.0.57";  //"218.22.32.154"//	"220.178.0.58"
const u8 IPADDR[]=	"220.178.0.54";  //wei"218.22.32.154"//	"220.178.0.58"wei
const u8 IPPORT[]=	"9668";  //"4095" (��˾��Э�� ) //"6000" //	9668(����Э��)

//wei  const unsigned char  g_ucVERSION[18] = "900MTS001U-160129";         	// ϵͳģʽ Ӱ�ӼĴ���
const unsigned char  g_ucVERSION[18] = "900MTS001U-161220";         	// ϵͳģʽ Ӱ�ӼĴ���

//�����ȴӵ͵���
//ljw,20160914 ע�͵�ʹ�ò��Եģ���ָ� const u8 acceleration_para[3]={0x4a,0x3a,0x1a};
//ljw,20160914 ע�͵�ʹ�ò��Եģ���ָ� const u8 impact_time_para[3]={0x5,0x5,3};
//ljw,20160919����ָ�const u8 acceleration_para[3]={0x4a,0x3a,0x22}; //ljw,20160914.   2a  24  20
//ljw,20160919����ָ�const u8 impact_time_para[3]={0x7,0x5,3};

const u8 acceleration_para[3]={0x42,0x32,0x2a}; //ljw,20160914.����Ҫɾ 2a  24  20
const u8 impact_time_para[3]={0x03,0x03,0x03}; //ljw,20160914.����Ҫɾ
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
// δ��λ��ʱ
//wei,20160823�ŵ�main.h����  extern  u32 NOLocalCount;

/***********************************************************************
 * Function prototypes of global functions imported.  
 **********************************************************************/

/***********************************************************************
 * Data definitions of global data exported.  
 **********************************************************************/
SYSTEM_PARA  system_para;
unsigned char g_ucSysMode;  						// 0: SMS   1: SMS & GPRS
//unsigned char g_ucSMSMode;                                          // 0:�ر��Զ����Żظ���1�����Զ����Żظ�
//unsigned char g_ucSysInitialized		= SYS_UNINITI;		// ϵͳ��ʼ����־
unsigned char g_ucSim900Status			= IDLE;			// SIM900ģ��״̬ 
//ljw,20160924 unsigned char g_ucCurrentNum[11]		= "00000000000";	// ��ǰ�����豸�ĺ���
unsigned char g_ucCurrentNum[PHNUM_LEN]		= {0};	// ��ǰ�����豸�ĺ���
unsigned char g_ucUART6RXUsed			= 0;			// ����6��ǰ״̬
unsigned char g_ucTaskList			= 0;			// �����
unsigned char g_ucCurrentSMSNum[2]	        = {0,0};		// ��ǰ���Ŵ洢���
unsigned char volatile g_ucInitSettingFlag	= 0;			// ��ʼ�����ñ�־λ
volatile unsigned char g_ucChargingFlag 	= 0;			// ���״̬��־λ
volatile unsigned char g_ucUpdateFlag 		= 0;			// ϵͳ״̬���±�־
volatile unsigned char g_ucLowPowerFlag		= 0;			// ������
volatile unsigned char g_ucFStatus		= 0;			//����״̬
volatile unsigned char g_ucHeartUpdateFlag = 0;            // ������Ҫ�ϴ��ı�־
volatile unsigned char g_ucACCFlag = 0;            //wei,20160524 ACC״̬��־0:ACCû��   0x5a:ACCΪ�ϵ�
// GPRS ������ʽ��־λ
//volatile unsigned char g_ucGPRSMode 			= GPRS_MODE_WAKE; //GPRS_MODE_PERIOD;	// ��ʱ��ģʽ��־λ

// ��ǰʱ��
//unsigned short g_usCurrentTime;
// ��ǰ�ٶ�
unsigned short g_usCurrentSpeed = 0;
// ����ʱ��
//unsigned short g_usWakeTime;
// ����ģʽ�� �����ϴ���־ 

// ���ѹ���ʱ��
//unsigned short g_usGPRSWakeUpTime = 5*60;   				// Ĭ��: 5 ����
u8 entersleepflag=0;
//rtc�����ߵĻ��Ѵ���
u16 rtc_wakeup_timer=0;
//��ʱ������Ҫ����SLEEP
u8 g_ucGotoSleepFlag=0;
u8 REGNUMbeSET=0;
volatile unsigned short TimeUpLdCnt = 20;
// ��γ������
//double g_dCurrentLat = 0, g_dCurrentLon = 0;  	// ��ǰγ�� ����
//double g_dMaxLat     = 0, g_dMaxLon     = 0;  	// ���γ�� ����
//double g_dMinLat     = 0, g_dMinLon     = 0;  	// ��Сγ�� ����
//double g_dMetersPLonDegree = 0; 				// ��ǰ������ ����ÿ�ı�һ�ȵر�仯�ľ���

// GPRS ����ִ�������־λ
unsigned char volatile g_ucGPRSCmdMissionFlag = 0;  // 1 -- ��δִ������ 0 -- ��δִ������
// SMS ����ִ�������־λ
unsigned char volatile g_ucSMSCmdMissionFlag = 0;  	// 1 -- ��δִ������ 0 -- ��δִ������
//�񶯻�ACC ��������
unsigned char volatile g_ucACC = 0;
// �ٶ����Ʊ�����־
unsigned char volatile g_ucSpeedLimitAlarm = 0;
//��������״̬
unsigned char volatile g_ucAlarm = 0;
//����ģʽ
//unsigned char volatile g_ucENERGY;
//����͹���ģʽ�ı�־
unsigned char volatile g_FlagENERGY = 0;
//��ʱ����ʱ����
//unsigned  int  WtmCount;
//ACC ״̬�仯����ʱ��
unsigned short int volatile ACCCount=0;
//unsigned char  volatile ACCflag=0;
//�񶯼���źű仯�ĳ���ʱ��
unsigned short int volatile ShakerCount=0;
//�����ʼ���ı�־
volatile unsigned char  EN_initialize=1;
//�͵�������ʱ��
unsigned short volatile LOWPOWERCount=0;
//�ⲿ�жϴ���ϵͳ����
unsigned char intflag=0;
unsigned char g_ucSim900Buff[SIM900_BUFF_SIZE];	// GSM  ���������
unsigned char g_ucNeo6QBuff[85];//[75];		// GPS  ���������
unsigned char backup_g_ucNeo6QBuff[85];//[75];	 // ���һ����Ч��λ
unsigned char g_ucGPRSDataBuff[150];	// GPRS ���ݻ�����
//unsigned char g_ucGPRSDataBuff_hex[170];	// GPRS ���ݻ�����
unsigned char g_ucGPRSCmdBuff[GPRS_BUFF_SIZE];	// GPRS �������
//LJW,20161124,�������Ŀǰֻ���������в��������õ��ˡ� unsigned char g_ucGPRSCmdBuffback[GPRS_BUFF_SIZE];	// GPRS �������
unsigned char g_ucGPRSCmdBuffback[GPRS_BACK_BUFF_SIZE];	// GPRS �������//LJW,20161124,�������Ŀǰֻ���������в��������õ��ˡ�
//ljw,20160906for space unsigned char volatile GPRSCmdFlag;
u16 voltage;  //��ӵ�Դ��ѹ
u8 shakeon;  //��״̬
//�����״̬
u8  ring_sms;
volatile unsigned char  bit_flag = 0;//wei.20160530,λ0,1==1,��������AGPS;  λ2,3==1, ����Ҫ���³�ʼ����оƬ3DH
BITFlag BitFlag;//wei,20160603,����λ��
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
u8 SHACK_Init_Flag_Time = 0; //wei20160603,�𶯽ŵ͵�ƽ��ʱ������ʱ��������
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

//�͵�ѹ������
u32 lowbattcount=0;
u8 chinesesmsflag=0;
u8 startchinesesms=0;

//��ΪҪ�������һ����Ч��λ���ݣ����������ݴ���ʱ������gps������������
u8 gps_gate= 1; // 1:������GPS����g_ucNeo6QBuff����д�ջ�õ�GPS����, 0:���ڲ���g_ucNeo6QBuff�����е����ݣ���������д�µ�����
u8 gps_copy=0;
//ȷ���ǵ͵�ѹҪ������
u8 lowbatt_flag = 0;
//��������Ӧ��,һֱû�оͻ�����GSM
u8 server_ack_flag=0;
// û��Ӧ��һ���һ��,�ӵ�120�ξ�����ģ��
u16 server_noack_counter=0;
//����GSMģ���־
u8 rst_gsm_flag= 0;


mileage_ctl_t wzw_mileage;
u16 mileage_tmp=0;

//��ǰSIM����Ӫ�ˡ������ƶ���������ͨ
u32 MobileCompany=0;
/***********************************************************************
 * Function prototypes of functions requiring global scope
 * not included elsewhere.  
 **********************************************************************/
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
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
   //   GSM_Power(FALSE); //wei,20160608���Ź���λǰ����GSM�ϵ�
//ljw,20160908���Ч��       while(1); //�ȿ��Ź���λϵͳ
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
void 	energy(u8 power) //����͹���ģʽ
{
	NVIC_InitTypeDef   NVIC_InitStructure;
      GPIO_InitTypeDef GPIO_InitStructure;
	u16 i;
	g_ucGotoSleepFlag = 0;
	
	WaitGSMOK1();//ljw   WaitGSMOK();
	// ���ߵ�ʱ,Ҫ�����ݸ�λ,��ֹ���Ѻ���������gsm.
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
#if 0 //ljw,20160903������ϲ���
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

#if 0 //ljw,20160903������ϲ���
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

#if 0//wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ�
		GPIO_InitStructure.GPIO_Pin  =  DTR_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(DTR_PORT, &GPIO_InitStructure);
#endif //wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ�
//WEI20160607	}
	IWDG_ReloadCounter();

	 //while(RTC_ALARM_timer < (system_para.g_usSleepTime*6))
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
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
    
	 //�˳����ߣ�������������ʱ��
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

		//����λ��ʱ5���Ӽ�������λ��
		timegpserror = 0;
		GPSOn();
        delayms(30);//ljw,20161110GPS���ϵ��򿪴���
        ddi_uart2_init(ENABLE); //ljw,20161019
        delayms(10);//ljw,20161019,�ô����ȶ�һ��
//WEI20160607	GSM_reset();
		if(GSM_Power(TRUE))//ֱ�Ӹ������ϵ�
		{
			RING_EXTI0_Config();			
		}
		dial_status = 0;
		i=0;//wei20160607
		while(dial_status == 0)
		{
			StartGPRS();	
			i++;//wei20160607
			if((i%2==0)||(CPIN_READY_flag == 0))//wei20160607//��������û�������ϵ�����GSM
			{
	                 GSM_reset();////wei20160607����GSM
			}
		}
	 	// ����ģ�����߹���
#if 0//wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ�
		ddi_uart_Write(&UART1_ctl_t,"AT+QSCLK=1\r\n",strlen("AT+QSCLK=1\r\n"));
		WaitGSMOK1();//ljw   WaitGSMOK();
#endif //wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ�
             //����λ����־����֤���Ѻ�ȷ���Ƿ�λ����
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
	//��һ��GPRS 
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
//			   	//ֻ����ǰ��������		          	
//                SendALARMSMS1((char *)(system_para.g_ucRegNum[i]+4));
//				delayms(3500);	//���Ͷ��ź󣬲�������ͷ���GPRS ��ʧ��			   	
//			}
//ljw,20160928 for wulian>
		}
	}
	// ��ֹһ�����߾�����GSM
    ACC_GPS_flag = 0x00;
    ACC_GSM_flag = 0x00;
	rst_gsm_flag = 0;
	server_ack_flag = 0X5A;
	server_noack_counter = 0;
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
	SHACK_Init_Flag_Time = 0;//wei20160604��ֹһ�����ͳ�ʼ����оƬ
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
//wei,20160711 adjusting TIM3 interrupt to 1ms	TIM_TimeBaseStructure.TIM_Period = 4800; // ms, ����ֵ
	TIM_TimeBaseStructure.TIM_Period = 6000; // ms, ����ֵ//wei,20160711 adjusting TIM3 interrupt to 1ms
    TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//PSCԤ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���������뷽ʽ,���ϼ���

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
		TIM_Cmd(TIM3, ENABLE);//��ʼ��ʱ

}

//wei ��������洢�ĺ��������������
u8 Store_System_Para(void)
{
	u8  i = 0;
	for(i=0;i<3;i++)
	{
		if(!Set_Store_System_Para()) //�ɹ����� 0�����򷵻ط���
			break ;
	}
	if(i == 3) //����Ĵ洢ʧ����
	 FLASH_ErasePage_Complete((u32 )system_para_ADDRESS);//�������ҳ
	for(i=0;i<3;i++)
	{
		if(!Backup_Store_System_Para())  //�ɹ����� 0�����򷵻ط���
			break ;
	}
	if(i == 3) //����Ĵ洢ʧ����
	 FLASH_ErasePage_Complete((u32 )backup_system_para_ADDRESS);//�������ҳ;//�������ҳ
}

//wei,20160517,����һ��ҳ
//������ȷ������0 �� �������󷵻�0xff
u8  FLASH_ErasePage_Complete(u32 addr)
{
 
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
//wei,20160829����	if (FLASH_ErasePage(backup_system_para_ADDRESS)!= FLASH_COMPLETE)
    if (FLASH_ErasePage(addr)!= FLASH_COMPLETE)
	{
	 /* Error occurred while sector erase. 
	     User can add here some code to deal with this error  */
	  return 0xff;
	}
	FLASH_Lock(); 
	return 0;
}
//���ϵͳ�����ı��ݣ��ŵ�800FC00�����У�飬�ɹ�����0�������ķ���
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
			return 0xff;  //ֱ���Ƴ���Ȼ����д���Σ���ֱ��д�ɹ�
		}
		i +=4;
//wei		ptmp +=4;
//wei		ptmp_system_para += 4;
		ptmp += 1;  //wei
   	ptmp_system_para +=1; //wei

  }
	return 0;
}

//���ϵͳ�������ŵ��������棸�����������У�飬�ɹ����أ��������ķǣ�
//wei  u8 Store_System_Para(void)
u8 Set_Store_System_Para(void)   //wei,��0x800F800���洢�������ܹ��������ط��洢����һ���ط��Ǳ�����0x800f600
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
			return 0xff;  //ֱ���Ƴ���Ȼ����д���Σ���ֱ��д�ɹ�
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
	// ϵͳ������ʼ��
	memset((u8*)&system_para,0,sizeof(system_para));// id ������һ�ε�
	
	system_para.g_ucSysInitialized = SYS_INITIED;
	//system_para.g_ucSysMode_S 	  	= MODE;         	// ϵͳģʽ Ӱ�ӼĴ���
	//system_para.g_ucPowerMode               = IPOWERMODE; //ϵͳ�Ĺ���ģʽ
	system_para.g_ucTrackMode		= TRACK_MODE_GPRS;	// ��λģʽ
	// IP ��ַ
	if (FHTIP != system_para.g_ucSysMode_S)
	{
		memcpy(system_para.g_ucIPAddress,IPADDR, strlen(IPADDR) );
		//system_para.g_ucUDPPort[5] 	   	= IPPORT;		// UDP �˿�
		memcpy(system_para.g_ucUDPPort,IPPORT, strlen(IPPORT) );
	}	
	//system_para.g_ucDeviceID[6] 	   	= DEVICEID;		// �豸ID
	memcpy(system_para.g_ucDeviceID,tmp_mid, sizeof(tmp_mid));
	//system_para.g_ucNetUser[6] 	   	= "GPRS";		// �豸ID
	memcpy(system_para.g_ucNetUser,"GPRS", strlen("GPRS") );
	//system_para.g_ucNetPsw[8] 	   	= "GPRS";		// �豸ID
	memcpy(system_para.g_ucNetPsw,"GPRS", strlen("GPRS") );
	//system_para.g_ucNetAPN[20] 	   	= "CMNET";		// �豸APN
	memcpy(system_para.g_ucNetAPN,"CMNET", strlen("CMNET") );

	//system_para.g_ucPassWord[6] 	   	= PASSWD;  		// ϵͳ����
	memcpy(system_para.g_ucPassWord,PASSWD, strlen(PASSWD) );

	system_para.g_usGPRSUpLoadTime  = default_g_usGPRSUpLoadTime;		// �ϴ�����ʱ�� ��λ ����
	system_para.g_usGPRSUpLoadAternationTime = UPLOADTIME;		// �ϴ����
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
//ljw,20160926for wulian	  	strncpy((char *)system_para.g_ucRegNum[i],"e   00000000000",15);        // ɾ���ڴ�����Ȩ����
        strncpy((char *)system_para.g_ucRegNum[i],"e",1);        // ɾ���ڴ�����Ȩ����
	}
	mileage_flash_init();
}

//��ʼ�����˵ģ��������ݷŵ����ṹ���У�û�г�ʼ���ķ���Ĭ��ֵ
//wei,20160517
void sys_Init(void)
{
 	SYSTEM_PARA *tmp;
	//���Ѿ���ʼ������������flash�д�ŵ����ݣ�
	tmp = (SYSTEM_PARA *)system_para_ADDRESS;
	if (tmp->g_ucSysInitialized == SYS_INITIED)
	{
		memcpy((u8*)&system_para,(u8*)tmp,sizeof(system_para));
		tmp = (SYSTEM_PARA *)backup_system_para_ADDRESS;//wei,׼����鱸����
		if (tmp->g_ucSysInitialized != SYS_INITIED) //����������,�ٴ�д������
		{
				if(Backup_Store_System_Para())//������ûд�ɹ�����дһ�飬�ܹ�д����
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
//wei,20160622			if(Backup_Store_System_Para())//����������ȷ�ģ������Ĵ洢���Ǵ���ģ���дһ��洢�������ʧ����дһ��
//wei,20160622					Backup_Store_System_Para(); //�ϲ�дʧ���ˣ���дһ�飬�����ʧ�ܣ��Ͳ�д�ˡ�
            if(Set_Store_System_Para())//����������ȷ�ģ������Ĵ洢���Ǵ���ģ���дһ��洢�������ʧ����дһ��
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
		else  //��������ִ洢��������
		{
			RestoreSystem();//�����ӵ�һЩ������ΪĬ��ֵ		
            memcpy(system_para.g_ucDeviceID,DEVICEID, sizeof(DEVICEID));
//ljw,20161129��Ӧ������� strlen(DEVICEID)			memcpy(system_para.g_ucDeviceID,DEVICEID, strlen(DEVICEID) );
			
		}
	}
		

}	
#if 0  //wei
//��ʼ�����˵ģ��������ݷŵ����ṹ���У�û�г�ʼ���ķ���Ĭ��ֵ
void sys_Init(void)
{
 	SYSTEM_PARA *tmp;
	//���Ѿ���ʼ������������flash�д�ŵ����ݣ�
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

		RestoreSystem();//�����ӵ�һЩ������ΪĬ��ֵ		
		memcpy(system_para.g_ucDeviceID,DEVICEID, strlen(DEVICEID) );
		
	}	

}	
#endif  //wei

uint8_t readio=0xff;
//#define TESTLED
// #define test_adc
extern u8  test_a_gps(void);

//wei20160608,<������������֮ǰ����ʱ
void delayms_nodog(u32 count)
{	
	for (;count>0; count--)
	{
		delayus(1000);		
	}
}
//wei20160608,>������������֮ǰ����ʱ

//ljw,20160924ʹ�÷ֺ�����ȡ���ֻ��ţ������������ƣ�ֻҪ���ű��������
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
    if(system_para.g_ucSysInitialized == SYS_INITIED)	// ���ϵͳδ��ʼ�� ���������κζ�������
    {
        for(j=0; j<5; j++)
        {
            if (g_ucSMSCmdMissionFlag) // ��ֹ�µĶ����ֽ���
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
            UART0_ReceiveData(g_ucSim900Buff,0);  //0	// �����жϷ�ʽ���ն�������
            //+CMGR: "REC UNREAD","1064899150042",,"16/09/24,08:41:55+32"
            if (NULL == strstr((char *)g_ucSim900Buff,"+CMGR:"))
            continue;
            // Ѱ�Ҷ���Ϣ�����ߺ���
            pStart = (u8*) strstr(g_ucSim900Buff,"\"");   		// ��һ��������
            pStart = (u8*) strstr((pStart+1),",\"");         		//�Һ���ǰ��� ,"
            pEnd = (u8*) strstr((pStart+1),"\",");         		// �Һ�������",
            temp_len =pEnd-pStart-2;//ljw,2016092��ȡ���볤��
            if(temp_len >= PHNUM_LEN-3)
                continue;
//            if('1'  ==  *(char*)(pStart + 2))
//            pStart += 2;
//            else
//            pStart += 5;
//            strncpy((char *)g_ucCurrentNum,(char *)(pStart),11); 	//some has +86,someone no	// ָ���ͺ���
            pStart += 2;
            memset(g_ucCurrentNum,0x00,sizeof(g_ucCurrentNum));//ljw,20160925����գ��ٴ�ź���
            strncpy((char *)g_ucCurrentNum,(char *)(pStart),temp_len); 	//some has +86,someone no	// ָ���ͺ���
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
//LJW,20160928��Ҫ���ַ��ж�            if(isdigtit_len(pStart,temp_len))
//            {
//                memcpy(&(smsdatareg.mobilenum[smsdatareg.count]),g_ucCurrentNum,11);
                memset((u8 *)(&(smsdatareg.mobilenum[smsdatareg.count])),0x00,PHNUM_LEN);//ljw,20160925��������á�
                memcpy((u8 *)(&(smsdatareg.mobilenum[smsdatareg.count])),g_ucCurrentNum,temp_len);
                analysis_sms(pStart);
                setSMS();
 //           }
            //20131105: �������ʱ��ҲҪɾ����Ϣ 
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
            //UART0_ReceiveData(smssendbuf,2);  //0	// �����жϷ�ʽ���ն�������
            //20150721: ����о�Ҳû�����յ��Ľ��
            UART0_ReceiveData(g_ucSim900Buff,0); 
            delayms(1000);
            // �����ʱ��GPRS���ݾ͸Ͻ�����GPRS
            if(g_ucGPRSCmdMissionFlag || chinesesmsflag)
            {
                break;
//ljw,20160908                break;
            }
        }
        delayms(2000);  //wei,20160630,��ʱ2�룬�ٷ��ж��ǲ���Ҫ��ɾ���ŵĶ���
        if((g_ucSMSCmdMissionFlag == 0)&&(g_ucGPRSCmdMissionFlag == 0)&&(chinesesmsflag==0))
        {
            //wei,20160630                delayms(2000);  // 2000
            if((0x5a !=ring_start_flag)&&(ring_sms !=SMS))
            {
                ddi_uart_Write(&UART1_ctl_t,"AT+CMGD=1,4\r\n",13);
                delayms(500);  // 2000
            }
        }
    }// ����ʼ�����

    g_ucSMSCmdMissionFlag = 0;
    //��ֹ���Ŵ���ʱ�䳤����GSM��λ
    rst_gsm_flag = 0;
    server_ack_flag = 0X5A;
    server_noack_counter = 0;
}

/*���Ŵ����ı��ֻ�����ĳ���
void Deal_SMS(void)
{
    volatile u32 i;
	volatile u32 j;
    u8 *pStart;
	u8 *pEnd;	
    u8 smssendbuf[30]="AT+CMGR=";
//wei,20160825���Զ����ϱ�3��4������    ����Ҫɾ
//    ddi_uart_Write(&UART1_ctl_t,"AT+CNMI?\r",strlen("AT+CNMI?\r"));
//    WaitGSMOK2(250);
//    ddi_uart_Write(&UART1_ctl_t,"AT+CPMS=\"SM\"\r",strlen("AT+CPMS=\"SM\"\r"));  	// //��ѡ���Ŵ洢��ΪSIM��
////DebugOut("* Set SMS Format: TEXT.\r\n",0);
//    WaitGSMOK();
//    
//    ddi_uart_Write(&UART1_ctl_t,"AT+CPMS?\r",strlen("AT+CPMS?\r"));  	// //��ѡ���Ŵ洢��ΪSIM��
////DebugOut("* Set SMS Format: TEXT.\r\n",0);
//    WaitGSMOK();
//wei,20160825���Զ����ϱ�3��4������    ����Ҫɾ
    ring_sms = NO_USE;
    if(system_para.g_ucSysInitialized == SYS_INITIED)	// ���ϵͳδ��ʼ�� ���������κζ�������
    {
        for(j=0; j<5; j++)
        {
            if (g_ucSMSCmdMissionFlag) // ��ֹ�µĶ����ֽ���
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
            UART0_ReceiveData(g_ucSim900Buff,0);  //0	// �����жϷ�ʽ���ն�������
            if (NULL == strstr((char *)g_ucSim900Buff,"+CMGR:"))
            continue;
            // Ѱ�Ҷ���Ϣ�����ߺ���
            pStart = (u8*) strstr(g_ucSim900Buff,"\"");   		// ��һ��������
            pStart = (u8*) strstr((pStart+1),",\"");         		// �ڶ���������
            if('1'  ==  *(char*)(pStart + 2))
            pStart += 2;
            else
            pStart += 5;
            strncpy((char *)g_ucCurrentNum,(char *)(pStart),11); 	//some has +86,someone no	// ָ���ͺ���

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
            //20131105: �������ʱ��ҲҪɾ����Ϣ 
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
            //UART0_ReceiveData(smssendbuf,2);  //0	// �����жϷ�ʽ���ն�������
            //20150721: ����о�Ҳû�����յ��Ľ��
            UART0_ReceiveData(g_ucSim900Buff,0); 
            delayms(1000);
            // �����ʱ��GPRS���ݾ͸Ͻ�����GPRS
            if (g_ucGPRSCmdMissionFlag || chinesesmsflag)
            {
                break;
//ljw,20160908                break;
            }
        }
        delayms(2000);  //wei,20160630,��ʱ2�룬�ٷ��ж��ǲ���Ҫ��ɾ���ŵĶ���
        if((g_ucSMSCmdMissionFlag == 0)&&(g_ucGPRSCmdMissionFlag == 0)&&(chinesesmsflag==0))
        {
            //wei,20160630                delayms(2000);  // 2000
            if((0x5a !=ring_start_flag)&&(ring_sms !=SMS))
            {
                ddi_uart_Write(&UART1_ctl_t,"AT+CMGD=1,4\r\n",13);
                delayms(500);  // 2000
            }
        }
    }// ����ʼ�����

    g_ucSMSCmdMissionFlag = 0;
    //��ֹ���Ŵ���ʱ�䳤����GSM��λ
    rst_gsm_flag = 0;
    server_ack_flag = 0X5A;
    server_noack_counter = 0;
}
*/

//ljw,20160908�ж����������� 0x05,����  0x0a������
u8 check_sleep_state(void)
{
    if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))//��������������Ҫ����
    {
        if((g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE)) //wei,20160709,����˯����������Ҫ�ڼ���������
            return 0x05;
    }
    return 0x0a;
}

u32 alarm_interval_timer = 0;//wei,20160712,����������ʱ��������Ҫ����ʱ����������
u8 alarm_count = 0;//wei,20160712,���������Ĵ���
//u8 test_LIS3DH_timer; //ljw,20160913,������ ����Ҫɾ
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
    memset((u8 *)&BitFlag,0x00,sizeof(BITFlag));//wei,20160603,���λ����
    IWDG_Init(IWDG_Prescaler_256,0x0c30);//wei,20160805,�ѿ��Ź��ŵ���ǰ��   ʵ��15--20��// 20 miao,
	IO_Init();

//wei	set_GPS_POWERON (FALSE);
    set_GPS_POWERON (TRUE);
    GPIO_ResetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);//wei,20160805 ����GSMΪ�͵�ƽ
		//��ʼʱ��һ�¾���
//ljw	GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN);
//ljw	GPIO_ResetBits(LED2_GPIO_PORT,LED2_PIN);
//ljw	GPIO_ResetBits(LED3_GPIO_PORT,LED3_PIN);

//wei,20160805 �����Ź��ŵ�ǰ��ȥ	IWDG_Init(IWDG_Prescaler_256,0x0c30);//wei20160606,ʵ��15--20��// 20 miao, 

	sys_Init();
//wei,160517,���Ա�����
//		smssendbuf[0] =Store_System_Para();//wei
//	smssendbuf[2] = Set_Store_System_Para();//wei
//	smssendbuf[0] =Backup_Store_System_Para();//wei
//	mileage_flash_init();//wei
//	smssendbuf[1] =FLASH_ErasePage_Complete((u32 *)backup_system_para_ADDRESS);//wei
//wei,160517,���Ա�����

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
	
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);//����PWRKEYΪ����
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	GPIO_SetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);//��GSMģ���ϵ�
//wei	delayms( 10);
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	delayms(30);//wei
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	GPIO_SetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);	//����PWRKEY��

	//��ʼʱ��һ�¾���
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
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
	//������ͨ�����ڳ�ʼ������
	InitialSetting();
	EN_initialize = 0;

//wei,20160803,for space  ��Ҫ GPS_on();��	GPS_on();
    set_GPS_POWERON (TRUE);//wei,20160803,for space  ��Ҫ GPS_on();��

	// gsm ���������ó�115200
//wei,20160826 ��9600����AGPS	UART1_ctl_t.Parity = Parity_No;     //wei,20160826 ��9600����AGPS
//wei,20160826 ��9600����AGPS	ddi_uart_open(&UART1_ctl_t,115200);

	ddi_uart_Write(&UART1_ctl_t,(u8*)g_ucVERSION,17);//wei,20160603����1��ӡһ�³���汾��
	ddi_uart_Write(&UART1_ctl_t,"\r\n",2);//wei,20160603����1��ӡһ�³���汾��
	//send GSM start flag
	//ddi_uart_Write(&UART1_ctl_t,"SET GSM BUAD 115200\r\n",strlen("SET GSM BUAD 115200\r\n"));  
//ljw,20160904	memset(backup_g_ucNeo6QBuff,0,sizeof(backup_g_ucNeo6QBuff));
    memset((u8 *)(&gps_ascii),0,sizeof(gps_ascii));

 	mileage_init();
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
	LMS330DLC_init();
	/**********************************************************************
	//˵��:  ������ײ��ֵ����ײ����ʱ��
	//����1 accelero:  ��ײ��ֵ���ֱ���0.1g ��Χ0~79
	//����2 time: ��ײ����ʱ�䣬�ֱ���4ms, ��Χ0~38
	//����: 0 Ϊʧ�ܣ�1 Ϊ�ɹ�
	**********************************************************************/
	//u8 set_Impact(u8 accelero,u8 time)
		//set_Impact(10,5); //0x1a,3      (������)
						  // 0x3a, 5 
						  //0x 4a ,5  (���)
	//set_Impact(0X2A,3);
	set_Impact(system_para.acceleration,system_para.impact_time);
//    LMS330DLC_SetInt1Threshold (system_para.acceleration);//ljw,20160914,����Ҫɾ
//    LMS330DLC_SetInt1Duration(0x00);//ljw,20160914,����Ҫɾ
//wei20160603<

    SHACK_Init_Flag_Time = 0;
    BitFlag.SHACK_Init_Flag = 0;

//wei20160603>
#endif
	//if (SysTick_Config(SystemCoreClock / 1000))
	// 0.1 s
//wei20160819, �ŵ����ò���ǰ�������Ա����ò����Ķ�ʱ��ʹ��	if (SysTick_Config(SystemCoreClock / 10)) 
//wei20160819, �ŵ����ò���ǰ�������Ա����ò����Ķ�ʱ��ʹ��	{ 
		/* Capture error */ 
//wei20160819, �ŵ����ò���ǰ�������Ա����ò����Ķ�ʱ��ʹ��		while (1);
//wei20160819, �ŵ����ò���ǰ�������Ա����ò����Ķ�ʱ��ʹ��	}
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);//wei20160601�ͷ�PWRKEY
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	delayms(20);//wei20160601
//wei test
//    i=0;
    ddi_uart_Write(&UART1_ctl_t,"run later...\r\n",strlen("run later...\r\n")); //wei

//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	if(!Check_AT())//wei20160601
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	{
    ddi_uart1_init(DISABLE); //ljw,20161110
    delayms(500);//ljw,20161110,�ô����ȶ�һ��
    if(GSM_Power(TRUE))//�ϵ��ȴ�10ms,��������PWRKEY��1.02��
    {
        RING_EXTI0_Config();
        delayms(10);
        Clear_All_GSM_flag();
    }
	else    GSM_reset();//����GSM

//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	}
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬	else
//wei,20160805 ��ֹ�쳣��λ������GSMΪ�ϵ�״̬		RING_EXTI0_Config();
	/* User button configuration */  
	// STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	/* Configure ADC1 Channel 11 */
	ADC1_Config();
#if 0//LJW,20160904�����ˣ������Ѿ�����
	GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);
	GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN);
	GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);  	
#endif
    
   //ljw,20160912 ����Ҫɾ
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
//        else if(test_LIS3DH_timer >=2) //ljw,20160913,������ ����Ҫɾ)
//        {
//            test_LIS3DH_timer = 0;
//            LMS330DLC_GetWHO_AM_I((u8 *)&who_am);
//            LMS330DLC_ReadReg(LMS330DLC_CTRL_REG4,&ctrl4);
//            LMS330DLC_ReadReg( LMS330DLC_CTRL_REG1,&tmpReg);
//            
//            get_Pitch();
//        }
//    }
//ljw,20160912 ����Ҫɾ 
    
    
    
    
    
    
	delayms(2000);
	GetAvailableGPSInfoV();
	i=0;
	if(dingweied_flag == 0) // �Ѿ���λ�Ͳ���AGPS
	{
		do{
			StartGPRStoAGPS();	
			if(dial_status==1)//wei,20160621 if Dial-up success ,break
                break;
			i++;
//wei,20160601<		if(i>3) //���ξ����� 20151124: �������ˣ���װ��ʱ����Ҫֻ����GPS��״̬
			if(i>3) //���ξ����� 20151124: �������ˣ���װ��ʱ����Ҫֻ����GPS��״̬
			{
			 //	GSM_Power(FALSE);
			 //	while(1);
				break;
			}
            if((i%2==0)||CPIN_READY_flag == 0)//wei,20160617
                GSM_reset();//wei,20160617����GSM
			if(!dial_status)//wei,20160601�������û�ɹ����Ƚ����ж�һ��GPS�Ƿ�λ�������λ�Ͳ�����ȥ�����ˡ�
			{
				GetAvailableGPSInfoV_N(2); //wei,20160601
                if (dingweied_flag != 0)
                {
                    break;
                }
				if(!Check_AT())//wei201606001
                    GSM_reset();//����GSM
			}
		}
		while(dial_status == 0);
//wei201600601<		
//        set_GPS_POWERON (TRUE);//wei,20160803,for space  ��Ҫ GPS_on();��
//wei,20160801        GPS_on();//wei,20160801 for power
//wei,20160801        delayms(500);//wei,20160801 for power
		for(i=4;i>1;i--)//ִ������
		{
			GetAvailableGPSInfoV_N(1); //wei,20160601
            if(dingweied_flag)
                break;
			if((1 ==dial_status)&&(dingweied_flag == 0))//wei,20160601AGPS���ųɹ�����GPSû��λ
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
                    GSM_reset();//wei,20160617����GSM
                StartGPRStoAGPS();//wei20160601
				//if(1<)
			}//��ԣ��źŲ������£�TCP���ӶϿ��˻���ȴ��֪��������ɵɵ�ķ�����
		}
//wei201600601>
		GetAvailableGPSInfoV();
	// �л���������IP
		// gsm ���������ó�115200
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
//        if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))//��������������Ҫ����
//        {
//            if((g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE)) //wei,20160709,����˯����������Ҫ�ڼ���������
//                break ;
//        }
        if(0x05 == check_sleep_state())//ljw,20160908�ж����������� 0x05,����  0x0a������
            break;
		StartGPRS();	
//wei kill test        dial_status = 0;////wei,20160711 for test
        if(dial_status==1)//wei,20160621 if Dial-up success ,break
            break;
//wei,20160603		i++;
		i++;		
        if(cops_flag==0x5a)//��ȡ��Ӫ�̳ɹ�
        {
            if((secondcounter-cops_flag_timer >0)&&(secondcounter-cops_flag_timer< COPS_WAIT_TIME)&&((ring_sms != SMS)))
            {//��ȡ����Ӫ�̺󣬻�δ�����ܹ���ȡ�����ŵ���Сʱ��
                if((i%2==0)||(CPIN_READY_flag == 0))//��������û�������ϵ�����GSM
                {
                    i--;
                }
            }
            else //��ȡ����ʱ�䵽�ˡ�
            {
            //wei,    cops_flag = 0x00;
                cops_flag_timer = secondcounter;
            }
            if(ring_sms == SMS)
            {
                Configuration_TEXT_SMS();//����Ӣ�Ķ��Ž���
                delayms(200);
                SMS_StartGSM_Flag = 0x5a;
                Deal_SMS();
                SMS_StartGSM_Flag = 0x00;
            }
        }
        if((i%2==0)||(CPIN_READY_flag == 0))//��������û�������ϵ�����GSM
        {
            GSM_reset();//����GSM
        }
//wei kill test        dial_status = 0;//wei,20160711 for test
        
        
      //wei,20160603		if(i>3) //���ξ����� 20151124: �������ˣ���װ��ʱ����Ҫֻ����GPS��״̬
//wei,20160603		{
//wei,20160603		 //	GSM_Power(FALSE);
		 //	while(1);
//wei,20160603			break;
//wei,20160603		}
	}
	while(dial_status == 0);

    GetAvailableGPSInfoV_N(3);
#if 0//wei,20160803 for space    
     	// ����ģ�����߹���
	ddi_uart_Write(&UART1_ctl_t,"AT+QSCLK=1\r\n",strlen("AT+QSCLK=1\r\n"));
	WaitGSMOK();//wei,20160803���ڵĻ����Ѿ�û�������ˣ�ֻ�г��͹��ģ����ģ�����߹��ܣ�Ҳ��û��Ҫ��     

	GetAvailableGPSInfoV();
	//�ȶ��Ŵӻ�վ������
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
	ring_sms = SMS; //�ȴ������
	g_ucAlarm = 0;
	Impact_flag = 0;
	
	//��ֹ�տ����͸�λGSM
	rst_gsm_flag = 0;
	server_ack_flag = 0X5A;
	server_noack_counter = 0;
    
    g_ucUpdateFlag  = 0x5a;//ljw,20160901,������һ��λ��
    back_g_usGPRSUpLoadAternationTime = secondcounter;//ljw,20160901,������һ��λ��
	
	while (1) 
	{
		IWDG_ReloadCounter();
//ljw,20160908���Ч��         for(;;);
//ljw,20160924        SendMsg("+8618356957828","+86deduanxing",strlen("+86deduanxing"));
//ljw,����2Сʱ��λһ��   ����Ҫɾ
//        if(secondcounter >7200)
//        {
//            while(1);
//        }
//ljw,����2Сʱ��λһ��   ����Ҫɾ
//wei20160601
//    if(test_reset_time>=600)
//		{
//				GSM_Power(FALSE);
//				while(1);
//		}
//wei20160601		
		// ִ��GPRS�������� 
		if(g_ucGPRSCmdMissionFlag == 1)
		{
//ljw,20160906			g_ucGPRSCmdMissionFlag = 0;
			if(system_para.g_usGPRSUpLoadAternationTime > 30)
			{
				//TickPreScale = SYS_STA_UPD_TIME; // ������ʱ
				g_ucUpdateFlag = 0;
			}
			SetGPRS();
			g_ucGPRSCmdMissionFlag = 0;//ljw,20160906������־����λ��
			server_ack_flag = 0X5A;
			//delayms(1000);
			// ��������,��ʱʱ�����¼���,��������GPRS�ĸ���
			//back_g_usHeartAternationTime = secondcounter;
			//back_g_usGPRSUpLoadAternationTime = secondcounter;			
		}
		else
		if (rst_gsm_flag == 0x5a) //����GRPS��־
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
				if((i%2==0)||(CPIN_READY_flag == 0))//��������û�������ϵ�����GSM
				{
					GSM_reset();//����GSM
				}
//wei 20160603				StartGPRS();	
			}
		 	// ����ģ�����߹���
#if 0//wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ�
			ddi_uart_Write(&UART1_ctl_t,"AT+QSCLK=1\r\n",strlen("AT+QSCLK=1\r\n"));
			WaitGSMOK();
			set_GSM_SLEEP(FALSE);
#endif //wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ�
            
			rst_gsm_flag = 0;
			server_ack_flag = 0X5A;
			server_noack_counter = 0;	
		}
		//�绰����	
		if (ring_sms == RING)
		{
			ring_sms = NO_USE;
#if 0//wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ�
			//���������У���ʱֻ���𵽻��ѵ����ã��޷�ִ��ƽʱ�Ĺ���
			if (entersleepflag == 1)//�����ڵ͹���ģʽ�����ǳ��͹���,GSMģ���ǿ����յ��绰�Ͷ��ŵģ��յ��������ﴦ��
			{
				set_GSM_SLEEP(FALSE);
				entersleepflag = 0;	
				g_FlagENERGY = 0;
				//WtmCount = system_para.g_usSleepTime*150+1; 
				for(i=0; i<10;i++)
				{
					ddi_uart_Write(&UART1_ctl_t,"ATH\r\n",5);  // �Ҷϵ绰 
				    	delayms(100);
				}
				// �������������¼��㣬��ʼ5����					   	 
			}
#endif //wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ�
#if 0 //ljw,20160924 ˵������û�У�ȥ��
			ddi_uart_Write(&UART1_ctl_t,"AT+CLCC\r\n",strlen("AT+CLCC\r\n"));
            WaitGSMOK1();//ljw   WaitGSMOK();
			pStart = (u8 *)(strstr(g_ucSim900Buff,"+CLCC:"));
			if (pStart != NULL)
            {
                ManualRx0 = 0;
                strncpy((char *)g_ucCurrentNum,pStart+18,11);
                //  ƥ����Ȩ����,���ƥ�� ���ݵ�ǰ�豸ģʽִ����Ӧ����
                for(i = 0 ; i< 2 ; i++)
                {
                    if(!strncmp((char *)g_ucCurrentNum,(char *)(system_para.g_ucRegNum[i]+4),11))  // ����Ȩ����
                    {
//ljw,20160908for space                        DebugOut("\r\nSystem is a Tracker:\r\n  Hangup The Phone.\r\n  Send Location SMS.\r\n",0);
                        ddi_uart_Write(&UART1_ctl_t,"ATH\r\n",5);
                        WaitGSMOK1();//ljw   WaitGSMOK();
                        SendLocaSMS1(g_ucCurrentNum);   // ����λ����Ϣ
                             //��ʱ���Ż�û�����Ҫ����GPRS���ݷ��ͣ����׳�ͻ���ӵ���ʱ
                        delayms(3500);	//���Ͷ��ź󣬲�������ͷ���GPRS ��ʧ��
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
#endif //ljw,20160924 ˵������û�У�ȥ��
			//delayms(1000);
		}
		if(ring_sms == SMS)
		{
			ring_sms = NO_USE;
            Deal_SMS();
#if 0
			if(system_para.g_ucSysInitialized == SYS_INITIED)	// ���ϵͳδ��ʼ�� ���������κζ�������
			{
			     for(j=0; j<5; j++)
			     {
                    if (g_ucSMSCmdMissionFlag) // ��ֹ�µĶ����ֽ���
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
					UART0_ReceiveData(g_ucSim900Buff,0);  //0	// �����жϷ�ʽ���ն�������
					if (NULL == strstr((char *)g_ucSim900Buff,"+CMGR:"))
						continue;
					// Ѱ�Ҷ���Ϣ�����ߺ���
					pStart = (u8*) strstr(g_ucSim900Buff,"\"");   		// ��һ��������
					pStart = (u8*) strstr((pStart+1),",\"");         		// �ڶ���������
					if('1'  ==  *(char*)(pStart + 2))
						pStart += 2;
					else
						pStart += 5;
					strncpy((char *)g_ucCurrentNum,(char *)(pStart),11); 	//some has +86,someone no	// ָ���ͺ���

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
						//20131105: �������ʱ��ҲҪɾ����Ϣ 
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
					//UART0_ReceiveData(smssendbuf,2);  //0	// �����жϷ�ʽ���ն�������
					//20150721: ����о�Ҳû�����յ��Ľ��
					UART0_ReceiveData(g_ucSim900Buff,0); 
					delayms(1000); 
					// �����ʱ��GPRS���ݾ͸Ͻ�����GPRS
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
			}// ����ʼ�����

			g_ucSMSCmdMissionFlag = 0;
			//��ֹ���Ŵ���ʱ�䳤����GSM��λ
			rst_gsm_flag = 0;
			server_ack_flag = 0X5A;
			server_noack_counter = 0;	
#endif
		}
		else
		if ((chinesesmsflag == 0x5a)&&((startchinesesms % 11) == 0))
		{
			cmd_INQUIRE_CHINESE_POSITION();   // ���Ķ���
//ljw,20160918 22ֻ������			if(startchinesesms >= 22)//���ξͲ�Ҫ������
            if(startchinesesms >= 33)//ljw��20160918
			{
				chinesesmsflag = 0;
				startchinesesms = 0;
				//���ͽ�����û�
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
                        {	//ֻ����ǰ��������

//ljw,20160928for wulian                            Send_speed_ALARMSMS((char *)(system_para.g_ucRegNum[i]+4));
                            Send_speed_ALARMSMS((char *)(system_para.g_ucRegNum[i]+2));
                            delayms(3500);	//���Ͷ��ź󣬲�������ͷ���GPRS ��ʧ��
                        }
                    }
                }
                g_usCurrentSpeed = 0;
            }
			      // һ��һ��
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
                    RTC_Get();//wei,20160810 ����RTCʱ��
					//GetAvailableGPSInfoV();
					//201511130���������ͳ������û�취��׼���ڶ��Ų���ʱ�кܶ����ʱ���Ƕ�ʱ�䶼û��ͳ��
					
                    if(GetAvailableGPSInfoV())     // ��ȡһ����õ�λ����Ϣ
					{
#if 0//wei 20160523 �����ͳ�Ʒŵ������ж�������
						//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
						// ����ʱ��
						ptmp = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
						ptmp++;
						//memcpy((char *)(tmp_time+6),(char *)(ptmp+1),6);	// ����ʱ��
						ptmp +=6;
						ptmp = (unsigned char *)strstr((char *)ptmp,",");


						// ����GPS��Ϣ���ñ�־
						ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
						// ���� γ�ȣ�
						//p1= (unsigned char *)strstr((char *)(i+1),",");
						ptmp++;
						//strncat((char *)smsbuf,i,p1-i);

						ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
						ptmp++;
						//strncat((char *)smsbuf,ptmp,1);
						//strncat((char *)smsbuf,"*WD",3);//JD34.36988N    ����34.36988N ��γ
						ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
						ptmp++;
						// ���� ���ȣ�
						//p1= (unsigned char *)strstr((char *)(i+1),",");
						//strncat((char *)smsbuf,i,p1-i);

						ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
						ptmp++;
						//strncat((char *)smsbuf,i,1);
						//strncat((char *)smsbuf,"*SD",3);//SD000          ��ǰ�ٶ�
						ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
						ptmp++;	
						ptmp1 = ptmp;
						ptmp= (unsigned char *)strstr((char *)(ptmp+1),".");
						//ת����1/10����
						u16tmp = asciitospeed(ptmp1,ptmp-ptmp1);
						ptmp += 1;
						// ����С����֮�������
						u16tmp += ((*ptmp-'0') *1852)/1000;
						//acc ���ܴ򿪣�״̬����ACC�źŵ�ʱ��ż������
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
					//if ((voltage >= LOW_POWER) || (voltage < power_battary))  //����9 V ������
					if ((voltage >= LOW_POWER))  //����9 V ������
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
				//��ʱ����gprs���ݻ�û��ȫ�����߻�û�����ֱ�ӷ��Ͷ�ʱ�ش������ݣ������յ��������޷�����
				if (g_ucGPRSCmdMissionFlag==0 && proGPRSCmdFlag==0)	
				{
					// ��ʱ�ش�
				  	if(g_ucUpdateFlag)
				  	{
				  		g_ucUpdateFlag = 0;
						cmd_0200();//wei,20160823,���ϱ�λ����Ϣ�����ϱ���GPS���ǻ�վ�ŵ�
                        #if 0//wei,20160823,���ϱ�λ����Ϣ�����ϱ���GPS���ǻ�վ�ŵ�
					//ljw,20160920zhu luo,�������Ӳ���    if(NOLocalCount>=120)
					    if(NOLocalCount>=10)   //wei ,���ݳ��͹��ĵĳ���ĵ�����
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
					//����
					if(g_ucHeartUpdateFlag)
				  	{
				  		g_ucHeartUpdateFlag = 0;
						cmd_0002();
						back_g_usHeartAternationTime = secondcounter;
				  	}
					//delayms(1000);
				}

//˯��ģʽ����ʱ��ģʽ��
//ģ��˯�߹���Ĭ�Ϲرգ���AT+QSCLK=1������ɴ򿪸ù��ܡ�
//���á�AT+QSCLK=1������ģ��֮�󣬿�ͨ������DTR ���ŵ�ƽ������ģ�������˳�˯��ģʽ����
//DTR �����øߣ���û���жϲ���(��GPIO �жϻ������ݴ��ݷ����ڴ���)��ģ����Զ����뵽˯��ģʽ��
//˯��ģʽ�£�ģ����Ȼ���Խ������磬�����Լ�GPRS �������ݣ����Ǵ��ڲ��ɷ��ʡ�

// ���߻���
//��ģ�鴦��˯��ģʽ�����·�ʽ���Ի���ģ�飺
//. ����DTR���ţ�
//. ���������GPRS�������ݣ�
//. �յ�����Ϣ��				
//wei				if( (system_para.g_bACCon &&(!get_ACC_CHECK_status())) || (!system_para.g_bACCon))
                if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon));  //wei,20160524,ACC�޸� ((g_ucACCFlag ==0)&&system_para.g_bACCon) 
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
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
                if(Impact_Alarm_flag ==0x5a)//ljw,20160903,���𶯱���
                {
                    Impact_Alarm_flag =0xa5;
                    cmd_0200();//wei,20160823,�����𶯱�����λ���Ϸ�
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
//wei,20160712                            if((GetAvailableGPSInfoV() )//���͵�ǰλ�õ�ǰ������Ȩ������
//                            if((GetAvailableGPSInfoV())&&(alarm_count ==0))//wei,20160712,alarm_count������֤���Ų���Ƶ������
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
                                       {	//ֻ����ǰ��������                                            
//ljw,20160928for wulian                                            SendALARMSMS1((char *)(system_para.g_ucRegNum[i]+4));
                                           SendALARMSMS1((char *)(system_para.g_ucRegNum[i]+2));
                                           delayms(3500);	//���Ͷ��ź󣬲�������ͷ���GPRS ��ʧ��
                                       }
                                    }
                                }
                                //g_ucAlarm = 0;
                            }
                        }
                        //����һ�������Ϸ�                                          
                        if(alarm_count++ <5)
                        {
                            cmd_0200();//wei,20160823,���ϱ�λ����Ϣ�����ϱ���GPS���ǻ�վ�ŵ�
                            #if 0 //wei,20160823,���ϱ�λ����Ϣ�����ϱ���GPS���ǻ�վ�ŵ�
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
                            back_g_usGPRSUpLoadAternationTime = secondcounter;//wei,20160713,λ���ϴ�����                            
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
										   {	//ֻ����ǰ��������									          	
//ljw,20160928for wulian									          	Send_lowpower_LocaSMS((char *)(system_para.g_ucRegNum[i]+4));
                                               Send_lowpower_LocaSMS((char *)(system_para.g_ucRegNum[i]+2));
                                               delayms(3500);	//���Ͷ��ź󣬲�������ͷ���GPRS ��ʧ��
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
//wei,��оƬ�쳣�����ٴγ�ʼ��<
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
            if(BitFlag.SHACK_Init_Flag == 0x03)//wei,20160603,Ҫ���³�ʼ����оƬ
            {
                BitFlag.SHACK_Init_Flag = 0x02;//wei,20160604,��ʼ��3DH��ʱ���ж����治Ҫ�������źţ��������źŲ��Ȳ����ĸ���
                LMS330DLC_init();
                set_Impact(system_para.acceleration,system_para.impact_time);
                delayms(100);
                SHACK_Init_Flag_Time = 0;
                BitFlag.SHACK_Init_Flag = 0;
            }
#endif
//wei,��оƬ�쳣�����ٴγ�ʼ��>
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
