#ifndef __gprs_h__
#define __gprs_h__
#include "stm32f0xx.h"
#include "stm32f0xx_rtc.h"
//打算部标协议只有GPRS通讯,sms的还是按照原来的协议来
//老的协议只有: 
//   1.主动上传数据                               
//   2.修改主动连传时间间隔
//   3.回答主动连传时间间隔
//   4.单次定位
//   5.设置超速值
//   6.取消超速值
//   7.查询超速设置
//   8.修改IP地址
//   9.查询当前IP地址
//   10.设置断油电
//   11.查询断油电状态

//部标协议有的:

// 1. 设置终端参数 消息ID 0X8103 						参数ID 0x0001,中断心跳发送间隔
//																							0x0010, apn
//																							0x0011, gprs拨号用户名
//																							0x0012,gprs拨号密码
//																							0x0013,主服务器IP地址
//																							0x0019,UDP端口
//																							0x0020.设置为定时汇报;
//																							0x0027,休眠时汇报时间间隔
//																							0x0028, 紧急报警时间隔
//																							0x0029,缺省时间汇报间隔
//																							0x0055,最高速度
//  2  . 查询终端参数 0x8104
//  3. 查询指定终端参数 0x8106
// 4  . 终端应答 0x0104


//5. 终端控制 0x8105 控制终端连接指定服务器
//						

//6. 位置信息汇报: 0X0200, 								报警位:  0 紧急报警
//                                               1 超速报警
//                                               7 主电源欠压
//                                               8 主电源掉电
//                                               16 振动报警
//                                      状态位: 0 ACC状态
//                                             1 定位状态
//                                               2 北纬
//                                               3 东经
//                                             10 油路状态 


//7. 位置信息查询: 0x8201
//8. 位置信息汇报: 0x0201   


//9.	车辆控制之断油路-电路 (只用电路)
//消息ID：0x8f01
//消息体4个字节，目前暂时用第一字节，后面三字节供扩展用（下发默认用0x00）。
//按位表示，当该位为1时有效
//第一字节：
//位0：断油路
//位1：恢复油路
//位2：断电
//位3：恢复电路
//第二字节：备用
//第三字节：备用
//第四字节：备用
//终端回复消息ID：0x0f01,格式同消息ID 0x0500。



//最好预留小区位置查询, 中文位置下发

//消息ID 0x0f00 消息体 要有 类型 (1字节,1 GPS ; 2 小区)是小区的话 :后面小区项目数(1字节)小区编号,小区ID;
//                                                     如果gps,只发经度和纬度,1字节的状态字节(东经,北纬)
//

//部标协议里面没有断油电的指令,只有查看状态的


//服务器收到数据后,肯定会返回.终端可以对服务器下发的数据不做应答
//服务器回复消息ID 0x8f00 后面跟着UCS2编码的中文位置

typedef enum 
{
    SET_TERMINAL_PARAMETER = 0x8103,                 //设置中断参数
    INQUIRE_TERMINAL_PARAMETER = 0x8104,          //查询中断参数
    SET_GPRS_SERVICER   = 0x8105,                        //终端控制
    INQUIRE_ONE_TERMINAL_PARAMETER = 0x8106, //查询指定终端参数
    INQUIRE_POSITION = 0x8201,                            //位置信息查询
    TEMPORARY_POSTION = 0x8202,                        //临时跟踪
    CHINESE_POSITION_rece = 0x8f88,   //20150808:    0x8f00->0x8f88
    OIL_BREAK =0x8f01
}GPRS_RECE_ID;


typedef enum 
{
    GENERAL_RESPONSION= 0x0001,
    HEARTBEAT = 0x0002,
    INQUIRE_TERMINAL_PARAMETER_RESPONSION = 0x0104,
    POSITION_REPORT = 0x0200,
    INQUIRE_POSITION_RESPONSION = 0x0201,
    OIL_BREAK_RESPONSION =0x0f01,
    CHINESE_POSITION_send = 0x0f88,      //20150808: 0x0f00->0x0f88
//ljw,20160831用0x0f90    MOBILE_LOCATION_SEND = 0x0f89
    MOBILE_LOCATION_SEND = 0x0f90
}GPRS_SEND_ID;

typedef enum
{
	ALTERNATION_HEARTBEAT=0x0001,			//0x0001,中断心跳发送间隔
	APN = 0x0010,		//0x0010, apn
	GPRS_USERNAME = 0X0011,		//x0011, gprs拨号用户名
	GPRS_PASSWORD= 0X0012,		//0x0012,gprs拨号密码
      IP_ADDR =0X0013, 			//0x0013,主服务器IP地址
	UDP_PORT = 0X0019, //0x0019,UDP端口
	//SET_AS_TIMING=0X0020,		//0x0020.设置为定时汇报; 20150504:只有定时上传，无其它的定距之类的
      ALTERNATION_IN_SLEEP = 0X0027, //0x0027,休眠时汇报时间间隔
	ALTERNATION_IN_ALARM= 0X0028,		//0x0028, 紧急报警时间隔
	ALTERNATION_DEFAULT_POSITION=0X0029,		//0x0029,缺省时间汇报间隔
	HIGH_SPEED=0X0055		//0x0055,最高速度
}TERMINAL_PARAMETER;



