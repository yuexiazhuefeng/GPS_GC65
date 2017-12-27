#ifndef _MD_USER_DEF_
#define _MD_USER_DEF_
#include "stm32f0xx.h"

//wei
#define AGPS_SERVERS 0x0a //��AGPS����ͨ��
#define FHT_SERVERS  0x05	//��FHT����������ͨ��

// ϵͳ״̬����ʱ����(ż��)
#define SYS_STA_UPD_TIME 		30


#define ACCFLAG                   (1<<6)
#define SHAKEFLAG                (1<<4)
#define LOWPOWERFLAG        (1<<3)
#define OILBREAKFLAG          (1<<2)
#define ALARMFLAG               (1<<1)
#define OVERSPEEDFLAG        (1<<0)

#if 0
// LED ����
#define PWRLedOff() 			P7 |= Pn5_OUTPUT_1 | Pn0_OUTPUT_1
#define PWRLedRed() 			P7 |= Pn5_OUTPUT_1 | Pn0_OUTPUT_1;P7 &= ~Pn5_OUTPUT_1
#define PWRLedGrn() 			P7 |= Pn5_OUTPUT_1 | Pn0_OUTPUT_1;P7 &= ~Pn0_OUTPUT_1
#define PWRLedYlw() 			P7 |= Pn5_OUTPUT_1 | Pn0_OUTPUT_1;P7 &= ~Pn0_OUTPUT_1 & ~Pn5_OUTPUT_1
#define GPSLedOff() 			P7 |= Pn0_OUTPUT_1
#define GPSLedOn() 				P7 &= ~Pn0_OUTPUT_1
#define ALL_LED_OFF() 			P7 = 0xFF

//20140506 ��ƽ����
//20140821 �ٷ�һ��
#define GPSOff() 				P0_bit.no0 =  0//P0 &= ~Pn0_OUTPUT_1   	// GPS�ϵ�
#define GPSOn() 				P0_bit.no0 =  1 //P0 |= Pn0_OUTPUT_1   	// GPS�ϵ�
#endif

//ϵͳ�͹���ģʽ
#define EXTRA_LOW_POWER_MODE    1
#define LOW_POWER_MODE                 2
#define ALLONLINE_MODE                 3


//�͵�ѹ�ż�
#define VOLTAGE_THRESHOLD       11500 //zzg test 


#define T_MODE 					1
#define M_MODE 					2
#define M_MODE_ONLY 		4
#define FAIL   					8

// ϵͳ��ʼ����־
#define SYS_INITIED   			'A'
#define SYS_UNINITI   			255

// ϵͳ��λģʽ
#define TRACK_MODE_SMS			255
#define TRACK_MODE_GPRS		0


// ��γ�ȱ�־λ
#define LAT 					0
#define LON 					1

/*

// GPS,GSM��ʼ״�������־
#define ALL_SUCC 			0
#define ALL_FAIL 			1
#define MONITOR_ONLY 		2

*/

// AT ִ�н��
#define AT_CMD_SUCC 			1
#define AT_CMD_FAIL 			0

// GPSR ����״̬ 
#define GRPS_IP_PORT_err        3
#define GPRS_CONNECTED_AGPS     2
#define GPRS_CONNECTED    		1
#define GPRS_DISCONNECTED 		0

// UDP �������ݽ��
#define SEND_OK					1
#define SEND_FAIL				0

// �����б�
#define TASK_MOVE_LIMIT			1
#define TASK_AREA_LIMIT			2
#define TASK_SPEED_LIMIT		4

// GPRS����
#define GPRS_MODE_WAKE		1
#define GPRS_MODE_PAKG			2
#define GPRS_MODE_PERIOD		4

 // SIM900 ���ݻ�������С
//#define SIM900_BUFF_SIZE		190//100
/*--------------------------EEPROM ��ַ����-----------------------*/

// I2C ��������ַ����
#define EEP_SLAVE_ADDR 			0xA0

// �����ŵ�ַ
#define EEP_PSW_ADDR 			80  // 6 bytes
// �����û����ŵ�ַ
#define EEP_NET_USER 			        88//6 bytes


// ϵͳģʽ��ŵ�ַ
#define EEP_DEV_MODE 			96

// ϵͳ��ʼ����־��ŵ�ַ
#define SYS_INITED_ADDR   		100	// 1  byte
// IP��ַ��ŵ�ַ
#define EEP_IP_ADDR				112 // 16 bytes
#define EEP_IP_PORT				128 // 5  bytes

