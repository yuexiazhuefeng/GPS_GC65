/***************************************************************************
 * @Filename        -  
 * 
 * Copyright ?zzg Technology Ltd, 2015. All rights reserved.
 * 
 * @Brief           - 
 *      This file for GPRS CMD
 *      
 * 
 * 											   
 * @Revision        - 1.1.1.1 $
 * @Date            - 2015/03/11 11:50:35 $
 * @Author          - zzg $
 * @Project         - 
 *
 *
 * @Revision History- 
 * @Log             - $
 * 
***************************************************************************/
/***************************************************************************
 * Copyright  ?zzg Technology Ltd. 2015.  All rights reserved.
 ***************************************************************************/
/*
 * NOTE:
 * Comment banners infra can be edited or deleted as applicable.
 * They are included as ordinal reminders.
 *
 * Function banners are required.
 */
/***********************************************************************
 * Library header files (#include)
 **********************************************************************/
#include <stdio.h>
#include "main.h"
#include "led.h"
#include "User_define.h"
/***********************************************************************
 * Public user header files (#include)
 **********************************************************************/
//#include <includes.h> 
/***********************************************************************
 * Private user header files (#include)
 **********************************************************************/
 #include "gps.h"
/***********************************************************************
 * Constant definitions (#define)
 **********************************************************************/

/***********************************************************************
 * Macro definitions (#define)
 **********************************************************************/


/***********************************************************************
 * Data declarations of global data imported.  
 **********************************************************************/
   	   		   

/***********************************************************************
 * Function prototypes of global functions imported.  
 **********************************************************************/

/***********************************************************************
 * Data definitions of global data exported.  
 **********************************************************************/
 unsigned short timegpserror =0;
// ��ǰ�Ƿ�λ
 unsigned char dingwei_flag=0;
// �Ƿ�λ������λ���ľͿ���ʹ�ñ�������λ����Ϣ
unsigned char dingweied_flag=0;
unsigned char GPS_module = 0;//ljw,20161130,GPSģ����ͺţ�ATGM332D 0x01  #define UM220 0x02
 

/***********************************************************************
 * Function prototypes of functions requiring global scope
 * not included elsewhere.  
 **********************************************************************/

/***********************************************************************
 * Static function prototypes. (Forward declarations)
 **********************************************************************/
/* Forward declarations of private methods */


/***********************************************************************
 * Static variable definition (file scope).  
 **********************************************************************/

/***********************************************************************
 * Note: Function banner and body below are repeated for each function.
 **********************************************************************/


/***********************************************************************
 * Function banner
 *     |
 *     |
 *     v
 **********************************************************************/


/**************************************************************************/
/* 
 * @func      - 
 * @return    - n/a
 * @param     - n/a
 * @param     - n/a
 * @brief     - 
 * @notes:    - [if any]
 *              (1) ...
 *              (2) ...
 *
 */
/**************************************************************************/
/***********************************************************************
 * Function body (definition)  
 **********************************************************************/
/* static void funcname (void * data)
 * {
 *      etc...
 * }
 */
/*
**-----------------------------------------------------------------------------
**
**	����:
**		��ȡһ����õ� GPS λ����Ϣ
**
**	����:
**		��
**
**	����:
**		��
**
**-----------------------------------------------------------------------------
*/
#if 0
unsigned char GetAvailableGPSInfo(void)
{
	unsigned char i = 15;
	static unsigned char timegpserror =0;
        g_ucUART6RXUsed = 0;
	do
	{
		
		i--;
		
		if(strstr((char*)g_ucNeo6QBuff,",A,"))
                  break;
		delayms(100);
		IWDG_ReloadCounter();
		if(GPRSCmdFlag == 1)
			return 0;		
	}
	while(i != 0);

	if(i == 0)
	{
		GPSLedOff();
		timegpserror ++;
	}
	else
	{
	       GPSLedOn();
		   timegpserror = 0;
	}

	if(timegpserror>20)
	{
	 	timegpserror = 0;
	//����GPS
		GPSOff();
		GPSLedOff();
		delayms(2000);
		GPSOn();
		//GPSOpen();
        delayms(5000);
		i = 20;
		do
		{
			i--;
			if(strstr((char*)g_ucNeo6QBuff,",A,"))
	                  break;
			delayms(100);
			IWDG_ReloadCounter();
			if(GPRSCmdFlag == 1)
				return 0;
		}
		while(i != 0);
		if(i != 0)
	       {
			GPSLedOn();
		}

	} 
	return i;
}
#endif

