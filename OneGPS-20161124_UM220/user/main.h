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
#define IPADD_LEN         16         // IP 地址
#define UDPPORT_LEN     8//wei6  	    //25　// UDP 端口
#define DEVICEID_LEN    8//wei6       //31	// 设备ID
#define NETUSER_LEN     32//wei   6
#define NETPSW_LEN      32//wei  8
#define NETAPN_LEN       32//20      // 设备APN
#define PSW_LEN            8 //wei6        //71	// 系统密码

#define COPS_WAIT_TIME 50  //wei,20160709, 从获得运营商时开始计时，到收到短信的最短时间
extern u32 alarm_interval_timer;//wei,20160712,紧急报警的时间间隔，需要与总时间轴做减法
extern u8 alarm_count ;//wei,20160712,紧急报警的次数
//wei20160608,<用来看门重启之前的延时
extern void delayms_nodog(u32 count);
//wei


// 持续工作时间分钟
#define default_g_usGPRSUpLoadTime 10
// 休眠时间分钟
#define default_g_usSleepTime 50
// 心跳间隔
#define default_HeartAternationTime     30

#define WORKPERIODTIME    (30*60) //600//300  //miao 30*60
//#define WORKPERIODTIME  100  //miao  test

//#define ACCFLAG                    (1<<6)
//#define LOWPOWERFLAG        (1<<3)
//#define SHAKEFLAG                (1<<1)
//#define OVERSPEEDFLAG        (1<<0)
#define DOBREAK                    (0XA5) //断油电

//wei,20160708 for AT+QISTAT #define SIM900_BUFF_SIZE   256
#define SIM900_BUFF_SIZE   512 //wei,20160708 for AT+QISTAT 
#define UART1_tbuf_len   512
#define UART2_rbuf_len   128
//#define UART2_tbuf_len   150
//ljw,20161124扩展到2048字节，用作接收APGS  #define GPRS_BUFF_SIZE  384 //128
#define GPRS_BUFF_SIZE  384//2800//1550//2048//384 //128
#define GPRS_BACK_BUFF_SIZE 384//201 //LJW,20161124,这个数组目前只是用来进行参数设置用的了。

//stm32f030c8 　64k bytes, 放在倒数第二块 :0xf800
//wei,20160518, 经测试及阅读擦出函数前的说明，确认stm32f030c8,最小擦除单位是页，每页1k.
#define system_para_ADDRESS  0X800F800
#define backup_system_para_ADDRESS  0X800FC00  //wei,160517  加备份区,放到最后一页

/*wei test area***/
extern u8  ring_start_flag ;
/*wei test area***/
//里程统计数据
#define mileage_ADDRESS  0X800F000
//里程数据的备份区
#define backup_mileage_ADDRESS  0X800F400

#define mileage_flag   0xa5
//里程结构
typedef	struct _mileage_ctl_
{
	u8 flag;           //有效标志　0XA5          //char 0
	u8 low_erasure_number;  // 擦除次数的　高半字 //char 1
	u16 high_erasure_number;  //擦除次数的　                   //char 2 ,3     u32 0
                              //数据区  u32 1~255
	//u32 mileage;   //用于存FLASH的以0.1KM为单位
	u8 location;	  // 当前里程在ｆｌａｓｈ中的位置， (1~255) 在一块中的U32为单位制的偏移量，比如这个值是100
	//当前里程在100*4+首地址 
	unsigned long long mileage_64; //累计里程值，每一秒的数据之和　　　mileage*3600
}mileage_ctl_t;

extern u8 flag_mileage_save;
extern mileage_ctl_t wzw_mileage;

