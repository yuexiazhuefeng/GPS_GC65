/**
  ******************************************************************************
  * @file    SysTick/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "user_define.h"
#include "led.h"
#include "gsm.h"
#include "gps.h"
#include "gprs.h"
#include "lsm330dlc_driver.h"
#include "L3Gx.h"
#include "SmsCmd.h"
#include <string.h>



#ifdef DEBUGOUT
#undef DEBUGOUT
#endif

//wei
#define IPADD_LEN         16         // IP ��ַ
#define UDPPORT_LEN     8//wei6  	    //25��// UDP �˿�
#define DEVICEID_LEN    8//wei6       //31	// �豸ID
#define NETUSER_LEN     32//wei   6
#define NETPSW_LEN      32//wei  8
#define NETAPN_LEN       32//20      // �豸APN
#define PSW_LEN            8 //wei6        //71	// ϵͳ����

#define COPS_WAIT_TIME 50  //wei,20160709, �ӻ����Ӫ��ʱ��ʼ��ʱ�����յ����ŵ����ʱ��
extern u32 alarm_interval_timer;//wei,20160712,����������ʱ��������Ҫ����ʱ����������
extern u8 alarm_count ;//wei,20160712,���������Ĵ���
//wei20160608,<������������֮ǰ����ʱ
extern void delayms_nodog(u32 count);
//wei


// ��������ʱ�����
#define default_g_usGPRSUpLoadTime 10
// ����ʱ�����
#define default_g_usSleepTime 50
// �������
#define default_HeartAternationTime     30

#define WORKPERIODTIME    (30*60) //600//300  //miao 30*60
//#define WORKPERIODTIME  100  //miao  test

//#define ACCFLAG                    (1<<6)
//#define LOWPOWERFLAG        (1<<3)
//#define SHAKEFLAG                (1<<1)
//#define OVERSPEEDFLAG        (1<<0)
#define DOBREAK                    (0XA5) //���͵�

//wei,20160708 for AT+QISTAT #define SIM900_BUFF_SIZE   256
#define SIM900_BUFF_SIZE   512 //wei,20160708 for AT+QISTAT 
#define UART1_tbuf_len   512
#define UART2_rbuf_len   128
//#define UART2_tbuf_len   150
//ljw,20161124��չ��2048�ֽڣ���������APGS  #define GPRS_BUFF_SIZE  384 //128
#define GPRS_BUFF_SIZE  384//2800//1550//2048//384 //128
#define GPRS_BACK_BUFF_SIZE 384//201 //LJW,20161124,�������Ŀǰֻ���������в��������õ��ˡ�

//stm32f030c8 ��64k bytes, ���ڵ����ڶ��� :0xf800
//wei,20160518, �����Լ��Ķ���������ǰ��˵����ȷ��stm32f030c8,��С������λ��ҳ��ÿҳ1k.
#define system_para_ADDRESS  0X800F800
#define backup_system_para_ADDRESS  0X800FC00  //wei,160517  �ӱ�����,�ŵ����һҳ

/*wei test area***/
extern u8  ring_start_flag ;
/*wei test area***/
//���ͳ������
#define mileage_ADDRESS  0X800F000
//������ݵı�����
#define backup_mileage_ADDRESS  0X800F400

#define mileage_flag   0xa5
//��̽ṹ
typedef	struct _mileage_ctl_
{
	u8 flag;           //��Ч��־��0XA5          //char 0
	u8 low_erasure_number;  // ���������ġ��߰��� //char 1
	u16 high_erasure_number;  //���������ġ�                   //char 2 ,3     u32 0
                              //������  u32 1~255
	//u32 mileage;   //���ڴ�FLASH����0.1KMΪ��λ
	u8 location;	  // ��ǰ����ڣ������е�λ�ã� (1~255) ��һ���е�U32Ϊ��λ�Ƶ�ƫ�������������ֵ��100
	//��ǰ�����100*4+�׵�ַ 
	unsigned long long mileage_64; //�ۼ����ֵ��ÿһ�������֮�͡�����mileage*3600
}mileage_ctl_t;

