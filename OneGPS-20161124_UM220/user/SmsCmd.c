/*
*
* 文件名称：SmsCmd.c
* 摘    要：短信命令处理函数
* 
* 当前版本：1.0
* 作    者：tt383@qq.com
* 完成日期：2011年05月22日
*
* 取代版本：1.0 
* 原作者  ：tt383@qq.com
* 完成日期：2011年05月10日
*
*/

#include "math.h"
#include "string.h"
#include "user_define.h"
#include "SmsCmd.h"
#include "main.h"
#include "led.h"
#include "gprs.h"
#include "lsm330dlc_driver.h"

_smsdata smsdatareg;
unsigned char WakeTime1;

extern double g_dCurrentLon,g_dCurrentLat,g_dMaxLon,g_dMaxLat,g_dMinLon,g_dMinLat;
extern double g_dMetersPLonDegree;
extern const double g_dMetersPLatDegree;

//extern unsigned char g_ucSysMode;  
//extern unsigned char g_ucSysMode_S; 

extern unsigned char g_ucTaskList;
extern unsigned char g_ucTrackMode;

//ljw 20160924 for wulian extern unsigned char g_ucCurrentNum[11];
extern unsigned char g_ucCurrentNum[32];//ljw 20160924 for wulian

extern unsigned char g_ucSysInitialized;
extern unsigned short g_usSmsSendTime;
//extern unsigned char g_ucMaxSpeed;



extern unsigned char  g_ucSMSConter;
extern unsigned short g_usWakeTime;


// GPRS 工作方式标志位
extern volatile unsigned char g_ucCtnuSmsLocaMissionFlag;
extern volatile unsigned char g_ucCtnuSmsLocaSendFlag;
extern volatile unsigned char g_ucGPRSMode;
extern const unsigned char  g_ucVERSION[18] ;


extern unsigned char REGNUMbeSET;
extern unsigned short TimeUpLdCnt;


extern void cmd_INQUIRE_CHINESE_POSITION(void);
extern void RestoreSystem(void);



void Cmd_SETM(void) ;
void Cmd_CXCS(void) ;
void Cmd_CXZD(void) ;
void cmd_CSKMG(void);

/*
2，	配置短信终端基本参数
<SETM*PSD0*ip1*ip2*T*port*APN1*APN2*M*N*AN*AF*TT*CS*VT*TH>
具体参数说明如下：
Ip1：主IP或者主DNS，ASCAII码，主IP模式，总长度不大于25，IP部分如不足3位的前面补0 ，例如220.178.000.056,9666 ；若为主DNS模式则无要求 
Ip2： 辅IP或者辅DNS，ASCAII码，为辅IP模式则总长度不大于25，IP部分如不足3位的前面补0，例如220.178.000.056,9668 ；若为辅DNS模式则无要求
T：      通讯方式，U为UDP通讯方式；T为TCP通讯方式
Port：    根据前面的通讯方式得知相应的UDP端口或者TCP端口
APN1：   主APN访问接入点，ASCALL码，总长度不大于25 
APN2：   辅APN访问接入点，ASCALL码，总长度不大于25 
M：     本机号码 ASCAII码，总长度11
N：      主中心号码 ASCAII码，总长度不大于20
AN 25  ACC开回传间隔时间25 单位 秒，出厂默认60秒，
AF 250   ACC关回传间隔时间250  单位秒，出厂默认120秒
TT 45   TCP心跳时间，最长3字符，如45 表示45秒，秒为单位，出厂默认40秒
CS 40   当前超速阀40，最长3字符，单位为公里，出厂默认0
VT 10   停车超时报警值，最长3个字符，分钟为单位，10分表示10分钟，出厂默认0
TH:1     接听模式，0表示自动接听；1表示ACC ON时自动接听，出厂默认1
注意：UDPIP，TCPIP，T，APN，M，AN，AF，TT等为机器重要参数，不建议修改，使用默认参数为最佳配置。
例子：
<SETM*12345678*220.178.000.056*220.178.000.056*U*9668*CMNET*CMNET*13100000018*13800001111*25*250*45*40*10*1*>
主机回复
<CXCS*220.178.000.056*220.178.000.056*U*9668*CMNET*CMNET*13100000018*13800001111*AN25*AF250*TT45*CS40*VT10*TH:1*JR:IP>并重新启动。


*/


/*
3，	查询终端基本参数 <CXCS*PSD0>
主机收到CXCS指令后短信回复
<CXCS*ip1*ip2*T*port*APN1*APN2*M*N*AN25*AF250*TT45*CS40*VT10*TH:1*JR:IP>
其中参数解释如下：
ip1：	主IP模式xxx.xxx.xxx.xxx；主DNS模式为字符串
ip2：	辅IP模式xxx.xxx.xxx.xxx；辅DNS模式为字符串
T：	    终端网络连接方式，U表示UDP，T表示TCP
port:    端口，UDP连接时为UDP端口，TCP连接时为TCP端口
APN1：	当前主APN参数
APN2： 当前辅APN参数
M：      本机号码 ASCAII码，总长度11，
N：      主中心号码 ASCAII码，总长度不大于20
AN25    ACC开回传间隔时间25秒，出厂默认20秒
AF250    ACC关回传间隔时间250秒， 出厂默认300秒
TT45    TCP心跳时间45秒，出厂默认30秒
CS40    当前超速阀40公里/小时，出厂默认0
VT10    停车超时报警值10分钟。出厂默认0
TH:1     接听模式，0表示自动接听； 1表示ACC ON时自动接听
	JR:IP     接入状态；IP为直接IP接入，DNS为DNS域名接入
*/

/*
4，	查询终端状态 <CXZD*PSD0>
终端返回：
<FHT*900AGS001P130319*M13418181818*GP1*CGREG:1*CSQ17*LAC27A0,0DE1*GPS06*JD:34.36988N*WD1117.9365E*SD000*DH1*YL0*DL0*CM0*ER0000>
FHT：数据头
900AGS001P130319 终端版本
M:13418181818   本机号码13418181818
GP:1            1是拨号成功，0是不成功
CGREG:1        GPRS网络是否注册成功
CSQ17          GSM信号强度17
LAC27A0,0DE1  位置区号：27A0，小区ID：0DE1		    
GPS06          当前星数6颗
JD34.36988N    经度34.36988N 北纬
WD 117.9365E   纬度117.9365E 东经
SD000          当前速度
DH1            点火ACC状态，1表示点火，0表示熄火
YL0            油路状态，0表示正常，1表示断开
DL0            电路状态，0表示正常，1表示断开
CM0           车门状态，0表示车门解锁，1表示车门加锁
ER0000         出错内容，4个字符，备用。

*/

char *pStart;
char *p1;
char *pEnd;
void analysis_sms(char * ptmp)
{
	u8 i;
//wei,20160803 for warning	u8 time;
    u8 temp_w = 0;//wei,20160628
	pStart=ptmp;
	if(strstr(pStart,COMPANY_FLAG) == NULL)
	{
#if 1	
		IWDG_ReloadCounter();
		if(strstr(pStart,"<SETM*") != NULL)
		{
			Cmd_SETM();
			delayms(1);
			IWDG_ReloadCounter();
			Cmd_CXCS();			
			delayms(4000);
            if (FHTIP != system_para.g_ucSysMode_S)
            {
            #if 0
                IWDG_ReloadCounter();
                for(time=0; time<10 ; time++)    // 20151017: ip 切换不成功，重启才可以
                {
                    delayms(500);
                    ddi_uart_Write(&UART1_ctl_t,"AT+QICLOSE\r\n",strlen("AT+QICLOSE\r\n"));
                    if (0 == WaitGSMOK1())
                        break;
                }
            #else
//wei,20160617				GSM_reset();
            #endif
//wei,20160617				StartGPRS();
                if(SMS_StartGSM_Flag == 0x5a)//wei,20160628,如果机器还在启动状态，不需要进行下面的拨号，
                      return;
//wei,20160617				   for(i=0; i<5;i++)
                for(i=1; i<8;i++)  //wei,20160617,最多断电三次。
                {
//                    if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))//满足休眠条件，要休眠
//                    {
//                        if((g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE)) //wei,20160709,满足睡眠条件，不要在继续拨号了
//                            return ;
//                    }
                    if(0x05 == check_sleep_state())//ljw,20160908判断休眠条返回 0x05,休眠  0x0a不休眠
                        return;
                    if((i%2==0)||(CPIN_READY_flag == 0))//wei,20160617
                        GSM_reset();//wei,20160617
                    temp_w = GetGPRSStatus();
                    if (temp_w != GPRS_CONNECTED)
                    {
                        if(temp_w ==GRPS_IP_PORT_err) 
                            GSM_reset();
                        delayms(500); 
                        dial_status =0;//wei20160621
                        StartGPRS();	
                    }
                    else
                        break; 
                    if(dial_status==1)//wei,20160621 if Dial-up success ,break
                        break;
                }
//wei,20160617				if (i>4)
                if(i>7)  //重启GSM的时间超长，防止机器耗电太多，就不再重启GSM了
				{
				 //while(1); //等看门狗复位系统	
					 GSM_Power(FALSE);
					 delayms(600);
//wei,20160617                    GSM_reset();//wei,20160616重启GSM
//wei,20160617					 GSM_start();                    
				}
            }
		}
		else
		if (strstr(pStart,"<CXCS>") != NULL)
		{
			Cmd_CXCS();
			IWDG_ReloadCounter();
			delayms(3000);
		}
		else
#endif			
		if (strstr(pStart,"<CXZD>") != NULL)
		{
		
			Cmd_CXZD();
			IWDG_ReloadCounter();
			//delayms(3000);
		}
		else
		if (strstr(pStart,"<CSKMG>") != NULL)
		{
			cmd_CSKMG();
		}
			
		return;
	}
		
	p1 = strstr(pStart,COMPANY_FLAG);
	//if (0 != strncmp((char *)(p1-2),"\r\n",2))
	if (p1 == NULL)
		return;
	if(strstr(pStart,"position") != NULL)
	{
		//Cmd_Position();
		if ( smsdatareg.count < 3  )
                          smsdatareg._smscommand[smsdatareg.count ] = position;
		pStart = strstr(pStart,"position") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
                         
		ClrGSMBuff();
	}
	
	if(strstr(pStart,"chinposi") != NULL)
	{
		//Cmd_Position();
		if ( smsdatareg.count < 3  )
                          smsdatareg._smscommand[smsdatareg.count ] = chinese_position;
		pStart = strstr(pStart,"chinposi") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
                         
		ClrGSMBuff();
	}
	else if(strstr(pStart,"init") != NULL) // 初始化设备
	{

		//Cmd_Initial();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = init;
			pStart = strstr(pStart,"init") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
		ClrGSMBuff();
	}

	else if(strstr(pStart,"newnum") != NULL)  // 注册 授权号码
	{
		//Cmd_NewNum();
		if(smsdatareg.count < 3)
           smsdatareg._smscommand[smsdatareg.count ] = newnum;	
		pStart = strstr(pStart,"newnum") ;
		pEnd = strstr(pStart,"OK");
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
		ClrGSMBuff();
	}
	else if(strstr(pStart,"delnum") != NULL) // 删除 授权号码
	{	


  		//Cmd_DelNum();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = delnum;		  		
		pStart = strstr(pStart,"delnum") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
		ClrGSMBuff();
	}
	else if(strstr(pStart,"delall") != NULL) // 删除 授权号码
	{	

		//Cmd_DelAll();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = delall;				
		pStart = strstr(pStart,"delall") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
		ClrGSMBuff();
	}
	else if(strstr(pStart,"shownum") != NULL) // 删除 授权号码
	{	

		//Cmd_DelAll();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = shownum;				
		pStart = strstr(pStart,"shownum") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
		ClrGSMBuff();
	}
	else if(strstr(pStart,"setpsw") != NULL) // 设置 密码
	{

		//Cmd_SetPsw();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = setpsw;				
		pStart = strstr(pStart,"setpsw") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
		ClrGSMBuff();
	}
	else if(strstr(pStart,"net") != NULL) // 设置 密码
	{
//				Cmd_SetNET();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = net;
		pStart = strstr(pStart,"net") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}

       else if(strstr(pStart,"setuptime") != NULL)  // 设置设备工作模式
	{
//				Cmd_SetUpTime();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = setuptime;
		pStart = strstr(pStart,"setuptime") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}

	else if(strstr(pStart,"imei") != NULL)  		// 设置设备工作模式
	{
//		Cmd_IMEI();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = imei;
		pStart = strstr(pStart,"imei") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}

	else if(strstr(pStart,"devinfo") != NULL)  		// 查询设备基本信息
	{
//		Cmd_DeviceInfo();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = devinfo;
		pStart = strstr(pStart,"devinfo");
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
		ClrGSMBuff();
	}
	
           else if(strstr(pStart,"readspeedlimit") != NULL)  		// 查询设备基本信息
	{
//		Cmd_ReadSpeedLimit();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = readspeedlimit;
		pStart = strstr(pStart,"readspeedlimit") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));
		ClrGSMBuff();
	}

	else if(strstr(pStart,"speedlimit") != NULL)  		// 查询设备基本信息
	{
//		Cmd_SpeedLimit();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = speedlimit;
		pStart = strstr(pStart,"speedlimit") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}
                
	else if(strstr(pStart,"nolimit") != NULL)  		// 查询设备基本信息
	{
//		Cmd_NoLimit();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = nolimit;
		pStart = strstr(pStart,"nolimit") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}
                
	else if(strstr(pStart,"cellid") != NULL)  		// 查询设备基本信息
	{
//		Cmd_CellID();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = cellid;
		pStart = strstr(pStart,"cellid") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}

	else if(strstr(pStart,"mid") != NULL)  		// 查询设备基本信息
	{
//		Cmd_Setmid();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = mid;
		pStart = strstr(pStart,"mid") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}
	
	else if(strstr(pStart,"mode") != NULL)  		// 查询设备基本信息
	{
//		Cmd_SetMode();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = mode;
		pStart = strstr(pStart,"mode") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}
	else if(strstr(pStart,"period") != NULL)  		// 查询设备基本信息
	{
//		Cmd_SetWakeTime();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = period;
		pStart = strstr(pStart,"period") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}
	else if(strstr(pStart,"acc") != NULL)  		// 查询设备基本信息
	{
//		Cmd_ENACC();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = acc;
		pStart = strstr(pStart,"acc") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}
	else if(strstr(pStart,"autosms") != NULL)  		// 查询设备基本信息
	{
//		Cmd_ENAUTOSMS();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = autosms;
		pStart = strstr(pStart,"autosms") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}

	
	else if(strstr(pStart,"duration") != NULL)  		// 查询设备基本信息
	{
//		Cmd_Duration();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = duration;
		pStart = strstr(pStart,"duration") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}
	
	else if(strstr(pStart,"sleeptime") != NULL)  		// 查询设备基本信息
	{
//		Cmd_SleepTime();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = sleeptime;
		pStart = strstr(pStart,"sleeptime") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}	
	else if(strstr(pStart,"shocks") != NULL)  		// 查询设备基本信息
	{
//		Cmd_SleepTime();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = shocks;
		pStart = strstr(pStart,"shocks") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}	
	else if(strstr(pStart,"relay") != NULL)  		// 查询设备基本信息
	{
//		Cmd_SleepTime();
		if ( smsdatareg.count < 3  )
                       smsdatareg._smscommand[smsdatareg.count ] = relay;
		pStart = strstr(pStart,"relay") ;
		pEnd = strstr(pStart,"OK") ;				
		memcpy(&(smsdatareg._smscontent[smsdatareg.count++]),pStart,(pEnd-pStart));				
		ClrGSMBuff();
	}		


}

//******************************************************************
//
//  单次定位指令 FHT Position
//
//******************************************************************


void Cmd_Position(void)   // 单次定位
{
	unsigned char *temp;
	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"position");
	temp += 8;
	if (('?' == *(temp+6)) && (0 == strncmp((char *)(temp+7),"\r\n",2)))
	{
		if(strncmp((char *)temp,(char *)(system_para.g_ucPassWord),6)) // 检查密码yuandan
		{
			SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
		  	return;
		}
		SendLocaSMS1(g_ucCurrentNum);
	}
	else
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
	}
}


