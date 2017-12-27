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
//����:����GSM���ṩ�Ļ�վʱ�䣬ΪRTC��ʱ���׼
unsigned char Deal_GSMRTC(unsigned short waittime_OK);
extern void deal_g_ucSim900Status(void);
/*
**wei,20160628
**���ܣ��ر�֪��ͨ����TCP/UDP����
**������index,ָ����ͨ��0-
**���أ�0��ʧ��  1���ɹ�

*/
extern u8 GSM_QICLOSED(u8 index);
/*
**-----------------------------------------------------------------------------
**wei,20160628
**	����:
**		���ý���Ӣ�Ķ���Ϣ(GSM����)
**
**	����:
**		
**
**	����:
**		��
**-----------------------------------------------------------------------------
*/
extern void Configuration_TEXT_SMS(void);

extern u8 cops_flag ;//wei,cops_flag = 0x5a,�������Ӫ�̣�0��û�л����Ӫ��
extern u32 cops_flag_timer ;//wei,20160623,�ӻ����Ӫ�̿�ʼ��ʱ���������ն�������
extern u8 SMS_StartGSM_Flag ;//wei,20160628,������ʱ����GSM��־�����⴦������ź��ٵ���StartGPRS
extern u8 CPIN_READY_flag ;
//wei20160607
//�����ж�CGREG,
//����1������׼���ɹ�������0�����ɹ�
extern u8  WaitGSM_ReadyNet(void);
//wei20160608
//�����ж�CGREG,
//ʹ��ģ����ö�ͨ���������ݣ�����1�ɹ�������1ʧ��
//AT+QIMUX=1
extern u8  WaitGSM_ReadyQIMUX(void);
extern u8 get_gps_mesg_ascii(u8 *p); ////ljw,20160908,��ȡGPS����Ϣ ���ȣ�γ�ȣ��ٶ�

//ljw,20160909,����ǲ���0-9
extern u8 isdigtit_len(u8 *p,u8 len);
//ljw,�����Ӻ����������ַ��������ظ����֣����ٿռ�
extern u8 SendMsg_Parameter_error(void);
//ljw,�����Ӻ����������ַ��������ظ����֣����ٿռ�
extern u8 SendMsg_Password_error(void);

//ljw,20161128,������GSM�������ݣ�
extern u16 GC65_QIRD(void);
#endif /* __GSM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