extern u8 flag_mileage_save;
extern mileage_ctl_t wzw_mileage;

/******************************************************************************************************/
typedef  __packed struct
{
	// ϵͳ������ʼ��
	unsigned char g_ucSysInitialized		;		// ϵͳ��ʼ����־
	unsigned char  g_ucSysMode_S 	  	;         	// ϵͳģʽ Ӱ�ӼĴ���20151231:�ĳ��ж�ʱ��̶�IP
	unsigned char  g_ucPowerMode               ; //ϵͳ�Ĺ���ģʽ
	unsigned char  g_ucTrackMode		;	// ��λģʽ

	u8 g_usSleepTime ;                       // ����ʱ�䵥λ����  ,�Ժ�ר����һ����ʱ�����ߵĳ���
	u8 g_bAUTOSMSon ;       // ? ռ�ö����ڴ�
	u8 g_break; //���͵�״̬
	u8 REGNUMbeSET;

	u8 g_bACCon ;
	u8  acceleration; //01-79�𶯼��ٶ�
	u8  impact_time; //01-38��ʱ��
	unsigned char g_usGPRSUpLoadTime  	;		// �ϴ�����ʱ�� ��λ ����

	u32  g_usGPRSUpLoadAternationTime  ;		// �ϴ����
	u32 HeartAternationTime;                // ����ʱ����	
	u32 ALTERNATION_IN_SLEEPTime;   //0x0027,����ʱ�㱨ʱ����
	u32 ALTERNATION_IN_ALARMTime; //0x0028, ��������ʱ���
	u32 g_ucMaxSpeed;
	
	unsigned char  g_ucIPAddress[IPADD_LEN]  ;	   //WEI  16  	 // IP ��ַ
	unsigned char  g_ucNetAPN[NETAPN_LEN] ;		         //WEI  20  // �豸APN
	unsigned char  g_ucUDPPort[UDPPORT_LEN] ;	   //WEI  5	 //25��// UDP �˿�
	unsigned char  g_ucDeviceID[DEVICEID_LEN] ;	   //WEI  6       //31	// �豸ID
	unsigned char  g_ucNetUser[NETUSER_LEN] ;	   //WEI  6       // �豸ID
	unsigned char  g_ucNetPsw[NETPSW_LEN] ;		                //WEI  8      // �豸ID
	
	unsigned char  g_ucPassWord[PSW_LEN] ;  	               //WEI 6 //71	// ϵͳ����
/*	
	unsigned char  g_ucUDPPort[UDPPORT_LEN] ;	   //WEI  5	 //25��// UDP �˿�
	unsigned char  g_ucDeviceID[DEVICEID_LEN] ;	   //WEI  6       //31	// �豸ID
	unsigned char  g_ucNetUser[NETUSER_LEN] ;	   //WEI  6       // �豸ID
	unsigned char  g_ucNetPsw[NETPSW_LEN] ;		                //WEI  8      // �豸ID
	unsigned char  g_ucNetAPN[NETAPN_LEN] ;		         //WEI  20  // �豸APN
	unsigned char  g_ucPassWord[PSW_LEN] ;  	               //WEI 6 //71	// ϵͳ����
*/	

	// ��Ȩ����
//ljw,20160924	u8 g_ucRegNum[2][15];  // 162
    u8 g_ucRegNum[2][32];  //ljw,20160924, e/f,1�ֽ� len,1�ֽ�   ��������,30�ֽ�
	
}SYSTEM_PARA;

extern SYSTEM_PARA system_para;

//AGPS ��ͷ
extern u32  fB5 ;
extern u32 fB5s; 
extern u16 agps_len_to_send;
//AGPS����ʱ
extern u32 AGPS_ov;

/******************************************************************************************************/

//extern unsigned char g_ucSysMode;  						// 0: SMS   1: SMS & GPRS
//extern unsigned char g_ucSMSMode;                                          // 0:�ر��Զ����Żظ���1�����Զ����Żظ�
                                                                                       //��ͨ����������

