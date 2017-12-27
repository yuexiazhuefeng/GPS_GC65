/***************************************************************************
 * @Filename        -  
 * 
 * Copyright ?zzg Technology Ltd, 2005. All rights reserved.
 * 
 * @Brief           - 
 *      This file contains the implementation of the stm32f0xx iic driver
 *      for iic2.
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
 * Copyright  ?zzg Technology Ltd. 2005.  All rights reserved.
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

/***********************************************************************
 * Public user header files (#include)
 **********************************************************************/
#include <stdio.h>
#include "main.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "led.h"
#include "gps.h"
/***********************************************************************
 * Private user header files (#include)
 **********************************************************************/

/***********************************************************************
 * Constant definitions (#define)
 **********************************************************************/
//#define CHINESE_SMS
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
u8 dial_status = 0;
u8 net_status = 0;

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

void ClrGSMBuff(void)
{
	memset(g_ucSim900Buff,0,SIM900_BUFF_SIZE);
	gUart0RxCnt = 0;
}

u8 hex2BCD_ASCII1(unsigned char * decdata,unsigned char src)
{	
	unsigned char ret ;
	*decdata = (src/10)*0x10+src%10;
}


u8 hex2BCD_ASCII(unsigned char * decdata,unsigned short int src)
{
	volatile unsigned short int tmp;
	unsigned char ret;
	tmp = src;
	if (src>=10000)
	{
		* decdata++ = '0' + src /10000;
		* decdata++ = '0' + (src %10000)/1000;
		* decdata++ = '0' + (src %1000)/100;
		* decdata++ = '0' + (src %100)/10;
		* decdata++ = '0' + (src %10);
		ret = 5;
	}
	else
	if(src>=1000)
	{
		* decdata++ = '0' + src /1000;
		* decdata++ = '0' + (src %1000)/100;
		* decdata++ = '0' + (src %100)/10;
		* decdata++ = '0' + (src %10);
		ret = 4;
	}
	else
	if(src>=100)
	{
		* decdata++ = '0' + src /100;

		* decdata++ = '0' + (src %100)/10;
		* decdata++ = '0' + (src %10);
		ret = 3;
	}
		else
	if(src>=10)
	{
		* decdata++ = '0' + src /10;
		* decdata++ = '0' + (src %10);
		ret = 2;
	}
		else
	if(src>=0)
	{
		* decdata= '0' + src ;
		ret = 1;
	}
	return ret;
}
//ljw,20160908 获取ASCII时间
u8 hex2acsii_u8(unsigned char * decdata,unsigned  char src)
{    
    * decdata++ = '0' + src /10;
    * decdata++ = '0' + (src %10);
}
u8 get_ASCII_TIM(u8 *des)
{
    u8 *p_in;
    RTC_Get();
    p_in = des;
    hex2acsii_u8(p_in,(u8)(timer.w_year%2000));
    hex2acsii_u8(p_in+2,timer.w_month);
    hex2acsii_u8(p_in+4,timer.w_date);
    hex2acsii_u8(p_in+6,timer.hour);
    hex2acsii_u8(p_in+8,timer.min);
    hex2acsii_u8(p_in+10,timer.sec);
    
//    hex2BCD_ASCII(p_in,timer.w_year);
//    hex2BCD_ASCII(p_in+4,timer.w_month);
//    hex2BCD_ASCII(p_in+6,timer.w_date);
//    hex2BCD_ASCII(p_in+8,timer.hour);
//    hex2BCD_ASCII(p_in+10,timer.min);
//    hex2BCD_ASCII(p_in+12,timer.sec);
//    p_in[0] = (timer.w_year>>4)+0x30;
//    p_in[1] = (timer.w_year&0x0f)+0x30;
//    p_in[2] = (timer.w_month>>4)+0x30;
//    p_in[3] = (timer.w_month&0x0f)+0x30;
//    p_in[4] = (timer.w_date>>4)+0x30;
//    p_in[5] = (timer.w_date&0x0f)+0x30;
//    p_in[6] = (timer.hour>>4)+0x30;
//    p_in[7] = (timer.hour&0x0f)+0x30;
//    p_in[8] = (timer.min>>4)+0x30;
//    p_in[9] = (timer.min&0x0f)+0x30;
//    p_in[10] = (timer.sec>>4)+0x30;
//    p_in[11] = (timer.sec&0x0f)+0x30;
    return 1;
}
void hextoascii(unsigned char * srcdata,unsigned char * decdata,unsigned short int len)
{
	unsigned short int i;
	unsigned char tmp;
	for(i=0; i<len; i++)
	{
		tmp =((*srcdata)&0xf0 )/16;
		if(tmp <10)
		{
		   *decdata++ = '0'+tmp;
		}
		else
		{
		  *decdata++ = 'A'+tmp-10;		
		}
		tmp = ((*srcdata++)&0x0f );
		if(tmp <10)
		{
		   *decdata++ = '0'+tmp;
		}
		else
		{
		  *decdata++ = 'A'+tmp-10;
		}
	}
}

//20150729:这个函数只能两字节一组的来用，比如 "11"-> 11, ok; "117"->1*256+17 error
void asciitohex(unsigned char * src,unsigned char * dec,unsigned short int len)
{
	unsigned short int i;
	volatile unsigned char tmp=0;
      unsigned char tmpgprsbuf[50];
      unsigned char * decdata = tmpgprsbuf;
      memset(tmpgprsbuf,0,50);

	if (len ==1)
	{
		*dec = *src -'0';
		return;
	}
	else if (len==2) 
	{
		for(i=0; i<len; i++)
		{
			if ((*src >='A') &&(*src <'G'))
			{
				if((i%2) == 0)
				   tmp =(unsigned char) ((*src-'A')*16);
				else
				{
				   tmp += (*src-'A')+10;
				   *decdata++ = tmp;
				}
			}
			else
			 if((*src >='0') && (*src <='9'))
			 {
				if((i%2) == 0)
				   tmp = (unsigned char)( (*src-'0')*10);
				else
				{
				   tmp += (*src-'0');
				   *decdata++ = tmp;
				}   			
				
			 }
			 src ++;

		}
	//        memset(g_ucGPRSCmdBuff,0,sizeof(g_ucGPRSCmdBuff));
	        memcpy(dec,tmpgprsbuf,len/2+len%2);
	}
	else
	if (len== 3)
	{
		*dec = (*src-'0')*100;
		*dec += (*(src+1)-'0')*10;
		*dec += *(src+2)-'0';
	}
	
}
/*
**wei,20160628
**功能：关闭知道通道的TCP/UDP连接
**参数：index,指定的通道0-
**返回：0，失败  1，成功
**
**
**
**
*/

u8 GSM_QICLOSED(u8 index)
{
    u8 index_temp = 2;
    u8 temp[20]="AT+QICLOSE=";
    if(AGPS_SERVERS ==index)
        index_temp ='2';
    else if(FHT_SERVERS ==index)
        index_temp ='1';
    temp[11]=0x00;
    strncat((char *)temp,(char *)(&index_temp),1);
    strncat((char *)temp,(char *)("\r\n"),2);
    ddi_uart_Write(&UART1_ctl_t,temp,strlen((const char *)temp));
    if(0 == WaitGSMOK2(500))//收到OK
        return 1;
    else
        return 0;
}

