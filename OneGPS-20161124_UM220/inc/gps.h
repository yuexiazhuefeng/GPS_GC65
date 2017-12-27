#ifndef __gps_h__
#define __gps_h__
#include "stm32f0xx.h"

//extern unsigned char GetAvailableGPSInfo(void);
typedef  __packed struct //wei20160603,λ������
{
	u8 UTCtime[6+1];
    u8 status;
    u8 Lat[9+1];
    u8 uLat;
    u8 Lon[10+1];
    u8 uLon;
    u8 Spd[6+1];//ljw,20160904,�ٶ�ʹ��xxx.xxЭ��û�и����̶�����
    u8 Cog[6+1];//ljw,20160904,�Ե��溽�򣬵�λ�Ƕȣ�ʹ��xxx.xxЭ��Ҳû�и����̶��ĳ���
    u8 Date[6+1];//ljw,20160904ddmmyy
}GPS_ASCII;
extern GPS_ASCII gps_ascii;
extern unsigned char GetAvailableGPSInfoV(void);
extern void GPS_Reset(void);
extern unsigned char CheckSum(unsigned char *databuf,unsigned char buflen);
extern void reset_gps();
extern u8 Get_GPS_module(void); //ljw,20161201, ��ȡGPSģ����Ϣ

//LJW,20161130 GPS ģ���ͺ�
#define ATGM332D 0x01
#define UM220 0x02
extern unsigned char GPS_module ;

#endif //__gps_h__
