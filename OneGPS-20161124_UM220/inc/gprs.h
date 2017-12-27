#ifndef __gprs_h__
#define __gprs_h__
#include "stm32f0xx.h"
#include "stm32f0xx_rtc.h"
//���㲿��Э��ֻ��GPRSͨѶ,sms�Ļ��ǰ���ԭ����Э����
//�ϵ�Э��ֻ��: 
//   1.�����ϴ�����                               
//   2.�޸���������ʱ����
//   3.�ش���������ʱ����
//   4.���ζ�λ
//   5.���ó���ֵ
//   6.ȡ������ֵ
//   7.��ѯ��������
//   8.�޸�IP��ַ
//   9.��ѯ��ǰIP��ַ
//   10.���ö��͵�
//   11.��ѯ���͵�״̬

//����Э���е�:

// 1. �����ն˲��� ��ϢID 0X8103 						����ID 0x0001,�ж��������ͼ��
//																							0x0010, apn
//																							0x0011, gprs�����û���
//																							0x0012,gprs��������
//																							0x0013,��������IP��ַ
//																							0x0019,UDP�˿�
//																							0x0020.����Ϊ��ʱ�㱨;
//																							0x0027,����ʱ�㱨ʱ����
//																							0x0028, ��������ʱ���
//																							0x0029,ȱʡʱ��㱨���
//																							0x0055,����ٶ�
//  2  . ��ѯ�ն˲��� 0x8104
//  3. ��ѯָ���ն˲��� 0x8106
// 4  . �ն�Ӧ�� 0x0104


//5. �ն˿��� 0x8105 �����ն�����ָ��������
//						

//6. λ����Ϣ�㱨: 0X0200, 								����λ:  0 ��������
//                                               1 ���ٱ���
//                                               7 ����ԴǷѹ
//                                               8 ����Դ����
//                                               16 �񶯱���
//                                      ״̬λ: 0 ACC״̬
//                                             1 ��λ״̬
//                                               2 ��γ
//                                               3 ����
//                                             10 ��·״̬ 


//7. λ����Ϣ��ѯ: 0x8201
//8. λ����Ϣ�㱨: 0x0201   


//9.	��������֮����·-��· (ֻ�õ�·)
//��ϢID��0x8f01
//��Ϣ��4���ֽڣ�Ŀǰ��ʱ�õ�һ�ֽڣ��������ֽڹ���չ�ã��·�Ĭ����0x00����
//��λ��ʾ������λΪ1ʱ��Ч
//��һ�ֽڣ�
//λ0������·
//λ1���ָ���·
//λ2���ϵ�
//λ3���ָ���·
//�ڶ��ֽڣ�����
//�����ֽڣ�����
//�����ֽڣ�����
//�ն˻ظ���ϢID��0x0f01,��ʽͬ��ϢID 0x0500��



//���Ԥ��С��λ�ò�ѯ, ����λ���·�

//��ϢID 0x0f00 ��Ϣ�� Ҫ�� ���� (1�ֽ�,1 GPS ; 2 С��)��С���Ļ� :����С����Ŀ��(1�ֽ�)С�����,С��ID;
//                                                     ���gps,ֻ�����Ⱥ�γ��,1�ֽڵ�״̬�ֽ�(����,��γ)
//

//����Э������û�ж��͵��ָ��,ֻ�в鿴״̬��


//�������յ����ݺ�,�϶��᷵��.�ն˿��ԶԷ������·������ݲ���Ӧ��
//�������ظ���ϢID 0x8f00 �������UCS2���������λ��

typedef enum 
{
    SET_TERMINAL_PARAMETER = 0x8103,                 //�����жϲ���
    INQUIRE_TERMINAL_PARAMETER = 0x8104,          //��ѯ�жϲ���
    SET_GPRS_SERVICER   = 0x8105,                        //�ն˿���
    INQUIRE_ONE_TERMINAL_PARAMETER = 0x8106, //��ѯָ���ն˲���
    INQUIRE_POSITION = 0x8201,                            //λ����Ϣ��ѯ
    TEMPORARY_POSTION = 0x8202,                        //��ʱ����
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
//ljw,20160831��0x0f90    MOBILE_LOCATION_SEND = 0x0f89
    MOBILE_LOCATION_SEND = 0x0f90
}GPRS_SEND_ID;