//******************************************************************
//
//  单次中文定位指令 FHT Position
//
//******************************************************************

void Cmd_chinese_Position(void)   // 单次定位
{
	unsigned char *temp;
	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"chinposi");
	temp += 8;
	if (('?' == *(temp+6)) && (0 == strncmp((char *)(temp+7),"\r\n",2)))
	{
		if(strncmp((char *)temp,(char *)(system_para.g_ucPassWord),6)) // 检查密码yuandan
		{
			SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
		  	return;
		} 	
		
		//sendLocaSMS1(g_ucCurrentNum);
		cmd_INQUIRE_CHINESE_POSITION();
		//等待服务器哦下发中文位置信息，
	}
	else
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
	}

}

//ljw,20160924全0判断函数
//返回, 1--全0
//      0--非全0
u8 all_ascii0(u8 *p,u8 len)
{
    u8 i;
    for(i=0;i<len;i++)
    {
        if(*(p+i) != '0')
            return 0;
    }
    return 1;
}

/*
*****************************************************************
2，	配置短信终端基本参数
<SETM*IP* PORT*APN1*NN*NP*M*N*AN*MO*CS*AC*DX*YL*DT*ST*SN1*SN2>
具体参数说明如下：
IP：     主IP，ASCALL码，总长度不大于25，IP部分如不足3位的前面补0 ，例如220.178.000.056 
PT：    UDP端口，4字节
AP：   APN访问接入点，ASCALL码，总长度不大于25
NN：    网络用户名，4字节
NP：    网络密码，4字节
M：     本机号码 ASCALL码，总长度11字节
N：     上传间隔时间25 单位 秒，出厂默认30秒，3字节。
AN：   心跳时间，最长3字符，如45 表示45秒，秒为单位，出厂默认20秒，3字节
MO：   工作模式，1 elowpower，2 lowpower，3 allonline，默认为3。1字节
CS ：   当前超速阀，最长3字符，单位为公里，最大值250，出厂默认0，3字节
AC：   ACC使能 0关闭，1开启，默认开启，1字节
DX：   短信主动上传，0关闭，1开启，默认关闭，1字节
YL:     油路状态，0不切断，1切断
DT：   持续工作时间，模式1 elowpower和 2 lowpower省电模式下的工作时间，默认5分钟，3字节
ST：   休眠时间，模式1elowpower和 2 lowpower省电模式下休眠时间，默认50分钟，3字节
SN1：  特权号1，11字节
SN2：  特权号2，11字节
SS：   震动灵敏度，4字节，01-79震动加速度，01-38震动时间

例子：
<SETM*220.178.000.056*9668*CMNET*GPRS*GPRS*17800000000*030*020*3*000*1*0*0*005*050*17800000000*17800000000*0120>

终端回复
<CXCS*220.178.000.056*PT9668*APCMNET*NNGPRS*NPGPRS*M17800000000*N030*AN020*MO3*CS000*AC1*DX0*YL0*DT005*ST050*SN117800000000*SN21780000000*0120>

******************************************************************
*/

void Cmd_SETM(void)   // 
{
	unsigned char *temp;
	u8* temp1;
	u8 i,j;
	u16 WakeTime;
	unsigned char tmp_sn[12];
    u8 temp_len;

	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"SETM*");
	temp += 5;
	temp1 = (u8*)strstr((const char *)temp,"*");//wei,20160803 for warning (const char *)
	if ((temp1-temp) > 25 ) //总长度不大于25
		return;
	if(FHTIP != system_para.g_ucSysMode_S)
	{
	    for(i=0 ; i<16 ; i++)
		{
			system_para.g_ucIPAddress[i] = 0;
		}
		   
		for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)  //找下一个空格了
		{
			system_para.g_ucIPAddress[i] = *(temp+i);
//WEI			if (i>=16)
			if (i>=IPADD_LEN-2) //wei  if (i>=14)
			{
				//SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));
				break;
			}	
		}
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 端口
	for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)  //找下一个空格了
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{

			//SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));
		  	return;

		}
	}
	if (FHTIP != system_para.g_ucSysMode_S)
	{
		for(i=0 ; i<5 ; i++)
		{
		   system_para.g_ucUDPPort[i] = 0;
		}
		for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)
		{
		   system_para.g_ucUDPPort[i] = *(temp+i);
//wei		   if (i>=5)
			if (i>=4) //wei
			break;
		}
	}	


//	for(i=0 ; i<20 ; i++)
    for(i=0 ; i<NETAPN_LEN ; i++)
	{
	   system_para.g_ucNetAPN[i] = 0;
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 APN
	for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)
	{
		system_para.g_ucNetAPN[i] = *(temp+i);
//wei		if (i>=20)
		if (i>=NETAPN_LEN-2)
			break;
	}

	for(i=0 ; i<NETUSER_LEN ; i++)
	{
	      system_para.g_ucNetUser[i] = 0;
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 网络用户名
	for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)
	{
		system_para.g_ucNetUser[i] = *(temp+i);
//wei		if (i>=6)
		if (i>=NETUSER_LEN-2) //wei
			break;
	}

	for(i=0 ; i<NETPSW_LEN ; i++)
	{
	      system_para.g_ucNetPsw[i] = 0;
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 网络密码
	    temp1 = temp;
	for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)
	{
		system_para.g_ucNetPsw[i] = *(temp1+i);
//wei		if (i>=8)
		if (i>=NETPSW_LEN-2)
			break;			
	}


	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 mid
	temp1 = temp;
	//判断号码是否有效
	for(i =0; i<11; i++)
	  {
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			//return;
			//delayms(500); //wait for send end
			//setflag = 1;
			break;
		}
			
	  }
	if (i < 11)
	{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			//return;
			//delayms(500); //wait for send end
		  return;
	}  

	memcpy ((char *)(&tmp_sn[1]),temp1,11);
	tmp_sn [0] = '0';
	for (i=0;i<6;i++)
	{
		asciitohex((u8*)(&tmp_sn[2*i]),(u8*)(&(system_para.g_ucDeviceID[i])),2);
	}
	for (i=0;i<6;i++)
	{
		hex2BCD_ASCII1((u8*)(&tmp_sn[i]),system_para.g_ucDeviceID[i]);
	}
	memcpy(system_para.g_ucDeviceID,tmp_sn,6);



	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 设置GPRS主动上传间隔
	temp1 = temp;
	//判断号码是否有效
	  for(i =0; i<3; i++)
	  {
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
			
	  }
	WakeTime = (*temp - '0') * 100 + (*(temp+1) - '0') * 10 ;

	if(*(temp+2) >= '0' && *(temp+2) <= '9')
	{
	  	WakeTime = WakeTime + *(temp+2) - '0';
	}
	else
	{
	  	WakeTime = WakeTime / 10;
	}
	if ((WakeTime < 1) || (WakeTime > 300))
	{
		//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
		return;
	}
	system_para.g_usGPRSUpLoadAternationTime = WakeTime ;	
	


	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 心跳时间间隔
	temp1 = temp;
	//判断号码是否有效
	  for(i =0; i<3; i++)
	  {
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
			
	  }
	WakeTime = (*temp - '0') * 100 + (*(temp+1) - '0') * 10 ;

	if(*(temp+2) >= '0' && *(temp+2) <= '9')
	{
	  	WakeTime = WakeTime + *(temp+2) - '0';
	}
	else
	{
	  	WakeTime = WakeTime / 10;
	}
	if ((WakeTime < 1) || (WakeTime > 300))
	{
		//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
		return;
	}
	system_para.HeartAternationTime = WakeTime ;	
	 if(system_para.HeartAternationTime <10) //wei,20160603强制心跳间隔不能小于10s
	    system_para.HeartAternationTime = 10; //wei,20160603
	 if(system_para.HeartAternationTime >300) //wei,20160603强制心跳间隔不能小于10s
          system_para.HeartAternationTime = 300; //wei,20160603
	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 工作模式，1 elowpower，2 lowpower，3 allonline，默认为3。1字节

	if (*temp>'0'  && *temp <'4')
	{
		system_para.g_ucPowerMode = *temp-'0';

	}
		
	

	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 当前超速阀，最长3字符，单位为公里，最大值250，出厂默认0，3字节

	for(i =0; i<3; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		
	}
	 
	WakeTime = (*temp - '0')*100;
	WakeTime += (*(temp+1) - '0') * 10;
	WakeTime += (*(temp+2) - '0') ;

	if (WakeTime <=250 )
	{
		system_para.g_ucMaxSpeed = WakeTime;
	}

	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // ACC使能 0关闭，1开启，默认开启，1字节

	if(*temp == '0' || *temp == '1')
	{
		system_para.g_bACCon = *temp-'0';
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 短信主动上传，0关闭，1开启，默认关闭，1字节

	if(*temp == '0' || *temp == '1')
	{

		system_para.g_bAUTOSMSon = *temp-'0';

	}
	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 油路状态，0不切断，1切断
	if(*temp == '0' )
	{
		system_para.g_break = 0;
	}
	if(*temp == '1' )
	{
		system_para.g_break = 1;
		set_OIL_BREAK(TRUE);
	}
	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 持续工作时间，模式1 elowpower和 2 lowpower省电模式下的工作时间，默认5分钟，3字节
	for(i =0; i<3; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		
	}
	WakeTime = (*temp - '0')*100;
	WakeTime += (*(temp+1) - '0') * 10;
	WakeTime += (*(temp+2) - '0') ;
	system_para.g_usGPRSUpLoadTime = WakeTime;

	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 休眠时间，模式1elowpower和 2 lowpower省电模式下休眠时间，默认50分钟，3字节
	for(i =0; i<3; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		
	}
	WakeTime = (*temp - '0')*100;
	WakeTime += (*(temp+1) - '0') * 10;
	WakeTime += (*(temp+2) - '0') ;
	system_para.g_usSleepTime = WakeTime;	
#if 1
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  //  特权号1，11字节
    temp1 = (unsigned char *)strstr((char *)temp,"*"); //ljw,20160924 for wu lian
    temp_len = temp1-temp;//号码长度ljw,20160924 for wu lian
    if(temp_len >=PHNUM_LEN-3)
        return ;
 
//    if(!isdigtit_len(temp,temp_len))//LJW,20160928不要做字符判断
//        return;
//	for(i =0; i<11; i++)
//	{
//		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
//		{
//			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
//			return;
//		}
//	}
    memset((u8 *)(&system_para.g_ucRegNum[0][0]),0x00,PHNUM_LEN);
	if(all_ascii0(temp,temp_len))//ljw,20160924 号码1
    {
        system_para.g_ucRegNum[0][0]='e';
    }
    else
    {
        for(j=0 ; j<temp_len ; j++)
        {
            system_para.g_ucRegNum[0][2+j] = *(temp+j);
        }
        system_para.g_ucRegNum[0][1] = temp_len;//ljw,20160926,存储号码长度
        system_para.g_ucRegNum[0][0] = FULL;   //
    }
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  //  特权号2，11字节
    temp1 = (unsigned char *)strstr((char *)temp,"*"); //ljw,20160924 for wu lian
    temp_len = temp1-temp;//号码长度ljw,20160924 for wu lian
    if(temp_len >=PHNUM_LEN-3)
        return ;
//    if(!isdigtit_len(temp,temp_len))//LJW,20160928不要做字符判断
//        return;
    memset((u8 *)(&system_para.g_ucRegNum[1][0]),0x00,PHNUM_LEN);
    if(all_ascii0(temp,temp_len))//ljw,20160924 号码1
    {
        system_para.g_ucRegNum[1][0]='e';
    }
    else
    {
        for(j=0 ; j<temp_len ; j++)
        {
            system_para.g_ucRegNum[1][2+j] = *(temp+j);
        }
        system_para.g_ucRegNum[1][1] = temp_len;//ljw,20160926,存储号码长度
        system_para.g_ucRegNum[1][0] = FULL;   //
    }
#if 0 //ljw,    
	for(i =0; i<11; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
	}
	for(j=0 ; j<11 ; j++)
	{
		system_para.g_ucRegNum[1][4+j] = *(temp+j);
	}
	system_para.g_ucRegNum[1][0] = FULL;   // 
#endif
	//震动灵敏度，4字节，01-79震动加速度，01-38震动时间
	temp = (unsigned char *)strstr((char *)temp,"*") + 1; 
	WakeTime = (*temp - '0')*10;
	WakeTime += (*(temp+1) - '0') ;	
	i = (*(temp+2) - '0')*10;
	i += (*(temp+3) - '0') ;	
	
	if ((WakeTime<=79) && (WakeTime >  0) && (i>0) && (i<=38))
	{
		system_para.acceleration = WakeTime;
		system_para.impact_time = i;
		set_Impact(system_para.acceleration,system_para.impact_time);
	}
#endif
    
	Store_System_Para();
	sys_Init();
}
#if 0//ljw,20160924 for wu lian
void Cmd_SETM(void)   // 
{
	unsigned char *temp;
	u8* temp1;
	u8 i,j;
	u16 WakeTime;
	unsigned char tmp_sn[12];

	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"SETM*");
	temp += 5;
	temp1 = (u8*)strstr((const char *)temp,"*");//wei,20160803 for warning (const char *)
	if ((temp1-temp) > 25 ) //总长度不大于25
		return;
	if (FHTIP != system_para.g_ucSysMode_S)
	{	
	      for(i=0 ; i<16 ; i++)
		{
			system_para.g_ucIPAddress[i] = 0;
		}
		   
		for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)  //找下一个空格了
		{
			system_para.g_ucIPAddress[i] = *(temp+i);
//WEI			if (i>=16)
			if (i>=IPADD_LEN-2) //wei  if (i>=14)
			{
				//SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));
				break;
			}	
		}
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 端口
	for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)  //找下一个空格了
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{

			//SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));
		  	return;

		}
	}
	if (FHTIP != system_para.g_ucSysMode_S)
	{
		for(i=0 ; i<5 ; i++)
		{
		   system_para.g_ucUDPPort[i] = 0;
		}
		for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)
		{
		   system_para.g_ucUDPPort[i] = *(temp+i);
//wei		   if (i>=5)
			if (i>=4) //wei
			break;
		}
	}	