/******************************************************************************************************/
typedef  __packed struct
{
	// 系统参数初始化
	unsigned char g_ucSysInitialized		;		// 系统初始化标志
	unsigned char  g_ucSysMode_S 	  	;         	// 系统模式 影子寄存器20151231:改成判断时候固定IP
	unsigned char  g_ucPowerMode               ; //系统的功耗模式
	unsigned char  g_ucTrackMode		;	// 定位模式

	u8 g_usSleepTime ;                       // 休眠时间单位分钟  ,以后专门做一个长时间休眠的程序，
	u8 g_bAUTOSMSon ;       // ? 占用多少内存
	u8 g_break; //断油电状态
	u8 REGNUMbeSET;

	u8 g_bACCon ;
	u8  acceleration; //01-79震动加速度
	u8  impact_time; //01-38震动时间
	unsigned char g_usGPRSUpLoadTime  	;		// 上传持续时间 单位 分钟

	u32  g_usGPRSUpLoadAternationTime  ;		// 上传间隔
	u32 HeartAternationTime;                // 心跳时间间隔	
	u32 ALTERNATION_IN_SLEEPTime;   //0x0027,休眠时汇报时间间隔
	u32 ALTERNATION_IN_ALARMTime; //0x0028, 紧急报警时间隔
	u32 g_ucMaxSpeed;
	
	unsigned char  g_ucIPAddress[IPADD_LEN]  ;	   //WEI  16  	 // IP 地址
	unsigned char  g_ucNetAPN[NETAPN_LEN] ;		         //WEI  20  // 设备APN
	unsigned char  g_ucUDPPort[UDPPORT_LEN] ;	   //WEI  5	 //25　// UDP 端口
	unsigned char  g_ucDeviceID[DEVICEID_LEN] ;	   //WEI  6       //31	// 设备ID
	unsigned char  g_ucNetUser[NETUSER_LEN] ;	   //WEI  6       // 设备ID
	unsigned char  g_ucNetPsw[NETPSW_LEN] ;		                //WEI  8      // 设备ID
	
	unsigned char  g_ucPassWord[PSW_LEN] ;  	               //WEI 6 //71	// 系统密码
/*	
	unsigned char  g_ucUDPPort[UDPPORT_LEN] ;	   //WEI  5	 //25　// UDP 端口
	unsigned char  g_ucDeviceID[DEVICEID_LEN] ;	   //WEI  6       //31	// 设备ID
	unsigned char  g_ucNetUser[NETUSER_LEN] ;	   //WEI  6       // 设备ID
	unsigned char  g_ucNetPsw[NETPSW_LEN] ;		                //WEI  8      // 设备ID
	unsigned char  g_ucNetAPN[NETAPN_LEN] ;		         //WEI  20  // 设备APN
	unsigned char  g_ucPassWord[PSW_LEN] ;  	               //WEI 6 //71	// 系统密码
*/	

	// 授权号码
//ljw,20160924	u8 g_ucRegNum[2][15];  // 162
    u8 g_ucRegNum[2][32];  //ljw,20160924, e/f,1字节 len,1字节   号码内容,30字节
	
}SYSTEM_PARA;

extern SYSTEM_PARA system_para;

//AGPS 桢头
extern u32  fB5 ;
extern u32 fB5s; 
extern u16 agps_len_to_send;
//AGPS　超时
extern u32 AGPS_ov;

/******************************************************************************************************/

//extern unsigned char g_ucSysMode;  						// 0: SMS   1: SMS & GPRS
//extern unsigned char g_ucSMSMode;                                          // 0:关闭自动短信回复；1，打开自动短信回复
                                                                                       //再通过机器发回

extern unsigned char g_ucSim900Status			;			// SIM900模块状态 
//ljw,20160924 for wulian extern unsigned char g_ucCurrentNum[11]		;	// 当前操作设备的号码
extern unsigned char g_ucCurrentNum[32]		;	// 当前操作设备的号码//ljw,20160924 for wulian
extern unsigned char g_ucUART6RXUsed			;			// 串口6当前状态
extern unsigned char g_ucTaskList			;			// 任务表
extern unsigned char g_ucCurrentSMSNum[2]	        ;		// 当前短信存储序号
extern unsigned char volatile g_ucInitSettingFlag	;			// 初始化设置标志位
extern volatile unsigned char g_ucChargingFlag 	;			// 充电状态标志位
extern volatile unsigned char g_ucUpdateFlag 		;			// 系统状态更新标志
extern volatile unsigned char g_ucLowPowerFlag		;			// 电量低
extern volatile unsigned char g_ucFStatus		;			//各种状态
extern volatile unsigned char g_ucACCFlag ;            //wei,20160524 ACC状态标志0:ACC没有   0x5a:ACC为上电
extern u16 voltage;
extern u8 shakeon;
/******************************************************************************************************/