//消息头
//消息头内容详见表2：
//表2 消息头内容
//起始字节         字段               数据类型  描述及要求
//     0                      消息ID               WORD
//     2                    消息体属性    WORD              消息体属性格式结构图见图2
//     4                    终端手机号    BCD[6]       根据安装后终端自身的手机号转换。手机号不足12
//                                                                           位，则在前补充数字，大陆手机号补充数字0，港澳
//                                                                           台则根据其区号进行位数补充。
//     10                  消息流水号      WORD          按发送顺序从0 开始循环累加
//     12                  消息包封装项                 如果消息体属性中相关标识位确定消息分包处理，
//                                                                            则该项有内容，否则无该项
typedef struct gprs_message_head
{
  u16 id;
  u16 attribute;
  u8   mbphonenum[6];
  u16 serial_number;
}gprs_message_head_struct;


//消息体属性格式结构图如图2 所示：
//15     14  |  13   |   12   11    10           |        9    8   7     6     5    4    3   2   1   0
//保留  | 分包| 数据加密方式 |              消息体长度
//数据加密方式：
//——bit10~bit12 为数据加密标识位；
//——当此三位都为0，表示消息体不加密；
//——当第10 位为1，表示消息体经过RSA 算法加密；
//——其他保留。
#define  EXCURSION_MESSAGE_LENGTH    (1<<0)
#define  EXCURSION_MESSAGE_ENCRYPT  (1<<10)
#define  EXCURSION_MESSAGE_DISTRIBUTE (1<<13)


//6. 位置信息汇报: 0X0200, 								报警位:  0 紧急报警
//                                               1 超速报警
//                                               7 主电源欠压
//                                               8 主电源掉电
//                                               16 振动报警
//                                      状态位: 0 ACC状态
//                                             1 定位状态
//                                               2 北纬
//                                               3 东经
//                                             10 油路状态 


typedef struct position_message
{
  u32 alarm;
  u32 status;
  u32 latitude;  // 纬度
  u32 longitude; //  经度
  u16 altitude;  //  海拔
  u16 speed;  // 速度
  u16  direction; //方向
  u8 time[6]; //   YY-MM-DD-hh-mm-ss
}position_message_struct;

#define EXCURSION_POSITION_ALARM_ALARM  (1<<0)
#define EXCURSION_POSITION_ALARM_BEYOND  (1<<1)
#define EXCURSION_POSITION_ALARM_LOWPOWER  (1<<7)
#define EXCURSION_POSITION_ALARM_POWEROFF  (1<<8)
#define EXCURSION_POSITION_ALARM_SHAKE  (1<<16)

#define EXCURSION_POSITION_STATUS_ACC  (1<<0)
#define EXCURSION_POSITION_STATUS_ORIENTATION  (1<<1)
#define EXCURSION_POSITION_STATUS_SOURTH  (1<<2)
#define EXCURSION_POSITION_STATUS_WEST  (1<<3)
#define EXCURSION_POSITION_STATUS_OILBREAKER  (1<<10)

#define DOING_OIL_BREAK         (1<<0)
#define DOING_OIL_ON              (1<<1)
#define AREA_ALARM                 (1<<4)  //WEI,20160520电子围栏
#define D_H 0
#define D_L 1
#define M_H 2
#define M_L 3
#define Y_H 4
#define Y_L 5
#define H_H 6
#define H_L 7
#define m_H 8
#define m_L 9
#define S_H 10
#define S_L 11

// status : 0：成功/确认；1：失败；2：消息有误；3：不支持
  typedef enum
    {
      message_SUCCESS = 0,              /**< No error */
	message_FALSE,
	message_ERROR,
	message_NOT_SUPPORT

    } message_status;

//extern  void cmd_0200(void);
extern  void cmd_0002(void);
extern  void uploadLAC(void);
extern  u8 time_gps_rtc[6];//wei,20160809,保存GPS的时间，做为RTC时间的基准  
extern  u8  RTC_Get(void);
extern  position_message_struct GPS_Deal(position_message_struct position);  //wei,20160820 GPS处理子程序
extern  position_message_struct Deal_GSM_LAC(position_message_struct position_in);//wei,20160822 GSM处理子程序
//ljw,20160906测完要删extern u8 Deal_GSM_LAC(void);//ljw,20160906测完要删
extern  void RTC2Position_time(u8 *p); //wei,20160825,取系统的RTC时间
extern  char *strstr_len(u8 *s1,u8 *s2,u16 len);
extern  u8 send_gps_gsm_flag ;//wei,20160822 0x5a,GSM  0xa5,GPS
extern void uploadLAC_InquireChinese(void) ;  //wei,20160918,for 上传小区号获取中文位置 
    
extern void get_ID(void);//ljw,20160918,获取GSM小区ID
extern volatile u32 Lac_g ;//ljw,20160918全局变量的小区位置和ID
extern volatile u32 LacId_g ;//ljw,20160918全局变量的小区位置和ID
    
extern u8  ACC_GPS_flag ;
extern u8  ACC_GSM_flag ;
#endif //__gprs_h__