//	for(i=0 ; i<20 ; i++)
    for(i=0 ; i<NETAPN_LEN ; i++)
	{
	   system_para.g_ucNetAPN[i] = 0;
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 APN
	for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)
	{
		system_para.g_ucNetAPN[i] = *(temp+i);
//wei		if (i>=20)
		if (i>=NETAPN_LEN-2)
			break;
	}

	for(i=0 ; i<NETUSER_LEN ; i++)
	{
	      system_para.g_ucNetUser[i] = 0;
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 网络用户名
	for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)
	{
		system_para.g_ucNetUser[i] = *(temp+i);
//wei		if (i>=6)
		if (i>=NETUSER_LEN-2) //wei
			break;
	}

	for(i=0 ; i<NETPSW_LEN ; i++)
	{
	      system_para.g_ucNetPsw[i] = 0;
	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 网络密码
	    temp1 = temp;
	for(i=0 ; i<((unsigned char *)strstr((char *)temp,"*")-temp) ; i++)
	{
		system_para.g_ucNetPsw[i] = *(temp1+i);
//wei		if (i>=8)
		if (i>=NETPSW_LEN-2)
			break;			
	}


	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 指向 mid
	temp1 = temp;
	//判断号码是否有效
	for(i =0; i<11; i++)
	  {
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			//return;
			//delayms(500); //wait for send end
			//setflag = 1;
			break;
		}
			
	  }
	if (i < 11)
	{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			//return;
			//delayms(500); //wait for send end
		  return;
	}  

	memcpy ((char *)(&tmp_sn[1]),temp1,11);
	tmp_sn [0] = '0';
	for (i=0;i<6;i++)
	{
		asciitohex((u8*)(&tmp_sn[2*i]),(u8*)(&(system_para.g_ucDeviceID[i])),2);
	}
	for (i=0;i<6;i++)
	{
		hex2BCD_ASCII1((u8*)(&tmp_sn[i]),system_para.g_ucDeviceID[i]);
	}
	memcpy(system_para.g_ucDeviceID,tmp_sn,6);



	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 设置GPRS主动上传间隔
	temp1 = temp;
	//判断号码是否有效
	  for(i =0; i<3; i++)
	  {
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
			
	  }
	WakeTime = (*temp - '0') * 100 + (*(temp+1) - '0') * 10 ;

	if(*(temp+2) >= '0' && *(temp+2) <= '9')
	{
	  	WakeTime = WakeTime + *(temp+2) - '0';
	}
	else
	{
	  	WakeTime = WakeTime / 10;
	}
	if ((WakeTime < 1) || (WakeTime > 300))
	{
		//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
		return;
	}
	system_para.g_usGPRSUpLoadAternationTime = WakeTime ;	
	


	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 心跳时间间隔
	temp1 = temp;
	//判断号码是否有效
	  for(i =0; i<3; i++)
	  {
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
			
	  }
	WakeTime = (*temp - '0') * 100 + (*(temp+1) - '0') * 10 ;

	if(*(temp+2) >= '0' && *(temp+2) <= '9')
	{
	  	WakeTime = WakeTime + *(temp+2) - '0';
	}
	else
	{
	  	WakeTime = WakeTime / 10;
	}
	if ((WakeTime < 1) || (WakeTime > 300))
	{
		//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
		return;
	}
	system_para.HeartAternationTime = WakeTime ;	
	 if(system_para.HeartAternationTime <10) //wei,20160603强制心跳间隔不能小于10s
	    system_para.HeartAternationTime = 10; //wei,20160603
	 if(system_para.HeartAternationTime >300) //wei,20160603强制心跳间隔不能小于10s
          system_para.HeartAternationTime = 300; //wei,20160603
	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 工作模式，1 elowpower，2 lowpower，3 allonline，默认为3。1字节

	if (*temp>'0'  && *temp <'4')
	{
		system_para.g_ucPowerMode = *temp-'0';

	}
		
	

	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 当前超速阀，最长3字符，单位为公里，最大值250，出厂默认0，3字节

	for(i =0; i<3; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		
	}
	 
	WakeTime = (*temp - '0')*100;
	WakeTime += (*(temp+1) - '0') * 10;
	WakeTime += (*(temp+2) - '0') ;

	if (WakeTime <=250 )
	{
		system_para.g_ucMaxSpeed = WakeTime;
	}

	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // ACC使能 0关闭，1开启，默认开启，1字节

	if(*temp == '0' || *temp == '1')
	{

		system_para.g_bACCon = *temp-'0';

	}
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 短信主动上传，0关闭，1开启，默认关闭，1字节

	if(*temp == '0' || *temp == '1')
	{

		system_para.g_bAUTOSMSon = *temp-'0';

	}
	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 油路状态，0不切断，1切断
	if(*temp == '0' )
	{
		system_para.g_break = 0;
	}
	if(*temp == '1' )
	{
		system_para.g_break = 1;
		set_OIL_BREAK(TRUE);
	}
	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 持续工作时间，模式1 elowpower和 2 lowpower省电模式下的工作时间，默认5分钟，3字节
	for(i =0; i<3; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		
	}
	WakeTime = (*temp - '0')*100;
	WakeTime += (*(temp+1) - '0') * 10;
	WakeTime += (*(temp+2) - '0') ;
	system_para.g_usGPRSUpLoadTime = WakeTime;

	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  // 休眠时间，模式1elowpower和 2 lowpower省电模式下休眠时间，默认50分钟，3字节
	for(i =0; i<3; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		
	}
	WakeTime = (*temp - '0')*100;
	WakeTime += (*(temp+1) - '0') * 10;
	WakeTime += (*(temp+2) - '0') ;
	system_para.g_usSleepTime = WakeTime;	
#if 1
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  //  特权号1，11字节
	for(i =0; i<11; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		
	}
	
	for(j=0 ; j<11 ; j++)
	{
		system_para.g_ucRegNum[0][4+j] = *(temp+j);
	}
	system_para.g_ucRegNum[0][0] = FULL;   // 
	
	temp = (unsigned char *)strstr((char *)temp,"*") + 1;  //  特权号2，11字节
	for(i =0; i<11; i++)
	{
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			//SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		
	}
	
	for(j=0 ; j<11 ; j++)
	{
		system_para.g_ucRegNum[1][4+j] = *(temp+j);
	}
	system_para.g_ucRegNum[1][0] = FULL;   // 

	//震动灵敏度，4字节，01-79震动加速度，01-38震动时间
	temp = (unsigned char *)strstr((char *)temp,"*") + 1; 
	WakeTime = (*temp - '0')*10;
	WakeTime += (*(temp+1) - '0') ;	
	i = (*(temp+2) - '0')*10;
	i += (*(temp+3) - '0') ;	
	
	if ((WakeTime<=79) && (WakeTime >  0) && (i>0) && (i<=38))
	{
		system_para.acceleration = WakeTime;
		system_para.impact_time = i;
		set_Impact(system_para.acceleration,system_para.impact_time);
	}
#endif		

	
	Store_System_Para();
	sys_Init();	

}

#endif

void set_ascii0(u8 *p,u8 len)
{
    u8 i;
    for(i=0;i<len;i++)
        *(p+i)='0';
}

const u8 zero_ascii[11]= {'0','0','0','0','0','0','0','0','0','0','0'};//ljw,20160924for

void sub_cxcs(u8* sms_in)
{
	unsigned char* smsbuf= sms_in;
	u8 tmp_sn[30];
	u8 t1;
	u16 u16tmp;
	u16 i;
	memset(smsbuf,0,sizeof(smsbuf));
	strncat((char *)smsbuf,"<CXCS*",strlen("<CXCS*"));
	strncat((char *)smsbuf,system_para.g_ucIPAddress,strlen(system_para.g_ucIPAddress));//
	strncat((char *)smsbuf,"*PT",3);
	strncat((char *)smsbuf,system_para.g_ucUDPPort,strlen(system_para.g_ucUDPPort));//
	strncat((char *)smsbuf,"*AP",3);	
	strncat((char *)smsbuf,system_para.g_ucNetAPN,strlen(system_para.g_ucNetAPN));//
	strncat((char *)smsbuf,"*NN",3);
	strncat((char *)smsbuf,system_para.g_ucNetUser,strlen(system_para.g_ucNetUser));//
	strncat((char *)smsbuf,"*NP",3);
	strncat((char *)smsbuf,system_para.g_ucNetPsw,strlen(system_para.g_ucNetPsw));//
	strncat((char *)smsbuf,"*M",2);
	hextoascii(system_para.g_ucDeviceID,tmp_sn,6);
	strncat((char *)smsbuf,&tmp_sn[1],11);//
	strncat((char *)smsbuf,"*N",2); // 上传间隔时间25 单位 秒，出厂默认30秒，3字节。
	u16tmp = (u16)system_para.g_usGPRSUpLoadAternationTime;
	tmp_sn[0] = (unsigned char)((u16tmp - u16tmp % 100) / 100) + '0';
	tmp_sn[1] = (unsigned char)((u16tmp % 100 - u16tmp % 10) / 10) + '0';
	tmp_sn[2] = u16tmp % 10 + '0';
	strncat((char *)smsbuf,tmp_sn,3);//
	strncat((char *)smsbuf,"*AN",3); // 心跳间隔时间25 单位 秒，出厂默认30秒，3字节。
	u16tmp = (u16)system_para.HeartAternationTime;
	tmp_sn[0] = (unsigned char)((u16tmp - u16tmp % 100) / 100) + '0';
	tmp_sn[1] = (unsigned char)((u16tmp % 100 - u16tmp % 10) / 10) + '0';
	tmp_sn[2] = u16tmp % 10 + '0';
	strncat((char *)smsbuf,tmp_sn,3);//	
	strncat((char *)smsbuf,"*MO",3); //  MO：   工作模式，1 elowpower，2 lowpower，3 allonline，默认为3。1字节
	t1 = system_para.g_ucPowerMode+'0';
	strncat((char *)smsbuf,&t1,1); //

	strncat((char *)smsbuf,"*CS",3);//CS ：   当前超速阀，最长3字符，单位为公里，最大值250，出厂默认0，3字节
	u16tmp = (u16)system_para.g_ucMaxSpeed;
	tmp_sn[0] = (unsigned char)((u16tmp - u16tmp % 100) / 100) + '0';
	tmp_sn[1] = (unsigned char)((u16tmp % 100 - u16tmp % 10) / 10) + '0';
	tmp_sn[2] = u16tmp % 10 + '0';	
	strncat((char *)smsbuf,tmp_sn,3);
	
    strncat((char *)smsbuf,"*AC",3);//AC：   ACC使能 0关闭，1开启，默认开启，1字节
	t1 = system_para.g_bACCon+'0';
	strncat((char *)smsbuf,&t1,1);
	
    strncat((char *)smsbuf,"*DX",3);//DX：   短信主动上传，0关闭，1开启，默认关闭，1字节
	t1 = system_para.g_bAUTOSMSon+'0';
	strncat((char *)smsbuf,&t1,1);
	strncat((char *)smsbuf,"*YL",3);//YL:     油路状态，0不切断，1切断
	t1 = system_para.g_break+'0';
	strncat((char *)smsbuf,&t1,1);
	strncat((char *)smsbuf,"*DT",3);//DT：   持续工作时间，模式1 elowpower和 2 lowpower省电模式下的工作时间，默认5分钟，3字节
	u16tmp = (u16)system_para.g_usGPRSUpLoadTime;
	tmp_sn[0] = (unsigned char)((u16tmp - u16tmp % 100) / 100) + '0';
	tmp_sn[1] = (unsigned char)((u16tmp % 100 - u16tmp % 10) / 10) + '0';
	tmp_sn[2] = u16tmp % 10 + '0';
	strncat((char *)smsbuf,tmp_sn,3);
	//strncat((char *)smsbuf,"*ST",3);//ST：   休眠时间，模式1elowpower和 2 lowpower省电模式下休眠时间，默认50分钟，3字节
	strncat((char *)smsbuf,"*",1);
	u16tmp = (u16)system_para.g_usSleepTime;
	tmp_sn[0] = (unsigned char)((u16tmp - u16tmp % 100) / 100) + '0';
	tmp_sn[1] = (unsigned char)((u16tmp % 100 - u16tmp % 10) / 10) + '0';
	tmp_sn[2] = u16tmp % 10 + '0';
	strncat((char *)smsbuf,tmp_sn,3);
	
	//strncat((char *)smsbuf,"*SN1",4);//SN1：  特权号1，11字节
//ljw,20160924for wulian<    
    strncat((char *)smsbuf,"*",1);
    if((system_para.g_ucRegNum[0][0] == FULL)&&(system_para.g_ucRegNum[0][1] == ((u8)strlen((u8 *)&system_para.g_ucRegNum[0][2]))))
    {
        strncat((char *)smsbuf,&system_para.g_ucRegNum[0][2],system_para.g_ucRegNum[0][1]);
    }
    else
        strncat((char *)smsbuf,zero_ascii,11);
    strncat((char *)smsbuf,"*",1);
    if((system_para.g_ucRegNum[1][0] == FULL)&&(system_para.g_ucRegNum[1][1] == ((u8)strlen((u8 *)&system_para.g_ucRegNum[1][2]))))
    {
        strncat((char *)smsbuf,&system_para.g_ucRegNum[1][2],system_para.g_ucRegNum[1][1]);
    }
    else
        strncat((char *)smsbuf,zero_ascii,11);
    
//ljw,20160924>
#if 0 //ljw,20160924 for wulian 
	strncat((char *)smsbuf,"*",1);
	strncat((char *)smsbuf,&system_para.g_ucRegNum[0][4],11);//
	//SN2：  特权号2，11字节
	//strncat((char *)smsbuf,"*SN2",4);
	strncat((char *)smsbuf,"*",1);
	strncat((char *)smsbuf,&system_para.g_ucRegNum[1][4],11);//
#endif   
    
	//strncat((char *)smsbuf,"*SS",3);//SS：   震动灵敏度，4字节，01-79震动加速度，01-38震动时间?
	strncat((char *)smsbuf,"*",1);
	hex2BCD_ASCII(tmp_sn,system_para.acceleration);
	//strncat((char *)smsbuf,tmp_sn,2);
	if (system_para.acceleration>9)
	{
		strncat((char *)smsbuf,tmp_sn,2);
	}
	else
	{
		strncat((char *)smsbuf,"0",1);
		strncat((char *)smsbuf,tmp_sn,1);
	}
	hex2BCD_ASCII(tmp_sn,system_para.impact_time);
	if (system_para.impact_time>9)
	{
		strncat((char *)smsbuf,tmp_sn,2);
	}
	else
	{
		strncat((char *)smsbuf,"0",1);
		strncat((char *)smsbuf,tmp_sn,1);
	}
	
	t1= '>' ;
	strncat((char *)smsbuf,&t1,1);

}
/*
*****************************************************************
<CXCS*220.178.000.056*PT9668*APCMNET*NNGPRS*NPGPRS*M17800000000*N030*AN020*MO3*
            CS000*AC1*DX0*YL0*DT005*ST050*SN117800000000*SN21780000000*0120>
******************************************************************
*/
//ljw,20160928 for wulian
void Cmd_CXCS(void)   // 
{
//wei	unsigned char smsbuf[160];
    unsigned char smsbuf[250]={0};
	u8 tmp_sn[30];
	u8 t1;
	u16 u16tmp;
	u16 i;
    u8 num_len ;//ljw,20160924 for wulian

	sub_cxcs(smsbuf);
	//SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
	memset(tmp_sn,0,sizeof(tmp_sn));
	strncat((char *)tmp_sn,"AT+CMGS=\"",strlen("AT+CMGS=\""));
    num_len = strlen(g_ucCurrentNum);//ljw,20160924 for wulian
    if(num_len >= PHNUM_LEN-3)//ljw,20160924 for wulian
        return ;
//ljw,20160924 for wulian	strncpy((char *)(tmp_sn+9),(char *)g_ucCurrentNum,11);
    strncpy((char *)(tmp_sn+9),(char *)g_ucCurrentNum,num_len);
    tmp_sn[num_len+9] = '\"';//ljw,20160924 for wulian	tmp_sn[20] = '\"';
    tmp_sn[num_len+10] = '\r';//ljw,20160924 for wulian	tmp_sn[21] = '\r';
    tmp_sn[num_len+11] = '\n';//ljw,20160924 for wulian	tmp_sn[22] = '\n';

    deal_g_ucSim900Status();
#if 0 //ljw,20160903 for spac
	for (i=0; i<500;i++)
	{
		if(BUSY == g_ucSim900Status)
		{
		   delayms(1);
		}
		else
			break;
	}
	g_ucSim900Status = BUSY;   // ?ü??SIM900?￡?é×′ì?
#endif 
	ddi_uart_Write(&UART1_ctl_t,tmp_sn,strlen(tmp_sn));
	WaitGSMOK1();

    // memset(temp,0,sizeof(temp));
	//memcpy((char *)temp,(char *)msg,strlen(msg));
	//temp[strlen(msg)] = 0x1A;
    if(strlen(smsbuf)>160)//ljw,201690928 for wulian
        memset((u8 *)&smsbuf[159],0x00,90);
	t1 = 0x1A;
	strncat((char *)smsbuf,&t1,1);
	ddi_uart_Write(&UART1_ctl_t,smsbuf,strlen(smsbuf));   //LedFlash();
//wei,20160628	WaitGSMOK1();
    WaitGSMOK2(500);//等待最长时间是10s
	delayms(1);
	g_ucSim900Status =IDLE;    // 更改SIM900模块状态
}