extern unsigned char g_ucSim900Status			;			// SIM900ģ��״̬ 
//ljw,20160924 for wulian extern unsigned char g_ucCurrentNum[11]		;	// ��ǰ�����豸�ĺ���
extern unsigned char g_ucCurrentNum[32]		;	// ��ǰ�����豸�ĺ���//ljw,20160924 for wulian
extern unsigned char g_ucUART6RXUsed			;			// ����6��ǰ״̬
extern unsigned char g_ucTaskList			;			// �����
extern unsigned char g_ucCurrentSMSNum[2]	        ;		// ��ǰ���Ŵ洢���
extern unsigned char volatile g_ucInitSettingFlag	;			// ��ʼ�����ñ�־λ
extern volatile unsigned char g_ucChargingFlag 	;			// ���״̬��־λ
extern volatile unsigned char g_ucUpdateFlag 		;			// ϵͳ״̬���±�־
extern volatile unsigned char g_ucLowPowerFlag		;			// ������
extern volatile unsigned char g_ucFStatus		;			//����״̬
extern volatile unsigned char g_ucACCFlag ;            //wei,20160524 ACC״̬��־0:ACCû��   0x5a:ACCΪ�ϵ�
extern u16 voltage;
extern u8 shakeon;
/******************************************************************************************************/

// GPRS ������ʽ��־λ
//extern volatile unsigned char g_ucGPRSMode 			; //GPRS_MODE_PERIOD;	// ��ʱ��ģʽ��־λ
extern volatile unsigned char g_ucWakeModeUpLdFlag 		;	// 0---����Ҫ�ϴ� 1---��Ҫ�ϴ�

extern u8 dial_status;
extern u8 net_status ;
/******************************************************************************************************/

/******************************************************************************************************/

// ��ǰ�ٶ�
extern unsigned short g_usCurrentSpeed;

// ���ѹ���ʱ��
extern u8 entersleepflag;

//��ʱ������Ҫ����SLEEP
extern u8 g_ucGotoSleepFlag;
/******************************************************************************************************/

// ��γ������
extern double g_dCurrentLat , g_dCurrentLon ;  	// ��ǰγ�� ����
extern double g_dMaxLat     , g_dMaxLon     ;  	// ���γ�� ����
extern double g_dMinLat     , g_dMinLon    ;  	// ��Сγ�� ����
extern double g_dMetersPLonDegree ; 				// ��ǰ������ ����ÿ�ı�һ�ȵر�仯�ľ���
extern const double g_dMetersPLatDegree;

/******************************************************************************************************/

// GPRS ����ִ�������־λ
extern unsigned char volatile g_ucGPRSCmdMissionFlag ;  // 1 -- ��δִ������ 0 -- ��δִ������

// SMS ����ִ�������־λ
extern unsigned char volatile g_ucSMSCmdMissionFlag ;  	// 1 -- ��δִ������ 0 -- ��δִ������

//�񶯻�ACC ��������
extern unsigned char volatile g_ucACC;

// �ٶ����Ʊ�����־
extern unsigned char volatile g_ucSpeedLimitAlarm;

//��������״̬
extern unsigned char volatile g_ucAlarm ;

//����ģʽ
//unsigned char volatile g_ucENERGY;
//����͹���ģʽ�ı�־
extern unsigned char volatile g_FlagENERGY ;

//��ʱ����ʱ����
extern unsigned  int  WtmCount;


//ACC ״̬�仯����ʱ��
extern unsigned short int volatile ACCCount;
//unsigned char  volatile ACCflag=0;

//�񶯼���źű仯�ĳ���ʱ��
extern unsigned short int volatile ShakerCount;
//�����ʼ���ı�־
extern volatile unsigned char  EN_initialize;
//�͵�������ʱ��
extern unsigned short volatile LOWPOWERCount;

//�ⲿ�жϴ���ϵͳ����
extern unsigned char intflag;