/*ljw,20160908  
**AT+CPIN?
**返回:  0: NOT READY  1:READY  
**
**主动上报  +CPIN: NOT READY
***/
u8 CPIN_READY_flag = 0;
u8 GC65_CPIN(void)
{
    u8* pt=NULL;
    u8 i = 0;
    CPIN_READY_flag = 0;
    for(i=0;i<2;i++)
    {
        ddi_uart_Write(&UART1_ctl_t,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
        if(NULL !=strstr((const char *)g_ucSim900Buff,"+CPIN: READY"))
        {
            CPIN_READY_flag = 0x5a;
            return 1;
        }
        delayms(2000);
    }    
    return 0;
}

/*wei,20160909,*******
**函数名:
**功能: 设置AT+CGATT?
**返回：  0, 失败   1,成功
**
*********/
u8 GC65_CGATT(void)
{
    u8 i;
    for(i=0; i<20; i++)
    {
        ddi_uart_Write(&UART1_ctl_t,"AT+CGATT?\r",strlen("AT+CGATT?\r"));
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
        if(strstr((char *)g_ucSim900Buff,"+CGATT: 1") != NULL)
        {
            net_status = 1;
            return 1;
        }
        if(i%5 == 0)//ljw,20160921,针对掉卡的现象做的处理
        {
            if(!GC65_CPIN())
            {
                return 0;
            }
        }
        delayms(1000);
    }
    return 0;
}

/*wei,20160625,*******
**函数名:
**功能: 设置APN
**返回：  0, 失败   1,成功
**
*********/
u8 SET_APN(void)
{
    u8 temp[120] = "AT+QICSGP=1,\"";
    u8 lens = 0;
//  g_ucNetAPN[NETAPN_LEN] ;    
    if ((lens = strlen((char *)system_para.g_ucNetAPN)) > NETAPN_LEN-1)
	  	lens = NETAPN_LEN-1;
    strncat((char *)temp,(char *)system_para.g_ucNetAPN,lens);
    strcat((char *)temp,"\",\"");
//    g_ucNetUser[NETUSER_LEN] ;
    if ((lens = strlen((char *)system_para.g_ucNetUser)) > NETUSER_LEN-1)
	  	lens = NETUSER_LEN-1;
    strncat((char *)temp,(char *)system_para.g_ucNetUser,lens);
    strcat((char *)temp,"\",\"");
//    g_ucNetPsw[NETPSW_LEN] ;
    if ((lens = strlen((char *)system_para.g_ucNetPsw)) > NETPSW_LEN-1)
	  	lens = NETUSER_LEN-1;
    strncat((char *)temp,(char *)system_para.g_ucNetPsw,lens);
    strcat((char *)temp,"\"\r\n");
    ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
}

//ljw,20161126AT+QINDI 配置是否缓存收到的数据
//返回，1-success  0-fail
#define CACHE 0x01
#define NO_CACHE 0x00
u8 GC65_QINDI(u8 model)
{
    u8 i;
    u8 mode_temp = model;
    u8 temp_send_arry[12] ="AT+QINDI=";
    if(mode_temp == CACHE)
        strcat((char *)temp_send_arry,"1\r");
    else 
        strcat((char *)temp_send_arry,"0\r");
    for(i=0; i<3; i++)
    {
        ddi_uart_Write(&UART1_ctl_t,temp_send_arry,strlen("AT+QINDI=1\r"));
        if(!WaitGSMOK1())//ljw20160903for space   WaitGSMOK();
            return 1;
        delayms(1000);
    }
    return 0;
}

/**
> x=874675487@qq.com:qfsGBL
2,SEND OK

+QIRDI: 0,1,2
AT+QIRD=0,1,2,1500
+QIRD: 114.242.203.140:8000,UDP,1500
**/
u16 GC65_QIRD(void)
{
    u16 i =0;
    memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
    GPRSCmdCount = 0;
    fB5 = 0;//
    ddi_uart_Write(&UART1_ctl_t,"AT+QIRD=0,1,2,1500\r",strlen("AT+QIRD=0,1,2,1500\r"));
    for(i=0;i<600;i++)
    {
        if(fB5 == AGPS_RE_END)
            return 1;
//        if(strstr((const char *)g_ucGPRSCmdBuff,"OK\r\n"))
//        {
//            return 1;
//        }
        delayms(5);
    }
    return 0;
}


//wei,20160622,判断COPS     //查询当前运营商
/**
**中国移动
AT+COPS?

+COPS: 0,2,"46000"

OK
**
*****/
u8 cops_flag = 0;//wei,cops_flag = 0x5a,获得了运营商，0，没有获得运营商
u32 cops_flag_timer = 0;//wei,20160623,从获得运营商开始计时，用作接收短信依据
u8 WaitGSM_ReadyCOPS(void)
{
    u8* pt=NULL;
    u8 i = 0;
    for(i=15;i>1;i--)
    {
        if(i%5==0)
        {
            ddi_uart_Write(&UART1_ctl_t,"AT+COPS=0,2\r\n",strlen("AT+COPS=0,2\r\n"));
            WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
        }
        ddi_uart_Write(&UART1_ctl_t,"AT+COPS?\r\n",strlen("AT+COPS?\r\n"));
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
        if(NULL !=strstr(g_ucSim900Buff,"+COPS: 0,2,\"460"))
        {
            pt = strstr(g_ucSim900Buff,"\"460");
            if(pt != NULL)
            {
                pt += 4;
                MobileCompany = *pt++ - '0';
                MobileCompany = *pt-'0' + MobileCompany*10;
            }
            if ( strstr(g_ucSim900Buff,"MOBILE")) //移动
            {
                MobileCompany = 0;
            }
            if(cops_flag==0x5a)//之前已经获得了运行商
                return 1;
            else //wei,20160623,首次获得运营商
            {
                cops_flag_timer = secondcounter;
                cops_flag = 0x5a;
                return 1;
            }              
        }
        if(i%5 ==0)//ljw,20160921,针对掉卡的现象做的处理
        {
            if(!GC65_CPIN())
            {
                return 0;
            }
        }
//        if(NULL !=strstr((const char *)g_ucSim900Buff,"+COPS: 0,2,\"460"))
//        {
//            if(cops_flag==0x5a)//之前已经获得了运行商
//                return 1;
//            else //wei,20160623,首次获得运营商
//            {
//                cops_flag_timer = secondcounter;
//                cops_flag = 0x5a;
//                return 1;
//            }              
//        }           
        delayms(1000);//即使没有获得运营商，也会返回OK,所以这里加个延时，已保证有足够的时间去获取运营商
        if(i<=2)
            return 0;
    }
//	pt = (u8 *)strstr((const char *)g_ucSim900Buff,"\"460");
//	if(pt != NULL)
//	{
//		pt += 4;
//		MobileCompany = *pt++ - '0';
//		MobileCompany = *pt-'0' + MobileCompany*10;
//	}
//	if ( strstr((const char *)g_ucSim900Buff,"MOBILE")) //移动
//	{
//		MobileCompany = 0;
//    }
}

//wei20160608
//用作判断CGREG,
//使能模块采用多通道传输数据，返回1成功，返回1失败
//AT+QIMUX=1

//OK
//AT+QIMUX?

//+QIMUX: 1
u8 WaitGSM_ReadyQIMUX(void)
{
    u8* ptmp = NULL;
    u8* p1 = NULL;
    u8 i = 0;
	
    for(i=6;i>1;i--)//wei,做5次
    {
        ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX=1\r",strlen("AT+QIMUX=1\r"));
	  WaitGSMOK1();//ljw20160903for space   WaitGSMOK();		
	  ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
	  WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
        ptmp = strstr((char *)g_ucSim900Buff,"+QIMUX:");
        if(NULL !=ptmp)
        {
            if(*(ptmp+8) =='1')
                return 1;
        }	      
	    if(i<=2)
	       return 0;
        delayms(1500);
   }
}

//wei20160607
//用作判断CGREG,
//返回1，拨号准备成功，返回0，不成功
u8  WaitGSM_ReadyNet(void)
{
    u8* ptmp = NULL;
    u8* p1 = NULL;
    u8 i = 0;
    ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=?\r\n",strlen("AT+CGREG=?\r\n"));
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
	 	
    for(i=15;i>1;i--)
    {
        ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=2\r\n",strlen("AT+CGREG=2\r\n"));
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
        ddi_uart_Write(&UART1_ctl_t,"AT+CGREG?\r\n",11);
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
        ptmp = (u8*)strstr((unsigned char *)g_ucSim900Buff,"+CGREG:");
        if(NULL !=ptmp)
        {
            if((*(ptmp+10) =='1')||(*(ptmp+10) =='5'))
                return 1;
        }
        if(i%5 ==0)//ljw,20160921,针对掉卡的现象做的处理
        {
            if(!GC65_CPIN())
            {
                return 0;
            }
        }
	    if(i<=2)
	       return 0;
        delayms(1500);
   }
}

//等gsm的OK字符
void WaitGSMOK(void)
{
//	unsigned short i;
//	//ManualRx0 = 1;
//	ClrGSMBuff();
//	//EI();
//	ManualRx0 = 1;
//	gUart0RxCnt = 0;
//	//wait for 500ms
//	for(i=0; i<150; i++)
//	{
//		if(strstr(g_ucSim900Buff,"OK") != NULL)
//		  break;
//		//feed_wdt();
//		if(strstr(g_ucSim900Buff,"ERROR") != NULL)
//             {  
//                    break;
//             }  
//             //feed_wdt();
//		if(strstr(g_ucSim900Buff,"ABORTED") != NULL)
//             {  
//                    break;
//             } 
//		delayms(10);
//             //feed_wdt();
//		if (GPRSCmdFlag == 1)
//		   	break ;
//	}
//	ManualRx0 = 0;
}

//等gsm的OK字符
unsigned char WaitGSMOK1(void)
{
	unsigned short i;
       unsigned char ret=0x5a;
	//ManualRx0 = 1;
    
    IWDG_ReloadCounter();//ljw,20160906防止在收7E数据
    while(proGPRSCmdFlag==1);//ljw,20160906防止在收7E数据
	ClrGSMBuff();
	//EI();
	ManualRx0 = 1;
	gUart0RxCnt = 0;
    
	//wait for 500ms
//wie	for(i=0; i<40; i++)
	for(i=0; i<80; i++)
	{
		if(strstr(g_ucSim900Buff,"OK") != NULL)
        {
            //ddi_uart_Write(&UART1_ctl_t,"OK=1\r",strlen("OK=1\r"));	
            ret = 0;
            break;
        }
		//feed_wdt();
		if(strstr(g_ucSim900Buff,"ERROR") != NULL)
        {
            //ddi_uart_Write(&UART1_ctl_t,"ERROR=1\r",strlen("ERROR=1\r"));	
            break;
        }
             //feed_wdt();
		if(strstr(g_ucSim900Buff,"ABORTED") != NULL)
        {
            //ddi_uart_Write(&UART1_ctl_t,"ABORTED=1\r",strlen("ERROR=1\r"));	
            break;
        }
            //feed_wdt();
		if(strstr(g_ucSim900Buff,">") != NULL)
        {
            //ddi_uart_Write(&UART1_ctl_t,">=1\r",strlen(">=1\r"));	
            break;
        }
	    delayms(20);
	}
	//if (i>399)
	//	ddi_uart_Write(&UART1_ctl_t,"WaitOK1 to\r",strlen("WaitOK1 to\r"));		
//	ddi_uart_Write(&UART1_ctl_t,"gUart0RxCnt=",strlen("gUart0RxCnt="));	
//	ddi_uart_Write(&UART1_ctl_t,(UCHAR*)&gUart0RxCnt,1);
//	ddi_uart_Write(&UART1_ctl_t,"\r",1);
	
	ManualRx0 = 0;
        return ret;
}
/*wei
**-----------------------------------------------------------------------------
**	概述:
**		可定时长等待GSM返回AT指令
**
**	参数:
**		waittime_OK :等待时长，1次20ms
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
unsigned char WaitGSMOK2(unsigned short waittime_OK)
{
	unsigned short i;
    unsigned char ret=0x5a;
    
    IWDG_ReloadCounter();//ljw,20160906防止在收7E数据
    while(proGPRSCmdFlag==1);//ljw,20160906防止在收7E数据
	//ManualRx0 = 1;
	ClrGSMBuff();
	//EI();
	ManualRx0 = 1;
	gUart0RxCnt = 0;
	//wait for 500ms
//WEI	for(i=0; i<40; i++)
	for(i=0; i<waittime_OK; i++)//20 ms??
	{
	      //wei,修复拨号误判BUG
		if((strstr(g_ucSim900Buff,"CONNECT OK\r") != NULL)||(strstr(g_ucSim900Buff,"ALREADY CONNECT\r") != NULL))
		 // if(strstr(g_ucSim900Buff,"CONNECT") != NULL)
                {  
                    //ddi_uart_Write(&UART1_ctl_t,"OK=1\r",strlen("OK=1\r"));	
                    ret = 0x05;
			break;
                }
	      //wei,
		if(strstr(g_ucSim900Buff,"OK") != NULL)
		{
			//ddi_uart_Write(&UART1_ctl_t,"OK=1\r",strlen("OK=1\r"));	
			ret = 0;
			break;
		}
		//feed_wdt();
		if(strstr(g_ucSim900Buff,"ERROR") != NULL)
		{
			//ddi_uart_Write(&UART1_ctl_t,"ERROR=1\r",strlen("ERROR=1\r"));	
			break;					
		}
             //feed_wdt();
		if(strstr(g_ucSim900Buff,"ABORTED") != NULL)
		{  
			//ddi_uart_Write(&UART1_ctl_t,"ABORTED=1\r",strlen("ERROR=1\r"));	
			break;					
		}
            //feed_wdt();
		if(strstr(g_ucSim900Buff,">") != NULL)
		{  
			//ddi_uart_Write(&UART1_ctl_t,">=1\r",strlen(">=1\r"));	
			break;					
		}
	     delayms(20); 
	}
	//if (i>399)
	//	ddi_uart_Write(&UART1_ctl_t,"WaitOK1 to\r",strlen("WaitOK1 to\r"));		
//	ddi_uart_Write(&UART1_ctl_t,"gUart0RxCnt=",strlen("gUart0RxCnt="));	
//	ddi_uart_Write(&UART1_ctl_t,(UCHAR*)&gUart0RxCnt,1);
//	ddi_uart_Write(&UART1_ctl_t,"\r",1);
	
	ManualRx0 = 0;
        return ret;
        
}

u8 isdigit_w(u8 num)
{
    if((num >='0')&&(num <='9'))
        return 1;
    else return 0;
}
//ljw,20160909,检测是不是0-9
//f
u8 isdigtit_len(u8 *p,u8 len)
{
    u8 *p_in =p;
    u8 i;
    for(i=0; p_in[i]!=0; i++)
    {
        if(i>=(len-1))
            return 1;
//        if(!isdigit(p_in[i]))
        if(!isdigit_w(p_in[i]))
            return 0 ;
    }
    if(i<len)
        return 0;            
}

//wei,20160812<
//功能:处理GSM的提供的基站时间，为RTC做时间基准
unsigned char Deal_GSMRTC(unsigned short waittime_OK)
{
	unsigned short i;
//    unsigned char ret=0x5a;
	u8 *p = NULL;
    u8 temp_array[13] = {0};
    
	ClrGSMBuff();
	ManualRx0 = 1;
	gUart0RxCnt = 0;
	//wait for 500ms
	for(i=0; i<waittime_OK; i++)//20 ms??
	{
//  +QNITZ: "16/09/09,04:10:21+32,0"        
		p = strstr(g_ucSim900Buff,"+QNITZ: ");        
        if(p != NULL)
		{
            //time_gps_rtc  DDMMYYHHmmSS
//            for(j=0;j<3;j++)
//            {
//                memcpy(temp_array+(j*2),p+15-(j*3),2);
//            }
//            for(j=0;j<3;j++)
//            {
//                memcpy(temp_array+6+(j*2),p+18+(j*3),2);
//            }
            delayms(30);//ljw,20160909接收完成
            memcpy(temp_array,p+15,2);
            memcpy(temp_array+2,p+12,2);
            memcpy(temp_array+4,p+9,2);
            memcpy(temp_array+6,p+18,2);
            memcpy(temp_array+8,p+21,2);
            memcpy(temp_array+10,p+24,2);
            if(!isdigtit_len((temp_array),12))
                break;
            asciitodate(temp_array,time_gps_rtc);//wei,20160810,将时间拷贝到time_gps_rtc数组用做RTC时间基准            
            backsecondforRTC = secondcounter;
            RTC_timeavalibe_flag = 0x0a;//wei,20160812 0x0a,时间无效  0x50,时间有效
//			ret = 0;
			break;
		}
        delayms(100);
	}
	ManualRx0 = 0;
    return 0;
}
//wei,20160812>

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
**
**-----------------------------------------------------------------------------
*/
void Configuration_TEXT_SMS(void)
{
//wei,20160628    ddi_uart_Write(&UART1_ctl_t,"AT+QISDE=0\r",strlen("AT+QISDE=0\r"));
//wei,20160628     WaitGSMOK();
//zzg test cell id

//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CREG=?\r\n",strlen("AT+CREG=?\r\n"));
//wei,20160628     WaitGSMOK(); 
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CREG=2\r\n",strlen("AT+CREG=2\r\n"));
//wei,20160628     WaitGSMOK(); 	
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r\n",strlen("AT+CREG?\r\n"));
//wei,20160628     WaitGSMOK(); 			

//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=?\r\n",strlen("AT+CGREG=?\r\n"));
//wei,20160628     WaitGSMOK(); 
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=2\r\n",strlen("AT+CGREG=2\r\n"));
//wei,20160628     WaitGSMOK(); 	
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
//wei,20160628     WaitGSMOK(); 

//test chinese sms
    ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"GSM\"\r\n",strlen("AT+CSCS=\"GSM\"\r\n"));
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
    ddi_uart_Write(&UART1_ctl_t,"AT+CSCA?\r\n",strlen("AT+CSCA?\r\n"));
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=0\r\n",strlen("AT+CMGF=0\r\n"));    
//wei,20160628     WaitGSMOK(); 

    ddi_uart_Write(&UART1_ctl_t,"AT+CPMS=\"SM\"\r",strlen("AT+CPMS=\"SM\"\r"));  	// //优选短信存储器为SIM卡
//DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    
    ddi_uart_Write(&UART1_ctl_t,"AT+CPMS?\r",strlen("AT+CPMS?\r"));  	// //优选短信存储器为SIM卡
//DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

    ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=1\r",strlen("AT+CMGF=1\r"));  	// 短信格式：TEXT//wei,20160628设置文本模式，0-PDU(中文)
//DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

    ddi_uart_Write(&UART1_ctl_t,"AT+CMGF?\r",strlen("AT+CMGF?\r"));  	// 短信格式：TEXT
//DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    if (strstr(g_ucSim900Buff,"1") == NULL)
    {
        ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=1\r\n",strlen("AT+CMGF=1\r\n"));  	// 短信格式：TEXT
        //DebugOut("* Set SMS Format: TEXT.\r\n",0);
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    }
    //AT+CSMP=17,255,0,0   17,167,0,241
    ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,0\r\n",strlen("AT+CSMP=17,255,0,0\r\n"));  	// 短信格式：TEXT
    //DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

    ddi_uart_Write(&UART1_ctl_t,"AT+CSMP?\r",strlen("AT+CSMP?\r"));  	// 短信格式：TEXT
    //DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

    if (strstr(g_ucSim900Buff,"17,255,0,0") == NULL)
    {
        ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,0\r\n",strlen("AT+CSMP=17,255,0,0\r\n"));  	// 短信格式：TEXT
        //DebugOut("* Set SMS Format: TEXT.\r\n",0);
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    }


    ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"8859-1\"\r",strlen("AT+CSCS=\"8859-1\"\r"));  // 短信特殊字符显示才能正常
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
}