#if 0
void Cmd_CXCS(void)   // 
{
//wei	unsigned char smsbuf[160];
    unsigned char smsbuf[250]={0};
	u8 tmp_sn[30];
	u8 t1;
	u16 u16tmp;
	u16 i;
    u8 num_len ;//ljw,20160924 for wulian

	sub_cxcs(smsbuf);	
	//SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
	memset(tmp_sn,0,sizeof(tmp_sn));
	strncat((char *)tmp_sn,"AT+CMGS=\"",strlen("AT+CMGS=\""));
    num_len = strlen(g_ucCurrentNum);//ljw,20160924 for wulian
    if(num_len >= PHNUM_LEN-3)//ljw,20160924 for wulian
        return ;
//ljw,20160924 for wulian	strncpy((char *)(tmp_sn+9),(char *)g_ucCurrentNum,11);
    strncpy((char *)(tmp_sn+9),(char *)g_ucCurrentNum,num_len);
    tmp_sn[num_len+9] = '\"';//ljw,20160924 for wulian	tmp_sn[20] = '\"';
    tmp_sn[num_len+10] = '\r';//ljw,20160924 for wulian	tmp_sn[21] = '\r';
    tmp_sn[num_len+11] = '\n';//ljw,20160924 for wulian	tmp_sn[22] = '\n';

    deal_g_ucSim900Status();
#if 0 //ljw,20160903 for spac
	for (i=0; i<500;i++)
	{
		if(BUSY == g_ucSim900Status)
		{
		   delayms(1);
		}
		else
			break;
	}
	g_ucSim900Status = BUSY;   // ?ü??SIM900?￡?é×′ì?
#endif 
	ddi_uart_Write(&UART1_ctl_t,tmp_sn,strlen(tmp_sn));
	WaitGSMOK1();

    // memset(temp,0,sizeof(temp));
	//memcpy((char *)temp,(char *)msg,strlen(msg));
	//temp[strlen(msg)] = 0x1A;
	t1 = 0x1A;
	strncat((char *)smsbuf,&t1,1);
	ddi_uart_Write(&UART1_ctl_t,smsbuf,strlen(smsbuf));   //LedFlash();
	WaitGSMOK1();
	delayms(1);
	g_ucSim900Status =IDLE;    // 更改SIM900模块状态
}
#endif
/*
//******************************************************************
4，	查询终端状态 <CXZD>
终端返回：
<FHT*900MTS001U*M13418181818*GP1*CGREG:1*CSQ17*LAC27A0,0DE1 *JD:34.36988N*WD1117.9365E*SD000*DH1*YL0*ER0000>
FHT：数据头
900MTS001U 终端版本，摩托车终端型号为：900MTS， 001U表示摩托车终端第一版。
M:13418181818   本机号码13418181818
GP:1            1是拨号成功，0是不成功
CGREG:1        GPRS网络是否注册成功
CSQ17          GSM信号强度17
LAC27A0,0DE1  位置区号：27A0，小区ID：0DE1		    
JD34.36988N    经度34.36988N 北纬
WD 117.9365E   纬度117.9365E 东经
SD000          当前速度
DH1            点火ACC状态，1表示点火，0表示熄火
YL0            油路状态，0表示正常，1表示断开
ER0000         出错内容，4个字符，备用。

//******************************************************************
*/

void Cmd_CXZD(void)   // 
{
	unsigned char smsbuf[180];
	u8 tmp_sn[12];
	u8 t1;
	u16 u16tmp;
	u8*ptmp;
	u8*p1;
	u8*i;
	position_message_struct positions;
	memset(smsbuf,0,sizeof(smsbuf));
	strncat((char *)smsbuf,"<FHT*",strlen("<FHT*"));
	strncat((char *)smsbuf,g_ucVERSION,sizeof(g_ucVERSION));//
	strncat((char *)smsbuf,"*M",2);
	hextoascii(system_para.g_ucDeviceID,tmp_sn,6);
	strncat((char *)smsbuf,&tmp_sn[1],11);//
	strncat((char *)smsbuf,"*GP",3);//GP:1            1是拨号成功，0是不成功
	t1= dial_status+'0';
	strncat((char *)smsbuf,&t1,1);
	strncat((char *)smsbuf,"*CGREG:",7);	//CGREG:1        GPRS网络是否注册成功
	
	t1 = GetGPRSStatus()+'0';
	strncat((char *)smsbuf,&t1,1);
	strncat((char *)smsbuf,"*CSQ",4);		//CSQ17          GSM信号强度17
    ddi_uart_Write(&UART1_ctl_t,"AT+CSQ\r\n",strlen("AT+CSQ\r\n"));

    WaitGSMOK1();	//ljw,20160903for space  WaitGSMOK();
	ptmp = strstr(g_ucSim900Buff,"+CSQ: ");
	ptmp += 6;
	p1 = strstr(ptmp,",");
	strncat((char *)smsbuf, ptmp,p1-ptmp);

	strncat((char *)smsbuf,"*LAC",4);//位置区号：27A0，小区ID：0DE1
	ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r",9);
	WaitGSMOK1();//ljw,20160903for space  WaitGSMOK();
	IWDG_ReloadCounter();
	ptmp = (unsigned char *)strstr((char *)g_ucSim900Buff,"\"");
	ptmp += 1;
	strncat((char *)smsbuf,ptmp,4);
	strncat((char *)smsbuf,", ",1);
	strncat((char *)smsbuf,ptmp+7,4);
    gps_gate = 0;
    GetAvailableGPSInfoV_N(3);
//ljw20160910换掉，一次获取的偶然性较大	if(GetAvailableGPSInfoV())     // 获取一组可用的位置信息
    if(dingwei_flag == 0x5a)
	{
        gps_gate = 1;
//ljw200160908,不好用        get_gps_mesg_ascii((u8 *)(smsbuf));
#if 1//ljw,20160908 for space 用get_gps_mesg_ascii((u8 *)(smsbuf));
		strncat((char *)smsbuf,"*JD",3);//JD34.36988N    经度34.36988N 北纬
#if 1
        strncat((char *)smsbuf,gps_ascii.Lon,10); //ljw,20160907 取经度
        strncat((char *)smsbuf,(u8 *)(&gps_ascii.uLon),1);
        strncat((char *)smsbuf,"*WD",3);//WD117.36988N    经度34.36988N 北纬
        strncat((char *)smsbuf,gps_ascii.Lat,9); //ljw,20160907 取经度3
        strncat((char *)smsbuf,(u8 *)(&gps_ascii.uLat),1);//ljw,20160907 取经度标志
        strncat((char *)smsbuf,"*SD",3);//SD000          当前速度
//		i = (unsigned char *)strstr((char *)(i+1),",");
//		i++;
        i = gps_ascii.Spd;
		p1= (unsigned char *)strstr((char *)(i+1),".");
		//转化成公里
		u16tmp = asciitospeed(i,p1-i);
		u16tmp /= 10;
		t1 = hex2BCD_ASCII(tmp_sn,u16tmp);
		strncat((char *)smsbuf,tmp_sn,t1);

#endif
#endif //ljw,20160908 for space 用get_gps_mesg_ascii((u8 *)(smsbuf));
#if 0//ljw,20160907 for space
		//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
		// 拷贝时间
		i = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
//ljw,20160902修复以前的错误的数据的解析		i++;
		//memcpy((char *)(tmp_time+6),(char *)(i+1),6);	// 拷贝时间
		i +=6;
		i = (unsigned char *)strstr((char *)i,",");


		// 拷贝GPS信息可用标志
		i = (unsigned char *)strstr((char *)(i+1),",");
		// 拷贝 纬度，
		p1= (unsigned char *)strstr((char *)(i+1),",");
		i++;
		strncat((char *)smsbuf,i,p1-i);

		i = (unsigned char *)strstr((char *)(i+1),",");
		i++;
		strncat((char *)smsbuf,i,1);
		strncat((char *)smsbuf,"*WD",3);//JD34.36988N    经度34.36988N 北纬
		i = (unsigned char *)strstr((char *)(i+1),",");
		i++;
		// 拷贝 经度，
		p1= (unsigned char *)strstr((char *)(i+1),",");
		strncat((char *)smsbuf,i,p1-i);

		i = (unsigned char *)strstr((char *)(i+1),",");
		i++;
		strncat((char *)smsbuf,i,1);
		strncat((char *)smsbuf,"*SD",3);//SD000          当前速度
		i = (unsigned char *)strstr((char *)(i+1),",");
		i++;	
		p1= (unsigned char *)strstr((char *)(i+1),".");
		//转化成公里
		u16tmp = asciitospeed(i,p1-i);
		u16tmp /= 10;
		t1 = hex2BCD_ASCII(tmp_sn,u16tmp);
		strncat((char *)smsbuf,tmp_sn,t1);
#endif
	}
    gps_gate = 1;
	strncat((char *)smsbuf,"*DH",3);//DH1            点火ACC状态，1表示点火，0表示熄火
//wei	t1='0'+get_ACC_CHECK_status();
	if(g_ucACCFlag)   //wei,20160524,ACC修改
		t1='1';          //wei,20160524,ACC修改
	else t1='0';		//wei,20160524,ACC修改
	strncat((char *)smsbuf,&t1,1);
	
	strncat((char *)smsbuf,"*YL",3);//YL0            油路状态，0表示正常，1表示断开
	t1='0'+system_para.g_break;
	strncat((char *)smsbuf,&t1,1);
	strncat((char *)smsbuf,"*ER0000>",strlen("*ER0000>"));//ER0000         出错内容，4个字符，备用。
	
	SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));	
	delayms(3000);
}
//<CSKMG>
void cmd_CSKMG(void)
{
	ddi_uart_Write(&UART1_ctl_t,"AT+CMGD=1,4\r\n",13);
	WaitGSMOK1();//ljw,20160903for space  WaitGSMOK();
	delayms(1000);
	set_GPS_POWERON (FALSE);
	SendMsg(g_ucCurrentNum,"Now restart system!!!\r\n",strlen("Now restart system!!!\r\n"));
//wei,	delayms(1000);
//wei20160608<
    delayms_nodog(8000);//wei20160608，延时8s 等待短信发送完成，这里不会有看门狗

    GPIO_SetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);
    delayms_nodog(800);//拉低GSM PWRKEY脚800ms
	GPIO_ResetBits(GSM_POWERKEY_PORT,GSM_POWERKEY_PIN);//wei,释放POWERKEY脚
//wei,20160803for space,只有超低功耗模式，不再需要GMS模块休眠了，而是直接断电	DTR_PIN_MODE_Contrl(DISABLE); //WEI,20160530关闭串口
	ddi_uart1_init(DISABLE); //WEI,20160530关闭串口

	delayms_nodog(2000);//释放GSM PWRKEY脚，让其恢复高电平
	GPIO_ResetBits(GSM_POWERON_PORT,GSM_POWERON_PIN);//

//wei20160608>
//ljw,20160908提高效    while(1); //等看门狗复位系统
   for(;;);
}
//******************************************************************
//
//  初始化设备 FHT initial *psw
//
//******************************************************************

void Cmd_Initial(void)   
{
        unsigned char *ptr;
        ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,"init");
	
	ptr += 4;
	if (0 != strncmp((char *)(ptr+6),"\r\n",2))
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
		return;
	}	
	if(strncmp((char *)ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}

	RestoreSystem();
	Store_System_Para();
	sys_Init();
#if 0 //下面的是恢复生产状态	
	EraseDevice();
	g_ucSysInitialized = SYS_UNINITI;
#endif	
	SendMsg(g_ucCurrentNum,"Initial OK!",11);
}

//fhtmid123456 18800088888 123654  // Y09964 多了5个字节
void Cmd_Setmid(void)
{
	unsigned char *temp;
	unsigned char mid_temp[6];
	unsigned char mid_back[6];
	const char superPSW[6]={'1','2','3','6','5','4'};
	unsigned char tmp_sn[12];
	unsigned char i;
	unsigned char *t1;
	
	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"mid");

	temp += 3;
	
	if (0 != strncmp((char *)(temp+20+5),"\r\n",2))
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
		return;
	}
	if(!strncmp((char *)(system_para.g_ucPassWord),(char *)(temp),6)) 		// 匹配 EEPROM 中的密码和新密码
	{
	  	//
	  	// 匹配成功 通知用户
	  	//
			if(!strncmp((char *)superPSW,(char *)(temp+14+5),6)) 		// 匹配 EEPROM 中的密码和新密码
			{

				memcpy(mid_back,system_para.g_ucDeviceID,6);
				t1 = temp+7;
				//判断号码是否有效
				for(i =0; i<11; i++)
				  {
					if ((*(t1+i)>'9') ||(*(t1+i)<'0'))
					{
						SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));

						return;
					}
						
				  }
				if (i < 11)
				{
						SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));

					  return;
				}  

				memcpy ((char *)(&tmp_sn[1]),t1,11);
				tmp_sn [0] = '0';
				for (i=0;i<6;i++)
				{
					asciitohex((u8*)(&tmp_sn[2*i]),(u8*)(&(system_para.g_ucDeviceID[i])),2);
				}
				for (i=0;i<6;i++)
				{
					hex2BCD_ASCII1((u8*)(&tmp_sn[i]),system_para.g_ucDeviceID[i]);
				}
				memcpy(system_para.g_ucDeviceID,tmp_sn,6);
				Store_System_Para();
				sys_Init();
				//if(strncmp((char *)(system_para.g_ucDeviceID),(char *)mid_back,6)) //一样返回0
				{
					SendMsg(g_ucCurrentNum,"Set mid ok!",strlen("set mid ok!"));
				}
				//else
				{
				//	Store_System_Para();
				//	sys_Init();
				//	if(strncmp((char *)(system_para.g_ucDeviceID),(char *)mid_back,6))
					{
				//		SendMsg(g_ucCurrentNum,"Set mid ok!",strlen("set mid ok!"));
					}
				//	else
					{
				//		SendMsg(g_ucCurrentNum,"Set mid error!",strlen("set mid error!"));
					}
				}
				
				
	  		}
			else
			{
				SendMsg(g_ucCurrentNum,"The super Password error!",strlen("The super Password error!"));
			}
	}
	else
	{
	  	//
	  	// 匹配失败 通知用户
	  	//
	  	SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	}


}

//******************************************************************
//
//  添加授权号码 FHT newnum *psw *num
//fhtnewnum123456@13900001111@13812341234
//******************************************************************

void Cmd_NewNum(void)   
{
	unsigned char *temp;
	unsigned char i,j;
	unsigned char *ptr;
    u8 *ptemp1 = NULL;//ljw,20160926for wulian
    u8 *ptemp2 = NULL;//ljw,20160926for wulian
    u8 temp_len = 0;//ljw,20160926for wulian
    u8 temp_num_flag = 0;//ljw,20160926for wulian 有几个号码
	ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,"newnum");
	ptr += 6;
    if(strncmp((char *)ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}
    ptemp1 = (unsigned char *)strstr(ptr,"@");
    ptemp2 = (unsigned char *)strstr(ptemp1+1,"@");
    temp_len = ptemp2-ptemp1-1;
    
    if((temp_len<1)||(temp_len >= PHNUM_LEN-3)||(ptemp2 == NULL))//ljw,20160926只有一个号码
    {
        ptemp2 = (unsigned char *)strstr(ptemp1+1,"\r\n");
        temp_len = ptemp2 - ptemp1-1;
        temp_num_flag = 0x01;
    }
    else   //ljw,20160926有两个号码
    {
        temp_num_flag = 0x02;
    }
    if((temp_len<1)||(temp_len >= PHNUM_LEN-3))////LJW,20160928不要做字符判断||(!isdigtit_len(ptemp1+1,temp_len)))//ljw,20160926,判断长度
//第一个号码的长度可以为0    if((temp_len >= PHNUM_LEN-3))
    {
        SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));        
        return;
    }
    temp = ptemp1+1;
    memset((u8 *)(&system_para.g_ucRegNum[0][0]),0x00,PHNUM_LEN);
    if(all_ascii0(temp,temp_len))//ljw,20160924 号码1
    {
        system_para.g_ucRegNum[0][0]='e';
    }
    else
    {
        for(j=0 ; j<temp_len ; j++)
        {
            system_para.g_ucRegNum[0][2+j] = *(temp+j);
        }
        system_para.g_ucRegNum[0][1] = temp_len;//ljw,20160926,存储号码长度
        system_para.g_ucRegNum[0][0] = FULL;   //
    }
    
    if(temp_num_flag != 0x02)
    {        
        Store_System_Para();
        sys_Init();
        SendMsg(g_ucCurrentNum,"The newnum 1 ok!",strlen("The newnum 1 ok!"));
        return;
    }
    