extern volatile unsigned char g_ucHeartUpdateFlag;

extern volatile unsigned char  bit_flag;//wei.20160530,λ0,1==1,��������AGPS
//��ǰSIM����Ӫ�ˡ������ƶ���������ͨ
extern u32 MobileCompany;
// ƽʱ���㡡
#define NO_USE     0
#define RING          1
#define SMS           2
// �绰����� �ж�
extern u8  ring_sms;
extern u32 delayms_counter;

extern u32 back_g_usGPRSUpLoadAternationTime;
extern u32 back_g_usGPRSUpLoadTime;
extern u16 RTC_ALARM_timer;
extern u32 back_g_usHeartAternationTime;
extern u32 secondcounter;


//rtc�����ߵĻ��Ѵ���
extern u16 rtc_wakeup_timer;

 //�͵�ѹ������
extern u32 lowbattcount;

//����ײ��־
extern u8 Impact_flag;
extern u8 Impact_Alarm_flag;////ljw,20160903,�𶯹��˺�ı�����־
//���Ķ���
extern u8 chinesesmsflag;
extern u8 startchinesesms ;
extern unsigned char dingwei_flag; //��ǰ��λ��־
extern  unsigned char dingweied_flag;
/******************************************************************************************************/

// ��Ȩ���� EEPROM ��ŵ�ַ
extern const unsigned char g_ucRegNumAddr[5] ;


extern unsigned char g_ucSim900Buff[SIM900_BUFF_SIZE];	// GSM  ���������
extern unsigned char g_ucNeo6QBuff[85];//[75];		// GPS  ���������
extern unsigned char g_ucGPRSDataBuff[150];	// GPRS ���ݻ�����
//extern unsigned char g_ucGPRSDataBuff_hex[170];	// GPRS ���ݻ�����
extern unsigned char g_ucGPRSCmdBuff[GPRS_BUFF_SIZE];	// GPRS �������



extern  volatile unsigned char ManualRx0;
//ljw,20160906for space extern unsigned char volatile GPRSCmdFlag;


/* Private typedef -----------------------------------------------------------*/
#define Parity_No   0
#define Parity_Odd  1
#define Parity_Even 2

#define LOW_POWER      10500//wei,20160823,�ĳ�10.5V  11000
#define POWEROFF       9000
#define power_battary  6500

#define GPSOff() 				GPIO_ResetBits(GPS_POWERON_PORT,GPS_POWERON_PIN)
#define GPSOn() 				GPIO_SetBits(GPS_POWERON_PORT,GPS_POWERON_PIN)
// LED ����

#define GPSLedOn() 			GPIO_ResetBits(LED2_GPIO_PORT,LED2_PIN)
#define GPSLedOff() 			GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN)

#define GSMLedOn() 			GPIO_ResetBits(LED3_GPIO_PORT,LED3_PIN)
#define GSMLedOff() 			GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN)

#define SHAKELedOn() 			GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN)
#define SHAKELedOff() 			GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN)






typedef	struct _uart_ctl_
{
	void* reg;  
	void *pRbuf,*pTbuf;
	u8 Parity;
	u8 DataBit;
	u8 StopBit;

}uart_ctl_t;



//�����ȴӸߵ���
extern const u8 acceleration_para[3];

extern const u8 impact_time_para[3];


extern uart_ctl_t UART1_ctl_t;
extern uart_ctl_t UART2_ctl_t;

extern u8 g_ucSim900Buff[SIM900_BUFF_SIZE];
extern u8 UART1_tbuf[UART1_tbuf_len];
extern u8 UART2_rbuf[UART2_rbuf_len];


//wei 20160708 for AT+QISTAT extern u8 gUart0RxCnt;
extern u16 gUart0RxCnt;//wei 20160708 for AT+QISTAT
extern u8 gps_reclen;
extern unsigned char backup_g_ucNeo6QBuff[85]; //[75]; // ���һ����Ч��λ
extern u8 gps_gate;
extern u8 gps_copy;
extern u8 send_alarm_flag;
//ȷ���ǵ͵�ѹҪ������
extern u8 lowbatt_flag;
//ACC ���������Ӻ�����ϱ���
extern u8 shake_gate;
extern u8 flag_start_duage_g_ucAlarm;
// ��������������һ��
extern u8 alarm_in_flag;