#if 0//ljw20160924
void Configuration_TEXT_SMS(void)
{
//wei,20160628    ddi_uart_Write(&UART1_ctl_t,"AT+QISDE=0\r",strlen("AT+QISDE=0\r"));
//wei,20160628     WaitGSMOK();
//zzg test cell id

//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CREG=?\r\n",strlen("AT+CREG=?\r\n"));
//wei,20160628     WaitGSMOK(); 
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CREG=2\r\n",strlen("AT+CREG=2\r\n"));
//wei,20160628     WaitGSMOK(); 	
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r\n",strlen("AT+CREG?\r\n"));
//wei,20160628     WaitGSMOK(); 			

//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=?\r\n",strlen("AT+CGREG=?\r\n"));
//wei,20160628     WaitGSMOK(); 
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=2\r\n",strlen("AT+CGREG=2\r\n"));
//wei,20160628     WaitGSMOK(); 	
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
//wei,20160628     WaitGSMOK(); 

//test chinese sms
    ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"GSM\"\r\n",strlen("AT+CSCS=\"GSM\"\r\n"));
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
    ddi_uart_Write(&UART1_ctl_t,"AT+CSCA?\r\n",strlen("AT+CSCA?\r\n"));
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
//wei,20160628     ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=0\r\n",strlen("AT+CMGF=0\r\n"));    
//wei,20160628     WaitGSMOK(); 

    ddi_uart_Write(&UART1_ctl_t,"AT+CPMS=\"SM\"\r",strlen("AT+CPMS=\"SM\"\r"));  	// //优选短信存储器为SIM卡
//DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    
    ddi_uart_Write(&UART1_ctl_t,"AT+CPMS?\r",strlen("AT+CPMS?\r"));  	// //优选短信存储器为SIM卡
//DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

    ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=1\r",strlen("AT+CMGF=1\r"));  	// 短信格式：TEXT//wei,20160628设置文本模式，0-PDU(中文)
//DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

    ddi_uart_Write(&UART1_ctl_t,"AT+CMGF?\r",strlen("AT+CMGF?\r"));  	// 短信格式：TEXT
//DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    if (strstr(g_ucSim900Buff,"1") == NULL)
    {
        ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=1\r\n",strlen("AT+CMGF=1\r\n"));  	// 短信格式：TEXT
        //DebugOut("* Set SMS Format: TEXT.\r\n",0);
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    }

    ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,167,0,241\r\n",strlen("AT+CSMP=17,167,0,241\r\n"));  	// 短信格式：TEXT
    //DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

    ddi_uart_Write(&UART1_ctl_t,"AT+CSMP?\r",strlen("AT+CSMP?\r"));  	// 短信格式：TEXT
    //DebugOut("* Set SMS Format: TEXT.\r\n",0);
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

    if (strstr(g_ucSim900Buff,"17,167,0,241") == NULL)
    {
        ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,167,0,241\r\n",strlen("AT+CSMP=17,167,0,241\r\n"));  	// 短信格式：TEXT
        //DebugOut("* Set SMS Format: TEXT.\r\n",0);
        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    }


    ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"8859-1\"\r",strlen("AT+CSCS=\"8859-1\"\r"));  // 短信特殊字符显示才能正常
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
}
#endif
/*
**-----------------------------------------------------------------------------
**
**	概述:
**		发送信息到指定号码.
**
**	参数:
**		num -- 手机号码存放指针
**              msg -- 要发送信息内容
**              len -- 要发送信息长度
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
//ljw,20160903,for space
void deal_g_ucSim900Status(void)
{
    u16 i;
    for (i=0; i<500;i++)
	{
		if (BUSY == g_ucSim900Status)
		{
		   delayms(1);
		}
		else
			break;
	}
	g_ucSim900Status = BUSY;   // 更改SIM900模块状态
}

unsigned char SendMsg(unsigned char *num, unsigned char *msg, unsigned char len)
{
	unsigned char temp[160] = {'A','T','+','C','M','G','S','=','\"'};
	unsigned short i;
	unsigned char ret=1;
    u8 num_len = strlen(num);//ljw,20160924 for wulian
    if(num_len >= PHNUM_LEN-3) //ljw
        return 0;
	//unsigned char smsbuf[160];

    if(strlen(msg) >= 160)//ljw,20160907,for overflow
        return 0x5a;
//ljw,20160924	strncpy((char *)temp+9,(char *)num,11);
    strncpy((char *)temp+9,(char *)num,num_len);
	temp[num_len+9] = '\"';//temp[20] = '\"';
	temp[num_len+10] = '\r';//temp[21] = '\r';
    temp[num_len+11] = '\n';//temp[22] = '\n';

    deal_g_ucSim900Status();
#if 0 //ljw,20160903 for space
	for (i=0; i<500;i++)
	{
		if (BUSY == g_ucSim900Status)
		{
		   delayms(1);
		}
		else
			break;
	}
	g_ucSim900Status = BUSY;   // 更改SIM900模块状态
#endif
	ddi_uart_Write(&UART1_ctl_t,temp,strlen(temp));
    WaitGSMOK1();

    memset(temp,0,sizeof(temp));
	memcpy((char *)temp,(char *)msg,strlen(msg));
	temp[strlen(msg)] = 0x1A;
	
	ddi_uart_Write(&UART1_ctl_t,temp,strlen(msg)+1);   //LedFlash();
//wei,	ret= WaitGSMOK1();
    ret= WaitGSMOK2(500);//wei,20160717
	g_ucSim900Status =IDLE;    // 更改SIM900模块状态
	delayms(100);
	return ret;	
}

/* ljw,20160924 for wulian
unsigned char SendMsg(unsigned char *num, unsigned char *msg, unsigned char len)
{
	unsigned char temp[160] = {'A','T','+','C','M','G','S','=','\"'};
	unsigned short i;
	unsigned char ret=1;
	//unsigned char smsbuf[160];

    if(strlen(msg) >= 160)//ljw,20160907,for overflow
        return 0x5a;
	strncpy((char *)temp+9,(char *)num,11);
	temp[20] = '\"';
	temp[21] = '\r';
    temp[22] = '\n';

    deal_g_ucSim900Status();
#if 0 //ljw,20160903 for space
	for (i=0; i<500;i++)
	{
		if (BUSY == g_ucSim900Status)
		{
		   delayms(1);
		}
		else
			break;
	}
	g_ucSim900Status = BUSY;   // 更改SIM900模块状态
#endif
	ddi_uart_Write(&UART1_ctl_t,temp,strlen(temp));
    WaitGSMOK1();

    memset(temp,0,sizeof(temp));
	memcpy((char *)temp,(char *)msg,strlen(msg));
	temp[strlen(msg)] = 0x1A;
	
	ddi_uart_Write(&UART1_ctl_t,temp,strlen(msg)+1);   //LedFlash();
//wei,	ret= WaitGSMOK1();
    ret= WaitGSMOK2(500);//wei,20160717
	g_ucSim900Status =IDLE;    // 更改SIM900模块状态
	delayms(100);
	return ret;	
}
*/

//ljw,做成子函数，减少字符串常量重复出现，减少空间
u8 SendMsg_Parameter_error(void)
{
    SendMsg(g_ucCurrentNum,"Parameter error!",strlen("Parameter error"));
}
//ljw,做成子函数，减少字符串常量重复出现，减少空间
u8 SendMsg_Password_error(void)
{
    SendMsg(g_ucCurrentNum,"Password error!",strlen("password error!"));
}
unsigned char SendMsg_UCS2(unsigned char *num, unsigned char *msg, unsigned char len)
{
	//短消息单次最大可发送字节152。大于也可以自动分开发送
	unsigned char temp[] = {'A','T','+','C','M','G','S','=','\"'};
//ljw,20161018	unsigned char temp1[30];
    unsigned char temp1[64];//ljw,20161018,1位变2位  hex to ascii转换缓存
	unsigned short i;
	unsigned char ret=1;
    u8 num_len =strlen(num);//ljw,20160924 for wulian 
    if(num_len >=PHNUM_LEN-3)
        return 0;

	memset(UART1_tbuf,0,sizeof(UART1_tbuf));
	memcpy(UART1_tbuf,temp,9);
	memset(temp1,0,sizeof(temp1));
	//strncpy((char *)temp+9,(char *)num,11);
//ljw,20160924 for wulian	hextoascii(num,temp1,11);
    hextoascii(num,temp1,num_len);

//ljw,20160924 for wulian	for(i=0; i<11;i++)
    for(i=0; i<num_len;i++)
	{
		UART1_tbuf[9+i*4] = '0';
		UART1_tbuf[9+i*4+1] = '0';
		memcpy((char*)(UART1_tbuf+9+i*4+2),(char*)(temp1+2*i),2);
	}
    i = (4*(i-1));
    if(i > (UART1_tbuf_len-13))
        return 0;
//ljw,20160924 for wulian	UART1_tbuf[53] = '\"';
//ljw,20160924 for wulian	UART1_tbuf[54] = '\r';
//ljw,20160924 for wulian    UART1_tbuf[55] = '\n';
    UART1_tbuf[13+i] = '\"';
	UART1_tbuf[14+i] = '\r';
    UART1_tbuf[15+i] = '\n';

    deal_g_ucSim900Status();
#if 0 //ljw,20160903 for spac
	for (i=0; i<500;i++)
	{
		if (BUSY == g_ucSim900Status)
		{
		   delayms(1);
		}
		else
			break;
	}
	g_ucSim900Status = BUSY;   // 更改SIM900模块状态
#endif 
	
	ddi_uart_Write(&UART1_ctl_t,UART1_tbuf,strlen(UART1_tbuf));
    WaitGSMOK2(500);
//ljw,20160924    WaitGSMOK1();

    memset(UART1_tbuf,0,sizeof(UART1_tbuf));
	memcpy((char *)UART1_tbuf,(char *)msg,len);
	UART1_tbuf[len] = 0x1A;
	
	ddi_uart_Write(&UART1_ctl_t,UART1_tbuf,(len+1));   //LedFlash();
//wei,	ret= WaitGSMOK1();
    ret= WaitGSMOK2(500);//wei,20160717
//wei,20160809	delayms(5000);
	g_ucSim900Status =IDLE;    // 更改SIM900模块状态
	return ret;
	
}

/* ljw,20160924 for
//bigbuf.big.UART1_tbuf
unsigned char SendMsg_UCS2(unsigned char *num, unsigned char *msg, unsigned char len)
{
	//短消息单次最大可发送字节152。大于也可以自动分开发送
	unsigned char temp[] = {'A','T','+','C','M','G','S','=','\"'};
	unsigned char temp1[30] ;
	unsigned short i;
	unsigned char ret=1;

	memset(UART1_tbuf,0,sizeof(UART1_tbuf));
	memcpy(UART1_tbuf,temp,9);
	memset(temp1,0,sizeof(temp1));
	//strncpy((char *)temp+9,(char *)num,11);
	hextoascii(num,temp1,11);
	
	for(i=0; i<11;i++)
	{
		UART1_tbuf[9+i*4] = '0';
		UART1_tbuf[9+i*4+1] = '0';
		memcpy((char*)(UART1_tbuf+9+i*4+2),(char*)(temp1+2*i),2);
	}
	UART1_tbuf[53] = '\"';
	UART1_tbuf[54] = '\r';
    UART1_tbuf[55] = '\n';

    deal_g_ucSim900Status();
#if 0 //ljw,20160903 for spac
	for (i=0; i<500;i++)
	{
		if (BUSY == g_ucSim900Status)
		{
		   delayms(1);
		}
		else
			break;
	}
	g_ucSim900Status = BUSY;   // 更改SIM900模块状态
#endif 
	
	ddi_uart_Write(&UART1_ctl_t,UART1_tbuf,strlen(UART1_tbuf));
    WaitGSMOK1();

    memset(UART1_tbuf,0,sizeof(UART1_tbuf));
	memcpy((char *)UART1_tbuf,(char *)msg,len);
	UART1_tbuf[len] = 0x1A;
	
	ddi_uart_Write(&UART1_ctl_t,UART1_tbuf,(len+1));   //LedFlash();
//wei,	ret= WaitGSMOK1();
    ret= WaitGSMOK2(500);//wei,20160717
//wei,20160809	delayms(5000);
	g_ucSim900Status =IDLE;    // 更改SIM900模块状态

	return ret;
	
}
*/
//wei 获取count次GPS数据
//参数count :获取次数
//返回: 0,当前没定位，1，当前定位
u8 GetAvailableGPSInfoV_N(u8 count)
{
	u8 i=0;
	u8 j=0 ;

//	for(i=0;i<count;i++)
    for(;;)
	{
        GetAvailableGPSInfoV();
        if(dingwei_flag == 0x5a)
            return 1;
        i++;
        if(i>=count)
            return 0;
		delayms(700);
	}
    return 0;
}