//fhtnewnum123456@13900001111@13812341234
    //ljw,20160926,提取号码2
    ptr = (unsigned char *)strstr(ptemp1+1,"\r\n");
    temp_len = ptr-ptemp2-1;
    if((temp_len<1)||(temp_len>=(PHNUM_LEN-3))) //LJW,20160928不要做字符判断||(!isdigtit_len(ptemp2+1,temp_len)))//判断号码2的长度是否合法
    {
//ljw        SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
        SendMsg_Parameter_error();
        return;
    }
    temp = ptemp2+1;
    memset((u8 *)(&system_para.g_ucRegNum[1][0]),0x00,PHNUM_LEN);
    if(all_ascii0(temp,temp_len))//ljw,20160924 号码1
    {        
        system_para.g_ucRegNum[1][0]='e';
    }
    else
    {
        for(j=0; j<temp_len; j++)
        {
            system_para.g_ucRegNum[1][2+j] = *(temp+j);            
        }
        system_para.g_ucRegNum[1][1] = temp_len;//ljw,20160926,存储号码长度
        system_para.g_ucRegNum[1][0] = FULL;   // 
    }
    Store_System_Para();
    sys_Init();
    SendMsg(g_ucCurrentNum,"The newnum 1@2 ok!",strlen("The newnum 1@2 ok!"));
    
#if 0//ljw,20160926for wulian<
	if(strlen(ptr) > 30 )
	{
		if (0 != strncmp((char *)(ptr+30),"\r\n",2))
		{
			SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
	}
	else
	{
		if (0 != strncmp((char *)(ptr+18),"\r\n",2))
		{
			SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
	}
	if(strncmp((char *)ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}
//fhtnewnum123456@13900001111@13812341234
	temp = ptr + 7;  		// 指向要添加的电话号码
	if ((*(temp)>'9') ||(*(temp)<'0'))
	   temp += 3;
   //判断号码是否有效
    for(i =0; i<11; i++)
    {
        if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
        {
            SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
            return;
        }
    }
	for(j=0 ; j<11 ; j++)
	{
		system_para.g_ucRegNum[0][4+j] = *(temp+j);
	}
	system_para.g_ucRegNum[0][0] = FULL;   // 
    
    if(0 ==strncmp((const char *)(&system_para.g_ucRegNum[0][4]),"00000000000",11))//wei,20160801 for all 0 then to delete
        strncpy((char *)(system_para.g_ucRegNum[0]),(char *)("e   00000000000"),15);
	Store_System_Para();
	sys_Init();

	temp +=12;
   //判断号码是否有效
    for(i =0; i<11; i++)
    {
        if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
        {
            SendMsg(g_ucCurrentNum,"The newnum 1 ok!",strlen("The newnum 1 ok!"));
            return;
        }
    }
 // 从短信命令中拷贝授权号码到授权号码缓冲区(包括号码类型和结束标记)
	for(j=0 ; j<11 ; j++)
	{
		system_para.g_ucRegNum[1][4+j] = *(temp+j);
	}
	system_para.g_ucRegNum[1][0] = FULL;   // 
	//EI();
    if(0 ==strncmp((const char *)(&system_para.g_ucRegNum[1][4]),"00000000000",11))//wei,20160801 for all 0 then to delete
        strncpy((char *)(system_para.g_ucRegNum[1]),(char *)("e   00000000000"),15);
	Store_System_Para();
	sys_Init();
	if(!strncmp((char *)(system_para.g_ucRegNum[1]+4),(char *)temp,11))
	{
		REGNUMbeSET= 1;
	  	SendMsg(g_ucCurrentNum,"The newnum 1@2 ok!",strlen("The newnum 1@2 ok!"));
	}
	else
	{
	  	SendMsg(g_ucCurrentNum,"The  Number registed Failed!",strlen("The  Number registed Failed!"));
	}
#endif //ljw,20160926for wulian<	
}

#if 0 //ljw,20160925 for wulian
void Cmd_NewNum(void)   
{
	unsigned char *temp;
	unsigned char i,j;
	unsigned char *ptr;

	ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,"newnum");
	
	ptr += 6;
	if (strlen(ptr) > 30 )
	{
		if (0 != strncmp((char *)(ptr+30),"\r\n",2))
		{
			SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
	}
	else
	{
		if (0 != strncmp((char *)(ptr+18),"\r\n",2))
		{
			SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
	}
	if(strncmp((char *)ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}
	
	temp = ptr + 7;  		// 指向要添加的电话号码
	if ((*(temp)>'9') ||(*(temp)<'0'))
	   temp += 3;
   //判断号码是否有效
      for(i =0; i<11; i++)
      {
		if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
		{
			SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
      }
	for(j=0 ; j<11 ; j++)
	{
		system_para.g_ucRegNum[0][4+j] = *(temp+j);
	}
	system_para.g_ucRegNum[0][0] = FULL;   // 
    
    if(0 ==strncmp((const char *)(&system_para.g_ucRegNum[0][4]),"00000000000",11))//wei,20160801 for all 0 then to delete
        strncpy((char *)(system_para.g_ucRegNum[0]),(char *)("e   00000000000"),15);
	Store_System_Para();
	sys_Init();

	temp +=12;
   //判断号码是否有效
    for(i =0; i<11; i++)
    {
        if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
        {
            SendMsg(g_ucCurrentNum,"The newnum 1 ok!",strlen("The newnum 1 ok!"));
            return;
        }
    }
 // 从短信命令中拷贝授权号码到授权号码缓冲区(包括号码类型和结束标记)
	for(j=0 ; j<11 ; j++)
	{
		system_para.g_ucRegNum[1][4+j] = *(temp+j);
	}
	system_para.g_ucRegNum[1][0] = FULL;   // 
	//EI();
    if(0 ==strncmp((const char *)(&system_para.g_ucRegNum[1][4]),"00000000000",11))//wei,20160801 for all 0 then to delete
        strncpy((char *)(system_para.g_ucRegNum[1]),(char *)("e   00000000000"),15);
	Store_System_Para();
	sys_Init();
	if(!strncmp((char *)(system_para.g_ucRegNum[1]+4),(char *)temp,11))
	{
		REGNUMbeSET= 1;
	  	SendMsg(g_ucCurrentNum,"The newnum 1@2 ok!",strlen("The newnum 1@2 ok!"));
	}
	else
	{
	  	SendMsg(g_ucCurrentNum,"The  Number registed Failed!",strlen("The  Number registed Failed!"));
	}	
}
#endif //ljw,20160925 for wulian
//******************************************************************
//
//  删除授权号码 FHT delnum *psw *num 或者 FHT delnum *psw num1/2/3/4/5
//
//******************************************************************
#if  0//wei,20160801 for space
void Cmd_DelNum(void)   
{
#if  0
	unsigned char i;
	unsigned char *temp;
	const unsigned char bufhead[21]= "The Register Number";
	const unsigned char bufend[27]= " was Deleted Successfully!";
	const unsigned char bufend1[21]= " was Deleted Failed!";	
	unsigned char tmpbuf[47];

	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"delnum");

	temp += 6;  // 指向密码


	if(strncmp((char *)temp,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}
	
	temp += 7; 	// 指向号码

	//
	// 根据位置删除
	//
	if(*temp == 'n' && *(temp+1) == 'u' && *(temp+2) == 'm')
	{

	         if ((*(temp+3)>0) && (*(temp+3)<6))
	         {
         		i = *(temp+3) - 1;
			strncpy((char *)g_ucRegNum[i],"e   00000000000#",16);       // 删除内存中授权号码
			EI();
			IIC_MWriteData(g_ucRegNumAddr[i],g_ucRegNum[i],16);  		// 删除EEPROM中的授权号码
				
			IIC_MReadData(g_ucRegNumAddr[i],g_ucRegNum[i],16);  		// 判断号码是否删除并短信通知用户
			memcpy(tmpbuf,bufhead,20);
			tmpbuf[20] = i+30;

			if(g_ucRegNum[i][0] == EMPTY)
			{

				memcpy(&(tmpbuf[20]),bufend,26);
				
			  	SendMsg(g_ucCurrentNum,tmpbuf,47);
			}
			else
			{

				memcpy(&(tmpbuf[20]),bufend1,20);
			  	SendMsg(g_ucCurrentNum,tmpbuf,41);
			}
		}	
	}
	//
	// 根据号码删除
	//
	else
	{
		for(i=0 ; i<5 ; i++)  
		{
			if(!strncmp((char *)g_ucRegNum[i]+4,(char *)temp,11))  // 在授权号码中找到要删除的号码
			{
				strncpy((char *)g_ucRegNum[i],"e   00000000000#",16);        // 删除内存中授权号码
				//EI();
				IIC_MWriteData(g_ucRegNumAddr[i],g_ucRegNum[i],16);  // 删除EEPROM中的授权号码
					
				IIC_MReadData(g_ucRegNumAddr[i],g_ucRegNum[i],16);  // 删除EEPROM中的授权号码
				if(g_ucRegNum[i][0] == EMPTY)
				{
					SendMsg(g_ucCurrentNum,"The Number has been delete Successful!",strlen("The Number has been delete Successful!"));
				}
				else
				{
					SendMsg(g_ucCurrentNum,"The Number has been delete Fail!",strlen("The Number has been delete Fail!"));
				}
			}
		}
			
	}

	//判断是否是最后一个
	REGNUMbeSET = 0;
	for(i =0;i<5; i++)
	{
	
		if(g_ucRegNum[i][0] == 'f')
		{
			REGNUMbeSET = 1;
		}	

	}
#endif	
}
#endif  //wei,20160801 for space
//******************************************************************
//
//  删除授权号码 FHT delnum *psw *num 或者 FHT delnum *psw num1/2/3/4/5
//
//******************************************************************
#if 0  //wei,20160801 for space
void Cmd_DelAll(void)   
{
	unsigned char i;
	unsigned char *temp;

	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"delall");

	temp += 6;  // 指向密码

	if (0 != strncmp((char *)(temp+6),"\r\n",2))
	{
		SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
		return;
	}
	
	if(strncmp((char *)temp,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	} 
	for(i=0; i<2; i++)
	{
	  	strncpy((char *)(system_para.g_ucRegNum[i]),(char *)("e   00000000000"),15);	
	}
	REGNUMbeSET = 0;
	for(i=0; i<2; i++)
	{
		Store_System_Para();
		sys_Init();
		if(system_para.g_ucRegNum[i][0] != EMPTY)
		{
			REGNUMbeSET = 1;
			SendMsg(g_ucCurrentNum,"The  Number has been delete Fail!",strlen("The  Number has been delete Fail!"));
			return;
		}
	}
	
	SendMsg(g_ucCurrentNum,"The Number has been delete Successful!",strlen("The Number has been delete Successful!"));

	for(i =0;i<5; i++)
	{	
		if(system_para.g_ucRegNum[i][0] == 'f')
		{
			REGNUMbeSET = 1;
		}
	}
}
#endif  //wei,20160801 for space

//******************************************************************
//
//  修改设备密码 FHT setpsw *oldpsw *newpsw *newpsw
//
//******************************************************************

void Cmd_SetPsw(void)   
{
	unsigned char *temp;
	unsigned char *pt1;
	unsigned char i;
	
	unsigned char psw_temp[6];
	unsigned char psw_old[6];

	memcpy(psw_old,(system_para.g_ucPassWord),6);
	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"setpsw");
	if (0 != strncmp((char *)(temp+19),"\r\n",2))
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
		return;
	}
	if(!strncmp((char *)(system_para.g_ucPassWord),(char *)(temp+6),6))  // 旧密码匹配而且两次新密码输入相同 
	{
	      //判断密码是否有效
	      pt1 = temp+13;
	      for(i =0; i<6; i++)
	      {
			if ((*(pt1+i)>'9') ||(*(pt1+i)<'0'))
			{
				SendMsg(g_ucCurrentNum,"The new password error!",strlen("The new password error!"));
				return;
			}
				
	      }
	
		strncpy((char *)(system_para.g_ucPassWord),(char *)(temp+13),6);  // 拷贝密码到内存
		//EI();
		Store_System_Para();

	}
	else
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
		return;
	}
	
	sys_Init();			// 读取当前密码
	
	if(!strncmp((char *)(system_para.g_ucPassWord),(char *)(temp+13),6)) 		// 匹配 EEPROM 中的密码和新密码
	{
	  	//
	  	// 匹配成功 通知用户
	  	//
	  	SendMsg(g_ucCurrentNum,"Password setup ok!",
                        strlen("Password setup ok!"));
	}
	else
	{
		memcpy((system_para.g_ucPassWord),psw_old,6);
	  	//
	  	// 匹配失败 通知用户
	  	//
	  	SendMsg(g_ucCurrentNum,"Password setup Fail!",
                        strlen("Password setup Fail!"));
	}
}

//******************************************************************
//
//  设置GPRS主动连穿时间 FHT setuptime *psw *time
//
//******************************************************************
#if 0//wei,20160803
void Cmd_SetUpTime(void)
{
#if 0
	unsigned char *temp;
	unsigned char i;
	unsigned short time;

	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"setuptime");
	
	temp += 10;
	
	if(strncmp((char *)temp,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));	
	  	return;
	} 
	
	i = (unsigned char *)strstr((char *)(temp),"\r\n") - temp - 7;
	
	if(i == 2)
	{
	  	time = (*(temp+7) - '0') * 10 + *(temp+8) - '0';
	}
	else if(i == 3)
	{
	  	time = (*(temp+7) - '0') * 100 + (*(temp+8) - '0') * 10 + *(temp+9) - '0';
	}
	
	g_usGPRSUpLoadTime = time;
	IIC_MWriteData(EEP_GPRS_UPTIME,(unsigned char *)(&g_usGPRSUpLoadTime),2);
	
	IIC_MReadData(EEP_GPRS_UPTIME,(unsigned char *)(&g_usGPRSUpLoadTime),2);
	
	if((time == g_usGPRSUpLoadTime) && (g_usGPRSUpLoadTime > 30) && (g_usGPRSUpLoadTime < 300))
	{
	  	SendMsg(g_ucCurrentNum,"The New GPRS UpLoad Time Setup Successful!",strlen("The New GPRS UpLoad Time Setup Successful!"));
	}
	else
	{
	  	g_usGPRSUpLoadTime = 40;
		IIC_MWriteData(EEP_GPRS_UPTIME,(unsigned char *)(&g_usGPRSUpLoadTime),2);
	  	SendMsg(g_ucCurrentNum,"The New GPRS UpLoad Time Setup Fail!Use 40s by default.",strlen("The New GPRS UpLoad Time Setup Fail!Use 40s by default."));
	}
#endif
}
#endif //wei,20160803
//******************************************************************
//
//  设置网络参数　包括IP地址和UDP端口　APN　用户名　网络密码 
//
//******************************************************************