#define EEP_IP_APN                       34       // 134 //g_ucNetAPN 10

// �豸ID��ŵ�ַ
#define EEP_DEV_ID				144 // 6  byte

#define EEP_DEV_IDBACK               45 //6 BYTE


//gprs �ϴ����ʱ���ַ
#define  EEP_GPRSUpLoadAternationTime 155 // 2 bytes

//�������ô�ŵ�ַ
#define EEP_MAX_SPEED                 160  // 1 BYTES

// ��������ŵ�ַ
#define EEP_NET_PSW 			            170//8 bytes


// ����˿ڷŵ�ַ
#define EEP_UDP_PORT 			  178//5 bytes


// ϵͳ��λģʽ ��ŵ�ַ
#define EEP_TRACK_MODE   		225	// 1  byte

// GPRS ���ѹ���ʱ�� ��ŵ�ַ
#define EEP_WAKE_TIME   		226	// 2  byte

// GPRS ����ģʽ ��ŵ�ַ
#define EEP_GPRS_MODE   		228	// 1  byte

//�͹���ģʽ��ŵ�ַ
#define EEP_LOWPOWER               229  // 1BYTE 

//smsģʽ��ŵ�ַ
//20120811: this bit as autosms flag
#define EEP_SMS_MODE 			230 // 

//���͵�״̬��ŵ�ַ
//��־Ϊ0XA5
#define EEP_BREAK1			231 // 


// GPRS�ϴ�����ʱ���ŵ�ַ
#define EEP_GPRS_UPTIME			233//150	// 2  bytes

//����ʱ��
#define EEP_SLEEPTIME           235  // 1BYTE

//acc mode
#define EEP_ACC_MODE 			236 // 1BYTE

//��Դģʽ��12V ������12v���ϵ�,Ĭ���ǣ������ϵġ�
#define EEP_POWER_MODE 		237// 1BYTE 




/*---------------------------------------------------------------*/


// ��Ȩ����洢״̬���
#define EMPTY 					'e'
#define FULL  					'f'

// SIM900ģ��״̬
#define BUSY 					1
#define IDLE 					0

// ����״̬
#define TASK_ATIVED  			1
#define TASK_UNATIVE 			0
#define TASK_ALL_CANCEL 		0

// ������λģʽ
#define MODE_SMS				0
#define MODE_GPRS				1

// �ǵ���ִ��������
#define CON_TRACK 				0x01
//#define AREA_LIMIT			0x02
//#define MOVE_LIMIT			0x04

// ��������
//#define CALL_A					0x0A
//#define CALL_B					0x0C
//#define SOS						0x06

// �����ʼ�� ID
#define ID_REGNUM1				'A'
#define ID_REGNUM2				'B'
#define ID_REGNUM3				'C'
#define ID_REGNUM4				'D'
#define ID_REGNUM5				'E'
#define ID_CALLANUM				'F'
#define ID_CALLBNUM				'G'
#define ID_DEVICEID				'H'
#define ID_PASSWD				'I'
#define ID_SYSMODE					'J'
#define ID_IPADDR				'K'
#define ID_IPPORT				'L'
#define ID_UPLOADTIME			'M'
#define ID_ERASE			     'N'
#define ID_FINISH				'O'
#define ID_POWERMODE		'P'
#define ID_SMSMODE		'Q'



// �����ʼ�� CMD
#define READ					'R'
#define WRITE					'W'

// ��ʼ�������ַƫ��
#define CMD 					1
#define CMD_ID					3
#define PARAMETER				5

// ��Ȩ����洢�ṹ
//  | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
//  bit00 		  	�洢״̬���
//  bit01 		 	��������
//  bit04-bit14:  	��Ȩ����
//  bit15	  		�������

struct TRACK_TASK_DETAIL
{
  unsigned char  Mode;    // ��λ��ʽ  SMS/GPRS
  unsigned short Time;    // ʱ����
  unsigned short Count;   // ����
};


// Ĭ�ϲ���
#define REGNUM1					"e   00000000000#"
#define REGNUM2					"e   00000000000#"


#define MODE				T_MODE
#define IPOWERMODE  LOW_POWER_MODE

#define FHTIP          0X5A
	

#define UPLOADTIME				30

//��ʱ����ʱ�����û�����ʱ��
#define  WtmCountUser          30*150   //30*150  //30*60 seconds

