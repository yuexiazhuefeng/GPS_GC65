#ifndef _MD_USER_DEF_
#define _MD_USER_DEF_
#include "stm32f0xx.h"

//wei
#define AGPS_SERVERS 0x0a //往AGPS数据通道
#define FHT_SERVERS  0x05	//往FHT服务器数据通道

// 系统状态更新时间间隔(偶数)
#define SYS_STA_UPD_TIME 		30


#define ACCFLAG                   (1<<6)
#define SHAKEFLAG                (1<<4)
#define LOWPOWERFLAG        (1<<3)
#define OILBREAKFLAG          (1<<2)
#define ALARMFLAG               (1<<1)
#define OVERSPEEDFLAG        (1<<0)

#if 0
// LED 操作
#define PWRLedOff() 			P7 |= Pn5_OUTPUT_1 | Pn0_OUTPUT_1
#define PWRLedRed() 			P7 |= Pn5_OUTPUT_1 | Pn0_OUTPUT_1;P7 &= ~Pn5_OUTPUT_1
#define PWRLedGrn() 			P7 |= Pn5_OUTPUT_1 | Pn0_OUTPUT_1;P7 &= ~Pn0_OUTPUT_1
#define PWRLedYlw() 			P7 |= Pn5_OUTPUT_1 | Pn0_OUTPUT_1;P7 &= ~Pn0_OUTPUT_1 & ~Pn5_OUTPUT_1
#define GPSLedOff() 			P7 |= Pn0_OUTPUT_1
#define GPSLedOn() 				P7 &= ~Pn0_OUTPUT_1
#define ALL_LED_OFF() 			P7 = 0xFF

//20140506 电平反了
//20140821 再反一次
#define GPSOff() 				P0_bit.no0 =  0//P0 &= ~Pn0_OUTPUT_1   	// GPS上电
#define GPSOn() 				P0_bit.no0 =  1 //P0 |= Pn0_OUTPUT_1   	// GPS上电
#endif

//系统低功耗模式
#define EXTRA_LOW_POWER_MODE    1
#define LOW_POWER_MODE                 2
#define ALLONLINE_MODE                 3


//低电压门槛
#define VOLTAGE_THRESHOLD       11500 //zzg test 


#define T_MODE 					1
#define M_MODE 					2
#define M_MODE_ONLY 		4
#define FAIL   					8

// 系统初始化标志
#define SYS_INITIED   			'A'
#define SYS_UNINITI   			255

// 系统定位模式
#define TRACK_MODE_SMS			255
#define TRACK_MODE_GPRS		0


// 经纬度标志位
#define LAT 					0
#define LON 					1

/*

// GPS,GSM初始状化结果标志
#define ALL_SUCC 			0
#define ALL_FAIL 			1
#define MONITOR_ONLY 		2

*/

// AT 执行结果
#define AT_CMD_SUCC 			1
#define AT_CMD_FAIL 			0

// GPSR 连接状态 
#define GRPS_IP_PORT_err        3
#define GPRS_CONNECTED_AGPS     2
#define GPRS_CONNECTED    		1
#define GPRS_DISCONNECTED 		0

// UDP 发送数据结果
#define SEND_OK					1
#define SEND_FAIL				0

// 任务列表
#define TASK_MOVE_LIMIT			1
#define TASK_AREA_LIMIT			2
#define TASK_SPEED_LIMIT		4

// GPRS任务
#define GPRS_MODE_WAKE		1
#define GPRS_MODE_PAKG			2
#define GPRS_MODE_PERIOD		4

 // SIM900 数据缓冲区大小
//#define SIM900_BUFF_SIZE		190//100
/*--------------------------EEPROM 地址定义-----------------------*/

// I2C 从器件地址定义
#define EEP_SLAVE_ADDR 			0xA0

// 密码存放地址
#define EEP_PSW_ADDR 			80  // 6 bytes
// 网络用户名放地址
#define EEP_NET_USER 			        88//6 bytes


// 系统模式存放地址
#define EEP_DEV_MODE 			96

// 系统初始化标志存放地址
#define SYS_INITED_ADDR   		100	// 1  byte
// IP地址存放地址
#define EEP_IP_ADDR				112 // 16 bytes
#define EEP_IP_PORT				128 // 5  bytes

#define EEP_IP_APN                       34       // 134 //g_ucNetAPN 10

// 设备ID存放地址
#define EEP_DEV_ID				144 // 6  byte

#define EEP_DEV_IDBACK               45 //6 BYTE


//gprs 上传间隔时间地址
#define  EEP_GPRSUpLoadAternationTime 155 // 2 bytes

//超速设置存放地址
#define EEP_MAX_SPEED                 160  // 1 BYTES

// 网络密码放地址
#define EEP_NET_PSW 			            170//8 bytes


// 网络端口放地址
#define EEP_UDP_PORT 			  178//5 bytes