/*wei  20160510
**-----------------------------------------------------------------------------
**
**	概述:
**		开始 GPRS 连接
**
**	参数:
**		无
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
//wei, 次函数是在原来的void StartGPRS(void)基础上修改成了可以打开两条通道的。
u8 SMS_StartGSM_Flag = 0 ;//wei,20160628,启动的时候处理GSM标志，避免处理完短信后再调用StartGPRS
void StartGPRS(void)
{
//wei        unsigned char temp[50] = "AT+QIOPEN=\"UDP\",\"";
	unsigned char temp[50+2] = "AT+QIOPEN=1,\"UDP\",\"";
	volatile unsigned char i=0;
	unsigned char lens;
	u8* pt=NULL;
	  u8 return_wait_s = 0x5a; //wei
     
//    if((system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))//满足休眠条件，要休眠
//    {
//        if((g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE)) //wei,20160709,满足睡眠条件，不要在继续拨号了
//            return ;
//    }
    if(0x05 == check_sleep_state())//ljw,20160908判断休眠条返回 0x05,休眠  0x0a不休眠
        return;
//ljw,20160903 for space       DebugOut("\r\nNow StartGPRS!!\r\n",0);
    
//wei,    ddi_uart_Write(&UART1_ctl_t,"AT+CCLK?\r",strlen("AT+CCLK?\r"));
//wei,    Deal_GSMRTC(500);//wei,20160812,处理GSM提供的时间基准，做为系统RTC时间
    
	  if ((lens = strlen((char *)system_para.g_ucIPAddress)) > 16)
	  	lens = 16;
      strncat((char *)temp,(char *)system_para.g_ucIPAddress,lens);
      strcat((char *)temp,"\",\"");
	  if ((lens = strlen((char *)system_para.g_ucUDPPort)) > 5)
	  	lens = 5;	
        strncat((char *)temp,(char *)system_para.g_ucUDPPort,lens);
        strcat((char *)temp,"\"\r"); 

#if 0 //wei,20160824 废话不要    20160906测完要删
	// 判断波特率是否为115200
	 ddi_uart_Write(&UART1_ctl_t,"AT+IPR?\r\n",strlen("AT+IPR?\r\n"));
	 WaitGSMOK(); 	
     if(strstr((char *)g_ucSim900Buff,"115200") == NULL) 
	 {
        ddi_uart_Write(&UART1_ctl_t,"AT+IPR=115200&W\r\n",strlen("AT+IPR=115200&W\r\n"));
        WaitGSMOK();
	 }
     UART1_ctl_t.Parity = Parity_No;
	 ddi_uart_open(&UART1_ctl_t,115200);
#endif //9600
#if 0//ljw 20160909 for space
//     ddi_uart_Write(&UART1_ctl_t,"AT+IPR?\r\n",strlen("AT+IPR?\r\n"));
//	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 	
//     if(strstr((char *)g_ucSim900Buff,"9600") == NULL)
//	 {
//        ddi_uart_Write(&UART1_ctl_t,"AT+IPR=9600&W\r\n",strlen("AT+IPR=9600&W\r\n"));
//        WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
//	 }
//     UART1_ctl_t.Parity = Parity_No;
//	 ddi_uart_open(&UART1_ctl_t,9600);
#endif

#if 0 //ljw,20160824 for space //WIE
	ddi_uart_Write(&UART1_ctl_t,"ATI\r\n",strlen("ATI\r\n"));
	WaitGSMOK();

	ddi_uart_Write(&UART1_ctl_t,"AT+QCFG=\"RI/Hopping\",1\r\n",strlen("AT+QCFG=\"RI/Hopping\",1\r\n"));
	WaitGSMOK();
#endif //WEI	
//    ddi_uart_Write(&UART1_ctl_t,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));
//    WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
    if(!GC65_CPIN())
        return  ;
#if 0
     if(!GC65_QINDI(NO_CACHE))//ljw,20161126,GPRS数据使用缓存。
        return ;
#endif
    ddi_uart_Write(&UART1_ctl_t,"AT+CSQ\r\n",strlen("AT+CSQ\r\n"));
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();


//        ddi_uart_Write(&UART1_ctl_t,"ATD18551090021;\r\n",strlen("ATD18551090021;\r\n"));
//	  WaitGSMOK();
//        DebugOut(g_ucSim900Buff,0);
	GetAvailableGPSInfoV_N(4);  //wei  GetAvailableGPSInfoV_N(7);
    if(!WaitGSM_ReadyCOPS())//ljw,20161108
        return ;

    if(!WaitGSM_ReadyNet())  //wei20160607
		return ; //wei20160607
    if(!GC65_CGATT())//ljw,20160909for space
        return;
#if 0 //ljw,20160909for space
	  for(i=0; i<20; i++)
	  {
		  ddi_uart_Write(&UART1_ctl_t,"AT+CGATT?\r",strlen("AT+CGATT?\r"));
	 	  WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 	
		  if(strstr((char *)g_ucSim900Buff,"+CGATT: 1") != NULL) 
		  {
		     i ++;
			i--;
		     break;	
		  }
          delayms(1000);//wei,20160829,之前没有这个延时
	  }
	  if( i <19)
	  {
		net_status = 1;
	  }
#endif
//   ddi_uart_Write(&UART1_ctl_t,"AT+COPS=?\r\n",strlen("AT+COPS=?\r\n"));
//	WaitGSMOK();  

    WaitGSM_ReadyCOPS();//wei,20160623
#if 0//wei,20160623for SMS<
	ddi_uart_Write(&UART1_ctl_t,"AT+COPS=0,2\r\n",strlen("AT+COPS=0,2\r\n"));
	WaitGSMOK();  	 
	GetAvailableGPSInfoV();	  
	delayms(1000);
	GetAvailableGPSInfoV();
	delayms(1000);	
      //查询当前运营商
    ddi_uart_Write(&UART1_ctl_t,"AT+COPS?\r\n",strlen("AT+COPS?\r\n"));
	WaitGSMOK();  
	pt = strstr(g_ucSim900Buff,"\"460");
	if(pt != NULL)
	{
		pt += 4;
		MobileCompany = *pt++ - '0';
		MobileCompany = *pt-'0' + MobileCompany*10;
	}
	if ( strstr(g_ucSim900Buff,"MOBILE")) //移动
	{
		MobileCompany = 0;
    }	
#endif //wei,20160623 for sms>	
	
	ddi_uart_Write(&UART1_ctl_t,"AT+QIFGCNT=0\r",strlen("AT+QIFGCNT=0\r"));
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

//wei   ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX=1\r",strlen("AT+QIMUX=1\r"));//wei
//wei	WaitGSMOK();//wei
//wei
//wei	ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
//wei	WaitGSMOK();

	if(!WaitGSM_ReadyQIMUX()) //wei,20160608
		return ;//wei,20160608
	

    if(CPIN_READY_flag == 0)//ljw,20160921,针对掉卡的现象做的处理
        return ;
    if(ring_sms == SMS)//wei,20160624,拨号前判断下有没有短信要处理
    {
        Configuration_TEXT_SMS();//wei,20160628,配置英文短信接收
        delayms(200);
        SMS_StartGSM_Flag = 0x5a;
        Deal_SMS();
        SMS_StartGSM_Flag = 0x00;
    }
	for(i=0;i<3;i++)
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+QIACT\r",strlen("AT+QIACT\r"));
		WaitGSMOK2(QIACT_WAIT);//wei 180s
		
		ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
		return_wait_s = WaitGSMOK2(QIOPEN_WAIT);//180s接收返回的缓冲时间
		if (return_wait_s != 0x05)//10s
		{
			if (return_wait_s == NULL)  //如果收到的是OK，还要再等一会
				return_wait_s = WaitGSMOK2(QIOPEN_WAIT);//wei 180s
			if (return_wait_s == 0x05)  //
			{	
				GSMLedOn();
				dial_status = 1;//数据通道打开成功，结束上级循环
				break ;
			}				
		}
		else
		{
			// 拨号成功，打开led指示灯
			GSMLedOn();
			dial_status = 1;
			break;
		}
	}
	if(dial_status !=1) //wei20160607
		return ;  //wei20160607
#if 0

	ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
	if (WaitGSMOK1())
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
		WaitGSMOK();

		ddi_uart_Write(&UART1_ctl_t,"AT+QIACT\r",strlen("AT+QIACT\r"));
		WaitGSMOK();
		ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
		WaitGSMOK();
	}
	else
	{
		// 拨号成功，打开led指示灯
		GSMLedOn();
		dial_status = 1;
	}
#endif
	ddi_uart_Write(&UART1_ctl_t,"AT+QISDE=0\r",strlen("AT+QISDE=0\r"));
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
	//zzg test cell id

	 ddi_uart_Write(&UART1_ctl_t,"AT+CREG=?\r\n",strlen("AT+CREG=?\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CREG=2\r\n",strlen("AT+CREG=2\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 	
	 ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r\n",strlen("AT+CREG?\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 			
	 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=?\r\n",strlen("AT+CGREG=?\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=2\r\n",strlen("AT+CGREG=2\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 	
 	 ddi_uart_Write(&UART1_ctl_t,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 

//test chinese sms
	 ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"GSM\"\r\n",strlen("AT+CSCS=\"GSM\"\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CSCA?\r\n",strlen("AT+CSCA?\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=0\r\n",strlen("AT+CMGF=0\r\n"));
	 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 

      ddi_uart_Write(&UART1_ctl_t,"AT+CPMS=\"SM\"\r",strlen("AT+CPMS=\"SM\"\r"));  	// //优选短信存储器为SIM卡
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();


      ddi_uart_Write(&UART1_ctl_t,"AT+CPMS?\r",strlen("AT+CPMS?\r"));  	// //优选短信存储器为SIM卡
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();


	
        ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=1\r",strlen("AT+CMGF=1\r"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();


        ddi_uart_Write(&UART1_ctl_t,"AT+CMGF?\r",strlen("AT+CMGF?\r"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();


	if (strstr(g_ucSim900Buff,"1") == NULL)
	{
	        ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=1\r\n",strlen("AT+CMGF=1\r\n"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
	}
		
//ljw,20160924	ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,167,0,241\r\n",strlen("AT+CSMP=17,167,0,241\r\n"));  	// 短信格式：TEXT
	ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,0\r\n",strlen("AT+CSMP=17,255,0,0\r\n"));  	// 短信格式：TEXT
    //DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

        ddi_uart_Write(&UART1_ctl_t,"AT+CSMP?\r",strlen("AT+CSMP?\r"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();


	if (strstr(g_ucSim900Buff,"17,255,0,0") == NULL) //17,167,0,241
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,0\r\n",strlen("AT+CSMP=17,255,0,0\r\n"));  	// 短信格式：TEXT
		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
	}


	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"8859-1\"\r",strlen("AT+CSCS=\"8859-1\"\r"));  // 短信特殊字符显示才能正常
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
#if 0
	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"GSM\"\r",strlen("AT+CSCS=\"GSM\"\r")); //GSM
	WaitGSMOK();


	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS?\r",strlen("AT+CSCS?\r")); //GSM
	WaitGSMOK();


	if (strstr(g_ucSim900Buff,"GSM") == NULL)
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"GSM\"\r",strlen("AT+CSCS=\"GSM\"\r")); //GSM		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK();
	}	
#endif	
//zzg test del all sms

 //  ddi_uart_Write(&UART1_ctl_t,"AT+QMGDA=\"DEL ALL\"\r",strlen("AT+QMGDA=\"DEL ALL\"\r")); //GSM
//	WaitGSMOK();

	
    //  SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));

#ifdef CHINESE_SMS
	//SEND CHINESE
	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r")); //GSM
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();


	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS?\r",strlen("AT+CSCS?\r")); //GSM
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();	

	if (strstr(g_ucSim900Buff,"UCS2") == NULL)
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r")); //GSM		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
	}
	ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=49,167,0,25\r\n",strlen("AT+CSMP=49,167,0,25\r\n"));  	// 短信格式：TEXT
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

	ddi_uart_Write(&UART1_ctl_t,"AT+CNMI=2,1,0,0,0\r\n",strlen("AT+CNMI=2,1,0,0,0\r\n"));  	// 短信格式：TEXT
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();	
	SendMsg_UCS2(g_ucCurrentNum,"60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D",
		                                   strlen("60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D"));
	delayms(1000);
#endif
#if 0//ljw,20160903for space
	ddi_uart_Write(&UART1_ctl_t,"AT+CBC\r\n",strlen("AT+CBC\r\n"));   //LedFlash();
	WaitGSMOK();

        //关闭回显
	 ddi_uart_Write(&UART1_ctl_t,"ATE0\r\n",strlen("ATE0\r\n"));
	 WaitGSMOK(); 	
#endif  	
}


/*
**-----------------------------------------------------------------------------
**
**	概述:
**		开始 GPRS 连接
**
**	参数:
**		无
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
#if 0 //wei
void StartGPRS(void)
{

        unsigned char temp[50] = "AT+QIOPEN=\"UDP\",\"";
        volatile unsigned char i=0;
	  unsigned char lens;
	  u8* pt=NULL;
	  
        DebugOut("\r\nNow StartGPRS!!\r\n",0);
//WIE
//ddi_uart_Write(&UART1_ctl_t,"ATI\r\n",strlen("ATI\r\n"));
//WaitGSMOK();

//ddi_uart_Write(&UART1_ctl_t,"AT+QCFG=\"RI/Hopping\",1\r\n",strlen("AT+QCFG=\"RI/Hopping\",1\r\n"));
//WaitGSMOK();
//WEI	
	  if ((lens = strlen((char *)system_para.g_ucIPAddress)) > 16)
	  	lens = 16;
	  
        strncat((char *)temp,(char *)system_para.g_ucIPAddress,lens);
        strcat((char *)temp,"\",\"");
	  if ((lens = strlen((char *)system_para.g_ucUDPPort)) > 5)
	  	lens = 5;			
        strncat((char *)temp,(char *)system_para.g_ucUDPPort,lens);
        strcat((char *)temp,"\"\r"); 
			

	// 判断波特率是否为115200
	 ddi_uart_Write(&UART1_ctl_t,"AT+IPR?\r\n",strlen("AT+IPR?\r\n"));
	 WaitGSMOK(); 	
     	 if(strstr((char *)g_ucSim900Buff,"115200") == NULL) 
	{
	        ddi_uart_Write(&UART1_ctl_t,"AT+IPR=115200&W\r\n",strlen("AT+IPR=115200&W\r\n"));
		 WaitGSMOK(); 		
	}
	 UART1_ctl_t.Parity = Parity_No;
	 ddi_uart_open(&UART1_ctl_t,115200);	 
       ddi_uart_Write(&UART1_ctl_t,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));
	 WaitGSMOK(); 

        ddi_uart_Write(&UART1_ctl_t,"AT+CSQ\r\n",strlen("AT+CSQ\r\n"));

        WaitGSMOK();

//WIE
ddi_uart_Write(&UART1_ctl_t,"ATI\r\n",strlen("ATI\r\n"));
WaitGSMOK();

ddi_uart_Write(&UART1_ctl_t,"AT+QCFG=\"RI/Hopping\",1\r\n",strlen("AT+QCFG=\"RI/Hopping\",1\r\n"));
WaitGSMOK();
//WEI
//        ddi_uart_Write(&UART1_ctl_t,"ATD18551090021;\r\n",strlen("ATD18551090021;\r\n"));
//	  WaitGSMOK();
//        DebugOut(g_ucSim900Buff,0);
    //delayms(4000);
	  delayms(1000);
    GetAvailableGPSInfoV();
	  delayms(1000);
    GetAvailableGPSInfoV();
	  delayms(1000);
    GetAvailableGPSInfoV();	 
	  delayms(1000);
    GetAvailableGPSInfoV();	  
		delayms(1000);
    GetAvailableGPSInfoV();
	  delayms(1000);	
		GetAvailableGPSInfoV();	  
	  delayms(1000);
        GetAvailableGPSInfoV();
	  delayms(1000);	
	  for(i=0; i<20; i++)
	  {
		  ddi_uart_Write(&UART1_ctl_t,"AT+CGATT?\r",strlen("AT+CGATT?\r"));
	 	  WaitGSMOK(); 	
		  if(strstr((char *)g_ucSim900Buff,"+CGATT: 1") != NULL) 
		  {
		     i ++;
			i--;
		     break;	
		  }
	  }
	  if( i <19)
	  {
		net_status = 1;
	  }
//   	ddi_uart_Write(&UART1_ctl_t,"AT+COPS=?\r\n",strlen("AT+COPS=?\r\n"));
//	WaitGSMOK();  

	ddi_uart_Write(&UART1_ctl_t,"AT+COPS=0,2\r\n",strlen("AT+COPS=0,2\r\n"));
	WaitGSMOK();  	 
	GetAvailableGPSInfoV();	  
	delayms(1000);
	GetAvailableGPSInfoV();
	delayms(1000);	
      //查询当前运营商
  ddi_uart_Write(&UART1_ctl_t,"AT+COPS?\r\n",strlen("AT+COPS?\r\n"));
	WaitGSMOK();  
	pt = strstr(g_ucSim900Buff,"\"460");
	if(pt != NULL)
	{
		pt += 4;
		MobileCompany = *pt++ - '0';
		MobileCompany = *pt-'0' + MobileCompany*10;

	}
	if ( strstr(g_ucSim900Buff,"MOBILE")) //移动
	{
		MobileCompany = 0;
  }	
	
	
	ddi_uart_Write(&UART1_ctl_t,"AT+QIFGCNT=0\r",strlen("AT+QIFGCNT=0\r"));
	WaitGSMOK();



	ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
	WaitGSMOK();

	ddi_uart_Write(&UART1_ctl_t,"AT+QIACT\r",strlen("AT+QIACT\r"));
	WaitGSMOK();


	ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
	if (WaitGSMOK1())
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
		WaitGSMOK();

		ddi_uart_Write(&UART1_ctl_t,"AT+QIACT\r",strlen("AT+QIACT\r"));
		WaitGSMOK();
		ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
		WaitGSMOK();
	}
	else
	{
		// 拨号成功，打开led指示灯
		GSMLedOn();
		dial_status = 1;
	}

	ddi_uart_Write(&UART1_ctl_t,"AT+QISDE=0\r",strlen("AT+QISDE=0\r"));
	WaitGSMOK();
	//zzg test cell id

	 ddi_uart_Write(&UART1_ctl_t,"AT+CREG=?\r\n",strlen("AT+CREG=?\r\n"));
	 WaitGSMOK(); 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CREG=2\r\n",strlen("AT+CREG=2\r\n"));
	 WaitGSMOK(); 	
	 ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r\n",strlen("AT+CREG?\r\n"));
	 WaitGSMOK(); 			
	 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=?\r\n",strlen("AT+CGREG=?\r\n"));
	 WaitGSMOK(); 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CGREG=2\r\n",strlen("AT+CGREG=2\r\n"));
	 WaitGSMOK(); 	
 	 ddi_uart_Write(&UART1_ctl_t,"AT+CGREG?\r\n",strlen("AT+CGREG?\r\n"));
	 WaitGSMOK(); 

//test chinese sms
	 ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"GSM\"\r\n",strlen("AT+CSCS=\"GSM\"\r\n"));
	 WaitGSMOK(); 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CSCA?\r\n",strlen("AT+CSCA?\r\n"));
	 WaitGSMOK(); 
	 ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=0\r\n",strlen("AT+CMGF=0\r\n"));
	 WaitGSMOK(); 

      ddi_uart_Write(&UART1_ctl_t,"AT+CPMS=\"SM\"\r",strlen("AT+CPMS=\"SM\"\r"));  	// //优选短信存储器为SIM卡
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK();


      ddi_uart_Write(&UART1_ctl_t,"AT+CPMS?\r",strlen("AT+CPMS?\r"));  	// //优选短信存储器为SIM卡
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK();


	
        ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=1\r",strlen("AT+CMGF=1\r"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK();


        ddi_uart_Write(&UART1_ctl_t,"AT+CMGF?\r",strlen("AT+CMGF?\r"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK();


	if (strstr(g_ucSim900Buff,"1") == NULL)
	{
	        ddi_uart_Write(&UART1_ctl_t,"AT+CMGF=1\r\n",strlen("AT+CMGF=1\r\n"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK();
	}
		
	ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,167,0,241\r\n",strlen("AT+CSMP=17,167,0,241\r\n"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK();

        ddi_uart_Write(&UART1_ctl_t,"AT+CSMP?\r",strlen("AT+CSMP?\r"));  	// 短信格式：TEXT
	//DebugOut("* Set SMS Format: TEXT.\r\n",0);
	WaitGSMOK();


	if (strstr(g_ucSim900Buff,"17,167,0,241") == NULL)
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,167,0,241\r\n",strlen("AT+CSMP=17,167,0,241\r\n"));  	// 短信格式：TEXT
		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK();
	}


	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"8859-1\"\r",strlen("AT+CSCS=\"8859-1\"\r"));  // 短信特殊字符显示才能正常
	WaitGSMOK();
#if 0
	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"GSM\"\r",strlen("AT+CSCS=\"GSM\"\r")); //GSM
	WaitGSMOK();


	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS?\r",strlen("AT+CSCS?\r")); //GSM
	WaitGSMOK();

	if (strstr(g_ucSim900Buff,"GSM") == NULL)
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"GSM\"\r",strlen("AT+CSCS=\"GSM\"\r")); //GSM		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK();
	}	
#endif	
//zzg test del all sms

 //  ddi_uart_Write(&UART1_ctl_t,"AT+QMGDA=\"DEL ALL\"\r",strlen("AT+QMGDA=\"DEL ALL\"\r")); //GSM
//	WaitGSMOK();

	
    //  SendMsg(g_ucCurrentNum,"Wrong number!",strlen("Wrong number!"));

#ifdef CHINESE_SMS
	//SEND CHINESE
	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r")); //GSM
	WaitGSMOK();


	ddi_uart_Write(&UART1_ctl_t,"AT+CSCS?\r",strlen("AT+CSCS?\r")); //GSM
	WaitGSMOK();	

	if (strstr(g_ucSim900Buff,"UCS2") == NULL)
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r")); //GSM		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK();
	}
	ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=49,167,0,25\r\n",strlen("AT+CSMP=49,167,0,25\r\n"));  	// 短信格式：TEXT
	WaitGSMOK();

	ddi_uart_Write(&UART1_ctl_t,"AT+CNMI=2,1,0,0,0\r\n",strlen("AT+CNMI=2,1,0,0,0\r\n"));  	// 短信格式：TEXT
	WaitGSMOK();	
	SendMsg_UCS2(g_ucCurrentNum,"60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D",
		                                   strlen("60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D60A8597D"));
//wei,	delayms(1000);
#endif
	ddi_uart_Write(&UART1_ctl_t,"AT+CBC\r\n",strlen("AT+CBC\r\n"));   //LedFlash();
	WaitGSMOK();

        //关闭回显
	 ddi_uart_Write(&UART1_ctl_t,"ATE0\r\n",strlen("ATE0\r\n"));
	 WaitGSMOK(); 	
	
}
#endif //wei

/*  wei
**-----------------------------------------------------------------------------
**@ Date            - 2016/05/10 16:35:35 $
**@ Author  wei
**	概述:
**		建立到AGPS的TCP连接
**
**-----------------------------------------------------------------------------
*/
void StartGPRStoAGPS(void)
{
	 // unsigned char temp[50] = "AT+QIOPEN=\"UDP\",\"";
//wei		unsigned char temp[50] = "AT+QIOPEN=\"TCP\",\"";
    unsigned char temp[50+2] = {0};//ljw,20161201  兼容两个AGPS
#ifdef ZHONGKEWEI_GPS //ljw,20161125,中科微的GPS模块  
//	unsigned char temp[50+2] = "AT+QIOPEN=2,\"TCP\",\"";
#endif  //ljw,20161125,中科微的GPS模块  
#ifdef UM220_GPS
//    unsigned char temp[50+2] = "AT+QIOPEN=2,\"TCP\",\"";
#endif
	volatile unsigned char i=0;
	unsigned char lens;
	u8 return_wait_s = 0x5a; //wei
	
//ljw,20160903 for space	DebugOut("\r\nNow StartGPRS!!\r\n",0);
#if 0//WIE
	ddi_uart_Write(&UART1_ctl_t,"ATI\r\n",strlen("ATI\r\n"));
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

	ddi_uart_Write(&UART1_ctl_t,"AT+QCFG=\"RI/Hopping\",1\r\n",strlen("AT+QCFG=\"RI/Hopping\",1\r\n"));
	WaitGSMOK();
#endif
//WEI		
//wei20160625	  if ((lens = strlen((char *)system_para.g_ucIPAddress)) > 16)
//wei20160625	  	lens = 16;

//ljw,20161201没用    if ((lens = strlen((char *)system_para.g_ucIPAddress)) > IPADD_LEN)
//ljw,20161201没用        lens = IPADD_LEN-1;
#ifdef ZHONGKEWEI_GPS //ljw,20161125,中科微的GPS模块	  
		strncat((char *)temp,"121.41.40.95",strlen("121.41.40.95"));
#endif
#ifdef UM220_GPS //ljw,20161125,中科微的GPS模块	
    if(GPS_module == UM220)
    {
        strcpy(temp,"AT+QIOPEN=2,\"UDP\",\"");
        strncat((char *)temp,"114.242.203.140",strlen("114.242.203.140"));
    }
    else
    {
        strcpy(temp,"AT+QIOPEN=2,\"TCP\",\"");
        strncat((char *)temp,"121.41.40.95",strlen("121.41.40.95"));
    }
//    strncat((char *)temp,"114.242.203.140",strlen("114.242.203.140"));
#endif
		//strncat((char *)temp,"agps.u-blox.com",strlen("agps.u-blox.com"));
		strcat((char *)temp,"\",\"");
//ljw,20161201没用	if((lens = strlen((char *)system_para.g_ucUDPPort)) > 5)
//ljw,20161201没用	  	lens = 5;
       // strncat((char *)temp,"46434",strlen("46434"));
#ifdef ZHONGKEWEI_GPS //ljw,20161125,中科微的GPS模块	        
        strncat((char *)temp,"2621",strlen("2621"));   
#endif
#ifdef UM220_GPS //ljw,20161125,中科微的GPS模块	
    if(GPS_module == UM220)
        strncat((char *)temp,"8000",strlen("8000"));
    else
        strncat((char *)temp,"2621",strlen("2621"));
#endif      
        strcat((char *)temp,"\"\r"); 
			
#if 0 //115200  wei,20160824,  ljw,20160906测完要删
	// 判断波特率是否为115200
	 ddi_uart_Write(&UART1_ctl_t,"AT+IPR?\r\n",strlen("AT+IPR?\r\n"));
	 WaitGSMOK(); 
/*//wei	
	 if(strstr((char *)g_ucSim900Buff,"9600") == NULL) 
	{
	        ddi_uart_Write(&UART1_ctl_t,"AT+IPR=9600\r\n",strlen("AT+IPR=9600\r\n"));
		 WaitGSMOK(); 		
	}*/
	if(strstr((char *)g_ucSim900Buff,"115200") == NULL) 
	{
	   ddi_uart_Write(&UART1_ctl_t,"AT+IPR=115200\r\n",strlen("AT+IPR=115200\r\n"));
		 WaitGSMOK(); 		
	}

	UART1_ctl_t.Parity = Parity_No;
//wei	ddi_uart_open(&UART1_ctl_t,9600);
	ddi_uart_open(&UART1_ctl_t,115200);
//#else //9600 ,wei,20160826for AGPS
#endif
#if 0 //ljw,20160909 for space
    ddi_uart_Write(&UART1_ctl_t,"AT+IPR?\r\n",strlen("AT+IPR?\r\n"));
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    if(strstr((char *)g_ucSim900Buff,"9600") == NULL) 
	{
	   ddi_uart_Write(&UART1_ctl_t,"AT+IPR=9600\r\n",strlen("AT+IPR=115200\r\n"));
		 WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 		
	}
#endif
    
#if 0//WIE
	ddi_uart_Write(&UART1_ctl_t,"ATI\r\n",strlen("ATI\r\n"));
	WaitGSMOK();

	ddi_uart_Write(&UART1_ctl_t,"AT+QCFG=\"RI/Hopping\",1\r\n",strlen("AT+QCFG=\"RI/Hopping\",1\r\n"));
	WaitGSMOK();
#endif //WEI	
//  ddi_uart_Write(&UART1_ctl_t,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));
//	WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 
    if(!GC65_CPIN()) //ljw,20160908for CPIN NOT READY
        return  ;
	ddi_uart_Write(&UART1_ctl_t,"AT+QISDE=0\r",strlen("AT+QISDE=0\r"));
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
#if 0//ljw,20161202,这个原来是使用不主动上报数据的
    if(!GC65_QINDI(CACHE))//ljw,20161126,GPRS数据使用缓存。
        return ;
#endif
    
#if 0//ljw,20160903 for space
	ddi_uart_Write(&UART1_ctl_t,"AT+CBC\r\n",strlen("AT+CBC\r\n"));   //LedFlash();
	WaitGSMOK();

    //关闭回显
    ddi_uart_Write(&UART1_ctl_t,"ATE0\r\n",strlen("ATE0\r\n"));
    WaitGSMOK(); 
#endif //ljw,20160903   for space
    ddi_uart_Write(&UART1_ctl_t,"AT+CSQ\r\n",strlen("AT+CSQ\r\n"));
    WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

//    ddi_uart_Write(&UART1_ctl_t,"ATD18551090021;\r\n",strlen("ATD18551090021;\r\n"));
//	 WaitGSMOK();
//    DebugOut(g_ucSim900Buff,0);
    if(!WaitGSM_ReadyCOPS())//ljw,20161108
        return ;
    GetAvailableGPSInfoV_N(4);  //wei GetAvailableGPSInfoV_N(7);

    if(!WaitGSM_ReadyNet()) //wei,20160608
        return ;//wei,20160608
    if(!GC65_CGATT())//ljw,20160909 for space
        return;
#if 0 //ljw,20160909 for space
	  for(i=0; i<20; i++)
	  {
		  ddi_uart_Write(&UART1_ctl_t,"AT+CGATT?\r",strlen("AT+CGATT?\r"));
	 	  WaitGSMOK1();//ljw20160903for space   WaitGSMOK(); 	
		  if(strstr((char *)g_ucSim900Buff,"+CGATT: 1") != NULL) 
		  {
		     i ++;
			i--;
		     break;	
		  }
	  }
	  if( i <19)
	  {
			net_status = 1;
	  }
#endif //ljw,20160909
      GetAvailableGPSInfoV_N(2);
      
//		GetAvailableGPSInfoV();	  
//	  delayms(1000);
//    GetAvailableGPSInfoV();
//	  delayms(1000);	

// 设置使用域名的方式
//	 ddi_uart_Write(&UART1_ctl_t,"AT+QIDNSIP=1\r\n",strlen("AT+QIDNSIP=1\r\n"));
//	WaitGSMOK();
	
	ddi_uart_Write(&UART1_ctl_t,"AT+QIFGCNT=0\r",strlen("AT+QIFGCNT=0\r"));
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
    SET_APN();//wei,20160625
    WaitGSMOK2(500);
      
//收到ｇｐｒｓ数据后要手动去读取
//	ddi_uart_Write(&UART1_ctl_t,"AT+QINDI=1\r",strlen("AT+QINDI=1\r"));
//	WaitGSMOK();
//wei
//wei	ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX=1\r",strlen("AT+QIMUX=1\r"));
//wei	WaitGSMOK();
//wei
		
//wei	ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
//wei	WaitGSMOK();
	WaitGSM_ReadyCOPS();//wei,20160623
    
	if(!WaitGSM_ReadyQIMUX()) //wei,20160608
		return ;//wei,20160608
	if(CPIN_READY_flag == 0)//ljw,20160921,针对掉卡的现象做的处理
        return ;
	for(i=0;i<3;i++)
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+QIACT\r",strlen("AT+QIACT\r"));
		WaitGSMOK2(QIACT_WAIT);//wei 180s
		
		ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
		return_wait_s = WaitGSMOK2(QIOPEN_WAIT);//180s接收返回的缓冲时间
		if (return_wait_s != 0x05)//10s
		{
			if (return_wait_s == NULL)  //如果收到的是OK，还要再等一会
				return_wait_s = WaitGSMOK2(QIOPEN_WAIT);//wei 180s
			if (return_wait_s == 0x05)  //
			{	
				GSMLedOn();
				dial_status = 1;//数据通道打开成功，结束上级循环
				break ;
			}				
		}
		else
		{
			// 拨号成功，打开led指示灯
			GSMLedOn();
			dial_status = 1;
			break;
		}
	}
	if(dial_status != 1)
		return ;
