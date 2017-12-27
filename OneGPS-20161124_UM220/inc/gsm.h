 /* @author  JiaWei Li
  * @version V1.0.0
  * @date    26-March-2015
  * @brief   Header for gsm.c module
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
#ifndef __GSM_H
#define __GSM_H


#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "user_define.h"
#include "led.h"
#include <string.h>

#define QIACT_WAIT 500  // 1==20ms
#define QIOPEN_WAIT 500
extern void StartGPRStoAGPS(void);
extern void SendLocaSMS1(unsigned char *num);
extern u8 GetAvailableGPSInfoV_N(u8 count);
extern unsigned char WaitGSMOK1(void);
extern void WaitGSMOK(void);
extern unsigned char WaitGSMOK2(unsigned short waittime_OK);
extern u8 hex2BCD_ASCII1(unsigned char * decdata,unsigned char src);//wei,20160803 for warning
//wei,20160812<
//功能:处理GSM的提供的基站时间，为RTC做时间基准
unsigned char Deal_GSMRTC(unsigned short waittime_OK);
extern void deal_g_ucSim900Status(void);
/*
**wei,20160628
**功能：关闭知道通道的TCP/UDP连接
**参数：index,指定的通道0-
**返回：0，失败  1，成功

*/
extern u8 GSM_QICLOSED(u8 index);
/*
**-----------------------------------------------------------------------------
**wei,20160628
**	概述:
**		配置接收英文短消息(GSM编码)
**
**	参数:
**		
**
**	返回:
**		无
**-----------------------------------------------------------------------------
*/
extern void Configuration_TEXT_SMS(void);

extern u8 cops_flag ;//wei,cops_flag = 0x5a,获得了运营商，0，没有获得运营商
extern u32 cops_flag_timer ;//wei,20160623,从获得运营商开始计时，用作接收短信依据
extern u8 SMS_StartGSM_Flag ;//wei,20160628,启动的时候处理GSM标志，避免处理完短信后再调用StartGPRS
extern u8 CPIN_READY_flag ;
//wei20160607
//用作判断CGREG,
//返回1，拨号准备成功，返回0，不成功
extern u8  WaitGSM_ReadyNet(void);
//wei20160608
//用作判断CGREG,
//使能模块采用多通道传输数据，返回1成功，返回1失败
//AT+QIMUX=1
extern u8  WaitGSM_ReadyQIMUX(void);
extern u8 get_gps_mesg_ascii(u8 *p); ////ljw,20160908,提取GPS的信息 经度，纬度，速度

//ljw,20160909,检测是不是0-9
extern u8 isdigtit_len(u8 *p,u8 len);
//ljw,做成子函数，减少字符串常量重复出现，减少空间
extern u8 SendMsg_Parameter_error(void);
//ljw,做成子函数，减少字符串常量重复出现，减少空间
extern u8 SendMsg_Password_error(void);

//ljw,20161128,主动读GSM缓存数据，
extern u16 GC65_QIRD(void);
#endif /* __GSM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