void Cmd_SetNET(void)
{
	unsigned char *temp;
      unsigned char *temp1;
	unsigned char i;
    u8 temp_len = 0;//wei,20160708
//wei,20160708	unsigned char smsbuf[100];
    unsigned char smsbuf[130];
	unsigned char* pbegin;
	unsigned char* pend;
    unsigned char temp_w=0;//wei,20160628
	
	temp = (unsigned char *)strstr((char *)g_ucSim900Buff,"net");
	temp += 3;  // 指向 密码
	if(strncmp((char *)temp,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}
	memset(smsbuf,0,sizeof(smsbuf));
	temp += 7;  // 指向 ip
	if (*temp == '?') //查询
	{
		i =0;
		strncat((char *)smsbuf,"ip: ",strlen("ip: "));
		strncat((char *)smsbuf,system_para.g_ucIPAddress,strlen(system_para.g_ucIPAddress));//g_ucIPAddress
		strncat((char *)smsbuf,"@",1);
		strncat((char *)smsbuf,"port: ",strlen("port: "));
		strncat((char *)smsbuf,system_para.g_ucUDPPort,strlen(system_para.g_ucUDPPort));//g_ucIPAddress
		strncat((char *)smsbuf,"@",1);
		strncat((char *)smsbuf,"apn: ",strlen("apn: "));
		strncat((char *)smsbuf,system_para.g_ucNetAPN,strlen(system_para.g_ucNetAPN));//g_ucIPAddress
		strncat((char *)smsbuf,"@",1);
		strncat((char *)smsbuf,"username: ",strlen("username: "));
		strncat((char *)smsbuf,system_para.g_ucNetUser,strlen(system_para.g_ucNetUser));//g_ucIPAddress
		strncat((char *)smsbuf,"@",1);
		strncat((char *)smsbuf,"password: ",strlen("password: "));
		strncat((char *)smsbuf,system_para.g_ucNetPsw,strlen(system_para.g_ucNetPsw));//g_ucIPAddress
		strncat((char *)smsbuf," ok!",strlen(" ok!"));
		SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
	}
	else
	{
		strncat((char *)smsbuf,"set ",strlen("set "));
		pbegin= temp;
		for(i=0 ; i<((unsigned char *)strstr((char *)temp,"@")-temp) ; i++)  //找下一个空格了
		{
			if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
			{
				if (*(temp+i) != '.')
				{
					SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));
				  	return;


				}
			}
		}
		if (FHTIP != system_para.g_ucSysMode_S)
		{
			for(i=0 ; i<16 ; i++)
			{
				system_para.g_ucIPAddress[i] = 0;
			}
		       
			for(i=0 ; i<((unsigned char *)strstr((char *)temp,"@")-temp) ; i++)  //找下一个空格了
			{
				system_para.g_ucIPAddress[i] = *(temp+i);
//wei				if (i>=16)
				 if (i>=IPADD_LEN-2)  //wei if (i>=14)
					break;
			}
		}

		temp = (unsigned char *)strstr((char *)temp,"@") + 1;  // 指向 端口
		for(i=0 ; i<((unsigned char *)strstr((char *)temp,"@")-temp) ; i++)  //找下一个空格了
		{
			if ((*(temp+i)>'9') ||(*(temp+i)<'0'))
			{

				SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));
			  	return;

			}
		}
		if (FHTIP != system_para.g_ucSysMode_S)
		{
			for(i=0 ; i< UDPPORT_LEN; i++)
			{
			   system_para.g_ucUDPPort[i] = 0;
			}
			for(i=0 ; i<((unsigned char *)strstr((char *)temp,"@")-temp) ; i++)
			{
			   system_para.g_ucUDPPort[i] = *(temp+i);
//wei			   if (i>=5)
			   if (i>= UDPPORT_LEN-2)
				break;
			}
		}
//wei		for(i=0 ; i<20 ; i++)
        for(i=0 ; i<NETAPN_LEN ; i++)
		{
		   system_para.g_ucNetAPN[i] = 0;
		}
		temp = (unsigned char *)strstr((char *)temp,"@") + 1;  // 指向 APN
        temp_len = (u8)((unsigned char *)strstr((char *)temp,"@")-temp);
		for(i=0; i<temp_len; i++)
		{
            if(i >= (NETAPN_LEN-1))//wei,20160708
				break;
			system_para.g_ucNetAPN[i] = *(temp+i);
//wei			if (i>=20)            
		}

		for(i=0 ; i<NETUSER_LEN ; i++)
		{
	          system_para.g_ucNetUser[i] = 0;
		}
		temp = (unsigned char *)strstr((char *)temp,"@") + 1;  // 指向 网络用户名
		for(i=0 ; i<((unsigned char *)strstr((char *)temp,"@")-temp) ; i++)
		{
			system_para.g_ucNetUser[i] = *(temp+i);
//wei			if (i>=6)
			if (i>=NETUSER_LEN-2)//wei
				break;
		}

		for(i=0 ; i<NETPSW_LEN ; i++)
		{
	          system_para.g_ucNetPsw[i] = 0;
		}
		temp = (unsigned char *)strstr((char *)temp,"@") + 1;  // 指向 网络密码
	        temp1 = temp;
		for(i=0 ; i<((unsigned char *)strstr((char *)temp,"\r")-temp) ; i++)
		{
			system_para.g_ucNetPsw[i] = *(temp1+i);
//wei			if (i>=8)
			if (i>=NETPSW_LEN-2) //wei
				break;			
		}
		pend =temp + i;
		
		Store_System_Para();
		sys_Init();
		
		if(NULL != strstr((char *)g_ucSim900Buff,(char *)system_para.g_ucIPAddress) &&(FHTIP != system_para.g_ucSysMode_S))  //&& (NULL != strstr((char *)g_ucSim900Buff,(char *)g_ucUDPPort)))
		{

			strncat((char *)smsbuf,pbegin,(pend-pbegin));
			
			strncat((char *)smsbuf," ok!",strlen(" ok!"));
		  	//SendMsg(g_ucCurrentNum,"Set net ok!",strlen("Set net ok!"));
		  	SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
//wei,            ddi_uart_Write(&UART1_ctl_t,"AT+SHENGQIAN\r\n",strlen("AT+SHENGQIAN\r\n"));
			delayms(4000);
			
			#if 0
			IWDG_ReloadCounter();
			for(time=0; time<10 ; time++)    // 20151017: ip 切换不成功，重启才可以
			{
				delayms(500);
				ddi_uart_Write(&UART1_ctl_t,"AT+QICLOSE\r\n",strlen("AT+QICLOSE\r\n"));
				if (0 == WaitGSMOK1())
					break;
			}
			#else
//wei,20160617			GSM_reset();
			#endif
			
//wei,20160617			StartGPRS();	
             if(SMS_StartGSM_Flag == 0x5a)//wei,20160628
                 return;
//wei,20160617			for(i=0; i<5;i++)
               for(i=1; i<8;i++)
		       {
//                    if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))//满足休眠条件，要休眠
//                    {
//                        if((g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE)) //wei,20160709,满足睡眠条件，不要在继续拨号了
//                            return ;
//                    }
                   if(0x05 == check_sleep_state())//ljw,20160908判断休眠条返回 0x05,休眠  0x0a不休眠
                      return;
                   if((i%2==0)||(CPIN_READY_flag == 0)) //wei,20160617
                          GSM_reset();//wei,20160617
                   temp_w = GetGPRSStatus();
			       if (temp_w != GPRS_CONNECTED)
                   {
                       if(temp_w == GRPS_IP_PORT_err) //IP或端口错误
                           GSM_reset();
                       delayms(500); 
                       dial_status =0;//wei20160621
                       StartGPRS();	
                   }
                   else
                      break;
                   if(dial_status==1)//wei,20160621 if Dial-up success ,break
                    break;
		       }
			if (i>7)
            {
                GSM_Power(FALSE); //wei,20160608看门狗复位前，给GSM断电
//ljw,20160908提高效率                  while(1); //等看门狗复位系统
               for(;;);
            }
		}
		else
		{
		  	SendMsg(g_ucCurrentNum,"Set net fail!",strlen("Set net fail!"));
		}
	}	
	
}



//******************************************************************
//
//  查询所有授权号码 FHT shownum *psw
//
//******************************************************************
void Cmd_ShowRegNum(void)
{
	u8 i;
	unsigned char temp[100];
	unsigned char *ptr;
	ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,"shownum");
	ptr += 7;
	if (0 != strncmp((char *)(ptr+6),"\r\n",2))
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
		return;
	}
	if(strncmp((char *)ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));	
	  	return;
	}
	for( i = 0 ; i < 100 ; ++i)
	{
		temp[i] = 0;
	}
	strcat((char *)temp,"Num.1:");
	if((system_para.g_ucRegNum[0][0] == 'e')||(system_para.g_ucRegNum[0][1] != ((u8)strlen((u8 *)&system_para.g_ucRegNum[0][2]))))
	{
		strcat((char *)temp,"Empty;");
	}
	else
	{
//ljw,20160926for wulian		strncat((char *)temp,(char *)(system_para.g_ucRegNum[0]+1),11);
        strncat((char *)temp,(char *)(system_para.g_ucRegNum[0]+2),system_para.g_ucRegNum[0][1]);
		strcat((char *)temp,";");
	}
	strcat((char *)temp,"Num.2:");
	if((system_para.g_ucRegNum[1][0] == 'e')||(system_para.g_ucRegNum[1][1] != ((u8)strlen((u8 *)&system_para.g_ucRegNum[1][2]))))
	{
		strcat((char *)temp,"Empty;");
	}
	else
	{
//ljw,20160926for wulian		strncat((char *)temp,(char *)(system_para.g_ucRegNum[1]+4),11);
        strncat((char *)temp,(char *)(system_para.g_ucRegNum[1]+2),system_para.g_ucRegNum[1][1]);
		strcat((char *)temp,";");
	}
	SendMsg(g_ucCurrentNum,temp,strlen((char *)temp));
}

#if 0 //ljw,20160926 for wulian
void Cmd_ShowRegNum(void)
{
	u8 i;
	unsigned char temp[100];
	unsigned char *ptr;
	
	ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,"shownum");	
	ptr += 7;
	if (0 != strncmp((char *)(ptr+6),"\r\n",2))
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
		return;
	}
	if(strncmp((char *)ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));	
	  	return;
	} 
	for( i = 0 ; i < 100 ; ++i)
	{
		temp[i] = 0;
	}
	strcat((char *)temp,"Num.1:");
	if(system_para.g_ucRegNum[0][0] == 'e')
	{
		strcat((char *)temp,"Empty;");
	}
	else
	{
		strncat((char *)temp,(char *)(system_para.g_ucRegNum[0]+4),11);
		strcat((char *)temp,";");
	}
	strcat((char *)temp,"Num.2:");
	if(system_para.g_ucRegNum[1][0] == 'e')
	{
		strcat((char *)temp,"Empty;");
	}
	else
	{
		strncat((char *)temp,(char *)(system_para.g_ucRegNum[1]+4),11);
		strcat((char *)temp,";");
	}
	SendMsg(g_ucCurrentNum,temp,strlen((char *)temp));
}
#endif //ljw,20160926 for wulian

//******************************************************************
//
//  设置GPRS主动上传间隔
//
//******************************************************************

void Cmd_SetWakeTime(void)
{
	unsigned char *uc_ptr;
	unsigned short  WakeTime;
	//unsigned char tmp[40];
	unsigned char smsbuf[30];
	unsigned char i;
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"period");

	uc_ptr += 6;  // 使 uc_ptr 指向密码



	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	} 

	uc_ptr += 7;  // 使 uc_ptr 指向时间
	memset(smsbuf,0,30);
	strncat((char*)smsbuf,"period ",strlen("period "));
	if ('?' == *uc_ptr)
	{
		if (0 != strncmp((char *)(uc_ptr+1),"\r\n",2))
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}

	 
		WakeTime = (u16)system_para.g_usGPRSUpLoadAternationTime;

		smsbuf[7] = (unsigned char)((WakeTime - WakeTime % 100) / 100) + '0';
		smsbuf[8] = (unsigned char)((WakeTime % 100 - WakeTime % 10) / 10) + '0';
		smsbuf[9] = WakeTime % 10 + '0';



		strncat((char *)(smsbuf+10)," ok!",strlen(" ok!"));
		SendMsg(g_ucCurrentNum,(unsigned char *)smsbuf,strlen(smsbuf));
	}
	else
	{
		if (0 != strncmp((char *)(uc_ptr+3),"\r\n",2))
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
	   //判断号码是否有效
	      for(i =0; i<3; i++)
	      {
			if ((*(uc_ptr+i)>'9') ||(*(uc_ptr+i)<'0'))
			{
				SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
				return;
			}
				
	      }
		WakeTime = (*uc_ptr - '0') * 100 + (*(uc_ptr+1) - '0') * 10 ;
		
		if(*(uc_ptr+2) >= '0' && *(uc_ptr+2) <= '9')
		{
		  	WakeTime = WakeTime + *(uc_ptr+2) - '0';
		}
		else
		{
		  	WakeTime = WakeTime / 10;
		}
		if ((WakeTime < 2) || (WakeTime > 300))
		{
			SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		strncat((char*)(smsbuf),uc_ptr,3);
		strncat((char*)(smsbuf)," ok!",strlen(" ok!"));

		system_para.g_usGPRSUpLoadAternationTime = WakeTime ;
		Store_System_Para();
		sys_Init();
		if(system_para.g_usGPRSUpLoadAternationTime == WakeTime)
		{
		  	//SendMsg(g_ucCurrentNum,"set period ok!",strlen("set period ok!"));
		  	SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
		}
		else
		{
			system_para.g_usGPRSUpLoadAternationTime  = UPLOADTIME; //默认值
		}
		TimeUpLdCnt = system_para.g_usGPRSUpLoadAternationTime - 1;  // 时间裕量，防止设置的时候出错
	}	
}


//******************************************************************
//
//  设置 设备工作模式 FHT gprsmode *psw *mode
//  
//  参数 psw 为设备密码 mode 为设备工作模式 wake/package/time
//
//******************************************************************

void Cmd_SetMode(void)
{
	unsigned char *uc_ptr;
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"mode");

	uc_ptr += 4;  // 使 uc_ptr 指向密码


	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6))
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));	
		return;  // 密码匹配失败 返回
	}


	if ('?' == *(uc_ptr+7))
	{
		if (0 != strncmp((char *)(uc_ptr+8),"\r\n",2))
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
		switch(system_para.g_ucPowerMode)
		{
			case EXTRA_LOW_POWER_MODE:
				 SendMsg(g_ucCurrentNum,"Mode elowpower!",strlen("mode elowpower!"));
				break;

#if 0//wei,20160803 for space<
			case LOW_POWER_MODE:
				 SendMsg(g_ucCurrentNum,"Mode  lowpower ok!",strlen("mode  lowpower ok!"));
				break;
#endif //wei,20160803 for space>
			case ALLONLINE_MODE:
				 SendMsg(g_ucCurrentNum,"Mode  allonline ok!",strlen("mode  allonline ok!"));
				break;

			default:
				SendMsg(g_ucCurrentNum,"Mode  error!",strlen("mode  error!"));
				break;
		}
                return; //不要把标志写到eeprom中了
	}
	else
	{
		 if (strstr((char *)g_ucSim900Buff,(char *)"elowpower"))
		{
			if (0 != strncmp((char *)(uc_ptr+16),"\r\n",2))
			{
				SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}
			system_para.g_ucPowerMode = EXTRA_LOW_POWER_MODE;
			 SendMsg(g_ucCurrentNum,"Mode elowpower ok!",strlen("mode elowpower ok!")); 
		}
#if 0//wei,20160803 for space<        
		else
		if (strstr((char *)g_ucSim900Buff,(char *)"lowpower"))
		{
			if (0 != strncmp((char *)(uc_ptr+15),"\r\n",2))
			{
				SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}

			system_para.g_ucPowerMode = LOW_POWER_MODE;
			SendMsg(g_ucCurrentNum,"Mode lowpower ok!",strlen("mode lowpower ok!"));
		}
#endif //wei,20160803 for space>
		else
		if (strstr((char *)g_ucSim900Buff,(char *)"allonline"))
		{
			if (0 != strncmp((char *)(uc_ptr+16),"\r\n",2))
			{
				SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}

			system_para.g_ucPowerMode = ALLONLINE_MODE;
			SendMsg(g_ucCurrentNum,"Mode allonline ok!",strlen("mode allonline ok!"));
            		Store_System_Para();	
			sys_Init();					
		}
		else
		{
			SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));			
		}	
		if((system_para.g_ucPowerMode < ALLONLINE_MODE) && (system_para.g_ucPowerMode > 0))
		{
				Store_System_Para();
				sys_Init();					

		        delayms(5000);	
				g_ucGotoSleepFlag  = 0x5a;
		}
	}
}