/*
**-----------------------------------------------------------------------------
**
**	����:
**		��ȡһ����Կ��õ� GPS λ����Ϣ
**
**	����:
**		��
**
**	����:
**		��
**
**-----------------------------------------------------------------------------
*/
GPS_ASCII gps_ascii;
unsigned char GetAvailableGPSInfoV(void)
{
	unsigned char i = 15;
	unsigned char *ptmp;
    
    u8 *p_dollar ;
    u8 *P_xinghao;
    u8 *pCommard;
    u8 *pTempBuf;
    u8 ucTen;
    u8 ucOne;
    u8 len;
    GPS_ASCII gps_ascii_tempbackup;
    
    g_ucUART6RXUsed = 0;
	do
	{
		i--;
		if(strstr((char*)g_ucNeo6QBuff,",A,"))
            break;
		#if 0
		if(strstr((char*)g_ucNeo6QBuff,",V,"))
		{
		     ptmp = (u8*)strstr((u8*)g_ucNeo6QBuff,",V,");
		     if (((u8*)strstr((u8*)(ptmp+3),",")-ptmp > 7))
                  break;
		}
		#endif
		IWDG_ReloadCounter();
//ljw,20160906for space		if(GPRSCmdFlag == 1)
//ljw,20160906for space			return 0;
	}
	while(i != 0);
	if(i == 0)
	{
//ljw,20160831�ŵ��ж�������		GPSLedOff();
		//memcpy(g_ucNeo6QBuff,backup_g_ucNeo6QBuff,sizeof(g_ucNeo6QBuff));  
		
        GPS_BaudRate_FLAG = 0;
		dingwei_flag = 0;
	}
	else
	{
        p_dollar = (u8 *)strstr((char*)g_ucNeo6QBuff,"$");
        P_xinghao = (u8 *)strstr((char*)g_ucNeo6QBuff,"*");
#if 0//wei, ׼����У��
//ljw,20160831�Ѿ��ŵ�������������        GPSLedOn();
//ljw,20160904        memcpy(backup_g_ucNeo6QBuff,g_ucNeo6QBuff,sizeof(g_ucNeo6QBuff));  
        timegpserror = 0;
//        dingwei_flag = 0x5a;
//        dingweied_flag = 0x5a;
#endif //wei, ׼����У��  
        
        if((p_dollar != NULL)&&(P_xinghao != NULL))
        {
            len = P_xinghao - p_dollar;
            if((len<sizeof(g_ucNeo6QBuff))&&(CheckSum((p_dollar),(len+5))))
            {
                //$GNRMC,044527.000,A,3151.6690,N,11715.9152,E,0.00,0.00,040916,,,A*79
                memcpy((u8 *)(&gps_ascii_tempbackup),(u8 *)(&gps_ascii),sizeof(gps_ascii));
                memset((u8 *)(&gps_ascii),0x00,sizeof(gps_ascii));
                pTempBuf = p_dollar;
                ucOne = 0x00;
//#if 0 //ljw,20160904GPS������ȡ
                do			//��do...while�ṹ���ҳ�$GPRMC�е�UTCʱ���������ʱ��
                {
//ljw                    pCommard = (u8*)strchr((const char*)pTempBuf, ',');
                    pCommard = (u8*)strstr((u8 *)pTempBuf, ",");
                    ucTen = pCommard - pTempBuf;
                    if (0x01 == ucOne) //ȡGPS��UTCʱ�䣨ʱ���룩 HHMMSS.SSS
                    {
                        if ((ucTen >= 0x06) && (ucTen <= 0x0A))
                            memcpy((u8 *)(gps_ascii.UTCtime),(u8 *)(pTempBuf),6);	// ����ʱ��
                        else break;
                    }
                    else if (0x02 == ucOne) //��ȡGPSʱ���Ƿ���Ч�ı�־����A--��Ч��V--��Ч��
                    {
                        if((*pTempBuf)=='A')
                        {
                            dingweied_flag = 0x5a;
                            gps_ascii.status = 'A';		//��ȡGPSʱ���Ƿ���Ч�ı�־����A--��Ч��V--��Ч��
                        }
                        else break;
                    }
                    else if(0x03==ucOne)//��ȡγ����Ϣ
                    {
                        if(ucTen >=9)//UM220,С����󾫶�6λ���п�΢4λ(ucTen ==9) //γ����9���ַ�
                        {//ddmm.mmmm
                            memcpy((u8 *)(gps_ascii.Lat),(u8 *)(pTempBuf),ucTen);	// ����γ����Ϣ
                        }
                        else break;
                    }
                    else if(0x04==ucOne)//��ȡγ�ȱ�־
                    {
                        if(((*pTempBuf) =='N')||((*pTempBuf) =='S'))
                            gps_ascii.uLat = *pTempBuf;
                        else break;
                    }
                    else if(0x05==ucOne)//��ȡ������Ϣ
                    {
                        if(ucTen >=10)  //UM220,С����󾫶�6λ���п�΢4λ (ucTen ==10) //γ����9���ַ�
                        {
                            memcpy((u8 *)(gps_ascii.Lon),(u8 *)(pTempBuf),ucTen);//����γ����Ϣ
                        }
                        else break ;//����γ����Ϣ
                    }
                    else if(0x06==ucOne)//��ȡ���ȱ�־
                    {
                        if(((*pTempBuf)=='E')||((*pTempBuf)=='W'))
                            gps_ascii.uLon = *pTempBuf;
                        else break;
                    }
                    else if(0x07==ucOne)//��ȡ�ٶ���Ϣ��20140618�칫���޷��õ��ٶ�ֵ��������
                    {
                        if((ucTen > 0)&&(ucTen<9)) //���ٶ�ֵ�� �ٶ�ֵ�ö��ٸ��ֽڱ�ʾ��ȷ����
                        {
                            if(ucTen > 6)
                            {
                                ucTen =6;
                            }
                            memcpy((char *)(gps_ascii.Spd),(char *)(pTempBuf),ucTen);//�����ٶ�
                        }
                        else break;
                    }
                    else if(0x08==ucOne)//��ȡ�ٶȷ���
                    {
                        if((ucTen > 3)&&(ucTen <9))//�Ե��溽��
                        {//dddmm.mmmm
                            if(ucTen > 6)
                                ucTen = 6;
                            memcpy((char *)(gps_ascii.Cog),(char *)(pTempBuf),ucTen);//��������
                        }
                        else break;
                    }
                    else if (0x09 == ucOne)		//��ȡGPS������
                    {
                        if ((ucTen >= 0x03) && (ucTen <= 0x06))
                        {
                            memcpy((char *)(gps_ascii.Date),(char *)(pTempBuf),ucTen);//��������                            
                            dingweied_flag = 0x5a;
                            dingwei_flag = 0x5a;
                        }
                        else break;
                    }
                    pTempBuf = pCommard + 1;
                    ucOne++;
                    if(ucOne >0x09)
                        break;                   
                }while(1);
//                memset(g_ucNeo6QBuff,0x00,sizeof(g_ucNeo6QBuff));//ljw,20160904�����۳ɲ��ɹ�����Ҫ���GPS����
//                if(0x09 == ucOne)//ljw,20160908for data you know?
//                {
//                    dingwei_flag = 0x00;
//                    return i;
//                }
                if(ucOne <= 0x09) //ljw,20161129,um220�ն�λ��ʱ��ʱ�����������Ч�ģ���γ������Ч�ģ����ﶼ��Ҫ
                {
                    dingwei_flag = 0x00;
                    memcpy((u8 *)(&gps_ascii),(u8 *)(&gps_ascii_tempbackup),sizeof(gps_ascii));                    
                }
//                else
//                {
//                    dingwei_flag = 0x5a;
//                    dingweied_flag = 0x5a;
//                }
//#endif //ljw,20160904GPS������ȡ
//ljw,20160903�ŵ�����                GPSLedOn();
               memcpy(backup_g_ucNeo6QBuff,g_ucNeo6QBuff,sizeof(g_ucNeo6QBuff));
            }
        }        
	}
/*******************************************************/
#if 0  //20151027:  l���ܾ����������ˣ���������������źŶ�λ�ٶ�
	if(timegpserror> 250)  //20   ���5���ӻ�����λ��������GPS
	{
	 	timegpserror = 0;
		dingwei_flag = 0;
	//����GPS
		GPSOff();
		GPSLedOff();
		delayms(10000);
		GPSOn();
		//GPSOpen();
        delayms(5000);
		i = 15;
		do
		{
			i--;
			if(strstr((char*)g_ucNeo6QBuff,",A,"))
	                  break;
			#if 0
			if(strstr((char*)g_ucNeo6QBuff,",V,"))
			{
			     ptmp = (u8*)strstr((char*)g_ucNeo6QBuff,",V,");
			     if (((u8*)strstr((char*)(ptmp+1),",")-ptmp > 8))
	                  break;
			}
			#endif
			IWDG_ReloadCounter();
			if(GPRSCmdFlag == 1)
				return 0;
		}
		while(i != 0);
		if(i != 0)
	    {
            u8 *p_dollar ;
            u8 *P_xinghao;
            u8 len;
            p_dollar = strstr((char*)g_ucNeo6QBuff,"$");
            P_xinghao = strstr((char*)g_ucNeo6QBuff,"*");
            if((p_dollar != NULL)&&(P_xinghao != NULL))
            {
                len = P_xinghao - p_dollar;
                if(CheckSum(p_dollar,len))
                {

                    GPSLedOn();
                    memcpy(backup_g_ucNeo6QBuff,g_ucNeo6QBuff,sizeof(g_ucNeo6QBuff));
                    dingwei_flag = 0x5a;
                    dingweied_flag = 0x5a;
                }
            }
		}
	}
#endif

#if 0	
	//ӦΪ���ڻ�ʹ�ñ��������ٰѲ���һ��
	if (i == 0)
	{
		i =10;
		do
		{
			i--;
			if(strstr((char*)g_ucNeo6QBuff,",A,"))
				break;
			#if 0
			if(strstr((char*)g_ucNeo6QBuff,",V,"))
			{
					 ptmp = (u8*)strstr((u8*)g_ucNeo6QBuff,",V,");
					 if (((u8*)strstr((u8*)(ptmp+3),",")-ptmp > 7))
										break;
			}
			#endif
			
			IWDG_ReloadCounter();
			if(GPRSCmdFlag == 1)
				return 0;
		}
		while(i != 0);
	}
#endif
	return i;
}