//��������Ӧ��,һֱû�оͻ�����GSM
extern u8 server_ack_flag;
// û��Ӧ��һ���һ��,�ӵ�120�ξ�����ģ��
extern u16 server_noack_counter;
//����GSMģ���־
extern u8 rst_gsm_flag;

//�����ϴ����������ǣ���ʾ��Ҫ������
extern u16 T_P_g_usGPRSUpLoadAternationTime;

//����ʱ��
extern u32 T_P_time;
//��̼�������жϷ������У�����������׼�����������жϴ���ʱ�䣬��ϵͳ����
extern u16 mileage_tmp;
/**********************************************************************
//??:???????
//??1:????
//??2:?????
//??3:?????
//??:???????
**********************************************************************/
s16 ddi_uart_Read(uart_ctl_t *pCOM,u8 *pDat,s16 len);
/**********************************************************************
//??:???????
//??1:????
//??2:?????
//??3:?????
//??:???????
**********************************************************************/
s16 ddi_uart_Write(uart_ctl_t *pCOM,const u8 *pDat,s16 size);
/**********************************************************************
//??:	?????????
			??:	????????,
					???????????
//??1:????
//??:?
**********************************************************************/
void ddi_uart_flush(uart_ctl_t *pCOM);
/**********************************************************************
//??:??COM ???????
//??1:????????
//??2:???
//??3:?????
//??:????????,????NULL
**********************************************************************/
uart_ctl_t* ddi_uart_open(uart_ctl_t *pCOM,u32 BaudRate);
/**********************************************************************
//??:??????
//??:?
//??:?
**********************************************************************/
void ddi_uart_init(void);
//wei,20160530
//���������� ENABLE,�򿪴���, DISABLE,�رմ���
extern void ddi_uart1_init(FunctionalState New_state);
extern void ddi_uart2_init(FunctionalState New_state);

//wei,20160803 for space void GPS_on(void);

void UpLoadMID(void);


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */



 extern void hextoascii(unsigned char * srcdata,unsigned char * decdata,unsigned short int len);

 extern void asciitohex(unsigned char * src,unsigned char * dec,unsigned short int len);

//wei,20160622,�Ż�ʱ�½��ĺ���<
extern  u8 Store_System_Para(void); //
extern  u8 Backup_Store_System_Para(void);//wei , 160517,�ӱ�����
extern  u8 Set_Store_System_Para(void); //wei,160517,�����洢������
extern  u8 FLASH_ErasePage_Complete(u32 addr);//wei,20160517,����һҳ���ɹ�����0��ʧ�ܷ���0xff
extern void Deal_SMS(void);
extern u8 check_sleep_state(void);//ljw,20160908�ж����������� 0x05,����  0x0a������
//wei,20160622,�Ż�ʱ�½��ĺ���>

 //��ʼ�����˵ģ��������ݷŵ����ṹ���У�û�г�ʼ���ķ���Ĭ��ֵ
extern void sys_Init(void);

extern void IWDG_Init(u8 prer,u16 rlr) ; 
// �������Ķ���
extern void send_chinese_sms(unsigned char *num, unsigned char *msg, unsigned char len);
//������ѯ�Ӻ���
extern void sub_cxcs(u8* sms_in);

// �������
extern u8 mileage_flash_init(void);
//wei20160603<
typedef  __packed struct //wei20160603,λ������
{
	u8  SHACK_Init_Flag : 2;

}BITFlag;

extern BITFlag BitFlag;
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
extern u8 SHACK_Init_Flag_Time ;
#endif
extern unsigned char volatile proGPRSCmdFlag ;

//extern u8 test_LIS3DH_timer; //ljw,20160913,������ ����Ҫɾ
//wei20160603>
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