#if 0

	ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
	if (WaitGSMOK1())
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
		WaitGSMOK();

		ddi_uart_Write(&UART1_ctl_t,"AT+QIACT\r",strlen("AT+QIACT\r"));
		WaitGSMOK();
		ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
		WaitGSMOK();
	}
	else
	{
		// 拨号成功，打开led指示灯
		GSMLedOn();
		dial_status = 1;
	}
#endif
}

#if 0 //wei
void StartGPRStoAGPS(void)
{

       // unsigned char temp[50] = "AT+QIOPEN=\"UDP\",\"";
        unsigned char temp[50] = "AT+QIOPEN=\"TCP\",\"";
        volatile unsigned char i=0;
	  unsigned char lens;
	  
	  
        DebugOut("\r\nNow StartGPRS!!\r\n",0);
//WIE
ddi_uart_Write(&UART1_ctl_t,"ATI\r\n",strlen("ATI\r\n"));
WaitGSMOK();

ddi_uart_Write(&UART1_ctl_t,"AT+QCFG=\"RI/Hopping\",1\r\n",strlen("AT+QCFG=\"RI/Hopping\",1\r\n"));
WaitGSMOK();
//WEI		
	  if ((lens = strlen((char *)system_para.g_ucIPAddress)) > 16)
	  	lens = 16;
	  
        strncat((char *)temp,"121.41.40.95",strlen("121.41.40.95"));
        //strncat((char *)temp,"agps.u-blox.com",strlen("agps.u-blox.com"));
        strcat((char *)temp,"\",\"");
	  if ((lens = strlen((char *)system_para.g_ucUDPPort)) > 5)
	  	lens = 5;			
       // strncat((char *)temp,"46434",strlen("46434"));
        strncat((char *)temp,"2621",strlen("2621"));
       
        strcat((char *)temp,"\"\r"); 
			

	// 判断波特率是否为115200
	 ddi_uart_Write(&UART1_ctl_t,"AT+IPR?\r\n",strlen("AT+IPR?\r\n"));
	 WaitGSMOK(); 	
     	 if(strstr((char *)g_ucSim900Buff,"9600") == NULL) 
	{
	        ddi_uart_Write(&UART1_ctl_t,"AT+IPR=9600\r\n",strlen("AT+IPR=9600\r\n"));
		 WaitGSMOK(); 		
	}
	UART1_ctl_t.Parity = Parity_No;
	ddi_uart_open(&UART1_ctl_t,9600);
	
       ddi_uart_Write(&UART1_ctl_t,"AT+CPIN?\r\n",strlen("AT+CPIN?\r\n"));
	 WaitGSMOK(); 
		  
	ddi_uart_Write(&UART1_ctl_t,"AT+QISDE=0\r",strlen("AT+QISDE=0\r"));
	WaitGSMOK();


	ddi_uart_Write(&UART1_ctl_t,"AT+CBC\r\n",strlen("AT+CBC\r\n"));   //LedFlash();
	WaitGSMOK();

        //关闭回显
	 ddi_uart_Write(&UART1_ctl_t,"ATE0\r\n",strlen("ATE0\r\n"));
	 WaitGSMOK(); 	
       ddi_uart_Write(&UART1_ctl_t,"AT+CSQ\r\n",strlen("AT+CSQ\r\n"));
       WaitGSMOK();

//    ddi_uart_Write(&UART1_ctl_t,"ATD18551090021;\r\n",strlen("ATD18551090021;\r\n"));
//	 WaitGSMOK();
//    DebugOut(g_ucSim900Buff,0);
	  delayms(1000);
    GetAvailableGPSInfoV();
	  delayms(1000);
    GetAvailableGPSInfoV();
	  delayms(1000);
    GetAvailableGPSInfoV();	 
	  delayms(1000);
    GetAvailableGPSInfoV();	  
		delayms(1000);
    GetAvailableGPSInfoV();
	  delayms(1000);	
		GetAvailableGPSInfoV();	  
	  delayms(1000);
    GetAvailableGPSInfoV();
	  delayms(1000);	
	  for(i=0; i<20; i++)
	  {
		  ddi_uart_Write(&UART1_ctl_t,"AT+CGATT?\r",strlen("AT+CGATT?\r"));
	 	  WaitGSMOK(); 	
		  if(strstr((char *)g_ucSim900Buff,"+CGATT: 1") != NULL) 
		  {
		     i ++;
			i--;
		     break;	
		  }
	  }
	  if( i <19)
	  {
		net_status = 1;
	  }
		GetAvailableGPSInfoV();	  
	  delayms(1000);
    GetAvailableGPSInfoV();
	  delayms(1000);	

// 设置使用域名的方式
//	 ddi_uart_Write(&UART1_ctl_t,"AT+QIDNSIP=1\r\n",strlen("AT+QIDNSIP=1\r\n"));
//	WaitGSMOK();
	
	ddi_uart_Write(&UART1_ctl_t,"AT+QIFGCNT=0\r",strlen("AT+QIFGCNT=0\r"));
	WaitGSMOK();

//收到ｇｐｒｓ数据后要手动去读取
//	ddi_uart_Write(&UART1_ctl_t,"AT+QINDI=1\r",strlen("AT+QINDI=1\r"));
//	WaitGSMOK();
	
	ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
	WaitGSMOK();

	ddi_uart_Write(&UART1_ctl_t,"AT+QIACT\r",strlen("AT+QIACT\r"));
	WaitGSMOK();


	ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
	if (WaitGSMOK1())
	{
		ddi_uart_Write(&UART1_ctl_t,"AT+QIMUX?\r",strlen("AT+QIMUX?\r"));
		WaitGSMOK();

		ddi_uart_Write(&UART1_ctl_t,"AT+QIACT\r",strlen("AT+QIACT\r"));
		WaitGSMOK();
		ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)temp));
		WaitGSMOK();
	}
	else
	{
		// 拨号成功，打开led指示灯
		GSMLedOn();
		dial_status = 1;
	}	
}
#endif