// GPRS 工作方式标志位
//extern volatile unsigned char g_ucGPRSMode 			; //GPRS_MODE_PERIOD;	// 分时段模式标志位
extern volatile unsigned char g_ucWakeModeUpLdFlag 		;	// 0---不需要上传 1---需要上传

extern u8 dial_status;
extern u8 net_status ;
/******************************************************************************************************/

/******************************************************************************************************/

// 当前速度
extern unsigned short g_usCurrentSpeed;

// 叫醒工作时间
extern u8 entersleepflag;

//定时器提醒要进入SLEEP
extern u8 g_ucGotoSleepFlag;
/******************************************************************************************************/

// 经纬度数据
extern double g_dCurrentLat , g_dCurrentLon ;  	// 当前纬度 经度
extern double g_dMaxLat     , g_dMaxLon     ;  	// 最大纬度 经度
extern double g_dMinLat     , g_dMinLon    ;  	// 最小纬度 经度
extern double g_dMetersPLonDegree ; 				// 当前经度下 经度每改变一度地表变化的距离
extern const double g_dMetersPLatDegree;

/******************************************************************************************************/

// GPRS 命令执行任务标志位
extern unsigned char volatile g_ucGPRSCmdMissionFlag ;  // 1 -- 有未执行命令 0 -- 无未执行命令

// SMS 命令执行任务标志位
extern unsigned char volatile g_ucSMSCmdMissionFlag ;  	// 1 -- 有未执行命令 0 -- 无未执行命令

//振动或ACC 触发报警
extern unsigned char volatile g_ucACC;

// 速度限制报警标志
extern unsigned char volatile g_ucSpeedLimitAlarm;

//紧急报警状态
extern unsigned char volatile g_ucAlarm ;

//功耗模式
//unsigned char volatile g_ucENERGY;
//进入低功耗模式的标志
extern unsigned char volatile g_FlagENERGY ;

//定时唤醒时间间隔
extern unsigned  int  WtmCount;


//ACC 状态变化持续时间
extern unsigned short int volatile ACCCount;
//unsigned char  volatile ACCflag=0;

//振动检测信号变化的持续时间
extern unsigned short int volatile ShakerCount;
//允许初始化的标志
extern volatile unsigned char  EN_initialize;
//低电量持续时间
extern unsigned short volatile LOWPOWERCount;

//外部中断触发系统唤醒
extern unsigned char intflag;

extern volatile unsigned char g_ucHeartUpdateFlag;

extern volatile unsigned char  bit_flag;//wei.20160530,位0,1==1,则在请求AGPS
//当前SIM的运营伤　１是移动，二是联通
extern u32 MobileCompany;
// 平时是零　
#define NO_USE     0
#define RING          1
#define SMS           2
// 电话或短信 中断
extern u8  ring_sms;
extern u32 delayms_counter;

extern u32 back_g_usGPRSUpLoadAternationTime;
extern u32 back_g_usGPRSUpLoadTime;
extern u16 RTC_ALARM_timer;
extern u32 back_g_usHeartAternationTime;
extern u32 secondcounter;


//rtc在休眠的唤醒次数
extern u16 rtc_wakeup_timer;

 //低电压计数器
extern u32 lowbattcount;

//振动碰撞标志
extern u8 Impact_flag;
extern u8 Impact_Alarm_flag;////ljw,20160903,震动过滤后的报警标志
//中文短信
extern u8 chinesesmsflag;
extern u8 startchinesesms ;
extern unsigned char dingwei_flag; //当前定位标志
extern  unsigned char dingweied_flag;
/******************************************************************************************************/