//******************************************************************
//
//  查询设备 IMEI  FHT imei *psw
//
//******************************************************************
void Cmd_IMEI(void)
{
  	unsigned char *uc_ptr;
	unsigned char smsbuf[30];
	memset (smsbuf,0,30);
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"imei");
	
	uc_ptr += 4;
	if( (0 != strncmp((char *)(uc_ptr+7),"\r\n",2)) || ('?' != *(uc_ptr+6)))
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
		return;
	}
	
	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{

		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));	
		return;
	} 


	//EI();
	ddi_uart_Write(&UART1_ctl_t,"AT+GSN\r",7);
	WaitGSMOK1();//ljw,20160903for space  WaitGSMOK();
	//DI();
       strncpy((char *)smsbuf,"imei ",strlen("imei "));	
 	// 串口中断方式接收短信内容
	
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,"\r\n");
	
	uc_ptr += 2; //\r\n86**********
	memcpy((char *)(smsbuf+5),uc_ptr,15);	
	if(SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf)))
	{
		if(SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf)))
		{
			SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
		}
	}
}



//******************************************************************
//
//  查询设备信息 FHT devinfo *psw
//
//******************************************************************
void Cmd_DeviceInfo(void)
{
  	unsigned char *uc_ptr;
	unsigned char BattaryInfo;
	unsigned char temp[3];
	u8 tmp_sn[12];
    u8 num_len = strlen(g_ucCurrentNum);//ljw,20160924for wulian
    if(num_len >= PHNUM_LEN-3)//ljw,20160924for wulian
        return ;
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"devinfo");
	
	uc_ptr += 8;
	
	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));		
	  	return;
	} 
	
	BattaryInfo = BatteryStatusGet();
	
	//TM00_Stop();
	ddi_uart_Write(&UART1_ctl_t,"AT+CMGS=\"+86",12);
//ljw,20160924for wulian	ddi_uart_Write(&UART1_ctl_t,g_ucCurrentNum,11);
    ddi_uart_Write(&UART1_ctl_t,g_ucCurrentNum,num_len);//ljw,20160924for wulian
	ddi_uart_Write(&UART1_ctl_t,"\"\r",2);
	delayms(300);
	
	
	//
	// 输出设备ID
	//
	ddi_uart_Write(&UART1_ctl_t,"DeviceID:",9);
//	ddi_uart_Write(&UART1_ctl_t,system_para.g_ucDeviceID,6);
//	
	hextoascii(system_para.g_ucDeviceID,tmp_sn,6);
	ddi_uart_Write(&UART1_ctl_t,&tmp_sn[1],11);
	ddi_uart_Write(&UART1_ctl_t,"\r\n",2);
	
	//
	// 输出IP地址
	//
	ddi_uart_Write(&UART1_ctl_t,"IP:",3);
	ddi_uart_Write(&UART1_ctl_t,system_para.g_ucIPAddress,strlen((char *)system_para.g_ucIPAddress));
	ddi_uart_Write(&UART1_ctl_t,"\r\n",2);
	
	//
	// 输出UDP端口
	//
	ddi_uart_Write(&UART1_ctl_t,"Port:",5);
	ddi_uart_Write(&UART1_ctl_t,system_para.g_ucUDPPort,strlen((char *)system_para.g_ucUDPPort));
	ddi_uart_Write(&UART1_ctl_t,"\r\n",2);
	
	//
	// 输出GPRS上传时间
	//
	ddi_uart_Write(&UART1_ctl_t,"UploadTime:",11);
	if(system_para.g_usGPRSUpLoadTime > 99)
	{
	  	temp[0] = system_para.g_usGPRSUpLoadTime / 100 + '0';
		temp[1] = ((system_para.g_usGPRSUpLoadTime - system_para.g_usGPRSUpLoadTime % 10) % 100) + '0';
		temp[2] = system_para.g_usGPRSUpLoadTime % 10 + '0';
	  	ddi_uart_Write(&UART1_ctl_t,temp,3);
	}
	else
	{
	  	temp[0] = (system_para.g_usGPRSUpLoadTime / 10) + '0';
		temp[1] = system_para.g_usGPRSUpLoadTime % 10 + '0';
		ddi_uart_Write(&UART1_ctl_t,temp,2);
	}
	ddi_uart_Write(&UART1_ctl_t,"s\r\n",3);
	
	//
	// 输出GPRS叫醒工作时间
	//
	ddi_uart_Write(&UART1_ctl_t,"WakeTime:",9);
	if(system_para.g_usGPRSUpLoadTime > 99)
	{
	  	temp[0] = system_para.g_usGPRSUpLoadTime / 100 + '0';
		temp[1] = ((system_para.g_usGPRSUpLoadTime - system_para.g_usGPRSUpLoadTime % 10) % 100) / 10 + '0';
		temp[2] = system_para.g_usGPRSUpLoadTime % 10 + '0';
	  	ddi_uart_Write(&UART1_ctl_t,temp,3);
	}
	else
	{	
	  	temp[0] = system_para.g_usGPRSUpLoadTime / 10 + '0';
		temp[1] = system_para.g_usGPRSUpLoadTime % 10 + '0';
	  	ddi_uart_Write(&UART1_ctl_t,temp,2);
	}
	ddi_uart_Write(&UART1_ctl_t,"m\r\n",3);
	
	//
	// 输出电池电量
	//
	ddi_uart_Write(&UART1_ctl_t,"Battery:",8);
	if(BattaryInfo > 9)
	{
	  	temp[0] = (BattaryInfo / 10) + '0';
		temp[1] = '.';
		temp[2] = BattaryInfo % 10 + '0';
		ddi_uart_Write(&UART1_ctl_t,temp,2);
	}
	else
	{
	  	temp[0] = BattaryInfo;
	  	ddi_uart_Write(&UART1_ctl_t,temp,1);
	}
	ddi_uart_Write(&UART1_ctl_t,"V\r\n",3);
	

	//
	// 发送
	//
	ddi_uart_Write(&UART1_ctl_t,"\x1A",1);
    WaitGSMOK2(500);//wei,20160726 for send SMS
	//TM00_Start();
}


//******************************************************************
//
//  设置设备超速报警 FHT speedlimit *psw *speed
//
//******************************************************************
void Cmd_SpeedLimit(void)
{
  	unsigned char *uc_ptr;
	unsigned char Speed = 0;
	unsigned char tmp,i;
	unsigned short tmp1; 
	unsigned char smsbuf[80];
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"speedlimit");
	uc_ptr += 10;

	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6))
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
		return;  // 密码匹配失败 返回
		}
	uc_ptr += 7;
	memset(smsbuf,0,30);
	strncpy((unsigned char*)smsbuf,"Speedlimit ",strlen("speedlimit "));	
	if ('?' != *uc_ptr)
	{
		if (0 != strncmp((char *)(uc_ptr+3),"\r\n",2))
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}

		for(i=0; i<3; i++)
		{
			if ((*(uc_ptr+i) <'0') ||(*(uc_ptr+i) >'9'))//参数错误
			{
				SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));	
				return;
			}	
		}
		strncpy((unsigned char*)(smsbuf+11),uc_ptr,3);	
		
		Speed += (*uc_ptr - '0')*100;
		Speed += (*(uc_ptr+1) - '0') * 10;
		Speed += (*(uc_ptr+2) - '0') ;
		tmp1 = Speed;//(unsigned char)(Speed*100/185);
		if (tmp1 > 250)//参数错误
		{
			SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));	
			return;
		}	
		
		tmp	= system_para.g_ucMaxSpeed;
		system_para.g_ucMaxSpeed = (unsigned char)tmp1;
		Store_System_Para();
		sys_Init();

		if (system_para.g_ucMaxSpeed != Speed)
		{
				//g_ucGPRSDataBuff[15] = '0'; //ALL OK
				system_para.g_ucMaxSpeed = tmp;
			  SendMsg(g_ucCurrentNum,"Speedlimit error!",strlen("Speedlimit error"));			
		
		}
		else
		{
			//g_ucGPRSDataBuff[15] = '1'; //ALL OK
			g_ucTaskList |= TASK_SPEED_LIMIT;

			strcat((unsigned char*)smsbuf," ok!");
			SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
		}
		if (system_para.g_ucMaxSpeed == 0)
		{
			g_ucTaskList &= ~TASK_SPEED_LIMIT;

		}
	}
	else
	{
		if (0 != strncmp((char *)(uc_ptr+1),"\r\n",2))
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
		//IIC_MReadData(EEP_MAX_SPEED,(unsigned char *)(&system_para.g_ucMaxSpeed),1);
		strncpy((char *)smsbuf,"Speedlimit ",strlen("Speedlimit "));	
		     tmp = system_para.g_ucMaxSpeed;  //*185/100;
	        if(tmp/100)
	        {  
	          Speed = tmp/100+'0';

	          smsbuf[11] = Speed;
	          Speed = (tmp % 100)/10 + '0';
	          smsbuf[12] = Speed;
	          Speed = tmp%10 + '0';
	          smsbuf[13] = Speed;
		 }
	        else
	         if (tmp/10)
	         {
	          Speed = tmp/10 +'0';
	          smsbuf[11]= Speed;
	          Speed = tmp%10 +'0';
	          smsbuf[12] = Speed;
	         
	         }
	        else
	        {
	          Speed = tmp + '0';
	          smsbuf[11] = Speed;
	        }
	          
	        strcat((char *)smsbuf,"!");
	  	SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));


	}


}


//******************************************************************
//
//取消设备超速报警 FHT speedlimit *psw *speed
//
//******************************************************************
//void Cmd_NoLimit(void)
//{
//#if 0
//	//unsigned char Speed = 0;
//	
//	//uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"nolimit");
//	
//  	g_ucTaskList &= ~TASK_SPEED_LIMIT;
//	g_ucMaxSpeed = 0;

//	IIC_MWriteData(EEP_MAX_SPEED,(unsigned char *)(&g_ucMaxSpeed),1);
//	
//	SendMsg(g_ucCurrentNum,"Nolimit ok!",strlen("Nolimit ok!"));
//#endif	
//}

//******************************************************************
//
//查看设备超速报警 FHT speedlimit *psw *speed
//
//******************************************************************
#if 0 //wei,20160801 for space<
void Cmd_ReadSpeedLimit(void)
{
#if 0
  	unsigned char *uc_ptr;
	unsigned char Speed = 0;
	unsigned char smsbuf[30];
	unsigned char tmpspeed;

	memset(smsbuf,0,30);
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"readspeedlimit");
	
	uc_ptr += 14;
	
	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	} 
	IIC_MReadData(EEP_MAX_SPEED,(unsigned char *)(&g_ucMaxSpeed),1);
	strncpy((char *)smsbuf,"Speedlimit ",strlen("Speedlimit "));	
	tmpspeed = g_ucMaxSpeed*185/100;
        if(tmpspeed/100)
        {  
          Speed = tmpspeed/100+'0';

          smsbuf[11] = Speed;
          Speed = (tmpspeed % 100)/10 + '0';
          smsbuf[12] = Speed;

          
          Speed = tmpspeed%10 + '0';
          
          smsbuf[13] = Speed;
	}
        else
         if (tmpspeed/10)
         {
          Speed = tmpspeed/10 +'0';
          smsbuf[11]= Speed;
          Speed = tmpspeed%10 +'0';

          smsbuf[12] = Speed;
         
         }
        else
        {
          Speed = tmpspeed + '0';

          smsbuf[11] = Speed;
        }          
        strcat((char *)smsbuf,"!");
  	SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
#endif
}
#endif //wei,20160801 for space>
//******************************************************************
//
//  查询小区ID FHT cellid *psw
//
//******************************************************************
void Cmd_CellID(void)
{
  	unsigned char *uc_ptr;
	u8 num_len = strlen(g_ucCurrentNum);//ljw,20160924for wulian
    if(num_len >= PHNUM_LEN-3)//ljw,20160924for wulian
        return ;
    
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"cellid");
	
	uc_ptr += 6;
	if( (0 != strncmp((char *)(uc_ptr+7),"\r\n",2)) || ('?' != *(uc_ptr+6)))
	{
		SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
		return;
	}

	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}
	
	ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r",9);
	WaitGSMOK1();	//ljw,20160903for space  WaitGSMOK();
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,"\"");
	uc_ptr += 1;	
	
	ddi_uart_Write(&UART1_ctl_t,"AT+CMGS=\"+86",12);
//ljw,20160924for wulian	ddi_uart_Write(&UART1_ctl_t,g_ucCurrentNum,11);
    ddi_uart_Write(&UART1_ctl_t,g_ucCurrentNum,num_len);
	ddi_uart_Write(&UART1_ctl_t,"\"\r",2);
	delayms(300);
	
	ddi_uart_Write(&UART1_ctl_t,"CellID ",7);
	ddi_uart_Write(&UART1_ctl_t,uc_ptr,4);
	ddi_uart_Write(&UART1_ctl_t,", ",1);
	ddi_uart_Write(&UART1_ctl_t,uc_ptr+7,4);	
	
	ddi_uart_Write(&UART1_ctl_t,"\x1A",1);
//wei,20160726	WaitGSMOK1();
    WaitGSMOK2(500);//wei,20160726 for send SMS
}

//******************************************************************
//
//  使能ACC
//
//******************************************************************
void Cmd_ACC(void)
{
	unsigned char *uc_ptr;
	unsigned char tmp;
	
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"acc");
	
	uc_ptr += 3;
	
	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}
	uc_ptr += 7;
	if('?' == *uc_ptr)
	{
		if( (0 != strncmp((char *)(uc_ptr+1),"\r\n",2)))
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
		if (system_para.g_bACCon)
	   	{
	   		SendMsg(g_ucCurrentNum,"ACC on ok!",strlen("acc on ok!"));
	   	}
		else
		{
			SendMsg(g_ucCurrentNum,"ACC off ok!",strlen("acc off ok!"));
		
		}
	}
	else
	{

		if (strstr((char *)g_ucSim900Buff,(char *)"on"))	
		{
			if( (0 != strncmp((char *)(uc_ptr+2),"\r\n",2)))
			{
				SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}
			system_para.g_bACCon = 1;
			tmp = 1;
			SendMsg(g_ucCurrentNum,"ACC on ok!",strlen("acc on ok!"));
		}	
		if (strstr((char *)g_ucSim900Buff,(char *)"off"))
		{

			if( (0 != strncmp((char *)(uc_ptr+3),"\r\n",2)))
			{
				SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}		
			system_para.g_bACCon = 0;
			tmp = 0;
			SendMsg(g_ucCurrentNum,"ACC off ok!",strlen("acc off ok!"));
		}

	     Store_System_Para();
	     sys_Init();		
	}
}

//******************************************************************
//
// 打开短信自动发送功能
//
//******************************************************************
void Cmd_ENAUTOSMS(void)
{
	unsigned char *uc_ptr;
        unsigned char tmp;
	
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"autosms");
	uc_ptr += 7;
	
	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}

	uc_ptr += 7;
	if('?' == *uc_ptr)
	{
		if( (0 != strncmp((char *)(uc_ptr+1),"\r\n",2)))
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
		 if (system_para.g_bAUTOSMSon)
	   	{
	   		SendMsg(g_ucCurrentNum,"Autosms on ok!",strlen("autosms on ok!"));
	   	}
		else
		{
			SendMsg(g_ucCurrentNum,"Autosms off ok!",strlen("autosms off ok!"));
		
		}
	}
	else
	{
		if (strstr((char *)g_ucSim900Buff,(char *)"on"))	
		{
			if( (0 != strncmp((char *)(uc_ptr+2),"\r\n",2)))
			{
				SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}		
			system_para.g_bAUTOSMSon = TRUE;
			tmp = 1;
			SendMsg(g_ucCurrentNum,"Autosms on ok!",strlen("autosms on ok!"));
		}
		if (strstr((char *)g_ucSim900Buff,(char *)"off"))
		{
			if( (0 != strncmp((char *)(uc_ptr+3),"\r\n",2)))
			{
				SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}
			system_para.g_bAUTOSMSon = FALSE;
			tmp = 0;
			SendMsg(g_ucCurrentNum,"Autosms off ok!",strlen("autosms off ok!"));
		}
	      Store_System_Para();
	}
}