/*  wei
**-----------------------------------------------------------------------------
**@ Date            - 2016/05/10 15:48:35 $
**@ Author  wei
**	概述:
**		UDP方式 发送数据
**
**	参数:
**		data 	源数据地址
**		len 	数据长度
**		index 数据通道
**
**	返回:
**		SEND_OK  	发送成功
**		SEND_FAIL	发送失败
**
**-----------------------------------------------------------------------------
*/
//wei  unsigned char UDPSendData_Index(unsigned char *data,unsigned char len)在这个函数基础上添加通道选择参数
unsigned char UDPSendData_Index(unsigned char *data,unsigned char len,unsigned char index)
{
	const unsigned char head[]="AT+QISEND\r";
	unsigned char tmpdata[16+2];	
	unsigned short i;
	unsigned char ret=SEND_OK;
	u8 ascii_len[2];
 	
	memset(tmpdata,0,16+2);
//ljw,20160909 for space<
    deal_g_ucSim900Status();//ljw,20160909 for space
//	for(i=0; i<300; i++)
//	{
//		if (BUSY == g_ucSim900Status)
//		{
//		   delayms(1);
////ljw,20160906for space		   if (GPRSCmdFlag == 1)
////ljw,20160906for space		   	return SEND_FAIL;
//		}
//		else
//		 break;	
//	}

      //发送长度转成ASCII,以指定长度发送
	 if ((len >9) && (len <100))
	 	hex2BCD_ASCII(ascii_len,len);

//ljw,20160909 for space	g_ucSim900Status = BUSY;
//ljw,20160909 for space >
     
//wei	memcpy(&tmpdata,"AT+QISEND=",10);	 
//wei	 
	 if(index == AGPS_SERVERS)
	   memcpy(&tmpdata,"AT+QISEND=2,",10+2);
	 else if(index == FHT_SERVERS)
		 memcpy(&tmpdata,"AT+QISEND=1,",10+2);
//wei
	if (len >99)
	{
		tmpdata[10+2] = len/100+'0';
		tmpdata[11+2] = '0' + (len %100)/10;
		tmpdata[12+2] = '0'+len%10;
		tmpdata[13+2] = 0x0d;	
		tmpdata[14+2] = 0x0a;	
		
	}
	else if ((len >9) && (len <100))
	{
		tmpdata[10+2] = ascii_len[0];
		tmpdata[11+2] = ascii_len[1];
		tmpdata[12+2] = 0x0d;
		tmpdata[13+2] = 0x0a;	
		
	}
	else
	{
		tmpdata[10+2] = ascii_len[0];
		tmpdata[11+2] = 0x0d;
		tmpdata[12+2] = 0x0a;	
	}
	ddi_uart_Write(&UART1_ctl_t,tmpdata,strlen(tmpdata));
	WaitGSMOK1();

	ddi_uart_Write(&UART1_ctl_t,data,len);
	
	ret =WaitGSMOK1(); 
	
	
 	g_ucSim900Status = IDLE;
	//if(strstr((char *)g_ucSim900Buff,"OK") != NULL)
	if (ret == 0)
	{
		//ddi_uart_Write(&UART1_ctl_t,"SEND_OK\r",strlen("SEND_OK\r"));	
		return SEND_OK;
	}
	else
	{
		return SEND_FAIL;
	}
}