// 系统定位模式 存放地址
#define EEP_TRACK_MODE   		225	// 1  byte

// GPRS 叫醒工作时间 存放地址
#define EEP_WAKE_TIME   		226	// 2  byte

// GPRS 工作模式 存放地址
#define EEP_GPRS_MODE   		228	// 1  byte

//低功耗模式存放地址
#define EEP_LOWPOWER               229  // 1BYTE 

//sms模式存放地址
//20120811: this bit as autosms flag
#define EEP_SMS_MODE 			230 // 

//断油电状态存放地址
//标志为0XA5
#define EEP_BREAK1			231 // 


// GPRS上传持续时间存放地址
#define EEP_GPRS_UPTIME			233//150	// 2  bytes

//休眠时间
#define EEP_SLEEPTIME           235  // 1BYTE

//acc mode
#define EEP_ACC_MODE 			236 // 1BYTE

//电源模式　12V 或者是12v以上的,默认是１２以上的　
#define EEP_POWER_MODE 		237// 1BYTE 




/*---------------------------------------------------------------*/


// 授权号码存储状态标记
#define EMPTY 					'e'
#define FULL  					'f'

// SIM900模块状态
#define BUSY 					1
#define IDLE 					0

// 任务状态
#define TASK_ATIVED  			1
#define TASK_UNATIVE 			0
#define TASK_ALL_CANCEL 		0

// 连续定位模式
#define MODE_SMS				0
#define MODE_GPRS				1

// 非单次执行任务定义
#define CON_TRACK 				0x01
//#define AREA_LIMIT			0x02
//#define MOVE_LIMIT			0x04

// 按键定义
//#define CALL_A					0x0A
//#define CALL_B					0x0C
//#define SOS						0x06

// 软件初始化 ID
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



// 软件初始化 CMD
#define READ					'R'
#define WRITE					'W'

// 初始化命令地址偏移
#define CMD 					1
#define CMD_ID					3
#define PARAMETER				5

// 授权号码存储结构
//  | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
//  bit00 		  	存储状态标记
//  bit01 		 	号码类型
//  bit04-bit14:  	授权号码
//  bit15	  		结束标记

struct TRACK_TASK_DETAIL
{
  unsigned char  Mode;    // 定位方式  SMS/GPRS
  unsigned short Time;    // 时间间隔
  unsigned short Count;   // 次数
};


// 默认参数
#define REGNUM1					"e   00000000000#"
#define REGNUM2					"e   00000000000#"


#define MODE				T_MODE
#define IPOWERMODE  LOW_POWER_MODE

#define FHTIP          0X5A
	

#define UPLOADTIME				30

//定时唤醒时间间隔用户设置时间
#define  WtmCountUser          30*150   //30*150  //30*60 seconds

//wei,20160810 RTC时间结构体
typedef struct 
{
    u8 hour;
    u8 min;
    u8 sec;			
    //公历日月年周
    u16 w_year;
    u8  w_month;
    u8  w_date;
    u8  week;		 
}tm;
extern tm timer;
// 时间结构
struct TIME_STRUCT
{
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
};


//低压门槛，电动车用ADC12V，摩托车12ｖ供电的用ADC10V
#define ADC12V                      (0xb1)
#define ADC10V                      (0x93)


/*------------------------------------------*/
/*                                          */
/*               全局函数声明               */
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
**	概述:
**		发送当前时间、经纬度、速度、方向到指定号码.
**
**	参数:
**         prochar: 前缀字符
**         procharlen: 前缀字符长度，为零表示没有
**		num -- 手机号码存放指针
**
**	返回:
**		无
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

extern u8 time_gps_rtc[6];//wei,20160809,YYMMDDHHmmSS保存GPS的时间，做为RTC时间的基准
extern u32 backsecondforRTC ;//wei,20160809, RTC起始计时的点
extern u8 RTC_timeavalibe_flag ;//wei,20160812 0x0a,时间无效  0x50,时间有效
//ljw,20160920 NOLocalCount>=120不要GPS过滤了extern  u32 NOLocalCount;
extern u8 GPS_BaudRate_FLAG ;//ljw,20160826 x05a,GPS波特率正确
extern u8 RTC_Get(void);

extern u16  AGPS_rec_len ;//ljw,20161125,串口中收到的APGS辅助数据的总长度
#define GC65_RECE 0xa5
#define AGPS_START_SEND 0x5a
#define AGPS_RE_END 0x0a
//ljw,程序裁剪区
//#define ENSHOCK    //LJW,20161122震动功能裁剪，预定义该宏，则使能震动功能
//#define ZHONGKEWEI_GPS //LJW,20161125,定义该宏，使能中科微的GPS模块
#define UM220_GPS    //LJW,20161125,定义该宏，使能UM220 GPS模块

#endif
/* End user code for definition. Do not edit comment generated here */