//wei,20160810 RTCʱ��ṹ��
typedef struct 
{
    u8 hour;
    u8 min;
    u8 sec;			
    //������������
    u16 w_year;
    u8  w_month;
    u8  w_date;
    u8  week;		 
}tm;
extern tm timer;
// ʱ��ṹ
struct TIME_STRUCT
{
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
};


//��ѹ�ż����綯����ADC12V��Ħ�г�12���������ADC10V
#define ADC12V                      (0xb1)
#define ADC10V                      (0x93)


/*------------------------------------------*/
/*                                          */
/*               ȫ�ֺ�������               */
/*                                          */
/*------------------------------------------*/
#define   feed_wdt()            (1;)
//ljw,20160826 void UbxConfig(void);
u8 UbxConfig(void); //ljw,20160826
void HwInit(void);
void ClrGSMBuff(void);
void GPSOpen(void);
void GPRSOpen(void);
void RestoreSystem(void);
void SysTickHandler(void);
void PositionGet(void);
void PWRLedFlash(void);
void GPSLedFlash(void);
void DelDealedSMS(void);
void UpdateSysStatus(void);
void SwInit(void);
void StartGPRS(void);
void StopGPRS(void);
void UDPSendLoca(void);
void ClrGPRSDataBuff(void);
void ClrGPRSCmdBuff(void);
void PwrLedYellow(void);
void InitialSetting(void);
void EraseDevice(void);
void CallNum(unsigned char *phonenum);
/*
**-----------------------------------------------------------------------------
**
**	����:
**		���͵�ǰʱ�䡢��γ�ȡ��ٶȡ�����ָ������.
**
**	����:
**         prochar: ǰ׺�ַ�
**         procharlen: ǰ׺�ַ����ȣ�Ϊ���ʾû��
**		num -- �ֻ�������ָ��
**
**	����:
**		��
**
**-----------------------------------------------------------------------------
*/
void SendLocaSMS(const unsigned char* prochar ,unsigned char procharlen,unsigned char *num);
void DebugOut(unsigned char *info,unsigned char len);
unsigned char SendMsg(unsigned char *num, unsigned char *msg, unsigned char len);

unsigned char GetAvailableGPSInfoV(void);
unsigned char BatteryStatusGet(void);
unsigned char GetGPRSStatus(void);
unsigned char ATCmdExeCheck(void);
unsigned char CurrentSpeedGet(void);
unsigned short GetCurrentTime(void);
unsigned char UDPSendData(unsigned char *data,unsigned char len);
unsigned char UDPSendData_Index(unsigned char *data,unsigned char len,unsigned char index); //wei

//ljw,20161126for um220 extern unsigned char GPRSCmdCount ;//wei,20160706 for clear
extern u16 GPRSCmdCount ;//ljw,20161126for um220
extern uint8_t RTC_ByteToBcd2(uint8_t Value);
extern uint8_t RTC_Bcd2ToByte(uint8_t Value);

#define GPS_FLAG 0xa5
#define GSM_FLAG 0x5a

extern u8 time_gps_rtc[6];//wei,20160809,YYMMDDHHmmSS����GPS��ʱ�䣬��ΪRTCʱ��Ļ�׼
extern u32 backsecondforRTC ;//wei,20160809, RTC��ʼ��ʱ�ĵ�
extern u8 RTC_timeavalibe_flag ;//wei,20160812 0x0a,ʱ����Ч  0x50,ʱ����Ч
//ljw,20160920 NOLocalCount>=120��ҪGPS������extern  u32 NOLocalCount;
extern u8 GPS_BaudRate_FLAG ;//ljw,20160826 x05a,GPS��������ȷ
extern u8 RTC_Get(void);

extern u16  AGPS_rec_len ;//ljw,20161125,�������յ���APGS�������ݵ��ܳ���
#define GC65_RECE 0xa5
#define AGPS_START_SEND 0x5a
#define AGPS_RE_END 0x0a
//ljw,����ü���
//#define ENSHOCK    //LJW,20161122�𶯹��ܲü���Ԥ����ú꣬��ʹ���𶯹���
//#define ZHONGKEWEI_GPS //LJW,20161125,����ú꣬ʹ���п�΢��GPSģ��
#define UM220_GPS    //LJW,20161125,����ú꣬ʹ��UM220 GPSģ��

#endif
/* End user code for definition. Do not edit comment generated here */