//#if 0 //wei
/*
**-----------------------------------------------------------------------------
**
**	概述:
**		UDP方式 发送数据
**
**	参数:
**		data 	源数据地址
**		len 	数据长度
**
**	返回:
**		SEND_OK  	发送成功
**		SEND_FAIL	发送失败
**
**-----------------------------------------------------------------------------
*/
unsigned char UDPSendData(unsigned char *data,unsigned char len)
{
	const unsigned char head[]="AT+QISEND\r";
	unsigned char tmpdata[16];	
	unsigned short i;
	unsigned char ret=SEND_OK;
	u8 ascii_len[2];

 	
	memset(tmpdata,0,16);
	for(i=0; i<300; i++)
	{
		if (BUSY == g_ucSim900Status)
		{
		   delayms(1);
//ljw,20160906for space		   if (GPRSCmdFlag == 1)
//ljw,20160906for space		   	return SEND_FAIL;
		}
		else
		 break;	
	}
	
      //发送长度转成ASCII,以指定长度发送
    if ((len >9) && (len <100))
        hex2BCD_ASCII(ascii_len,len);

	g_ucSim900Status = BUSY;

	memcpy(&tmpdata,"AT+QISEND=",10);
	
	if (len >99)
	{
		tmpdata[10] = len/100+'0';
		tmpdata[11] = '0' + (len %100)/10;
		tmpdata[12] = '0'+len%10;
		tmpdata[13] = 0x0d;	
		tmpdata[14] = 0x0a;	
		
	}
	else if ((len >9) && (len <100))
	{
		tmpdata[10] = ascii_len[0];
		tmpdata[11] = ascii_len[1];
		tmpdata[12] = 0x0d;
		tmpdata[13] = 0x0a;	
		
	}
	else
	{
		tmpdata[10] = ascii_len[0];
		tmpdata[11] = 0x0d;
		tmpdata[12] = 0x0a;	
	}
	ddi_uart_Write(&UART1_ctl_t,tmpdata,strlen(tmpdata));
	WaitGSMOK1();

	ddi_uart_Write(&UART1_ctl_t,data,len);
	
	ret =WaitGSMOK1(); 
	
	
 	g_ucSim900Status = IDLE;
	//if(strstr((char *)g_ucSim900Buff,"OK") != NULL)
	if (ret == 0)
	{
		//ddi_uart_Write(&UART1_ctl_t,"SEND_OK\r",strlen("SEND_OK\r"));	
		return SEND_OK;
	}
	else
	{
		return SEND_FAIL;
	}
}
//#endif  //wei
/*
**-----------------------------------------------------------------------------
**
**	概述:
**		关闭 GPRS 连接
**
**	参数:
**		无
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
void StopGPRS(void)
{
	
}

#if 0
void UpLoadMID(void)
{
	unsigned char temp[8];

	memset(temp,0,sizeof(temp));
	temp[0] = '*';
	temp[7] = '#';
	if (GPRSCmdFlag == 1)
		return;
 //	EI();
	strncpy((char *)(temp+1),(char *)system_para.g_ucDeviceID,6);
	if ( UDPSendData(temp,sizeof(temp)))
	{
		;
	}
	
	
}
#endif 
/*
**-----------------------------------------------------------------------------
**
**	概述:
**		获取GPRS连接状态
**
**	参数:
**		无
**
**	返回:
**		GPRS_CONNECTED  	GPRS 连接可用
**		GPRS_DISCONNECTED	GPRS 连接不可用
**
**+QISTATE: 2, "TCP", "121.41.40.95", 2621, "CONNECTED"
**+QISTATE: 1, "UDP", "200.100.000.001", 1, "CONNECTED"
**-----------------------------------------------------------------------------
*/
//wei,20160621,for check GPRS status at condition AT+QIMUX=1
unsigned char GetGPRSStatus(void)
{
	unsigned char i;
    
    u8 *p_temp =NULL;
//ljw,20160909 for space
//	for(i=0; i<20; i++)
//	{
//		if (BUSY == g_ucSim900Status)
//		{
//		   delayms(10);
//		}
//		else
//			break;	
//	}
//	g_ucSim900Status = BUSY;
    deal_g_ucSim900Status();//ljw,20160909 for space
	ddi_uart_Write(&UART1_ctl_t,"AT+QISTAT\r",strlen("AT+QISTAT\r"));
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
//ljw20160907    delayms(1500);//wei,20160621,for check GPRS status at condition AT+QIMUX=1
	WaitGSMOK2(250);
    g_ucSim900Status = IDLE;
	if(strstr((char *)g_ucSim900Buff,"ERROR") != NULL)
	{
	  return GPRS_DISCONNECTED;	  
	}
    i =  GPRS_DISCONNECTED;
    if((strstr((char *)g_ucSim900Buff,"STATE:") != NULL) && (strstr((char *)g_ucSim900Buff,"OK"))!= NULL ) 
	{
        if(strstr((char *)g_ucSim900Buff,"+QISTATE: 2, \"TCP\",") != NULL)
        {
            ClrGSMBuff();
            i =  GPRS_CONNECTED_AGPS;
        }
        p_temp = strstr((char *)g_ucSim900Buff,"+QISTATE: 1, \"UDP\",");
        
		if(p_temp != NULL)
        {            
            if((0==strncmp((u8 *)(p_temp+21),(u8 *)&system_para.g_ucIPAddress,strlen((char *)(&system_para.g_ucIPAddress))))\
                &&(0==strncmp((u8 *)(p_temp+39),(u8 *)&system_para.g_ucUDPPort,strlen((char *)(&system_para.g_ucUDPPort)))))
            {
                ClrGSMBuff();
                i =  GPRS_CONNECTED;
            }
            else i = GRPS_IP_PORT_err;
        }        
	}
	return i;
}
//wei,20160621,for check GPRS status at condition AT+QIMUX=1
#if 0//wei,20160621,for check GPRS status at condition AT+QIMUX=1
unsigned char GetGPRSStatus(void)
{
	unsigned char i;
	for(i=0; i<20; i++)
	{
		if (BUSY == g_ucSim900Status)
		{
		   delayms(10);
		}
		else
			break;	
	}	
		
	g_ucSim900Status = BUSY;
	ddi_uart_Write(&UART1_ctl_t,"AT+QISTAT\r",strlen("AT+QISTAT\r"));
	WaitGSMOK();
	g_ucSim900Status = IDLE;
	if(strstr((char *)g_ucSim900Buff,"ERROR") != NULL)
	{
	  return GPRS_DISCONNECTED;
	  
	}
      if((strstr((char *)g_ucSim900Buff,"STATE:") != NULL) && (strstr((char *)g_ucSim900Buff,"OK"))!= NULL ) 
	{
		ClrGSMBuff();
		i =  GPRS_CONNECTED;
	}
    	else
      		i =  GPRS_DISCONNECTED;

	return i;
}
#endif //wei,20160621,for check GPRS status at condition AT+QIMUX=1