typedef enum
{
	ALTERNATION_HEARTBEAT=0x0001,			//0x0001,�ж��������ͼ��
	APN = 0x0010,		//0x0010, apn
	GPRS_USERNAME = 0X0011,		//x0011, gprs�����û���
	GPRS_PASSWORD= 0X0012,		//0x0012,gprs��������
      IP_ADDR =0X0013, 			//0x0013,��������IP��ַ
	UDP_PORT = 0X0019, //0x0019,UDP�˿�
	//SET_AS_TIMING=0X0020,		//0x0020.����Ϊ��ʱ�㱨; 20150504:ֻ�ж�ʱ�ϴ����������Ķ���֮���
      ALTERNATION_IN_SLEEP = 0X0027, //0x0027,����ʱ�㱨ʱ����
	ALTERNATION_IN_ALARM= 0X0028,		//0x0028, ��������ʱ���
	ALTERNATION_DEFAULT_POSITION=0X0029,		//0x0029,ȱʡʱ��㱨���
	HIGH_SPEED=0X0055		//0x0055,����ٶ�
}TERMINAL_PARAMETER;



//��Ϣͷ
//��Ϣͷ���������2��
//��2 ��Ϣͷ����
//��ʼ�ֽ�         �ֶ�               ��������  ������Ҫ��
//     0                      ��ϢID               WORD
//     2                    ��Ϣ������    WORD              ��Ϣ�����Ը�ʽ�ṹͼ��ͼ2
//     4                    �ն��ֻ���    BCD[6]       ���ݰ�װ���ն�������ֻ���ת�����ֻ��Ų���12
//                                                                           λ������ǰ�������֣���½�ֻ��Ų�������0���۰�
//                                                                           ̨����������Ž���λ�����䡣
//     10                  ��Ϣ��ˮ��      WORD          ������˳���0 ��ʼѭ���ۼ�
//     12                  ��Ϣ����װ��                 �����Ϣ����������ر�ʶλȷ����Ϣ�ְ�����
//                                                                            ����������ݣ������޸���
typedef struct gprs_message_head
{
  u16 id;
  u16 attribute;
  u8   mbphonenum[6];
  u16 serial_number;
}gprs_message_head_struct;


//��Ϣ�����Ը�ʽ�ṹͼ��ͼ2 ��ʾ��
//15     14  |  13   |   12   11    10           |        9    8   7     6     5    4    3   2   1   0
//����  | �ְ�| ���ݼ��ܷ�ʽ |              ��Ϣ�峤��
//���ݼ��ܷ�ʽ��
//����bit10~bit12 Ϊ���ݼ��ܱ�ʶλ��
//����������λ��Ϊ0����ʾ��Ϣ�岻���ܣ�
//��������10 λΪ1����ʾ��Ϣ�徭��RSA �㷨���ܣ�
//��������������
#define  EXCURSION_MESSAGE_LENGTH    (1<<0)
#define  EXCURSION_MESSAGE_ENCRYPT  (1<<10)
#define  EXCURSION_MESSAGE_DISTRIBUTE (1<<13)


//6. λ����Ϣ�㱨: 0X0200, 								����λ:  0 ��������
//                                               1 ���ٱ���
//                                               7 ����ԴǷѹ
//                                               8 ����Դ����
//                                               16 �񶯱���
//                                      ״̬λ: 0 ACC״̬
//                                             1 ��λ״̬
//                                               2 ��γ
//                                               3 ����
//                                             10 ��·״̬ 


typedef struct position_message
{
  u32 alarm;
  u32 status;
  u32 latitude;  // γ��
  u32 longitude; //  ����
  u16 altitude;  //  ����
  u16 speed;  // �ٶ�
  u16  direction; //����
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
#define AREA_ALARM                 (1<<4)  //WEI,20160520����Χ��
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

// status : 0���ɹ�/ȷ�ϣ�1��ʧ�ܣ�2����Ϣ����3����֧��
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
extern  u8 time_gps_rtc[6];//wei,20160809,����GPS��ʱ�䣬��ΪRTCʱ��Ļ�׼  
extern  u8  RTC_Get(void);
extern  position_message_struct GPS_Deal(position_message_struct position);  //wei,20160820 GPS�����ӳ���
extern  position_message_struct Deal_GSM_LAC(position_message_struct position_in);//wei,20160822 GSM�����ӳ���
//ljw,20160906����Ҫɾextern u8 Deal_GSM_LAC(void);//ljw,20160906����Ҫɾ
extern  void RTC2Position_time(u8 *p); //wei,20160825,ȡϵͳ��RTCʱ��
extern  char *strstr_len(u8 *s1,u8 *s2,u16 len);
extern  u8 send_gps_gsm_flag ;//wei,20160822 0x5a,GSM  0xa5,GPS
extern void uploadLAC_InquireChinese(void) ;  //wei,20160918,for �ϴ�С���Ż�ȡ����λ�� 
    
extern void get_ID(void);//ljw,20160918,��ȡGSMС��ID
extern volatile u32 Lac_g ;//ljw,20160918ȫ�ֱ�����С��λ�ú�ID
extern volatile u32 LacId_g ;//ljw,20160918ȫ�ֱ�����С��λ�ú�ID
    
extern u8  ACC_GPS_flag ;
extern u8  ACC_GSM_flag ;
#endif //__gprs_h__