// 授权号码 EEPROM 存放地址
extern const unsigned char g_ucRegNumAddr[5] ;


extern unsigned char g_ucSim900Buff[SIM900_BUFF_SIZE];	// GSM  输出缓冲区
extern unsigned char g_ucNeo6QBuff[85];//[75];		// GPS  输出缓冲区
extern unsigned char g_ucGPRSDataBuff[150];	// GPRS 数据缓冲区
//extern unsigned char g_ucGPRSDataBuff_hex[170];	// GPRS 数据缓冲区
extern unsigned char g_ucGPRSCmdBuff[GPRS_BUFF_SIZE];	// GPRS 命令缓冲区



extern  volatile unsigned char ManualRx0;
//ljw,20160906for space extern unsigned char volatile GPRSCmdFlag;


/* Private typedef -----------------------------------------------------------*/
#define Parity_No   0
#define Parity_Odd  1
#define Parity_Even 2

#define LOW_POWER      10500//wei,20160823,改成10.5V  11000
#define POWEROFF       9000
#define power_battary  6500

#define GPSOff() 				GPIO_ResetBits(GPS_POWERON_PORT,GPS_POWERON_PIN)
#define GPSOn() 				GPIO_SetBits(GPS_POWERON_PORT,GPS_POWERON_PIN)
// LED 操作

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



//灵敏度从高到低
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
extern unsigned char backup_g_ucNeo6QBuff[85]; //[75]; // 最后一次有效定位
extern u8 gps_gate;
extern u8 gps_copy;
extern u8 send_alarm_flag;
//确认是低电压要报警了
extern u8 lowbatt_flag;
//ACC 关了两分钟后才能上报振动
extern u8 shake_gate;
extern u8 flag_start_duage_g_ucAlarm;
// 紧急抱紧按下了一秒
extern u8 alarm_in_flag;

//服务器有应答,一直没有就会重启GSM
extern u8 server_ack_flag;
// 没响应就一秒加一次,加到120次就重启模块
extern u16 server_noack_counter;
//重启GSM模块标志
extern u8 rst_gsm_flag;

//跟踪上传间隔，如果是０表示不要跟踪了
extern u16 T_P_g_usGPRSUpLoadAternationTime;

//跟踪时间
extern u32 T_P_time;
//里程计算放在中断服务函数中，这样按理会更准，但增加了中断处理时间，对系统不利
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
//参数数明， ENABLE,打开串口, DISABLE,关闭串口
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

//wei,20160622,优化时新建的函数<
extern  u8 Store_System_Para(void); //
extern  u8 Backup_Store_System_Para(void);//wei , 160517,加备份区
extern  u8 Set_Store_System_Para(void); //wei,160517,正常存储的区域
extern  u8 FLASH_ErasePage_Complete(u32 addr);//wei,20160517,擦除一页，成功返回0，失败返回0xff
extern void Deal_SMS(void);
extern u8 check_sleep_state(void);//ljw,20160908判断休眠条返回 0x05,休眠  0x0a不休眠
//wei,20160622,优化时新建的函数>

 //初始化过了的，读出数据放到，结构体中，没有初始化的放入默认值
extern void sys_Init(void);

extern void IWDG_Init(u8 prer,u16 rlr) ; 
// 发送中文短信
extern void send_chinese_sms(unsigned char *num, unsigned char *msg, unsigned char len);
//参数查询子函数
extern void sub_cxcs(u8* sms_in);

// 里程清零
extern u8 mileage_flash_init(void);
//wei20160603<
typedef  __packed struct //wei20160603,位定义区
{
	u8  SHACK_Init_Flag : 2;

}BITFlag;

extern BITFlag BitFlag;
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
extern u8 SHACK_Init_Flag_Time ;
#endif
extern unsigned char volatile proGPRSCmdFlag ;

//extern u8 test_LIS3DH_timer; //ljw,20160913,测试震动 测完要删
//wei20160603>
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