/*
**-----------------------------------------------------------------------------
**
**	概述:
**		UDP方式发送经纬度
**
**	参数:
**		无
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
//void UDPSendLoca(void)
//{
//	unsigned char i=0;
//	while(GetGPRSStatus() != GPRS_CONNECTED)
//	{
//		i ++;
//		StartGPRS();
//		delayms(2000);
//		if (i>3)
//		   break;	
//		//feed_wdt();		
//	}

//	if(!GetAvailableGPSInfo())
//	{
//		UDPSendData(g_ucNeo6QBuff+19,26);
//	}
//	else return;
//}

/*
**-----------------------------------------------------------------------------
**
**	概述:
**		拨打指定号码
**
**	参数:
**		phonenum	电话号码
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
void CallNum(unsigned char *phonenum)
{
	unsigned char temp[16] = {"ATD00000000000;\r"};

	strncpy((char *)temp+3,(char *)phonenum,11);

	ddi_uart_Write(&UART1_ctl_t,temp,16);
}

/*
**-----------------------------------------------------------------------------
**
**	概述:
**		删除处理完的短信
**
**	参数:
**		无
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
void DelDealedSMS(void)
{
	unsigned char temp[10] = {'A','T','+','C','M','G','D','='};

	strncat((char *)temp,(char *)g_ucCurrentSMSNum,strlen((char *)g_ucCurrentSMSNum));
	strcat((char *)temp,"\r");

	ddi_uart_Write(&UART1_ctl_t,temp,strlen((char *)g_ucCurrentSMSNum));

	delayms(100);

	g_ucCurrentSMSNum[0] = 0;
	g_ucCurrentSMSNum[1] = 0;
}

/*
**-----------------------------------------------------------------------------
**
**	概述:
**		输出调试信息
**
**	参数:
**		info: 调试信息内容
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
void DebugOut(unsigned char *info,unsigned char len)
{

#ifdef DEBUGOUT
	uart6sendok();
	if(len == 0)
	{
		UART6_SendData(info,strlen((char *)info));
		//delayms(100);
	}
	else
	{
		UART6_SendData(info,len);
		delayms(100);
	}
#endif
}

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
//ljw,20160908,提取GPS的信息 经度，纬度，速度
//#define GPS_dou 0x0a
//#define GPS_xin 0x05
u8 get_gps_mesg_ascii(u8 *p)
{
    u8 *p_in = p;
    u8 *i;
    u8 *p1;
    u16 u16tmp;
    u8 tmp_sn[12];
    u8 t1;
    //ljw,长度要加判断，防止溢出
    strncat((char *)p_in,"*JD",3);//JD34.36988N    经度34.36988N 北纬       
    strncat((char *)p_in,gps_ascii.Lon,10); //ljw,20160907 取经度
    strncat((char *)p_in,(u8 *)(&gps_ascii.uLon),1);
    strncat((char *)p_in,"*WD",3);//WD117.36988N    经度34.36988N 北纬
    strncat((char *)p_in,gps_ascii.Lat,9); //ljw,20160907 取经度
    strncat((char *)p_in,(u8 *)(&gps_ascii.uLat),1);//ljw,20160907 取经度标志
    strncat((char *)p_in,"*SD",3);//SD000          当前速度
    i = gps_ascii.Spd;
    p1= (unsigned char *)strstr((char *)(i+1),".");
    //转化成公里
    u16tmp = asciitospeed(i,p1-i);
    u16tmp /= 10;
    t1 = hex2BCD_ASCII(tmp_sn,u16tmp);
    strncat((char *)p_in,tmp_sn,t1);
}
#if 0
//ljw,20160908,新改
void SendLocaSMS(const u8*prochar ,u8 procharlen,unsigned char *num)
{
	unsigned char temp[80] = {'A','T','+','C','M','G','S','=','\"'};
	unsigned short i,j;
	unsigned char *ptmp,*p1;
	unsigned char tmpchar;
	
	strncpy((char *)temp+9,(char *)num,11);
	temp[20] = '\"';
	temp[21] = '\r';
    temp[22] = '\n';

	gps_gate = 0;
	//if(!GetAvailableGPSInfoV())     // 获取一组可用的位置信息
	if((!GetAvailableGPSInfoV())&&(dingweied_flag == 0))
	{
//		for (i=0; i<50;i++)
//		{
//			if (BUSY == g_ucSim900Status)
//			{
//			   delayms(10);
//			}
//			else
//			 break;
//		}
        deal_g_ucSim900Status();
		if(procharlen)
		{
//			g_ucSim900Status = BUSY;   // 更改SIM900模块状态
			ddi_uart_Write(&UART1_ctl_t,temp,strlen(temp));			
	        WaitGSMOK1();
			delayms(1000);
			strncpy((char *)temp,prochar,procharlen);
			//strncpy((char *)temp,(char *)("The GPS signal is not Ready"),27);
			temp[procharlen] = 0x1A;
			ddi_uart_Write(&UART1_ctl_t,temp,procharlen+1);   //LedFlash();
//wei,20160726			WaitGSMOK1();
            WaitGSMOK2(500);//wei,20160726 for send SMS			
		}
        g_ucSim900Status =IDLE;    // 更改SIM900模块状态		
		return;
	}
    gps_gate = 1;
    deal_g_ucSim900Status();
	ddi_uart_Write(&UART1_ctl_t,temp,strlen(temp));
#if 1
    WaitGSMOK1();
	delayms(1000);
#else
       UART0_ReceiveData(g_ucSim900Buff,0); 
#endif  
    memset(temp,0,sizeof(temp));
	if(procharlen)
	{
         strncpy((char *)temp,prochar,procharlen);
    }
//$GPRMC,,204700.128,A,3403.868,N,11709.432,W,001.9,336.9,170698,013.6,E*6E    
    if((procharlen+13) <sizeof(temp))//ljw,20160908获取时间
        get_ASCII_TIM((u8 *)(temp+procharlen));
    if((procharlen+35) <sizeof(temp))//ljw,20160908获取时间
        get_gps_mesg_ascii((u8 *)(temp+procharlen+12));
    strncat(temp,"*FX",3);
    p1= gps_ascii.Cog;
	ptmp = (unsigned char *)strstr((char *)(p1),".");
    if(((ptmp-p1)<4)&&((procharlen+12+35+4)<sizeof(temp)))
        strncat((u8 *)(temp),(char *)p1,ptmp-p1);  // 拷贝 角度
    
#if 0 //ljw,20160908,重新处理GPS数据
	if (dingwei_flag == 0x5a)
		p1 = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
	else
		p1 = (unsigned char *)strstr((char *)backup_g_ucNeo6QBuff,",");
//      p1 = (unsigned char *)strstr((char *)(g_ucNeo6QBuff),",");
//ljw,20160902修复以前的错误的数据的解析	p1 = (unsigned char *)strstr((char *)(p1+1),","); 
      //时间加8 
	*(p1+2) +=8;
	if( *(p1+2) >= ('0'+10))
	{
		*(p1+1) = *(p1+1)+1;
		*(p1+2) = *(p1+2)-10;
	}
	if((*(p1+2) >='4') &&(*(p1+1) >= '2'))
	{
		if(*(p1+1) == '2')
		{
			*(p1+2)  -= 4;
		}
		else
		if(*(p1+1) == '3')
		{
			*(p1+2) = *(p1+2)+6;
		}
		*(p1+1) = '0'; 
	}
#endif //ljw,20160908,重新处理GPS数据
//	gps_gate = 1;
	tmpchar = 0x1a;
	strncat((char *)temp,&tmpchar,1);
	ddi_uart_Write(&UART1_ctl_t,temp,strlen(temp));   //LedFlash();
    WaitGSMOK2(500);
//ljw,20160908	WaitGSMOK1();
	g_ucSim900Status =IDLE;    // 更改SIM900模块状态
//ljw,20160908短信等待不用	delayms(3000);
}
#endif 
#if 1 //ljw,20160908改GPS信息提取
void SendLocaSMS(const u8*prochar ,u8 procharlen,unsigned char *num)
{
	unsigned char temp[80] = {'A','T','+','C','M','G','S','=','\"'};
	unsigned short i,j;
	unsigned char *ptmp,*p1;
	unsigned char tmpchar;
    u8 num_len = strlen(num);//ljw,20160924 for wulian
    if(num_len >= PHNUM_LEN-3) //ljw
        return ;

//ljw,20160924 for wulian
//	strncpy((char *)temp+9,(char *)num,11);
//	temp[20] = '\"';
//	temp[21] = '\r';
//    temp[22] = '\n';
    
    //ljw,20160924	strncpy((char *)temp+9,(char *)num,11);
    strncpy((char *)temp+9,(char *)num,num_len);
	temp[num_len+9] = '\"';//temp[20] = '\"';
	temp[num_len+10] = '\r';//temp[21] = '\r';
    temp[num_len+11] = '\n';//temp[22] = '\n';

	gps_gate = 0;
	//if(!GetAvailableGPSInfoV())     // 获取一组可用的位置信息
	if((!GetAvailableGPSInfoV()) && (dingweied_flag == 0))
	{
//ljw,20160909for space        
//		for (i=0; i<500;i++)
//		{
//			if (BUSY == g_ucSim900Status)
//			{
//			   delayms(1);
//			}
//			else
//			 break;
//		}
        deal_g_ucSim900Status();//ljw,20160909for space
		if (procharlen)
		{
//ljw,20160909for space 			g_ucSim900Status = BUSY;   // 更改SIM900模块状态
			ddi_uart_Write(&UART1_ctl_t,temp,strlen(temp));			
            WaitGSMOK1();
			delayms(1000);
			strncpy((char *)temp,prochar,procharlen);
			//strncpy((char *)temp,(char *)("The GPS signal is not Ready"),27);
			temp[procharlen] = 0x1A;
			ddi_uart_Write(&UART1_ctl_t,temp,procharlen+1);   //LedFlash();
//wei,20160726			WaitGSMOK1();
            WaitGSMOK2(500);//wei,20160726 for send SMS
//ljw,20160909for space			g_ucSim900Status =IDLE;    // 更改SIM900模块状态
		}
        g_ucSim900Status =IDLE;    // 更改SIM900模块状态//ljw,20160909for space
		gps_gate = 1;
		return;
	}

    deal_g_ucSim900Status();
#if 0 //ljw,20160903 for spac
	for (i=0; i<500;i++)
	{
		if (BUSY == g_ucSim900Status)
		{
		   delayms(1);
		}
		else
			break;
	}
	g_ucSim900Status = BUSY;   // 更改SIM900模块状态
#endif 
	ddi_uart_Write(&UART1_ctl_t,temp,strlen(temp));
#if 1 
      WaitGSMOK1();
	delayms(1000);
#else
       UART0_ReceiveData(g_ucSim900Buff,0); 
#endif  
    memset(temp,0,sizeof(temp));
	if(procharlen)
	{
         strncpy((char *)temp,prochar,procharlen);
    }
	//p1 = (unsigned char *)strstr((char *)(g_ucNeo6QBuff),",");
	//strncpy((char *)(temp+strlen("lowbattery!")),(char *)g_ucNeo6QBuff+7,6); 
	if(dingwei_flag == 0x5a)
		p1 = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
	else
		p1 = (unsigned char *)strstr((char *)backup_g_ucNeo6QBuff,",");
//      p1 = (unsigned char *)strstr((char *)(g_ucNeo6QBuff),",");
//ljw,20160902修复以前的错误的数据的解析	p1 = (unsigned char *)strstr((char *)(p1+1),","); 
      //时间加8 
	*(p1+2) +=8;
	if( *(p1+2) >= ('0'+10))
	{
		*(p1+1) = *(p1+1)+1;
		*(p1+2) = *(p1+2)-10;
	}
	if((*(p1+2) >='4') &&(*(p1+1) >= '2'))
	{
		if(*(p1+1) == '2')
		{
			*(p1+2)  -= 4;
		}
		else
		if(*(p1+1) == '3')
		{
			*(p1+2) = *(p1+2)+6;
		}
		*(p1+1) = '0'; 
	}
	strncpy((char *)(temp+procharlen),(char *)(p1+1),6); //时间和逗号
		   
	p1 = (unsigned char *)strstr((char *)(p1+1),",");
	p1 = (unsigned char *)strstr((char *)(p1+1),",");
	ptmp = (unsigned char *)strstr((char *)(p1+1),",");
	ptmp= (unsigned char *)strstr((char *)(ptmp+1),",");	
	ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");
	ptmp = (unsigned char *)strstr((char *)(ptmp+1),",");

	strncpy((char *)(temp+6+procharlen),(char *)(p1),ptmp-p1); //经纬度和逗号
	p1 = ptmp;
	// 拷贝 速度
	ptmp = (unsigned char *)strstr((char *)(p1),".");
	strncat((char *)temp,(char *)p1,ptmp-p1);  // 拷贝 速度信息

 	p1 = (unsigned char *)strstr((char *)(p1+1),",");
	ptmp = p1;
	//20150514
	//$GPRMC,,204700.128,A,3403.868,N,11709.432,W,001.9,336.9,170698,013.6,E*6E

	// 拷贝 角度
	ptmp = (unsigned char *)strstr((char *)(p1),".");
	strncat((char *)temp,(char *)p1,ptmp-p1);  // 拷贝 速度信息
	gps_gate = 1;

	tmpchar = 0x1a;
	strncat((char *)temp,&tmpchar,1);
	ddi_uart_Write(&UART1_ctl_t,temp,strlen(temp));   //LedFlash();
	WaitGSMOK1();
	g_ucSim900Status =IDLE;    // 更改SIM900模块状态

	delayms(3000);
}
#endif //ljw,20160908该GPS信息提取

/*
**-----------------------------------------------------------------------------
**
**	概述:
**		发送当前时间、经纬度、速度、方向到指定号码.
**
**	参数:
**		num -- 手机号码存放指针
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
void SendLocaSMS1(unsigned char *num)
{
	const char*ptmp = NULL;
	SendLocaSMS(ptmp,0,num);
}

/*
**-----------------------------------------------------------------------------
**
**	概述:
**		发送当前时间、经纬度、速度、方向到指定号码.
**
**	参数:
**		num -- 手机号码存放指针
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
void SendALARMSMS1(unsigned char *num)
{
	//const unsigned char alarm[]="ALARM!";
	SendLocaSMS("ALARM!",strlen("ALARM!"),num);
}

void Send_lowpower_LocaSMS(unsigned char *num)
{
	SendLocaSMS("lowbattery!",strlen("lowbattery!"),num);
}

void Send_speed_ALARMSMS(unsigned char *num)
{	
	u8 tmpbuf[15]="SpeedAlarm ";
	if(g_usCurrentSpeed>250)
		return;
       tmpbuf[11]= (g_usCurrentSpeed/100) +'0';
       tmpbuf[12]= ((g_usCurrentSpeed%100)/10) +'0';
       tmpbuf[13]= (g_usCurrentSpeed%10) +'0';

	//SendLocaSMS("lowbattery!",strlen("lowbattery!"),num);
	SendMsg(num,tmpbuf,strlen(tmpbuf));

}

//wei 出区域报警
/*
void Send_Area_Alarm()
{
	//发送中文信息，出警戒。
	SendMsg(num,tmpbuf,strlen(tmpbuf));
}
*/

unsigned char BatteryStatusGet(void)
{
#if 1
	unsigned char *i,l;
       volatile unsigned char j=0;
	ddi_uart_Write(&UART1_ctl_t,"AT+CBC\r\n",strlen("AT+CBC\r\n"));   //LedFlash();
	WaitGSMOK1();//ljw20160903for space   WaitGSMOK();


	i = (unsigned char *)strstr((char *)g_ucSim900Buff,(char *)",");
        i ++;
	for(l=0; l<3; l++)
        {  
            //j = (*i - '0') * 10;
            
          //  if(((*(i+2)) >='0') && ((*(i+2)) =<'9'))
            if ((*i >= '0') && (*i <= '9'))
            {
                    j *= 10;
                    j += *i - '0';
            }
            else
            {
                   return j;
            }
            i ++;
            
        }
	return j;
#else

#endif


}
//ljw,20160924 for wulian
void send_chinese_sms(unsigned char *num, unsigned char *msg, unsigned char len)
{
			//SEND CHINESE
		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r")); //GSM
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS?\r",strlen("AT+CSCS?\r")); //GSM
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();	

		if (strstr(g_ucSim900Buff,"UCS2") == NULL)
		{
			ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r")); //GSM		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
			WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
		}
        //49,167,0,25
		ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,08\r\n",strlen("AT+CSMP=17,255,0,08\r\n"));  	// 短信格式：TEXT
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

		ddi_uart_Write(&UART1_ctl_t,"AT+CNMI=2,1,0,0,0\r\n",strlen("AT+CNMI=2,1,0,0,0\r\n"));  	// 短信格式：TEXT
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
		SendMsg_UCS2(num,msg,len);
        
		//恢复成英文文本模式17,167,0,241
		ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,0\r\n",strlen("AT+CSMP=17,255,0,0\r\n"));  	// 短信格式：TEXT
		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

	    ddi_uart_Write(&UART1_ctl_t,"AT+CSMP?\r",strlen("AT+CSMP?\r"));  	// 短信格式：TEXT
		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

		if (strstr(g_ucSim900Buff,"17,255,0,0") == NULL)
		{
			ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,0\r\n",strlen("AT+CSMP=17,255,0,0\r\n"));  	// 短信格式：TEXT
			//DebugOut("* Set SMS Format: TEXT.\r\n",0);
			WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
		}

		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"8859-1\"\r",strlen("AT+CSCS=\"8859-1\"\r"));  // 短信特殊字符显示才能正常
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

}
/* LJW,20160924 for wulian
void send_chinese_sms(unsigned char *num, unsigned char *msg, unsigned char len)
{
			//SEND CHINESE
		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r")); //GSM
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS?\r",strlen("AT+CSCS?\r")); //GSM
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();	

		if (strstr(g_ucSim900Buff,"UCS2") == NULL)
		{
			ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"UCS2\"\r",strlen("AT+CSCS=\"UCS2\"\r")); //GSM		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
			WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
		}
        
		ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=49,167,0,25\r\n",strlen("AT+CSMP=49,167,0,25\r\n"));  	// 短信格式：TEXT
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

		ddi_uart_Write(&UART1_ctl_t,"AT+CNMI=2,1,0,0,0\r\n",strlen("AT+CNMI=2,1,0,0,0\r\n"));  	// 短信格式：TEXT
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
		SendMsg_UCS2(num,msg,len);
        
		//恢复成英文文本模式17,167,0,241
		ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,0\r\n",strlen("AT+CSMP=17,255,0,0\r\n"));  	// 短信格式：TEXT
		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

	    ddi_uart_Write(&UART1_ctl_t,"AT+CSMP?\r",strlen("AT+CSMP?\r"));  	// 短信格式：TEXT
		//DebugOut("* Set SMS Format: TEXT.\r\n",0);
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

		if (strstr(g_ucSim900Buff,"17,255,0,0") == NULL)
		{
			ddi_uart_Write(&UART1_ctl_t,"AT+CSMP=17,255,0,0\r\n",strlen("AT+CSMP=17,255,0,0\r\n"));  	// 短信格式：TEXT
			//DebugOut("* Set SMS Format: TEXT.\r\n",0);
			WaitGSMOK1();//ljw20160903for space   WaitGSMOK();
		}

		ddi_uart_Write(&UART1_ctl_t,"AT+CSCS=\"8859-1\"\r",strlen("AT+CSCS=\"8859-1\"\r"));  // 短信特殊字符显示才能正常
		WaitGSMOK1();//ljw20160903for space   WaitGSMOK();

}
*/