void reset_gps()
{
    set_GPS_POWERON (FALSE);//�ϵ�
    delayms(2000);
    set_GPS_POWERON (TRUE);
    delayms(2000);
    if(!UbxConfig())
    {
        UbxConfig();
    }
}
#if 0
//wei,20160826 GPS����ģʽ
void GPS_Reset(void)
{
   ddi_uart_Write(&UART2_ctl_t,"$PCAS10,2*1E\r\n",strlen("$PCAS10,2*1E\r\n")); 
 //    ddi_uart_Write(&UART2_ctl_t,"$PCAS10,1*1D\r\n",strlen("$PCAS10,1*1D\r\n"));
}
#endif
//wei,GPSУ�����,��ȷ���� 1 ���󷵻�0
unsigned char CheckSum(unsigned char *databuf,unsigned char buflen)
{
    u8 sum;
    u8 c1;
    u8 c2;
    u8 i;
    sum = databuf[1];
    for(i=2;i<(buflen-5);i++)
    {
        sum = (sum ^ databuf[i]);
    }
    c2 = sum&0x0F;//24 47 4E 52 4D 43 2C 2C 30 39 30 36 32 37 2E 30 30 30 2C 41 2C 33 31 35 31 2E 36 39 34  36 2C 4E 2C 31 31 37 31 35  2E 39 33 33 37
    c1 = (sum>>4)&0x0F;
    if(c1<10) c1 +='0';else c1 += 'A' -10;
    if(c2<10) c2 +='0';else c2 += 'A' -10;
    if(c1 == databuf[buflen-4] && c2 == databuf[buflen-3]) 
    {
        return 1;
    }
    return 0;
}