//******************************************************************
//
//  设置持续工作时间  ； 单位分钟
//
//******************************************************************
void Cmd_Duration(void)
{
	unsigned char *uc_ptr;
	volatile unsigned short WakeTime=0;
	//unsigned char WakeTime1;
	//unsigned char tmp[40];
	unsigned char smsbuf[30];
	unsigned char i;
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"duration");
	uc_ptr += 8;  // 使 uc_ptr 指向密码

	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));	
	  	return;
	} 
	uc_ptr += 7;  // 使 uc_ptr 指向时间
	memset(smsbuf,0,30);
	strncpy((unsigned char*)smsbuf,"Duration ",strlen("duration "));
	if ('?' == *uc_ptr)
	{
		if( 0 != strncmp((char *)(uc_ptr+1),"\r\n",2)) 
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}

                WakeTime1 = system_para.g_usGPRSUpLoadTime;
                if(99 < WakeTime1)
	        {  

		          smsbuf[9] = WakeTime1/100+'0';
		          smsbuf[10] = (WakeTime1 % 100)/10 + '0';
		          smsbuf[11] = WakeTime1%10 +'0';
		  }
	        else
	         if (9 < WakeTime1)
	         {
			   smsbuf[9]= '0';
			   smsbuf[10] = WakeTime1/10 +'0';
		         smsbuf[11] = WakeTime1%10 +'0';
	         
	         }
	        else
	        {
	        	   smsbuf[9]= '0';
		          smsbuf[10]= '0';		  
		          smsbuf[11] = WakeTime1 + '0';
	        } 
		strcat(smsbuf," ok!");
		
		SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
	}
	else
	{
		if( 0 != strncmp((char *)(uc_ptr+3),"\r\n",2)) 
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}
		for(i=0; i<3; i++)
		{
			if ((*(uc_ptr+i) <'0') ||(*(uc_ptr+i) >'9'))//参数错误
			{
				SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));	
				return;
			}	
		}
	       
		WakeTime = (*uc_ptr - '0') * 100 + (*(uc_ptr+1) - '0') * 10 ;
		
		if(*(uc_ptr+2) >= '0' && *(uc_ptr+2) <= '9')
		{
		  	WakeTime = WakeTime + *(uc_ptr+2) - '0';
		}
		else
		{
		  	WakeTime = WakeTime / 10; // 防止只有两位数
		}
		if ((WakeTime < 1) ||(WakeTime > 255))
		{
			SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		system_para.g_usGPRSUpLoadTime = (unsigned char)WakeTime;
		Store_System_Para();
		sys_Init();
		
       	if(system_para.g_usGPRSUpLoadTime == WakeTime)
		{

			memset(smsbuf,0,30);
			strncat((char*)smsbuf,"Duration ",strlen("duration "));
			strncat((char*)(smsbuf+9),uc_ptr,3);
			strncat((char*)(smsbuf+12)," ok!" ,4);
			
		  	SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
		}

	}	

}



//******************************************************************
//
//设置休眠时间
//
//******************************************************************
void Cmd_SleepTime(void)
{
	unsigned char *uc_ptr;
	volatile unsigned short WakeTime=0;
	unsigned char smsbuf[30];
	unsigned char i;

	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"sleeptime");
	uc_ptr += 9;  // 使 uc_ptr 指向密码
	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));	
	  	return;
	} 
	uc_ptr += 7;  // 使 uc_ptr 指向时间
	if ('?' == *uc_ptr)
	{
		if( 0 != strncmp((char *)(uc_ptr+1),"\r\n",2)) 
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}	
		memset(smsbuf,0,30);
		strncpy((char*)smsbuf,"Sleeptime ",strlen("sleeptime "));

              WakeTime1 = system_para.g_usSleepTime;
              //  if(99 < WakeTime1)
	        {  

		          smsbuf[10] = WakeTime1/100+'0';
		          smsbuf[11] = (WakeTime1 % 100)/10 + '0';
 		          smsbuf[12] = WakeTime1%10 +'0';
		  }
		strcat(smsbuf," ok!");
		SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
	}
	else
	{
		if( 0 != strncmp((char *)(uc_ptr+3),"\r\n",2)) 
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}	
		for(i=0; i<3; i++)
		{
			if ((*(uc_ptr+i) <'0') ||(*(uc_ptr+i) >'9'))//参数错误
			{
				SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error!"));	
				return;
			}	
		}

		WakeTime = (*uc_ptr - '0') * 100 + (*(uc_ptr+1) - '0') * 10 ;
		
		if(*(uc_ptr+2) >= '0' && *(uc_ptr+2) <= '9')
		{
		  	WakeTime = WakeTime + *(uc_ptr+2) - '0';
		}
		else
		{
		  	WakeTime = WakeTime / 10;
		}
		if( (WakeTime <1) || (WakeTime >255 ))
		{
			SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));
			return;
		}
		system_para.g_usSleepTime = (unsigned char)WakeTime;

		Store_System_Para();
		sys_Init();		
		if(system_para.g_usSleepTime == WakeTime)
		{
			memset(smsbuf,0,30);
			strncat((char*)smsbuf,"Sleeptime ",strlen("sleeptime "));
			strncat((char*)(smsbuf+10),uc_ptr,3);
			strncat((char*)(smsbuf+13)," ok!" ,4);
			
		  	SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
		}
            WakeTime1 = 0;
	}	

}

//******************************************************************
//
//  设置振动
//
//******************************************************************
void Cmd_shocks(void)
{
	unsigned char *uc_ptr;
	volatile unsigned short WakeTime=0;
	//unsigned char WakeTime1;
	//unsigned char tmp[40];
	unsigned char smsbuf[30];
	u8 tmp_sn[11];
	unsigned char i;
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"shocks");
	uc_ptr += 6;  // 使 uc_ptr 指向密码

	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));	
	  	return;
	} 
	uc_ptr += 7;  // 使 uc_ptr 指向时间
	memset(smsbuf,0,30);
	strncpy((unsigned char*)smsbuf,"Shocks ",strlen("Shocks "));
	if ('?' == *uc_ptr)
	{
		if( 0 != strncmp((char *)(uc_ptr+1),"\r\n",2)) 
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}


	}
	else
	{
		//震动灵敏度，4字节，01-79震动加速度，01-38震动时间
		//temp = (unsigned char *)strstr((char *)temp,"*") + 1; 
		WakeTime = (*uc_ptr - '0');
		
		if ((WakeTime<=3) && (WakeTime >  0))
		{
			system_para.acceleration = acceleration_para[WakeTime-1];
			system_para.impact_time = impact_time_para[WakeTime-1];
			set_Impact(system_para.acceleration,system_para.impact_time);
			Store_System_Para();
			sys_Init();
			
		}
		else
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}

	}
	#if 0
	hex2BCD_ASCII(tmp_sn,system_para.acceleration);
	//strncat((char *)smsbuf,tmp_sn,2);
	if (system_para.acceleration>9)
	{
		strncat((char *)smsbuf,tmp_sn,2);
	}
	else
	{
		strncat((char *)smsbuf,"0",1);
		strncat((char *)smsbuf,tmp_sn,1);
	}
	hex2BCD_ASCII(tmp_sn,system_para.impact_time);
	if (system_para.impact_time>9)
	{
		strncat((char *)smsbuf,tmp_sn,2);
	}
	else
	{
		strncat((char *)smsbuf,"0",1);
		strncat((char *)smsbuf,tmp_sn,1);
	}
	#else
	for(i=0; i<3; i++)
	{
		if (system_para.acceleration == acceleration_para[i])
			break;
	}
	if(i <3) //正常的三挡
	{
		tmp_sn[0] = i+'0'+1; 
		strncat((char *)smsbuf,tmp_sn,1);	

	}
	else //设置值不在规定的三个里面
	{
        system_para.acceleration = acceleration_para[2];
        system_para.impact_time = impact_time_para[2];       
        set_Impact(system_para.acceleration,system_para.impact_time);
        Store_System_Para();
        tmp_sn[0] = '3'; 
        strncat((char *)smsbuf,tmp_sn,1);	
	}
	#endif
	
	SendMsg(g_ucCurrentNum,smsbuf,strlen(smsbuf));
}

//******************************************************************
//
//  设置振动
//
//******************************************************************
void Cmd_relay(void)
{
	unsigned char *uc_ptr;
	unsigned char tmp;
	
	uc_ptr = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)"relay");
	
	uc_ptr += 5;
	
	if(strncmp((char *)uc_ptr,(char *)(system_para.g_ucPassWord),6)) // 检查密码
	{
		SendMsg_Password_error();//ljw,20160926for space    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
	  	return;
	}
	uc_ptr += 7;
	if('?' == *uc_ptr)
	{
		if( (0 != strncmp((char *)(uc_ptr+1),"\r\n",2)))
		{
			SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
			return;
		}

	}
	else
	{

		if (strstr((char *)g_ucSim900Buff,(char *)"on"))	
		{
			if( (0 != strncmp((char *)(uc_ptr+2),"\r\n",2)))
			{
				SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}
			system_para.g_break = 1;
			tmp = 1;
			//SendMsg(g_ucCurrentNum,"ACC on ok!",strlen("acc on ok!"));
		}	
		else
		if (strstr((char *)g_ucSim900Buff,(char *)"off"))
		{

			if( (0 != strncmp((char *)(uc_ptr+3),"\r\n",2)))
			{
				SendMsg_Parameter_error(); //ljw,20160926for space       SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
				return;
			}		
			system_para.g_break = 0;
			tmp = 0;
			//SendMsg(g_ucCurrentNum,"ACC off ok!",strlen("acc off ok!"));
		}
	     Store_System_Para();
	     sys_Init();		
	}
	if (system_para.g_break)
	{
		SendMsg(g_ucCurrentNum,"Relay on!",strlen("Relay on!"));
		set_OIL_BREAK(TRUE);
	}
	else
	{
		SendMsg(g_ucCurrentNum,"Relay off!",strlen("Relay off!"));
		set_OIL_BREAK(FALSE);
	}
}
/*
**-----------------------------------------------------------------------------
**
**	概述:
**		// 执行sms设置命令 

**
**	参数:
**		无
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
//ljw,20160925for wulian
void setSMS(void)
{
	u8 * pStart;
	u8 i;
	u8 j;
    u8 phnum_len;

#if 1	//短消息放在暂存器中
	for(i =0; i<smsdatareg.count;i++)
	{
		if(smsdatareg.count)
		{
			memset(g_ucSim900Buff,0,sizeof(g_ucSim900Buff));
//ljw,20160925for wulian			memcpy(g_ucCurrentNum,&(smsdatareg.mobilenum[i]),11);
            phnum_len = strlen((u8 *)&smsdatareg.mobilenum[i]);         //ljw,20160925for wulian
            if(phnum_len >= PHNUM_LEN-3)                     //ljw,20160925for wulian
                continue ;    //ljw,20160925for wulian
            memset(g_ucCurrentNum,0x00,PHNUM_LEN);//ljw,20160926用前清空
            memcpy(g_ucCurrentNum,&(smsdatareg.mobilenum[i]),phnum_len);//ljw,20160925for wulian
			memcpy(g_ucSim900Buff,smsdatareg._smscontent[i],130);				
			switch(smsdatareg._smscommand[i])
			{
				case position:
					Cmd_Position();
					break;
				case chinese_position:
					//Cmd_chinese_Position();
					chinesesmsflag = 0x5a;
					// 开始计时器，没有响应就十秒重发一条
					startchinesesms = 0;
					break;				
				case init:
					Cmd_Initial();
					break;					
				case newnum:
					Cmd_NewNum();
					break;					
				case delnum:
//wei,20160801 for space			  		Cmd_DelNum();
					break;						
				case delall:
//wei,20160801 for space					Cmd_DelAll();
					break;
				case shownum:
					Cmd_ShowRegNum();
					break;	
				case setpsw:
					Cmd_SetPsw();
					break;	
			      case net:
					Cmd_SetNET();
					break;	
				case setuptime:
//wei,20160801 for space					Cmd_SetUpTime();
					break;	
				case imei:
					Cmd_IMEI();
					break;	
				case devinfo:
					Cmd_DeviceInfo();
					break;	
				case readspeedlimit:
//wei,20160801 for space					Cmd_ReadSpeedLimit();
					break;	
				case speedlimit:
					Cmd_SpeedLimit();
					break;	
				case nolimit:
//wei,20160801 for space				Cmd_NoLimit();
					break;	
				case cellid:
					Cmd_CellID();
					break;	
				case mid:
					Cmd_Setmid();
					break;	
				case mode:
					Cmd_SetMode();
					break;	
				case period:
					Cmd_SetWakeTime();
					break;	
				case acc:
					Cmd_ACC();
					break;	
                case autosms:
					Cmd_ENAUTOSMS();
					break;	
				case duration:
					Cmd_Duration();
					break;	
				case sleeptime:
					Cmd_SleepTime();
					break;	
				case relay:
					Cmd_relay();
					break;		
				case shocks:
					Cmd_shocks();
					break;
				default:
					break;
			}
		}
	}
#endif		

		memset(&smsdatareg,0,sizeof(_smsdata));
		g_ucSMSCmdMissionFlag = 0;

}

#if 0//ljw,20160925
void setSMS(void)
{	
	u8 * pStart;
	u8 i;
	u8 j;

#if 1	//短消息放在暂存器中
	for(i =0; i<smsdatareg.count;i++)
	{
		if ( smsdatareg.count )
		{
			memset(g_ucSim900Buff,0,sizeof(g_ucSim900Buff));
			memcpy(g_ucCurrentNum,&(smsdatareg.mobilenum[i]),11);
			memcpy(g_ucSim900Buff,smsdatareg._smscontent[i],130);				
			switch (smsdatareg._smscommand[i])
			{
				case position:
					Cmd_Position();
					break;
				case chinese_position:
					//Cmd_chinese_Position();
					chinesesmsflag = 0x5a;
					// 开始计时器，没有响应就十秒重发一条
					startchinesesms = 0;
					break;				
				case init:
					Cmd_Initial();
					break;
					
				case newnum:
					Cmd_NewNum();
					break;
					
				case delnum:
//wei,20160801 for space			  		Cmd_DelNum();

					break;						
				case delall:
//wei,20160801 for space					Cmd_DelAll();
					break;
				case shownum:
					Cmd_ShowRegNum();
					break;	
				case setpsw:

					Cmd_SetPsw();

					break;	
			      case net:
					Cmd_SetNET();

					break;	
				case setuptime:
//wei,20160801 for space					Cmd_SetUpTime();

					break;	
				case imei:
					Cmd_IMEI();

					break;	
				case devinfo:
					Cmd_DeviceInfo();

					break;	
				case readspeedlimit:
//wei,20160801 for space					Cmd_ReadSpeedLimit();

					break;	
				case speedlimit:
					Cmd_SpeedLimit();

					break;	
				case nolimit:
//wei,20160801 for space				Cmd_NoLimit();

					break;	
				case cellid:
					Cmd_CellID();

					break;	
				case mid:
					Cmd_Setmid();

					break;	
				case mode:
					Cmd_SetMode();

					break;	
				case period:
					Cmd_SetWakeTime();

					break;	
				case acc:
					Cmd_ACC();

					break;	

					case autosms:
					Cmd_ENAUTOSMS();

					break;	
				case duration:
					Cmd_Duration();

					break;	
				case sleeptime:
					Cmd_SleepTime();

					break;	
				case relay:
					Cmd_relay();

					break;		
				case shocks:
					Cmd_shocks();

					break;		

				default:
					break;

			}
		}
	}
#endif		

		memset(&smsdatareg,0,sizeof(_smsdata));
		g_ucSMSCmdMissionFlag = 0;

}
#endif //ljw,20160925


