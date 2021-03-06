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
#include "gps.h"
#include "gprs.h"
#include "user_define.h"
/***********************************************************************
 * Public user header files (#include)
 **********************************************************************/
//#include <includes.h> 
/***********************************************************************
 * Private user header files (#include)
 **********************************************************************/

/***********************************************************************
 * Constant definitions (#define)
 **********************************************************************/

/***********************************************************************
 * Macro definitions (#define)
 **********************************************************************/

/***********************************************************************
 * Data declarations of global data imported.  
 **********************************************************************/
u16 flowwater=0; 	  //终端流水号  ，自加		   
u16 flowwater_form_table;   // 从平台收到的流水号
u16 message_id_from_table; // 从平台收到的消息ID	
//15656049332
//  u8 mbnumber[6]={0x01,0x56,0x56,0x04,0x93,0x32};
////ljw,20160902,测试完要删 u8 mbnumber[6]={0x01,0x98,0x00,0x00,0x20,0x12};
/***********************************************************************
 * Function prototypes of global functions imported.  
 **********************************************************************/
//跟踪上传间隔，如果是０表示不要跟踪了
 u16 T_P_g_usGPRSUpLoadAternationTime;

//跟踪时间
 u32 T_P_time;
/***********************************************************************
 * Data definitions of global data exported.  
 **********************************************************************/
//ljw,20161126 extern unsigned char GPRSCmdCount;
//LJW,20161124,这个数组目前只是用来进行参数设置用的了。 extern unsigned char g_ucGPRSCmdBuffback[GPRS_BUFF_SIZE];
extern unsigned char g_ucGPRSCmdBuffback[GPRS_BACK_BUFF_SIZE]; //LJW,20161124,这个数组目前只是用来进行参数设置用的了。
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
//交换高低字节，只能是字或双字
void swapHLchar(u8*src,u8 len)
{
	u8 tmp;
	if (len == 4)
	{
		tmp = *src;
		*src = *(src +3); // 
		*(src +3) = tmp;
		tmp = *(src +1);
		*(src +1) = *(src +2);
		*(src +2) = tmp;
	}
	else
	if (len == 2)
	{
		tmp = *src;
		*src = *(src +1); // 
		*(src +1) = tmp;
	}

}

//协议采用大端模式(big-endian)的网络字节序来传递字和双字。
//双字(DWORD)的传输约定：先传递高24 位，然后传递高16 位，再传递高八位,最后传递低八位。
//每条消息由标识位、消息头、消息体和校验码组成，消息结构图如图1 所示：
//  标识位(0X7E)|   消息头 |消息体 |检验码| 标识位(0X7E)
//若校验码、消息头以及消息体中出现0x7e，则要进行转义处理，转义
//规则定义如下：
//0x7e <————> 0x7d 后紧跟一个0x02；
//0x7d <————> 0x7d 后紧跟一个0x01。


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


//消息体属性格式结构图如图2 所示：
//15     14  |  13   |   12   11    10           |        9    8   7     6     5    4    3   2   1   0
//保留  | 分包| 数据加密方式 |              消息体长度
//数据加密方式：
//——bit10~bit12 为数据加密标识位；
//——当此三位都为0，表示消息体不加密；
//——当第10 位为1，表示消息体经过RSA 算法加密；
//——其他保留。

//分包：
//当消息体属性中第13 位为1 时表示消息体为长消息，进行分包发送处理，具体分包信
//息由消息包封装项决定；若第13 位为0，则消息头中无消息包封装项字段。

// 消息包封装项内容
// 起始字节| 字段              | 数据类型 |  描述及要求
// 0                |   消息总包数| WORD              |该消息分包后的总包数
// 2                   | 包序号          |WORD               |从1 开始


//校验码
//校验码指从消息头开始，同后一字节异或，直到校验码前一个字节，占用一个字节。

//按照部标协议转义，遇到0x7d 0x02->0x7e 、 0x7d 0x01 ->0x7d;
//从0x7e后一个字节开始和后一个字节异或，直到校验码，如果和校验码相同，就是正确的



unsigned short analysis(unsigned char* buffer,unsigned short number)
{
   unsigned char* tmp ;

  // u8 tmpbuf[GPRS_BUFF_SIZE+50];
   unsigned short i,j,k;
   unsigned short sum=0 ;

   if( number  > GPRS_BUFF_SIZE)
   	return 0;
 //  memcpy(tmpbuf,buffer,number);
   tmp = buffer;
   if ( *tmp != 0X7E )
	   return 0;
   if (*(tmp+number-1) != 0x7e)
	 	return 0;  
   k = number-2; //去头尾两个0X7E
   j = 0; //转义 计数器

   for ( i=0; i<k;  i++ )//转义
   {
         if(0x7d == *(tmp+i+j+1))   // 加上0x7e的第一个字符
         {
			if (0x02 == *(tmp+i+j+2))
            {
			   *(tmp+i) = 0x7e;	
         	j ++;
					k --; //?????? 总数这样才会对									
			}
			else
			if (0x01 == *(tmp+i+j+2))
            {
			   *(tmp+i) = 0x7d;	
				 			j ++;
			      k --; //?????? 总数这样才会对				
			} 
		    else
		    {
				return 0;
			}			
		 }
		 else
		 {
			*(tmp+i) = *(tmp+i+j+1);
		 }
   }
   sum = 0;
   for ( i=0; i<(k-1);  i++ )//计算校验码
   {
		sum ^= *(tmp+i);  //先转义再校验
   }	
   if (sum != *(tmp+k-1))
   {
	return 0;
   }
   else
    return k;


}


// 数据封装
unsigned short constructor(unsigned char* buffer,unsigned short number)
{
   volatile unsigned char* tmp = buffer;
   u8 tmp_buf[150];
   unsigned short i,j=0,k;
   volatile short sum=0 ;
   for ( i=0; i<number;  i++ )
   {
		sum ^= *(tmp+i);  //先转义再校验

   }	
   *(buffer+number) = sum;
   number ++;
   k = number; 
   
   tmp_buf[0] = 0x7e; //标志符
      j = 1; 
   for ( i=0; i<k;  i++ )
   {
		 if(0x7d == *(tmp+i))
		 {
		 	tmp_buf[j++ ] = 0x7d; //
			tmp_buf[j++] = 0x01; //

			
		 }
		 else if (0x7e == *(tmp+i))
		 {
		 	tmp_buf[j++ ] = 0x7d; //
			tmp_buf[j++] = 0x02; //			

		 }
		 else
		 {
			tmp_buf[j++] = *(tmp+i); //

		 }
              if(j>149)//ljw,20160907,for overflow
                return 0;


   }
   tmp_buf[j] = 0x7e; //标志符
   memset(buffer,0,number);   
   memcpy(buffer,tmp_buf,j+1);
  
   return j+1;
}

//wei
/*****************
参数说明，
函数名:void  SendGprs_Index(u16 len,u8 index)
len:发送的数据长度
index:发送数据的通道
*****************/
void  SendGprs_Index(u16 len,u8 index)
{
	//volatile unsigned char g_ucGPRSDataBuff_hex[160];	
	int i;
	
	//memset((void*)g_ucGPRSDataBuff_hex,0,sizeof(g_ucGPRSDataBuff_hex));
	//hextoascii(g_ucGPRSDataBuff,(void*)g_ucGPRSDataBuff_hex,strlen((char *)g_ucGPRSDataBuff));
//wei,20160709,防止到睡眠条件时还没有睡眠
    if ( (system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))
    {
        if( (g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE))//wei,20160709,满足睡眠条件，不要在继续拨号了
            return ;
    }
//wei,20160709,防止到睡眠条件时还没有睡眠

//wei 	if (SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,len)))
	if (SEND_FAIL == (UDPSendData_Index((unsigned char*)g_ucGPRSDataBuff,len,index)))
	{
		//DebugOut("\r[ ------SEND_FAIL ==UDPSendData------- ]\r\n",0);
			 for(i=0; i<100; i++ )
			 {
					if(g_ucSMSCmdMissionFlag == 1)
						return;							
//wei					delayms(50); 
					delayms(100);  //wei 变成等待10s
			 }	
//wei		if (SEND_FAIL != (UDPSendData((unsigned char*)g_ucGPRSDataBuff,len)))
		if (SEND_FAIL != (UDPSendData_Index((unsigned char*)g_ucGPRSDataBuff,len,index)))
		{;}
//wei20160530
		if(!((~bit_flag)&(0x03<<0)))
		{
			//StartGPRStoAGPS();
			return ;
		}
		if (SEND_FAIL != (UDPSendData_Index((unsigned char*)g_ucGPRSDataBuff,len,index)))
		{;}
//wei20160530
		else
		if (GetGPRSStatus() != GPRS_CONNECTED)
		{
		      //关闭GPRS led
			GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);  
#if 0		
			while(	GetGPRSStatus() != GPRS_CONNECTED)
			{
				if(i>3)
				{	
					DebugOut("  PowerOff SIM900.\r\n",0);
					P1 |= Pn6_OUTPUT_1;  	// 关闭GSM
					SoftDelay_ms(3000);  
					P1 &= ~Pn6_OUTPUT_1;  	// 拉低POWERkey
					DebugOut("  PowerOff SIM900 Done!\r\n",0);
					GPRSOpen();
					UDPSendData((char*)g_ucGPRSDataBuff_hex,strlen((char *)g_ucGPRSDataBuff_hex));
					break;
				}	
				StartGPRS();		
				SoftDelay_ms(1000);
				i ++;
			}
#endif	
            //20130906 发现有时候唤醒后模块回死机，所以这个地方直接通过看门狗复位
            //   while(1);
            for(i=0; i<100; i++ )
            {
                if(g_ucSMSCmdMissionFlag == 1)
                    return;		
                delayms(50); 
            }
//ljw.20160903			DebugOut("  PowerOff SIM900.\r\n",0);
			StartGPRS();		
			delayms(1500);
//wei			if (SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,len)))
			if(SEND_FAIL == (UDPSendData_Index((unsigned char*)g_ucGPRSDataBuff,len,index)))
			{
                for(i=0; i<100; i++ )
                {
                    if(g_ucSMSCmdMissionFlag == 1)
                        return;		
                    delayms(50); 
                }     
                StartGPRS();		
			}			
			delayms(1500);
//wei			if (SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,len)))
			if(SEND_FAIL == (UDPSendData_Index((unsigned char*)g_ucGPRSDataBuff,len,index)))
			{
				 for(i=0; i<180; i++ )
				 {
						if(g_ucSMSCmdMissionFlag == 1)
							return;		
						delayms(50); 
				 }
 #if 0                     
					//UART0_Stop();
  restartgprs:       
					PMK4 = 1;  //禁止ring中断
					// PM1 |= (1<<0|1<<1); //UART0 RX TX AS INPUT
					PU3 &= ~(1<<3);
					PU1 &= ~(1<<1);

					P1 |= Pn6_OUTPUT_1;  	// 关闭GSM
					SoftDelay_ms(4000);  
					P1 &= ~Pn6_OUTPUT_1;  	// 拉低POWERkey
					SoftDelay_ms(100);  
					P7_bit.no2 = 0;  //GSM POWER OFF
					SoftDelay_ms(6000);  
					P7_bit.no2 = 1; //GSM POWER On
					DebugOut("  PowerOff SIM900 Done!\r\n",0);
					PU3 |= (1<<3);
					PU1 |= (1<<1);
					// PM1 &= ~(1<<0|1<<1); //UART0 RX TX AS INPUT
                                    SoftDelay_ms(3000);  
					// UART0_Init_38400();	
					 //SoftDelay_ms(20000); 
                                    GPRSOpen();
					    SoftDelay_ms(3000);  
				 //20130906 发现有时候唤醒后模块回死机，所以这个地方直接通过看门狗复位
//wei				if(SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,strlen((char *)g_ucGPRSDataBuff))))
				if(SEND_FAIL == (UDPSendData_Index((unsigned char*)g_ucGPRSDataBuff,strlen((char *)g_ucGPRSDataBuff),index)))
				{  
					UART0_SendData("  GPRS dead!\r\n",strlen("  GPRS dead!\r\n"));
					SoftDelay_ms(1000);
					//main1();						  
					while(1)
					{
						;
					}  
				}  
#endif				
                GSMLedOff();
                
//wei,20160709                    GSM_Power(FALSE); //wei,20160608看门狗复位前，给GSM断电
//wei,20160709                    while(1); //等看门狗复位系统//20130906 发现有时候唤醒后模块回死机，所以这个地方直接通过看门狗复位					
                dial_status = 0;//wei,20160709,将标志清零
                GSM_reset();//wei,20160616重启GSM
                StartGPRS();//wei,20160617重启GSM
                if(dial_status ==0)
                {
                    ddi_uart_Write(&UART1_ctl_t,"SEND_index err, dog reset...",strlen("SEND_index err, dog reset..."));
                    GSM_Power(FALSE); //wei,20160608看门狗复位前，给GSM断电
                    while(1); //等看门狗复位系统//20130906 发现有时候唤醒后模块回死机，所以这个地方直接通过看门狗复位
                }
            }
		}
		else
		{
			//UDPSendData((char*)g_ucGPRSDataBuff,strlen((char *)g_ucGPRSDataBuff));
			UDPSendData_Index((unsigned char*)g_ucGPRSDataBuff,strlen((char *)g_ucGPRSDataBuff),index);
		}			
	}

	//ClrGPRSDataBuff();
	memset(g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	//ClrGPRSCmdBuff();
	memset(g_ucGPRSCmdBuff,0,sizeof(g_ucGPRSCmdBuff));
}

#if 0 //wei
void  SendGprs(u16 len)
{
	//volatile unsigned char g_ucGPRSDataBuff_hex[160];	
	int i;
	
	//memset((void*)g_ucGPRSDataBuff_hex,0,sizeof(g_ucGPRSDataBuff_hex));
	//hextoascii(g_ucGPRSDataBuff,(void*)g_ucGPRSDataBuff_hex,strlen((char *)g_ucGPRSDataBuff));
	if (SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,len)))
	{
		//DebugOut("\r[ ------SEND_FAIL ==UDPSendData------- ]\r\n",0);
           for(i=0; i<100; i++ )
           	{
           		if(g_ucSMSCmdMissionFlag == 1)
			return;		
           		delayms(50); 
           	}	
		if (SEND_FAIL != (UDPSendData((char*)g_ucGPRSDataBuff,len)))
		{;}
		else
		if (GetGPRSStatus() != GPRS_CONNECTED)
		{
		      //关闭GPRS led
			GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);  
#if 0		
			while(	GetGPRSStatus() != GPRS_CONNECTED)
			{
				if(i>3)
				{	
					DebugOut("  PowerOff SIM900.\r\n",0);
					P1 |= Pn6_OUTPUT_1;  	// 关闭GSM
					SoftDelay_ms(3000);  
					P1 &= ~Pn6_OUTPUT_1;  	// 拉低POWERkey
					DebugOut("  PowerOff SIM900 Done!\r\n",0);
					GPRSOpen();
					UDPSendData((char*)g_ucGPRSDataBuff_hex,strlen((char *)g_ucGPRSDataBuff_hex));
					break;
				}	
				StartGPRS();		
				SoftDelay_ms(1000);
				i ++;
		
					
			}
#endif	
                        
                        
                        //20130906 发现有时候唤醒后模块回死机，所以这个地方直接通过看门狗复位
                     //   while(1);
                   for(i=0; i<100; i++ )
                   	{
                   		if(g_ucSMSCmdMissionFlag == 1)
					return;		
                   		delayms(50); 
                   	}			
//ljw,20160903			DebugOut("  PowerOff SIM900.\r\n",0);
			StartGPRS();		
			delayms(1500);
			if (SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,len)))
			{
	                   for(i=0; i<100; i++ )
	                   	{
	                   		if(g_ucSMSCmdMissionFlag == 1)
						return;		
	                   		delayms(50); 
	                   	}     
                      
                        StartGPRS();		
			}			
			delayms(1500);
			if (SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,len)))
			{
			                   for(i=0; i<180; i++ )
			                   	{
			                   		if(g_ucSMSCmdMissionFlag == 1)
								return;		
			                   		delayms(50); 
			                   	}       
 #if 0                     
                                    //UART0_Stop();
  restartgprs:       
                                    PMK4 = 1;  //禁止ring中断
                                    // PM1 |= (1<<0|1<<1); //UART0 RX TX AS INPUT
                                    PU3 &= ~(1<<3);
                                    PU1 &= ~(1<<1);
									 
                                    P1 |= Pn6_OUTPUT_1;  	// 关闭GSM
                                    SoftDelay_ms(4000);  
                                    P1 &= ~Pn6_OUTPUT_1;  	// 拉低POWERkey
                                    SoftDelay_ms(100);  
                                    P7_bit.no2 = 0;  //GSM POWER OFF
                                    SoftDelay_ms(6000);  
                                    P7_bit.no2 = 1; //GSM POWER On
                                    DebugOut("  PowerOff SIM900 Done!\r\n",0);
                                    PU3 |= (1<<3);
                                    PU1 |= (1<<1);
					// PM1 &= ~(1<<0|1<<1); //UART0 RX TX AS INPUT
                                    SoftDelay_ms(3000);  
					// UART0_Init_38400();	
					 //SoftDelay_ms(20000); 
                                    GPRSOpen();
					    SoftDelay_ms(3000);  
				 //20130906 发现有时候唤醒后模块回死机，所以这个地方直接通过看门狗复位
				if (SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,strlen((char *)g_ucGPRSDataBuff))))
                                {  
	                                UART0_SendData("  GPRS dead!\r\n",strlen("  GPRS dead!\r\n"));
						SoftDelay_ms(1000);
					       //main1();
						  
	                              while(1)
	                              {
	                                ;
	                              }  
                                }  
#endif				
					  GSMLedOff();
                                
                      GSM_Power(FALSE); //wei,20160608看门狗复位前，给GSM断电
                      while(1); //等看门狗复位系统//20130906 发现有时候唤醒后模块回死机，所以这个地方直接通过看门狗复位				
			}
		}
		else
		{
			UDPSendData((char*)g_ucGPRSDataBuff,strlen((char *)g_ucGPRSDataBuff));

		}
			
	}	

	//ClrGPRSDataBuff();
	memset(g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	//ClrGPRSCmdBuff();
	memset(g_ucGPRSCmdBuff,0,sizeof(g_ucGPRSCmdBuff));
}
#endif //wei


// 纬度，我们需要把它转换成度分秒的格式，计算方法：如接收到的纬度是：4546.40891
// 4546.40891/100=45.4640891可以直接读出45度, 4546.40891–45*100=46.40891, 可以直接读出46分
// 46.40891–46 =0.40891*60=24.5346读出24秒, 所以纬度是：45度46分24秒
// 待做
u32 asciitolatitude(u8*src)
{
	volatile u32 tmp=0;
	volatile u8 tmpbuf;
	volatile u8 len;
	const u8 * ptmp;
	const u8 * ptmp1;
	
   // g_dCurrentLat 4546.40891
    ptmp = (const u8 *)strstr(src,".");
    len = ptmp-src-2;
    //
    #if 0
    if ( len<= 2) 
        asciitohex(src, (unsigned char *) &tmpbuf, len);
    else
    {
        //asciitohex(src, (unsigned char *) &tmpbuf, len-2);
        tmp = (*src-'0' )*100;
        asciitohex(src+len-2, (unsigned char *) &tmpbuf, 2);
        tmpbuf +=tmp;
    }
       #endif
	asciitohex(src, (unsigned char *) &tmpbuf, len);
    tmp = tmpbuf*1000000;
	   //45  46.40891
	ptmp -=2;
	// asciitohex(src+2, (unsigned char *) &tmpbuf, 2);   
	asciitohex((unsigned char *)ptmp, (unsigned char *) &tmpbuf, 2);   
	tmp += tmpbuf*100000/6;
	//4546.  40891,
	ptmp += 3;
//ljw,20160922for 小数点后两位	ptmp1 = (const u8 *)strstr(ptmp,",");//
//ljw,20160922for 小数点后两位	len = ptmp1-ptmp;
	//if (len >= 4)
	{
		asciitohex((unsigned char *)ptmp, (unsigned char *) &tmpbuf, 2);  
		tmp += tmpbuf*500/3;  // 2500/9;    // 0.xx*60*1000000/3600;
//ljw,20160922for 小数点后两位		asciitohex((unsigned char *)(ptmp+2), (unsigned char *) &tmpbuf, len-2);  //有大于5的吗？
        asciitohex((unsigned char *)(ptmp+2), (unsigned char *)(&tmpbuf),2);  //有大于5的吗？
		tmp += tmpbuf*5/3;    // 0.00xx*60 *1000000/3600;
	}
//	asciitohex(src+7, (unsigned char *) &tmpbuf, 2);  
//	fen += tmpbuf;	

//	tmp += fen*100/60

      return tmp;
}
// 经度dddmm.mmmm(度分)格式(前面的0也将被传输)
u32 asciitolongitude(u8*src)
{
	volatile u32 tmp=0;
	volatile u8 tmpbuf;
	volatile u32 fen;
   // g_dCurrentLat
    asciitohex(src, (unsigned char *) &tmpbuf, 1);
    tmp = tmpbuf*1000000;
	asciitohex(src+1, (unsigned char *) &tmpbuf, 2);   
	tmp += tmpbuf;
	tmp *= 1000000;
	
	asciitohex(src+3, (unsigned char *) &tmpbuf, 2);  
	fen = tmpbuf;
	asciitohex(src+6, (unsigned char *) &tmpbuf, 2);  
	fen += tmpbuf*100;	
	asciitohex(src+8, (unsigned char *) &tmpbuf, 2);  
	fen += tmpbuf;
//	tmp += fen*100/60
	tmp += fen*5/3;
      return tmp;
}

// 0~359, 不能直接使用asciitohex，要每一字节分开
u16 asciitodirection(u8*src,u8 len)
{
	volatile u16 tmp;
	volatile u8 tmpbuf;

   // g_dCurrentLat
      if(len == 3)
      	{
	       asciitohex(src, (unsigned char *) &tmpbuf, 1);
	       tmp = tmpbuf*100;
		asciitohex(src+1, (unsigned char *) &tmpbuf, 1);   
		tmp += tmpbuf*10;

		asciitohex(src+2, (unsigned char *) &tmpbuf, 1);   
		tmp += tmpbuf;
      	}
	 else if (len == 2)
	 {
	       asciitohex(src, (unsigned char *) &tmpbuf, 1);
	       tmp = tmpbuf*10;
		asciitohex(src+1, (unsigned char *) &tmpbuf, 1);   
		tmp += tmpbuf;


	 }
	 else
	 	if (len == 1)
	 	{
		asciitohex(src, (unsigned char *) &tmpbuf, 1);   
		tmp = tmpbuf;



		}
	
	return tmp;

}
// 1/10km/h
u16 asciitospeed(u8*src,u8 len)
{

	volatile u16 tmp;
	volatile u8 tmpbuf;
	volatile u32 t1;

   // g_dCurrentLat
   #if 1
      if(len == 3)
      	{
	       //asciitohex(src, (unsigned char *) &tmpbuf, 1);
	       tmpbuf = *src -'0';
	       tmp = tmpbuf*1000;
		//asciitohex(src+1, (unsigned char *) &tmpbuf, 1);  
		tmpbuf = *(src+1) - '0';
		tmp += tmpbuf*100;

		//asciitohex(src+2, (unsigned char *) &tmpbuf, 1);   
		tmpbuf = *(src+2) - '0';
		tmp += tmpbuf*10;
      	}
	 else if (len == 2)
	 {
		tmpbuf = *(src) - '0';
		tmp = tmpbuf*100;

		//asciitohex(src+2, (unsigned char *) &tmpbuf, 1);   
		tmpbuf = *(src+1) - '0';
		tmp += tmpbuf*10;
	 }
	 else
 	if (len == 1)
 	{
		//asciitohex(src, (unsigned char *) &tmpbuf, 1);   
		tmpbuf = *(src) - '0';
		tmp = tmpbuf*10;
	}

	#else
	asciitohex(src, (unsigned char *) &tmpbuf, len);
	tmp = tmpbuf*10;
	#endif
	
	t1 = tmp*1852;
	tmp = (u16)(t1/1000);
	return tmp;

}

void  asciitodate(u8*src,u8*dec)
{
	volatile u16 tmp;
	volatile u8 tmp_dd;
	volatile u8 tmpbuf;
	volatile u32 t1;
   // dd
       asciitohex(src, (unsigned char *) &tmpbuf, 2);
   	hex2BCD_ASCII1(dec+2,tmpbuf);

	//*(dec+2) =tmpbuf;    
	// mm
	asciitohex(src+2, (unsigned char *) &tmpbuf, 2);   
	//*(dec+1) =tmpbuf;  
   	hex2BCD_ASCII1(dec+1,tmpbuf);
      // yy
	asciitohex(src+4, (unsigned char *) &tmpbuf, 2); 
   	hex2BCD_ASCII1(dec,tmpbuf);	
	//*dec = tmpbuf;
	asciitohex(src+6, (unsigned char *) &tmpbuf, 2); 
	hex2BCD_ASCII1(dec+3,tmpbuf);
	//*(dec+3) = tmpbuf;

	asciitohex(src+8, (unsigned char *) &tmpbuf, 2); 
   	hex2BCD_ASCII1(dec+4,tmpbuf);	
	//*(dec+4) = tmpbuf;
	asciitohex(src+10, (unsigned char *) &tmpbuf, 2); 
	//*(dec+5) = tmpbuf;
   	hex2BCD_ASCII1(dec+5,tmpbuf);	

	// 使用GMT+8时区，涉及到日期的更改
    tmp = (*(dec+3)/16)*10 +(*(dec+3))%16+8;
	if (tmp  >= 24)
	{
		*(dec+3) = ((tmp-24)/10)*16+(tmp-24)%10;
		//dd
		//tmp = *(dec+2) +1;//BCD码
		tmp_dd = (*(dec+2)/16)*10 +(*(dec+2))%16 +1; // hex码
		//mm  二月
		if (*(dec+1) == 2)
		{  
			//闰年
			if(( ((2000+*dec/16*10+*dec %16)%4 ) == 0) )
			{
				if (tmp_dd >= 30)
				{
					*(dec+1) += 1;
					*(dec+2)= 1;
				}
				else
				{
					//*(dec+2) += 1;
					*(dec+2) = (tmp_dd/10)*16+tmp_dd%10;
				}
			}
			else
			{
				if (tmp_dd >= 29)
				{
					*(dec+1) += 1;
					*(dec+2)= 1;
				}
				else
				{
					*(dec+2) = (tmp_dd/10)*16+tmp_dd%10;
				}
			}
		}
		else
		if ((*(dec+1) == 1) ||(*(dec+1) == 3)||(*(dec+1) == 5)||(*(dec+1) == 7) \
			||(*(dec+1) == 8)|| ((((*(dec+1))/16) == 1)&&(((*(dec+1))%16) == 0)) ||((((*(dec+1))/16) == 1)&&(((*(dec+1))%16) == 2)))
		{
				if (tmp_dd >= 32)
				{
					// 年翻转
					if (((*(dec+1)/16) == 1)&&((*(dec+1)%16) == 2))
					{
						*dec += 1;
						// 年的个位是9
						if ((*(dec)%16) ==10)
						{
							tmp =  (*dec /16)*10 +(*dec%16);
							*(dec) = (tmp/10)*16 + (tmp %16);
						}
						*(dec+1) = 1; //月为
					}
					else
					  *(dec+1) += 1;
					*(dec+2)= 1;
				}
				else
				{
					*(dec+2) = (tmp_dd/10)*16+tmp_dd%10;
				}
		}
		else
		{
				if (tmp_dd >= 31)
				{
					//tmp =  (*(dec+1) /16)*10 +(*(dec+1)%16)+1;
					//*((dec+1)) = (tmp/10)*16 + (tmp %16);
					if (*(dec+1) == 9)
					{
						*(dec+1) = 0x10;
					}	
					else
					{
						*(dec+1) +=1;
					}
					*(dec+2)= 1;
				}
				else
				{
					*(dec+2) = (tmp_dd/10)*16+tmp_dd%10;

				}
		}
	}
	else
	{
		*(dec+3) = ((tmp)/10)*16+(tmp)%10;
	}
}

u8  u32toport(u32 src,u8*dec)
{
	u8 len=0;

      if (src >100000)  //端口 一般都是４位的
      	{
		src %= 100000;
	}
	if (src/10000 >0)
	{
	  len = 5;
	  *dec++ = '0'+src/10000;
	  
	}
	else
	{
	  len = 4;
	}
	  *dec++ = '0'+(src%10000)/1000;
	  *dec++ = '0'+(src%1000)/100;
	  *dec++ = '0'+(src%100)/10;
	  *dec++ = '0'+(src%10);
}

u32  porttou32(u8* src)
{
	u8  i ,len =0;
	volatile u32 dec=0;

	for (i=0; i<5; i++)
	{
		if (*src >=  '0' )
		{
			dec = dec*10+*src-'0';
		}
		src ++;
	}
	return dec;
}

//flow_from_table: 平台发来的流水号，如果为0XFFFF?就不加平台流水号
u8  ACC_GPS_flag = 0;
u8  ACC_GSM_flag = 0;
//ljw,20160908 test reset flag extern u8 erro_flag ;//ljw,20160906,测试复位错误标志量   测完要删
//position_message_struct position_0200;//ljw,20160906测完要删

#if 1//ljw,20160906 测试点名复位问题
static u32 latitude_b = 0;//ljw,20160824for ACC
static u32 longitude_b = 0;//ljw,20160824for ACC    
static u32 Lac_b = 0;//ljw,20160824for ACC
static u32 LacID_b = 0;//ljw,20160824for ACC
#endif   //ljw,20160906 测试点名复位问题
void send_location_gprs(u16 flow_from_table ,u16  gprsid)
{
	static u8 *i;
	static u8 *ptmp;
	static u8 SMS_speed_turn = 0; //wei20160520
#if 0
    static u32 latitude_b = 0;//ljw,20160824for ACC
    static u32 longitude_b = 0;//ljw,20160824for ACC
    
    static u32 Lac_b = 0;//ljw,20160824for ACC
    static u32 LacID_b = 0;//ljw,20160824for ACC    
#endif   
	unsigned char n,m;
	unsigned char tmp;
	position_message_struct position;
	gprs_message_head_struct head;
	u16 sendlen;
	u16 sendnum;
	u8 tmp_time[12];
	u8 additional_beyond[3]={0x11,0x01,0}; //超速附加字段
	u8 add_flag=0;
    u32 tmp_mileage = wzw_mileage.mileage_64 /3600;

//	wzw_mileage.mileage_64 += 36000;//wei,测试里程，需要注销
//ljw,20160908 test reset flag    erro_flag = 0;//ljw,20160906,测试复位错误标志量   测完要删
//ljw,20160906 测试点名复位问题    memset((u8 *)(&position_0200),0x00,sizeof(position_0200));//ljw,20160906,测试复位错误标志量   测完要删

	position.alarm = 0;
	position.status = 0;
	position.altitude = 0;
//ljw,20160906for space	while(GPRSCmdFlag == 1);
    if(send_gps_gsm_flag == GPS_FLAG)
    {
        position = GPS_Deal( position);  //wei,20160820 整理GPS子程序        
    }
    else
    {
       position = Deal_GSM_LAC( position);  //wei,20160820 整理GPS子程序
//ljw,20160906测完要删          Deal_GSM_LAC();//ljw,20160906测完要删 
    }
//ljw,20160908 test reset flag    erro_flag = 50;//ljw,20160906,测试复位错误标志量   测完要删
#if 0  //wei,20160820 整理GPS子程序
////////////////////wei
	//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
	// 拷贝时间
	if (dingwei_flag == 0x5a)
		i = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
	else
		i = (unsigned char *)strstr((char *)backup_g_ucNeo6QBuff,",");
	i++;
	memcpy((char *)(tmp_time+6),(char *)(i+1),6);	// 拷贝时间
	i +=6;
	i = (unsigned char *)strstr((char *)i,",");

	if(('A' ==*(i+1) ) &&(dingwei_flag))
	{
		position.status |= EXCURSION_POSITION_STATUS_ORIENTATION; 
	}
	// 拷贝GPS信息可用标志
	i = (unsigned char *)strstr((char *)(i+1),",");
	// 拷贝 纬度，
	position.latitude = asciitolatitude(i+1);
	i = (unsigned char *)strstr((char *)(i+1),",");
	if ('S' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_SOURTH; 
	}
	i = (unsigned char *)strstr((char *)(i+1),",");
	// 拷贝 经度，
	position.longitude = asciitolatitude(i+1); //asciitolongitude(i+1);
	i = (unsigned char *)strstr((char *)(i+1),",");
	//position.status = 0;
	if ('W' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_WEST; 
	}
	i = (unsigned char *)strstr((char *)(i+1),",");
	
//	if( 1 )//(strstr((char *)g_ucNeo6QBuff,",A,"))  // 已经判断过了
	{
			// 拷贝 速度
			ptmp= (unsigned char *)strstr((char *)(i),".");

			position.speed = asciitospeed(i+1,ptmp-i-1);
			
			//防止GPS数据中出现特别大的数据
			if (dingwei_flag  != 0x5a)  
			{
				position.speed = 0;
			}
			if((position.speed > system_para.g_ucMaxSpeed*10) &&(system_para.g_ucMaxSpeed>0))//wei zhu,20160804超速报警
			{
				position.alarm |= EXCURSION_POSITION_ALARM_BEYOND; 
				add_flag = 0x5a;
				if(SMS_speed_turn++>=3)//wei,20160520
					SMS_speed_turn=3;
//wei,20160520				if(system_para.g_bAUTOSMSon)
				if( (system_para.g_bAUTOSMSon)&&(SMS_speed_turn==1))//we,20160520
				{
					g_usCurrentSpeed = position.speed/10;
				}
				else
				{
					g_usCurrentSpeed = 0;
				}
			}
			else
//wei,20160520				position.alarm &= ~EXCURSION_POSITION_ALARM_BEYOND; 
			{//wei,20160520
				position.alarm &= ~EXCURSION_POSITION_ALARM_BEYOND; 	
				SMS_speed_turn = 0;//WEI,20160520
			}//wei,20160520
			while(GPRSCmdFlag == 1);
		 	i = (unsigned char *)strstr((char *)(i+1),",");
			ptmp= (unsigned char *)strstr((char *)(i+1),".");
			//
			// 拷贝 角度

			position.direction = asciitodirection(i+1,ptmp-i-1);
			//防止GPS数据中出现特别大的数据
			if (dingwei_flag  != 0x5a)
			{
				position.direction = 0;
			}
			i = (unsigned char *)strstr((char *)(i+1),",");
			memcpy((char *)(tmp_time),(char *)(i+1),6);	// 拷贝日期
//wei,   DDMMYYHHmmSS
			asciitodate(tmp_time,position.time);
//wei,for rtc <
            if (dingwei_flag == 0x5a)
            {
                memcpy(time_gps_rtc,position.time,6);//wei,20160810,将时间拷贝到time_gps_rtc数组用做RTC时间基准
                backsecondforRTC = secondcounter;
                RTC_timeavalibe_flag = 0x50;//wei,20160812 0x0a,时间无效  0x50,时间有效
            }
//wei,for rtc >
	}
/////////////////////wei,
#if 0 //wei,20160804 for space
      else
      	{
      		tmp = '0';
		strncat((char *)g_ucGPRSDataBuff,(char *)(&tmp),1);  // 拷贝 速度信息
		strcat((char *)g_ucGPRSDataBuff,",");
		strncat((char *)g_ucGPRSDataBuff,(char *)(&tmp),1);  // 拷贝 速度信息
		strcat((char *)g_ucGPRSDataBuff,",");
		i = (unsigned char *)strstr((char *)(i+1),",");
	}
#endif //wei,20160804 for space
#endif
  //ljw,20160904超速报警判断放回来<
    if((position.speed > system_para.g_ucMaxSpeed*10)&&(system_para.g_ucMaxSpeed>0)&&(send_gps_gsm_flag == GPS_FLAG))//wei zhu,20160804超速报警
    {
        position.alarm |= EXCURSION_POSITION_ALARM_BEYOND; 
        add_flag = 0x5a;
        if(SMS_speed_turn++>=3)//wei,20160520
            SMS_speed_turn=3;
//wei,20160520				if(system_para.g_bAUTOSMSon)
        if((system_para.g_bAUTOSMSon)&&(SMS_speed_turn==1))//we,20160520
        {
            g_usCurrentSpeed = position.speed/10;
        }
        else
        {
            g_usCurrentSpeed = 0;
        }
    }
    else
    {//wei,20160520
        position.alarm &= ~EXCURSION_POSITION_ALARM_BEYOND; 	
        SMS_speed_turn = 0;//WEI,20160520
    }//wei,20160520
//ljw,20160908 test reset flag    erro_flag =7;//ljw,20160906,测试复位错误标志量   测完要删
  //ljw,20160904超速报警判断放回来>
    memset(g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
    
    if(send_gps_gsm_flag == GSM_FLAG)//wei,20160822
        head.id = MOBILE_LOCATION_SEND;
    else head.id = gprsid;//wei,20160822
	if((flow_from_table == 0xffff)||((head.id == MOBILE_LOCATION_SEND)&&(flow_from_table == 0xffff)))
	{
		head.attribute = sizeof(position)+6;  // 增加了里程统计　　　01　********,也需要高地位变化
        if((ACC_GPS_flag != 0x5a)&&(ACC_GSM_flag != 0x5a)&&(system_para.g_bACCon&&(g_ucACCFlag !=0x5a)))//GPS
        {//wei,20160825,关ACC后的第一个位置处理，
            if(head.id == MOBILE_LOCATION_SEND)//wei,最后一次是基站定位
            {
                ACC_GSM_flag = 0x5a;
                ACC_GPS_flag = 0x00;
                Lac_b = position.latitude;	
                LacID_b = position.longitude;
            }
            else //wei,最后一次是GPS定位
            {
                ACC_GPS_flag = 0x5a;
                ACC_GSM_flag = 0x00;
                latitude_b = position.latitude;
                longitude_b = position.longitude;
            }
        }
        if((system_para.g_bACCon&&(g_ucACCFlag ==0x5a))||(!system_para.g_bACCon)||(T_P_time > 1))
        {//
            ACC_GPS_flag = 0x00;
            ACC_GSM_flag = 0x00;
        }
        if(ACC_GPS_flag ==0x5a)//wei,20160825,发GPS备份数据
        {
            position.latitude = latitude_b;	
            position.longitude = longitude_b;
            if(dingwei_flag != 0x5a)
                RTC2Position_time(&position.time[0]);//wei,20160825,如果GPS不定位，取系统的RTC时间
            position.speed = 0x0000;//wei,20160822
            position.altitude = 0x0000;
            position.direction = 0x0000;
        }
        else if(ACC_GSM_flag ==0x5a)//wei,20160825发基站备份数据
        {
            position.latitude = Lac_b;	
            position.longitude = LacID_b;
            position.direction = 0x0000;
        }
        
//        else //wei,20160825正常上发位置
//        {
//            ACC_GPS_flag = 0x00;
//            ACC_GSM_flag = 0x00;
//        }
	}
	else //点名
	{
        head.attribute = sizeof(position)+6;
        if(head.id != MOBILE_LOCATION_SEND)//ljw,20160908点名，基站数据不要加应答流水号，GPS要加
            head.attribute +=2;
		
         ACC_GPS_flag = 0x00;
         ACC_GSM_flag = 0x00;
	}
//    if(T_P_time > 1)//wei,20160825,临时追踪
//    {
//        ACC_GPS_flag = 0x00;
//        ACC_GSM_flag = 0x00;
//    }
	if (add_flag)
	{
		head.attribute += sizeof(additional_beyond);
	}
//ljw,20160908 test reset flag	erro_flag =8;//ljw,20160906,测试复位错误标志量   测完要删
	head.serial_number = flowwater++;
	//tmp_time[0]='0';
	//memcpy(&tmp_time[1],g_ucCurrentNum,sizeof(g_ucCurrentNum));
	
	//asciitohex (tmp_time,head.mbphonenum,12);
	memcpy((u8*)(head.mbphonenum),(u8*)(system_para.g_ucDeviceID),sizeof(system_para.g_ucDeviceID));
//ljw,20160908 test reset flag    erro_flag = 9;//ljw,20160906,测试复位错误标志量   测完要删
	if (g_ucAlarm)
	{
	//	if(voltage >power_battary)
		if( ((BatteryStatusGet()>50)))
	//20150907: 电压现在的电路可以在没有外接电源的情况下紧急报警。
			position.alarm |= EXCURSION_POSITION_ALARM_ALARM;
		else
			position.alarm &= ~EXCURSION_POSITION_ALARM_ALARM;//端外电会误触发紧急报警
		g_ucAlarm = 0;		
	}

//wei	if (get_ACC_CHECK_status()&&system_para.g_bACCon)
	if ((g_ucACCFlag ==0x5a)&&system_para.g_bACCon) //wei,20160524,ACC修改
	{
        position.status |= EXCURSION_POSITION_STATUS_ACC;
	}

//ljw,20160903,震动加了过滤	if ((Impact_flag) && (shake_gate))
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
    if((Impact_Alarm_flag==0xa5)&&(shake_gate)&&(g_ucACCFlag != 0x5a))
	{
        position.alarm |= EXCURSION_POSITION_ALARM_SHAKE;
        Impact_Alarm_flag = 0x0a;//ljw,20160903,震动加了过滤,警情发出去了
//ljw,20160903,震动加了过滤	          Impact_flag = 0;
	}
    else 
    {
        Impact_Alarm_flag = 0x00;
    }
#endif

	if ((voltage <= LOW_POWER)&&(voltage>POWEROFF)&&lowbatt_flag)
	{
		position.alarm |= EXCURSION_POSITION_ALARM_LOWPOWER; 
		position.alarm &= ~EXCURSION_POSITION_ALARM_POWEROFF; 
		lowbatt_flag = 0;
	}
	else
	if (voltage<=POWEROFF && lowbatt_flag)
	{
		position.alarm &=~ EXCURSION_POSITION_ALARM_LOWPOWER; 
		position.alarm |= EXCURSION_POSITION_ALARM_POWEROFF; 
		lowbatt_flag = 0;		
	}
	else
	{
		position.alarm &= ~EXCURSION_POSITION_ALARM_LOWPOWER; 
		position.alarm &= ~EXCURSION_POSITION_ALARM_POWEROFF; 
	}

	if (system_para.g_break == 1)
	{
		position.status|= EXCURSION_POSITION_STATUS_OILBREAKER; 
	}
	else
		position.status &= ~EXCURSION_POSITION_STATUS_OILBREAKER; 	

	//交换高低字节
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));
	
	swapHLchar((u8*)&(position.alarm),sizeof (position.alarm));	
	swapHLchar((u8*)&(position.altitude),sizeof (position.altitude));	
	swapHLchar((u8*)&(position.direction),sizeof (position.direction));	
	swapHLchar((u8*)&(position.latitude),sizeof (position.latitude));	
	swapHLchar((u8*)&(position.longitude),sizeof (position.longitude));	
	swapHLchar((u8*)&(position.speed),sizeof (position.speed));	
	swapHLchar((u8*)&(position.status),sizeof (position.status));
	swapHLchar((u8*)&(tmp_mileage),sizeof (tmp_mileage));

	if((flow_from_table == 0xffff)||(head.id == 0x900F))//wei,20160824FMOBILE_LOCATION_SEND
	{
		memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	    memcpy(&g_ucGPRSDataBuff[sizeof(head)],(u8*)&position,sizeof(position));
		g_ucGPRSDataBuff[sizeof(head)+sizeof(position)] =  0x01; // 里程统计的ID
		g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+1] =  0x04; // 里程统计的长度
		memcpy(&g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+2],(u8*)&tmp_mileage,sizeof(tmp_mileage));
		if (add_flag)
		{
 		    add_flag = 0;
			memcpy(&g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+6],additional_beyond,sizeof(additional_beyond));
			sendlen = constructor(g_ucGPRSDataBuff,sizeof(position)+sizeof(head)+sizeof(additional_beyond)+6);
		}
		else
		{
			sendlen = constructor(g_ucGPRSDataBuff,sizeof(position)+sizeof(head)+6);
		}
	}
	else
	{
		memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
		memcpy(&g_ucGPRSDataBuff[sizeof(head)], &flowwater_form_table,sizeof(flowwater_form_table));
	    memcpy(&g_ucGPRSDataBuff[sizeof(head)+2],(u8*)&position,sizeof(position));
		g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+2] =  0x01; // 里程统计的ID
		g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+3] =  0x04; // 里程统计的长度
		memcpy(&g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+4],(u8*)&tmp_mileage,sizeof(tmp_mileage));  
		  
		//sendlen = constructor(g_ucGPRSDataBuff,sizeof(position)+sizeof(head)+2);
		if (add_flag)//wei,zhu,20160804超速，添加超速附加信息
		{
			add_flag = 0;
			memcpy(&g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+8],additional_beyond,sizeof(additional_beyond));
			sendlen = constructor(g_ucGPRSDataBuff,sizeof(position)+sizeof(head)+sizeof(additional_beyond)+8);
		}	
		else
		{
			sendlen = constructor(g_ucGPRSDataBuff,sizeof(position)+sizeof(head)+8);
		}
	}
//ljw,20160908 test reset flagerro_flag =10;//ljw,20160906,测试复位错误标志量   测完要删
	//
	// 发送 CMD00
	//
//ljw,20160906for space 	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei
//ljw,20160908 test reset flagerro_flag =11;//ljw,20160906,测试复位错误标志量   测完要删
    //ljw,20160903 for space	DebugOut("\r[ -------------------- GPRS Data Send OK! --------------------- ]\r\n",0);

//ljw,20160903 for space	DebugOut("\r[ ------------ User ",0);
//ljw,20160903 for space	DebugOut(g_ucGPRSDataBuff+8,3);
//ljw,20160903 for space	DebugOut(" Request GPRS Data.Send OK! ------------ ]\r\n",0);
}

//wei,20160805 GPS提取函数<


u8 time_gps_rtc[6]={0};//wei,20160809,YYMMDDHHmmSS 保存GPS的时间，做为RTC时间的基准
u32 backsecondforRTC = 0;//wei,20160809, RTC起始计时的点
u8 RTC_timeavalibe_flag = 0x0a;//wei,20160812 0x0a,时间无效  0x50,时间有效
//YYMMDDHHmmSS 0x16 0x08 0x10 0x10 0x50 0x54
//             0    1    2    3    4    5
//#if 0

//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
u8 Is_Leap_Year(u16 year)
{
    if(year%4==0) //必须能被4整除
    {
        if(year%100==0) 
        {
            if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
            else return 0;   
        }
        else return 1;   
    }
    else return 0;	
}
 			   
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//平年的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u32 RTC_seccount = 0;//wei,20160810,年月日时分秒都转换成秒，在加走过的秒数。
tm timer;
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 seccount=0;
    if(syear<2000||syear>2099)return 1;	   
    for(t=2000;t<syear;t++)	//把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
        else seccount+=31536000;			  //平年的秒钟数
    }
    smon-=1;
    for(t=0;t<smon;t++)	   //把前面月份的秒钟数相加
    {
        seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数	   
    }
    seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加 
    seccount+=(u32)hour*3600;//小时秒钟数
    seccount+=(u32)min*60;	 //分钟秒钟数
    seccount+=sec;//最后的秒钟加上去
    RTC_seccount=seccount+(secondcounter - backsecondforRTC);//得到计数器中的值(秒钟数)
    return 0;	    
}

u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;
    
    RTC_Set((RTC_Bcd2ToByte(time_gps_rtc[0])+2000),RTC_Bcd2ToByte(time_gps_rtc[1]),RTC_Bcd2ToByte(time_gps_rtc[2]),\
    RTC_Bcd2ToByte(time_gps_rtc[3]),RTC_Bcd2ToByte(time_gps_rtc[4]),RTC_Bcd2ToByte(time_gps_rtc[5]));
	
    temp=RTC_seccount/86400;   //得到天数(秒钟数对应的)
    
	if(daycnt!=temp)//超过一天了
	{
		daycnt=temp;
		temp1 = 2000;	//重GPS获取到的年，开始
		while(temp>=365)
		{
			if(Is_Leap_Year(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟数
				else {temp1++;break;}  
			}
			else temp-=365;	  //平年 
			temp1++;  
		}
		timer.w_year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if(Is_Leap_Year(timer.w_year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break;
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		timer.w_month=temp1+1;//得到月份
		timer.w_date=temp+1;  //得到日期 
	}
	temp=RTC_seccount%86400;     //得到秒钟数   	   
	timer.hour=temp/3600;     //小时
	timer.min=(temp%3600)/60; //分钟	
	timer.sec=(temp%3600)%60; //秒钟
//	timer.week=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);//获取星期   
	return 0;
}
#if 1  //wei,把GPS单独拿出来处理
//#define GPS_OLD
position_message_struct GPS_Deal(position_message_struct position_in)
{
    static unsigned char *i;
	static unsigned char *ptmp;
	static unsigned char SMS_speed_turn = 0; //wei20160520
    
	unsigned char n,m;
	unsigned char tmp;
	position_message_struct position =position_in ;
//	gprs_message_head_struct head;
    
	u16 sendlen;
	u16 sendnum;
	u8 tmp_time[12];
	u8 additional_beyond[3]={0x11,0x01,0}; //超速附加字段
	u8 add_flag=0;
    
    position.alarm = 0;
	position.status = 0;
	position.altitude = 0;
    
    IWDG_ReloadCounter();//ljw,20160904防止下面的等待复位
//ljw,20160906for space	while(GPRSCmdFlag == 1);

	//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
	// 拷贝时间
#ifndef GPS_OLD //ljw,20160904新改的GPS
    
    if(dingweied_flag) //ljw,20160904只要是定过为，结构体里面就有数据
    {
        position.latitude = asciitolatitude(gps_ascii.Lat);           //提取经度
        if('S' == gps_ascii.uLat)                                      //提取经度标志
        {
            position.status |= EXCURSION_POSITION_STATUS_SOURTH; 
        }
        position.longitude = asciitolatitude(gps_ascii.Lon);           //提取纬度
        if ('W' == gps_ascii.uLon)                                     //提取纬度标志
        {
            position.status |= EXCURSION_POSITION_STATUS_WEST; 
        }
        i = gps_ascii.Spd;
        ptmp = (unsigned char *)strstr((char *)(i),".");
//        if(ptmp-i >0)&&(ptmp-i <=3)//ljw,20160907速度值长度判断
        position.speed = asciitospeed(i,ptmp-i);//asciitospeed(gps_ascii.Spd,strlen((const u8 *)gps_ascii.Spd)); 
//        else position.speed = 0;
        if(position.speed > 2200)//ljw,20160928速度大于220认为无效
            position.speed = 0;
        //提取速度
        //加速度判断
        i = gps_ascii.Cog;
        ptmp = (unsigned char *)strstr((char *)(i),".");        
        position.direction = asciitodirection(gps_ascii.Cog,ptmp-i);//ljw strlen((u8 *)gps_ascii.Cog)
        if(position.direction > 359)
            position.direction = 0;
        
        position.status |= EXCURSION_POSITION_STATUS_ORIENTATION;//ljw,20160920 只要进到发送GPS数据，就一定用定位。NOLocalCount>=120改两分钟为10秒 
        if(('A' ==gps_ascii.status ) &&(dingwei_flag))                //提取定位标志
        {
            memcpy((char *)(tmp_time+6),(char *)(gps_ascii.UTCtime),6);	  // 拷贝时间.             
            memcpy((char *)(tmp_time),(char *)(gps_ascii.Date),6);	// 拷贝日期
            asciitodate(tmp_time,position.time);
//ljw,20160920 NOLocalCount>=120改两分钟为0秒            position.status |= EXCURSION_POSITION_STATUS_ORIENTATION;
            memcpy(time_gps_rtc,position.time,6);//wei,20160810,将时间拷贝到time_gps_rtc数组用做RTC时间基准
            backsecondforRTC = secondcounter;
            RTC_timeavalibe_flag = 0x50;//wei,20160812 0x0a,时间无效  0x50,时间有效
        }
        else 
        {
            position.speed = 0;//未定位，速度强制为零
            position.direction = 0;//未定位，角度强制为零
            RTC_Get();//wei,20160810 测试RTC时间
            RTC2Position_time(&position.time[0]);//wei,20160825,取系统的RTC时间            
        }
#if 0 //ljw,20160904超速报警判断放到原来的send函数里面
        //超速报警
        if((position.speed > system_para.g_ucMaxSpeed*10) &&(system_para.g_ucMaxSpeed>0))//wei zhu,20160804超速报警
        {
            position.alarm |= EXCURSION_POSITION_ALARM_BEYOND; 
            add_flag = 0x5a;
            if(SMS_speed_turn++>=3)//wei,20160520
                SMS_speed_turn=3;
//wei,20160520				if(system_para.g_bAUTOSMSon)
            if( (system_para.g_bAUTOSMSon)&&(SMS_speed_turn==1))//we,20160520
            {
                g_usCurrentSpeed = position.speed/10;
            }
            else
            {
                g_usCurrentSpeed = 0;
            }
        }
        else
        {//wei,20160520
            position.alarm &= ~EXCURSION_POSITION_ALARM_BEYOND; 	
            SMS_speed_turn = 0;//WEI,20160520
        }//wei,20160520
#endif //ljw,20160904超速报警判断放到原来的send函数里面
    }
#endif
#ifdef GPS_OLD  //ljw,20160904修改GPS提取函数，放到一个固定的函数，一次性提取放到结构体gps_ascii里面
	if(dingwei_flag == 0x5a)
    {
        i = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
        i++;
        memcpy((char *)(tmp_time+6),(char *)(i),6);	// 拷贝时间
    }
	else
		i = (unsigned char *)strstr((char *)backup_g_ucNeo6QBuff,",");	
	i +=6;
	i = (unsigned char *)strstr((char *)i,",");

	if(('A' ==*(i+1) ) &&(dingwei_flag))
	{
		position.status |= EXCURSION_POSITION_STATUS_ORIENTATION; 
	}
	// 拷贝GPS信息可用标志
	i = (unsigned char *)strstr((char *)(i+1),",");
	// 拷贝 纬度，
//wei...    if(((system_para.g_bACCon)&&(g_ucACCFlag))||(!system_para.g_bACCon))
//wei...    {
//wei...        position.latitude = asciitolatitude(&latitude_b);
//wei...        latitude_b = position.latitude;//for ACC
//wei...    }
//wei...    else 
    position.latitude = asciitolatitude(i+1);
	i = (unsigned char *)strstr((char *)(i+1),",");
	if ('S' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_SOURTH; 
	}
	i = (unsigned char *)strstr((char *)(i+1),",");
	// 拷贝 经度，
//wei...    if(((system_para.g_bACCon)&&(g_ucACCFlag))||(!system_para.g_bACCon))
//wei...    {
        position.longitude = asciitolatitude(i+1); //asciitolongitude(i+1);
//wei...        longitude_b = position.longitude;
//wei...   }
//wei...	else position.longitude = asciitolongitude(&longitude_b);
	i = (unsigned char *)strstr((char *)(i+1),",");
	//position.status = 0;
	if ('W' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_WEST; 
	}
	i = (unsigned char *)strstr((char *)(i+1),",");
	
//wei	if( 1 )//(strstr((char *)g_ucNeo6QBuff,",A,"))  // 已经判断过了
//wei	{
        // 拷贝 速度
        ptmp= (unsigned char *)strstr((char *)(i),".");
        position.speed = asciitospeed(i+1,ptmp-i-1);			
        //防止GPS数据中出现特别大的数据
        if(dingwei_flag  != 0x5a)//wei...||((system_para.g_bACCon)&&(!g_ucACCFlag)))
        {
            position.speed = 0;
        }
        if((position.speed > system_para.g_ucMaxSpeed*10) &&(system_para.g_ucMaxSpeed>0))//wei zhu,20160804超速报警
        {
            position.alarm |= EXCURSION_POSITION_ALARM_BEYOND; 
            add_flag = 0x5a;
            if(SMS_speed_turn++>=3)//wei,20160520
                SMS_speed_turn=3;
//wei,20160520				if(system_para.g_bAUTOSMSon)
            if( (system_para.g_bAUTOSMSon)&&(SMS_speed_turn==1))//we,20160520
            {
                g_usCurrentSpeed = position.speed/10;
            }
            else
            {
                g_usCurrentSpeed = 0;
            }
        }
        else
//wei,20160520				position.alarm &= ~EXCURSION_POSITION_ALARM_BEYOND; 
        {//wei,20160520
            position.alarm &= ~EXCURSION_POSITION_ALARM_BEYOND; 	
            SMS_speed_turn = 0;//WEI,20160520
        }//wei,20160520
 //ljw,20160906for space       while(GPRSCmdFlag == 1);
        i = (unsigned char *)strstr((char *)(i+1),",");
        ptmp= (unsigned char *)strstr((char *)(i+1),".");
        
        // 拷贝 角度
        position.direction = asciitodirection(i+1,ptmp-i-1);
        //防止GPS数据中出现特别大的数据
        if (dingwei_flag  != 0x5a)  
        {
            position.direction = 0;
        }
        i = (unsigned char *)strstr((char *)(i+1),",");
        if(dingwei_flag == 0x5a)
        {
            memcpy((char *)(tmp_time),(char *)(i+1),6);	// 拷贝日期
            asciitodate(tmp_time,position.time);
        }
        else
        {
            RTC_Get();//wei,20160810 测试RTC时间
            RTC2Position_time(&position.time[0]);//wei,20160825,取系统的RTC时间
//            position.time[0] = RTC_ByteToBcd2((u8)(timer.w_year-2000));
//            position.time[1] = RTC_ByteToBcd2(timer.w_month);
//            position.time[2] = RTC_ByteToBcd2(timer.w_date);
//            position.time[3] = RTC_ByteToBcd2(timer.hour);
//            position.time[4] = RTC_ByteToBcd2(timer.min);
//            position.time[5] = RTC_ByteToBcd2(timer.sec);
        }
        if (dingwei_flag == 0x5a)//更新基准时间
        {
            memcpy(time_gps_rtc,position.time,6);//wei,20160810,将时间拷贝到time_gps_rtc数组用做RTC时间基准
            backsecondforRTC = secondcounter;
            RTC_timeavalibe_flag = 0x50;//wei,20160812 0x0a,时间无效  0x50,时间有效
        }
//wei	}
#endif //ljw,20160904修改GPS提取函数，放到一个固定的函数，一次性提取放到结构体gps_ascii里面
        

    return position ;
}
//wei,20160825,取系统的RTC时间
void RTC2Position_time(u8 *p)
{
    p[0] = RTC_ByteToBcd2((u8)(timer.w_year-2000));
    p[1] = RTC_ByteToBcd2(timer.w_month);
    p[2] = RTC_ByteToBcd2(timer.w_date);
    p[3] = RTC_ByteToBcd2(timer.hour);
    p[4] = RTC_ByteToBcd2(timer.min);
    p[5] = RTC_ByteToBcd2(timer.sec);
}
#endif
#if 0
u8 time_ungps_flag =0x00;//wei,20160805,时间有效标志，此标志量也可以不要，直接用dingweied_flag来兼顾
void Time_Deal(void);
{
    u32 temp_second =0 ;
}
#endif
//wei,20160805 GPS提取函数>
/*
**-----------------------------------------------------------------------------
**
**	概述:
**		主动上传数据
**
**	参数:
**		无
**
**	返回:
**		无
**
**-----------------------------------------------------------------------------
*/
u8 send_gps_gsm_flag = 0;//wei,20160822 0x5a,GSM  0xa5,GPS
void cmd_0200(void)
{
//ljw,20160906for space	while(GPRSCmdFlag == 1);
	gps_gate = 0;
//wei,20160825	if(((!GetAvailableGPSInfoV())&&(dingweied_flag == 0))||(NOLocalCount>=120))
//ljw,20160920没必要    GetAvailableGPSInfoV_N(2);//ljw,20160920
    if((ACC_GPS_flag !=0x5a)&&(dingwei_flag == 0))//(((!GetAvailableGPSInfoV())&&(dingweied_flag == 0))||(NOLocalCount>=10)))//ljw,20160920 NOLocalCount>=120改两分钟为10秒
	{
	  	//cmd_0002();
//wei,20160822	  	uploadLAC();
        send_gps_gsm_flag = GSM_FLAG;//wei,20160822
//wei,20160823		gps_gate = 1;
//wei,20160823	  	return;
	}
    else 
    {
        send_gps_gsm_flag = GPS_FLAG;
        ACC_GSM_flag = 0x00;//wei,20160825,把GSM发送标志清零
    }
	send_location_gprs(0xffff,POSITION_REPORT);
	gps_gate = 1;
}

// 心跳命令
void cmd_0002(void)
{
	u16 sendlen;
	gprs_message_head_struct head;
	head.id = HEARTBEAT;
	head.serial_number = flowwater++;

	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
	head.attribute = 0;
	
	//交换高低字节
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));
	
	//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	sendlen = constructor(g_ucGPRSDataBuff, sizeof(head));

//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei
}


// status : 0：成功/确认；1：失败；2：消息有误；3：不支持
void cmd_GENERAL_RESPONSION(message_status status1)
{
	gprs_message_head_struct head;
	u8* ptmp;
	u16 sendlen;
	
	head.id = GENERAL_RESPONSION;
	head.serial_number = flowwater++;
	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
	head.attribute = 5;
//wei
			//交换高低字节
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));
//wei	
	//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	ptmp = &(g_ucGPRSDataBuff[sizeof(head)]);
	memcpy(ptmp, &flowwater_form_table,sizeof(flowwater_form_table));
	ptmp += sizeof(flowwater_form_table);
    swapHLchar((u8*)&(message_id_from_table),sizeof(message_id_from_table));//ljw,20161217,修复终端通用应答
	memcpy(ptmp, &message_id_from_table,sizeof(message_id_from_table));
	ptmp += sizeof(message_id_from_table);
	*ptmp = status1;


	
	sendlen = constructor(g_ucGPRSDataBuff, 5+sizeof(head));

//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei


}


//设置终端参数
void cmd_SET_TERMINAL_PARAMETER(void)
{
	u32 parameter_id,u32tmp;
	u8 parameter_num;
	u8* ptmp;
	u8 i,j;
	u16 sendlen;
	volatile u8 recelen=0;

	ptmp = & g_ucGPRSCmdBuff[12];
	parameter_num = *ptmp; 	//参数总字节数
//	if (parameter_num > 1)　//不止一项，	
      // parameter_id = bigbuf.big.g_ucGPRSCmdBuff[1]*0x100+bigbuf.big.g_ucGPRSCmdBuff[2];
      ptmp ++;//   = 8; //bigbuf.big.g_ucGPRSCmdBuff[12]
	for(i =0; i<parameter_num;)	
	{
		//parameter_id = *(u32 *)ptmp ; //如果不是双字对齐的话，这边会报HARDFAULT
		memcpy((u8*)&parameter_id,ptmp,sizeof(parameter_id));
		//交换高低字节
		swapHLchar((u8*)&(parameter_id),sizeof (parameter_id));
		
		ptmp +=4;
		//i +=4;
		switch(parameter_id)
		{
			case  ALTERNATION_HEARTBEAT:
				//system_para.HeartAternationTime = *(u32*)(ptmp);
				ptmp ++;
				memcpy((u8*)&system_para.HeartAternationTime,ptmp,sizeof(system_para.HeartAternationTime));
				//交换高低字节
				swapHLchar((u8*)&(system_para.HeartAternationTime),sizeof (system_para.HeartAternationTime));
				 if(system_para.HeartAternationTime <10) //wei,20160603强制心跳间隔不能小于10s
				     system_para.HeartAternationTime = 10; //wei,20160603
				 if(system_para.HeartAternationTime >300) //wei,20160603强制心跳间隔不能小于10s
				     system_para.HeartAternationTime = 300; //wei,20160603    
				ptmp += 4;
				i ++;
					break;

			case  APN:
//wei				memset(system_para.g_ucNetAPN,0,20);
                memset(system_para.g_ucNetAPN,0,NETAPN_LEN);
				recelen = *ptmp;
				ptmp ++;
//wei				if (strlen(ptmp) <= 20)
//wei,20160705				if (strlen(ptmp) < NETAPN_LEN)  //wei
                if (recelen < NETAPN_LEN)  //wei
					memcpy(system_para.g_ucNetAPN,ptmp,recelen);
				else
//wei					memcpy(system_para.g_ucNetAPN,ptmp,20);
				memcpy(system_para.g_ucNetAPN,ptmp,NETAPN_LEN-1);
				ptmp += recelen;
				i ++;
				break;			
			case  GPRS_USERNAME:
//wei				memset(system_para.g_ucNetUser,0,6);
                          memset(system_para.g_ucNetUser,0,NETUSER_LEN);
				recelen = *ptmp;
				ptmp ++;
//wei				if (strlen(ptmp) <= 6)
				if (recelen < NETUSER_LEN) //wei
					memcpy(system_para.g_ucNetUser,ptmp,recelen);
				else
//wei					memcpy(system_para.g_ucNetUser,ptmp,6);
				memcpy(system_para.g_ucNetUser,ptmp,NETUSER_LEN-1);  //wei
				ptmp += recelen;
				i ++;
				break;
			case  GPRS_PASSWORD:
//wei				memset(system_para.g_ucNetPsw,0,8);
                          memset(system_para.g_ucNetPsw,0,NETPSW_LEN);
				recelen = *ptmp;
				ptmp ++;
//wei				if (strlen(ptmp) <= 8)
				if (recelen < NETPSW_LEN)  //wei
					memcpy(system_para.g_ucNetPsw,ptmp,recelen);
				else
//wei					memcpy(system_para.g_ucNetPsw,ptmp,8);
					memcpy(system_para.g_ucNetPsw,ptmp,NETPSW_LEN-1);
				ptmp += recelen;
				i ++;				
				break;

			case IP_ADDR:

				recelen = *ptmp;
				ptmp ++;		
				if (FHTIP != system_para.g_ucSysMode_S)
				{
//WEI					memset(system_para.g_ucIPAddress,0,16);
//WEI					memset(system_para.g_ucIPAddress,0,IPADD_LEN);
//WEI					if (strlen(ptmp) <= 15)
//WEI						memcpy(system_para.g_ucIPAddress,ptmp,strlen(ptmp));
//WEI					else
//WEI						memcpy(system_para.g_ucIPAddress,ptmp,15);
					//WEI					
					memset(system_para.g_ucIPAddress,0,IPADD_LEN);
					if (recelen <= IPADD_LEN-1)
						memcpy(system_para.g_ucIPAddress,ptmp,recelen);
					else
						memcpy(system_para.g_ucIPAddress,ptmp,IPADD_LEN-1);
					//WEI
				}
				ptmp += recelen;
				i ++;				
				break;
			case  UDP_PORT: //发来的是HEX的要转成ａｓｃｉｉ字符
				
				recelen = *ptmp;
				ptmp ++;
				if (FHTIP != system_para.g_ucSysMode_S)
				{
					memset(system_para.g_ucUDPPort,0,5);
					//u32tmp = *(u32*)(ptmp);
					memcpy((u8*)&u32tmp,ptmp,sizeof(u32tmp));
					
					//交换高低字节
					swapHLchar((u8*)&(u32tmp),sizeof (u32tmp));
					

					j = u32toport(u32tmp,(u8*)(&system_para.g_ucUDPPort));
				}
				ptmp += recelen;
				i ++;				
				break;
//			case  SET_AS_TIMING:
				//cmd_OIL_BREAK();
//				break;	

			case ALTERNATION_IN_SLEEP:
				recelen = *ptmp;
				ptmp ++;				
				//system_para.ALTERNATION_IN_SLEEPTime = *(u32*)(ptmp);
				memcpy((u8*)&system_para.ALTERNATION_IN_SLEEPTime,ptmp,sizeof(system_para.ALTERNATION_IN_SLEEPTime));
				
				//交换高低字节
				swapHLchar((u8*)&(system_para.ALTERNATION_IN_SLEEPTime),sizeof (system_para.ALTERNATION_IN_SLEEPTime));
				
				ptmp += recelen;
				i ++;			
				break;

			case  ALTERNATION_IN_ALARM:
				recelen = *ptmp;
				ptmp ++;				
				//system_para.ALTERNATION_IN_ALARMTime = *(u32*)(ptmp);
				memcpy((u8*)&system_para.ALTERNATION_IN_ALARMTime,ptmp,sizeof(system_para.ALTERNATION_IN_ALARMTime));
				
				//交换高低字节
				swapHLchar((u8*)&(system_para.ALTERNATION_IN_ALARMTime),sizeof (system_para.ALTERNATION_IN_ALARMTime));
				ptmp += recelen;
				i ++;	

			case  ALTERNATION_DEFAULT_POSITION:  
				recelen = *ptmp;
				ptmp ++;				
//				system_para.g_usGPRSUpLoadAternationTime = *(u32*)(ptmp);
				memcpy((u8*)&system_para.g_usGPRSUpLoadAternationTime,ptmp,sizeof(system_para.g_usGPRSUpLoadAternationTime));

				//交换高低字节
				swapHLchar((u8*)&(system_para.g_usGPRSUpLoadAternationTime),sizeof (system_para.g_usGPRSUpLoadAternationTime));
				ptmp += recelen;
				i ++;					
				break;
						
			case  HIGH_SPEED:
				recelen = *ptmp;
				ptmp ++;				
				//system_para.g_ucMaxSpeed = (u8)(*(u32*)(ptmp));
				memcpy((u8*)&system_para.g_ucMaxSpeed,ptmp,sizeof(system_para.g_ucMaxSpeed));
				
				//交换高低字节
				swapHLchar((u8*)&(system_para.g_ucMaxSpeed),sizeof (system_para.g_ucMaxSpeed));
				
				ptmp += recelen;
				i ++;					
				break;

			default:
				break;


		}		
		
		
	}
	Store_System_Para();
	cmd_GENERAL_RESPONSION(message_SUCCESS);

	if( (parameter_id == IP_ADDR) ||(parameter_id ==UDP_PORT))
	{
		if (FHTIP != system_para.g_ucSysMode_S)
		{	
			delayms(2000);
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
//wei,20160617			delayms(600);
//wei,20160617			StartGPRS();	
			
//wei,20160617			for(i=0; i<5;i++)
               for(i=1; i<8;i++)
		       {
                   if((system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE)&&(g_ucGotoSleepFlag == 0x5a))
                    break ;//wei,20160617,防止到休眠时间了，却无法休眠，死在拨号循环里面
                   if((i%2==0)||(CPIN_READY_flag == 0))
                       GSM_reset();
			       if(GetGPRSStatus() != GPRS_CONNECTED)
                   {
                       delayms(3500); 
                       dial_status =0;//wei20160621
                       StartGPRS();	
                   }
                   else
                      break;
                   if(dial_status==1)//wei,20160621 if Dial-up success ,break
                        break;
		       }
//wei,20160617			if (i>4)
               if (i>7)
			{
                GSM_Power(FALSE); //wei,20160608看门狗复位前，给GSM断电
                while(1); //等看门狗复位系统
			}
		}
	}
}

//应该是把所以的参数一起发送给服务器
void cmd_INQUIRE_TERMINAL_PARAMETER(void)
{
	u32 parameter_id;
	u8 parameter_num;
	//u8* ptmp; //ｒｅｃｅ
	//u8 i;
	gprs_message_head_struct head;
	u8* ptmp_send;
	u16 sendnum;
	u16 sendlen;
	u32 u32tmp;

	head.id = INQUIRE_TERMINAL_PARAMETER_RESPONSION;
	head.serial_number = flowwater++;
	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
//	head.attribute = 5;   //一开始不知道，要等统计完了才行

	//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	//memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	ptmp_send = &(g_ucGPRSDataBuff[sizeof(head)]);
	memcpy(ptmp_send, &flowwater_form_table,sizeof(flowwater_form_table));
	//ptmp_send += sizeof(flowwater_form_table);
	sendnum = sizeof(flowwater_form_table);


	parameter_num = 10; 	//参数总数

     
	//这才是详细的参数数目
      *(ptmp_send+sendnum++) = parameter_num;
	 // sendnum ++;
	

//查询终端参数应答
// 消息ID：0x0104。
// 查询终端参数应答消息体数据格式见表16。
// 表16 查询终端参数应答消息体数据格式
// 起始字节                     字段             数据类型              描述及要求
// 0                           应答流水号            WORD                        对应的终端参数查询消息的流水号
// 2                         应答参数个数           BYTE
// 3                               参数项列表                                        参数项格式和定义见表10

			
	//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
	u32tmp = ALTERNATION_HEARTBEAT;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);
	//交换高低字节
	swapHLchar(ptmp_send+sendnum,4);
	 
	sendnum += 4; 
	*(ptmp_send+sendnum) = 4;
	//u32tmp = 4;
	//memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	//交换高低字节
	//swapHLchar(ptmp_send+sendnum,4);	
	sendnum ++;
	//*(u32*)(ptmp_send+sendnum) = system_para.HeartAternationTime ; 
	u32tmp = system_para.HeartAternationTime ;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);
	//交换高低字节
	swapHLchar(ptmp_send+sendnum,4);	
	sendnum += 4; 
	


	//*(u32*)(ptmp_send+sendnum) = APN;
	u32tmp = APN;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	//交换高低字节
	swapHLchar(ptmp_send+sendnum,4);	
	sendnum += 4; 

//wei	if (strlen(system_para.g_ucNetAPN) <= 20)
    if(strlen(system_para.g_ucNetAPN) < NETAPN_LEN)
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucNetAPN,strlen(system_para.g_ucNetAPN));
		*(ptmp_send+sendnum) = strlen(system_para.g_ucNetAPN)+1;//wei +1 for  "," ?
		sendnum ++;	
		sendnum += strlen(system_para.g_ucNetAPN)+1; //the end of string is /0 

	}
	else
	{
//wei		memcpy(ptmp_send+sendnum+1,system_para.g_ucNetAPN,20);
        memcpy(ptmp_send+sendnum+1,system_para.g_ucNetAPN,NETAPN_LEN-1);
		*(ptmp_send+sendnum) =( NETAPN_LEN-1)+1;//wei   21;
		sendnum ++;						
		sendnum +=( NETAPN_LEN-1)+1; //wei  21;
	}

	//*(u32*)(ptmp_send+sendnum) = GPRS_USERNAME;
	u32tmp = GPRS_USERNAME;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
		//交换高低字节
	swapHLchar(ptmp_send+sendnum,4);
	sendnum += 4; 				


	if (strlen(system_para.g_ucNetUser) < NETUSER_LEN)
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucNetUser,strlen(system_para.g_ucNetUser));
		*(ptmp_send+sendnum) = strlen(system_para.g_ucNetUser)+1;
		sendnum ++;	
		sendnum += strlen(system_para.g_ucNetUser)+1; //the end of string is /0 

	}
	else
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucNetUser,NETUSER_LEN-1);
		*(ptmp_send+sendnum) = (NETUSER_LEN-1)+1;
		sendnum ++;						
		sendnum += (NETUSER_LEN-1)+1 ;
	}				

	// *(u32*)(ptmp_send+sendnum) = GPRS_PASSWORD;
	u32tmp = GPRS_PASSWORD;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
	//交换高低字节
	swapHLchar(ptmp_send+sendnum,4);	
	sendnum += 4; 				


	if (strlen(system_para.g_ucNetPsw) < NETPSW_LEN)
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucNetPsw,strlen(system_para.g_ucNetPsw));
		*(ptmp_send+sendnum) = strlen(system_para.g_ucNetPsw)+1;
		sendnum ++;	
		sendnum += strlen(system_para.g_ucNetPsw)+1; //the end of string is /0 

	}
	else
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucNetPsw,NETPSW_LEN-1);//,8);
		*(ptmp_send+sendnum) = (NETPSW_LEN-1)+1;//9;
		sendnum ++;						
		sendnum += (NETPSW_LEN-1)+1;//9;
	}				


	//*(u32*)(ptmp_send+sendnum) = IP_ADDR;
	u32tmp = IP_ADDR;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
	//交换高低字节
	swapHLchar(ptmp_send+sendnum,4);	
	sendnum += 4; 				


//wei	if (strlen(system_para.g_ucIPAddress) <= 16)
	if (strlen(system_para.g_ucIPAddress) <= 15)
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucIPAddress,strlen(system_para.g_ucIPAddress));
		*(ptmp_send+sendnum) = strlen(system_para.g_ucIPAddress)+1;
		sendnum ++;	
		sendnum += strlen(system_para.g_ucIPAddress)+1; //the end of string is /0 

	}
	else
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucIPAddress,16);
		*(ptmp_send+sendnum) = 17;
		sendnum ++;						
		sendnum += 17;
	}	
	


	//*(u32*)(ptmp_send+sendnum) = UDP_PORT;
	u32tmp = UDP_PORT;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
	//交换高低字节
	swapHLchar(ptmp_send+sendnum,4);	
	sendnum += 4; 				
#if 0
	if (strlen(system_para.g_ucUDPPort) <= 5)
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucUDPPort,strlen(system_para.g_ucUDPPort));
		*(ptmp_send+sendnum) = strlen(system_para.g_ucUDPPort)+1;
		sendnum ++;	
		sendnum += strlen(system_para.g_ucUDPPort)+1; //the end of string is /0 

	}
	else
	{
		memcpy(ptmp_send+sendnum+1,system_para.g_ucUDPPort,5);
		*(ptmp_send+sendnum) = 5;
		sendnum ++;						
		sendnum += 5;
	}	
#endif
	*(ptmp_send+sendnum) = 4;
//	u32tmp = 4;
//	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	

	sendnum ++;
	 
	//*(u32*)(ptmp_send+sendnum) = porttou32((u8*)(&system_para.g_ucUDPPort));
	u32tmp = porttou32((u8*)(&system_para.g_ucUDPPort));
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
	swapHLchar(ptmp_send+sendnum,4);
	sendnum +=4;
	//break;
//			case  SET_AS_TIMING:
	//cmd_OIL_BREAK();
//				break;	

	// *(u32*)(ptmp_send+sendnum) = ALTERNATION_IN_SLEEP;
	u32tmp = ALTERNATION_IN_SLEEP;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
	swapHLchar(ptmp_send+sendnum,4);
	sendnum += 4; 
	*(ptmp_send+sendnum) = 4;
	//u32tmp = 4;
	//memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
	sendnum ++;				
	// *(u32*)(ptmp_send+sendnum) = system_para.ALTERNATION_IN_SLEEPTime ;
	u32tmp = system_para.ALTERNATION_IN_SLEEPTime ;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
	swapHLchar(ptmp_send+sendnum,4);

	sendnum += 4;


	//*(u32*)(ptmp_send+sendnum) = ALTERNATION_IN_ALARM;
	u32tmp = ALTERNATION_IN_ALARM;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	
	swapHLchar(ptmp_send+sendnum,4);

	sendnum += 4; 
	*(ptmp_send+sendnum) = 4;
	//u32tmp = 4;
	//memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);		
	sendnum ++;	
	
	//*(u32*)(ptmp_send+sendnum) = system_para.ALTERNATION_IN_ALARMTime  ;
	u32tmp = system_para.ALTERNATION_IN_ALARMTime  ;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);		
	swapHLchar(ptmp_send+sendnum,4);

	sendnum += 4;


	//*(u32*)(ptmp_send+sendnum) = ALTERNATION_DEFAULT_POSITION;
	u32tmp = ALTERNATION_DEFAULT_POSITION  ;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);		
	swapHLchar(ptmp_send+sendnum,4);

	sendnum += 4; 
	*(ptmp_send+sendnum) = 4;
	//u32tmp = 4  ;
	//memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);		
	sendnum ++;					
	//*(u32*)(ptmp_send+sendnum) = system_para.g_usGPRSUpLoadAternationTime ;
	u32tmp =  system_para.g_usGPRSUpLoadAternationTime ;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	swapHLchar(ptmp_send+sendnum,4);

	sendnum += 4;

	//*(u32*)(ptmp_send+sendnum) = HIGH_SPEED;
	u32tmp =  HIGH_SPEED;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);		
	swapHLchar(ptmp_send+sendnum,4);

	sendnum += 4; 
	*(ptmp_send+sendnum) = 4;
	//u32tmp =  4;
	//memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);		
	
	sendnum ++;					
	//*(u32*)(ptmp_send+sendnum) = system_para.g_ucMaxSpeed ;
	u32tmp =  system_para.g_ucMaxSpeed ;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);		
	
	swapHLchar(ptmp_send+sendnum,4);

	sendnum += 4;


      head.attribute = sendnum;
		//交换高低字节
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));	  
      memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	  
	sendlen = constructor(g_ucGPRSDataBuff, sendnum+sizeof(head));

//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei
}

//消息ID：0x8105。
//终端控制消息体数据格式见表17。
//表17 终端控制消息体数据格式
//19
//起始字节    字段      数据类型    描述及要求
//0          命令字        BYTE    终端控制命令字说明见表18
// 1         命令参数       STRING  命令参数格式具体见后面描述，每个字段之间采用
//半角”;”分隔，每个STRING 字段先按GBK 编码处
//理后再组成消息
//表18 终端控制命令字说明
//命令字 命令参数 描述及要求
// 1 命令参数格式见表19 无线升级。参数之间采用半角分号分隔。指令如下：“URL 地址;拨号点
//名称;拨号用户名;拨号密码;地址;TCP 端口;UDP 端口;制造商ID; 硬件
//版本;固件版本; 连接到指定服务器时限”，若某个参数无值，则放空
// 2 命令参数格式见表19 控制终端连接指定服务器。参数之间采用半角分号分隔。控制指令如下：
//“连接控制;监管平台鉴权码;拨号点名称;拨号用户名;拨号密码;地
//址;TCP 端口;UDP 端口;连接到指定服务器时限”，若某个参数无值，则
//放空，若连接控制值为1，则无后继参数
// 3 无 终端关机
// 4 无 终端复位
//5 无 终端恢复出厂设置
//6 无 关闭数据通信
//7 无 关闭所有无线通信



void cmd_SET_GPRS_SERVICER(void)
{



}
//查询参数列表
//起始字节 字段                   数据类型                        描述及要求
// 0                参数总数              BYTE                             参数总数为n
// 1               参数ID 列表           BYTE[4*n]                       参数顺序排列，如“参数ID1 参数ID2......参数IDn”。
void cmd_INQUIRE_ONE_TERMINAL_PARAMETER(void)
{
	u32 parameter_id,u32tmp,u32tmp1;
	u8 parameter_num;
	u8* ptmp;
	u8 i;
	gprs_message_head_struct head;
	u8* ptmp_send;
	u16 sendnum;
	u16 sendlen;

	head.id = INQUIRE_TERMINAL_PARAMETER_RESPONSION;
	head.serial_number = flowwater++;
	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
//	head.attribute = 5;   //一开始不知道，要等统计完了才行

	//g_ucGPRSDataBuff as send buf
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	//memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	ptmp_send = &(g_ucGPRSDataBuff[sizeof(head)]);
	memcpy(ptmp_send, &flowwater_form_table,sizeof(flowwater_form_table));
	//ptmp_send += sizeof(flowwater_form_table);
	sendnum = sizeof(flowwater_form_table);


	ptmp = & g_ucGPRSCmdBuff[12];
//	parameter_num = *ptmp; 	//参数总数
//	if (parameter_num > 1)　//不止一项，	
      // parameter_id = bigbuf.big.g_ucGPRSCmdBuff[1]*0x100+bigbuf.big.g_ucGPRSCmdBuff[2];
      parameter_num = *ptmp; //这才是详细的参数数目
      *(ptmp_send+sendnum++) = parameter_num;
	// sendnum ++;
      ptmp ++;
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 感觉这里的发送和接收的缓冲区搞的混乱了
	for(i =0; i<parameter_num;i++)	
	{
		//parameter_id = *(u32 *)ptmp ;
		//	u32tmp =  system_para.g_ucMaxSpeed ;
		//memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);		
		memcpy((u8*)&parameter_id,(u8*)ptmp,4);	
		swapHLchar((u8*)&parameter_id,4);		
		ptmp +=4;
		switch(parameter_id)
		{

//查询终端参数应答
// 消息ID：0x0104。
// 查询终端参数应答消息体数据格式见表16。
// 表16 查询终端参数应答消息体数据格式
// 起始字节                     字段             数据类型              描述及要求
// 0                           应答流水号            WORD                        对应的终端参数查询消息的流水号
// 2                         应答参数个数           BYTE
// 3                               参数项列表                                        参数项格式和定义见表10

			case  ALTERNATION_HEARTBEAT:
				//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
				u32tmp = ALTERNATION_HEARTBEAT;
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);				
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
			
				sendnum ++;
				//*(u32*)(ptmp_send+sendnum) = system_para.HeartAternationTime ; 
				u32tmp = system_para.HeartAternationTime ; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 
				break;

			case  APN:
				//*(u32*)(ptmp_send+sendnum) = APN;
				u32tmp = APN; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 

//wei				if (strlen(system_para.g_ucNetAPN) <= 20)
                if (strlen(system_para.g_ucNetAPN) <= NETAPN_LEN-1)
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucNetAPN,strlen(system_para.g_ucNetAPN));
					*(ptmp_send+sendnum) = strlen(system_para.g_ucNetAPN)+1;
					sendnum ++;	
					sendnum += strlen(system_para.g_ucNetAPN)+1; //the end of string is /0 

				}
				else
				{
//wei					memcpy(ptmp_send+sendnum+1,system_para.g_ucNetAPN,20);
                    memcpy(ptmp_send+sendnum+1,system_para.g_ucNetAPN,NETAPN_LEN-1);
					*(ptmp_send+sendnum) = (NETAPN_LEN-1)+1;
					sendnum ++;						
					sendnum +=  (NETAPN_LEN-1)+1;
				}
				break;			
			case  GPRS_USERNAME:
				//*(u32*)(ptmp_send+sendnum) = GPRS_USERNAME;
				u32tmp = GPRS_USERNAME; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
				
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 				


				if (strlen(system_para.g_ucNetUser) < NETUSER_LEN)
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucNetUser,strlen(system_para.g_ucNetUser));
					*(ptmp_send+sendnum) = strlen(system_para.g_ucNetUser)+1;
					sendnum ++;	
					sendnum += strlen(system_para.g_ucNetUser)+1; //the end of string is /0 

				}
				else
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucNetUser,NETUSER_LEN-1);
					*(ptmp_send+sendnum) = (NETUSER_LEN-1)+1;
					sendnum ++;						
					sendnum +=  (NETUSER_LEN-1)+1;
				}				
				break;
			case  GPRS_PASSWORD:
				//*(u32*)(ptmp_send+sendnum) = GPRS_PASSWORD;
				//!!!!!!!!!!!!!!!!!!!!!!
				u32tmp = GPRS_PASSWORD; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 				


				if (strlen(system_para.g_ucNetPsw) < NETPSW_LEN)
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucNetPsw,strlen(system_para.g_ucNetPsw));
					*(ptmp_send+sendnum) = strlen(system_para.g_ucNetPsw)+1;
					sendnum ++;	
					sendnum += strlen(system_para.g_ucNetPsw)+1; //the end of string is /0 

				}
				else
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucNetPsw,NETPSW_LEN-1);//wei,  8);
					*(ptmp_send+sendnum) =  (NETPSW_LEN-1)+1;
					sendnum ++;						
					sendnum += ((NETPSW_LEN-1)+1);
				}				
				break;

			case IP_ADDR:

				//*(u32*)(ptmp_send+sendnum) = IP_ADDR;
				u32tmp = IP_ADDR; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 				


//wei				if (strlen(system_para.g_ucIPAddress) <= 16)
				if (strlen(system_para.g_ucIPAddress) <= 15)
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucIPAddress,strlen(system_para.g_ucIPAddress));
					*(ptmp_send+sendnum) = strlen(system_para.g_ucIPAddress)+1;
					sendnum ++;	
					sendnum += strlen(system_para.g_ucIPAddress)+1; //the end of string is /0 

				}
				else
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucIPAddress,16);
					*(ptmp_send+sendnum) = 17;
					sendnum ++;						
					sendnum += 17;
				}	
				
				break;
			case  UDP_PORT:

				//*(u32*)(ptmp_send+sendnum) = UDP_PORT;
				u32tmp = UDP_PORT; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 				
#if 0
				if (strlen(system_para.g_ucUDPPort) <= 5)
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucUDPPort,strlen(system_para.g_ucUDPPort));
					*(ptmp_send+sendnum) = strlen(system_para.g_ucUDPPort)+1;
					sendnum ++;	
					sendnum += strlen(system_para.g_ucUDPPort)+1; //the end of string is /0 

				}
				else
				{
					memcpy(ptmp_send+sendnum+1,system_para.g_ucUDPPort,5);
					*(ptmp_send+sendnum) = 5;
					sendnum ++;						
					sendnum += 5;
				}	
#endif	
				*(ptmp_send+sendnum) = 4;
			
				sendnum ++;
				u32tmp = porttou32((u8*)(&system_para.g_ucUDPPort));
				//*(u32*)(ptmp_send+sendnum) = u32tmp;
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);				
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum +=4;

				break;
//			case  SET_AS_TIMING:
				//cmd_OIL_BREAK();
//				break;	

			case ALTERNATION_IN_SLEEP:
				//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
				u32tmp = ALTERNATION_IN_SLEEP; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
				
				sendnum ++;				
				 //*(u32*)(ptmp_send+sendnum) = system_para.ALTERNATION_IN_SLEEPTime ;
				u32tmp = system_para.ALTERNATION_IN_SLEEPTime; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					 
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4;
				break;

			case  ALTERNATION_IN_ALARM:
				//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
				u32tmp = ALTERNATION_IN_ALARM; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
				sendnum ++;					
				//*(u32*)(ptmp_send+sendnum) = system_para.ALTERNATION_IN_ALARMTime  ;
				u32tmp = system_para.ALTERNATION_IN_ALARMTime  ;
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					

				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4;

			case  ALTERNATION_DEFAULT_POSITION:  
				//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
				u32tmp = ALTERNATION_DEFAULT_POSITION; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
				sendnum ++;					
				//*(u32*)(ptmp_send+sendnum) = system_para.g_usGPRSUpLoadAternationTime ;
				u32tmp = system_para.g_usGPRSUpLoadAternationTime ;
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4;
				break;
						
			case  HIGH_SPEED:
				//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
				u32tmp = HIGH_SPEED; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
				sendnum ++;					
				//*(u32*)(ptmp_send+sendnum) = system_para.g_ucMaxSpeed ;
				u32tmp = system_para.g_ucMaxSpeed ;
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//交换高低字节
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4;
				break;

			default:
				break;


		}		
		
		
	}
      head.attribute = sendnum;
		//交换高低字节
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));	  
      memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	  
	sendlen = constructor(g_ucGPRSDataBuff, sendnum+sizeof(head));

//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei

}
void cmd_INQUIRE_POSITION(void)
{
	gps_gate = 0;
	//EI();
//wei,20160823	if(!GetAvailableGPSInfoV())
//ljw,20160920 NOLocalCount>=120改两分钟为10秒    if(((!GetAvailableGPSInfoV())&&(dingweied_flag == 0))||(NOLocalCount>=120))//ljw,20160823,防死机
	
    if(dingwei_flag == 0)
    {
		//wei 20160607cmd_GENERAL_RESPONSION(message_FALSE);
	  	//wei 20160607cmd_0002();
//wei,20160822	  	uploadLAC();//wei,20160607,市场部反应点名的时候没有GPS发基站
        send_gps_gsm_flag = GSM_FLAG;//wei,20160822
//wei,20160823		gps_gate = 1;
//wei,20160823	  	return;
	}
    else send_gps_gsm_flag = GPS_FLAG;//wei,20160822
	//head.id = POSITION_REPORT;
	send_location_gprs(flowwater_form_table,INQUIRE_POSITION_RESPONSION);
	gps_gate = 1;
}

//对于MOTO定位仪来说断油断电都只有一根线，平台只能用油路的控制
void cmd_OIL_BREAK(void)
{
	static unsigned char *ptmp;

	ptmp = & g_ucGPRSCmdBuff[12];
	if((*ptmp &DOING_OIL_BREAK) == DOING_OIL_BREAK)
	{
		//set_OIL_BREAK(TRUE);
		system_para.g_break = 1;	     
	}
	else
	if((*ptmp &DOING_OIL_ON) == DOING_OIL_ON)
	{
		//set_OIL_BREAK(FALSE);
		system_para.g_break = 0;	   
	}

//wei,20160520
	if((*ptmp &AREA_ALARM) == AREA_ALARM)
	{
		//短信报警	
	   
	}
//wei,20160520

   	Store_System_Para();	
	delayms(100);
	if (system_para.g_break == 0)
	{
		set_OIL_BREAK(FALSE);
	}
	else
	{
		set_OIL_BREAK(TRUE);
	}
	
//ljw,20160906for space	while(GPRSCmdFlag == 1);
	gps_gate =	0;

    GetAvailableGPSInfoV_N(3);
    if(dingwei_flag != 0x5a)
//ljw20160910换掉，一次获取的偶然性较大		if(!GetAvailableGPSInfoV())
	{
        send_gps_gsm_flag = GSM_FLAG;
//ljw	  	cmd_0002();
		gps_gate = 1;
//ljw	  	return;
	}
    else
    {
        send_gps_gsm_flag = GPS_FLAG;
        ACC_GSM_flag = 0x00;//wei,20160825,把GSM发送标志清零
    }
	send_location_gprs(flowwater_form_table,OIL_BREAK_RESPONSION);
	gps_gate = 1;
}
// 7E 82 02 00 06 01 85 51 09 00 21 02 8E 00 0A 00 00 07 08 F2 7E 
//7E 82 02 00 06 01 85 51 09 00 21 02 89 00 0A 00 00 07 08 F5 7E 
//车辆跟踪功能：　有指定上传时间间隔，和指定时间长度，过了这段时间就会恢复到之前的时间间隔
void cmd_TEMPORARY_POSTION(void)
{

	unsigned char *ptmp;
	u32 tmp;

	ptmp = & g_ucGPRSCmdBuff[12];

    T_P_g_usGPRSUpLoadAternationTime = (*ptmp)*0x100+ (*(ptmp+1));
	
	ptmp += 2;
	if(T_P_g_usGPRSUpLoadAternationTime == 0)
	{
		T_P_time = 0;
	}
	else
	{
	    tmp  = ((*ptmp)*0x1000000);
	    ptmp ++;
		tmp += ((*ptmp)*0x10000);
		ptmp ++;
		tmp += (*ptmp)*0x100;
		ptmp ++;
		tmp += *ptmp;
		
		T_P_time = tmp;
	}
//ljw,20160906for space	while(GPRSCmdFlag == 1);
	gps_gate =	0;
	cmd_GENERAL_RESPONSION(message_SUCCESS);
	gps_gate = 1;




}



//消息ID 0x0f00 消息体 要有 类型 (1字节,1 GPS ; 2 小区)是小区的话 :后面小区项目数(1字节)小区编号,小区ID;
//                                                     如果gps,只发经度和纬度,1字节的状态字节(东经,北纬)
//

//服务器收到数据后,肯定会返回.终端可以对服务器下发的数据不做应答
//服务器回复消息ID 0x8f00 后面跟着UCS2编码的中文位置


void cmd_INQUIRE_CHINESE_POSITION(void)
{
	static unsigned char *i;
	static unsigned char *ptmp;
	unsigned char n,m;
	unsigned char tmp;
	u16 sendlen;
	position_message_struct position;
	gprs_message_head_struct head;
	
	head.id = CHINESE_POSITION_send;
	head.serial_number = flowwater++;

	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
	//head.attribute = 0;
	sendlen ++;
	sendlen --;
	gps_gate = 0;
    
    GetAvailableGPSInfoV_N(3);
    if(dingwei_flag != 0x5a)
//ljw20160910换掉，一次获取的偶然性较大		if(!GetAvailableGPSInfoV())
	{
//ljw,20160918	  	cmd_0002();
	  	//基站定位先预留折
 		gps_gate = 1;
        //wei,20160723基站获取中文位置消息
    uploadLAC_InquireChinese();//wei,20160723,使用基站
//wei,20160723基站获取中文位置消息
		return;
	}
	head.attribute = 10; //
	//交换高低字节
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));
	
	//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));

	position.status = 0;

	// 类型 GPS定位
	//tmp = 1;
	g_ucGPRSDataBuff[sizeof(head)] = 1;
	
	//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
	// 拷贝时间
	i = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
//ljw,20160902修复以前的错误的数据的解析	i++;
	//memcpy((char *)(tmp_time+6),(char *)(i+1),6);	// 拷贝时间
	i +=6;
	i = (unsigned char *)strstr((char *)i,",");

	//if('A' ==*(i+1) )
	{
	//	position.status |= EXCURSION_POSITION_STATUS_ORIENTATION; 
	}
	// 拷贝GPS信息可用标志
	i = (unsigned char *)strstr((char *)(i+1),",");
	// 拷贝 纬度，
	position.latitude = asciitolatitude(i+1);
	swapHLchar((u8*)&(position.latitude),sizeof (position.latitude));
	memcpy((u8*)&g_ucGPRSDataBuff[sizeof(head)+1],  (u8*)&(position.latitude),4 );
	i = (unsigned char *)strstr((char *)(i+1),",");
	if ('S' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_SOURTH; 
	}
	i = (unsigned char *)strstr((char *)(i+1),",");
	// 拷贝 经度，
	position.longitude = asciitolatitude(i+1); //asciitolongitude(i+1);
	swapHLchar((u8*)&(position.longitude),sizeof (position.longitude));
	memcpy((u8*)&g_ucGPRSDataBuff[sizeof(head)+5],  (u8*)&(position.longitude),4 );

	i = (unsigned char *)strstr((char *)(i+1),",");

	if ('W' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_WEST; 
	}

	gps_gate = 1;
	memcpy((u8*)&g_ucGPRSDataBuff[sizeof(head)+9],  (u8*)&(position.status),1 );

	sendlen = constructor(g_ucGPRSDataBuff, 10+sizeof(head));

//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei

}


// 0X8F00    消息体：数据长度（2个字节），位置内容（N个字节，UNICODE编码）。
void cmd_CHINESE_POSITION_rece(void)
{	
	u16 chinese_position_num;
	u8* ptmp;
	u8 i,j;
	
	ptmp = &g_ucGPRSCmdBuff[12];
	chinese_position_num = (*ptmp++)*0xff; 	//参数总字节数
//ljw,20160907 测完要删    chinese_position_num = (*ptmp++)<<8; 	//参数总字节数   //ljw,20160907 测完要删
	chinese_position_num += (*ptmp++); 	//参数总字节数

      //服务器查询失败就直接退出
	if (chinese_position_num == 0)
	{
		return ;
	}

	for(i=0;i<chinese_position_num;)
	{
		swapHLchar(ptmp,2);
		ptmp += 2;
		i += 2;
	}

	memset(UART1_tbuf,0,sizeof(UART1_tbuf));
	//strncpy((char *)temp+9,(char *)num,11);
	hextoascii(g_ucGPRSCmdBuff+14,UART1_tbuf,chinese_position_num);
	memcpy(g_ucGPRSCmdBuff,UART1_tbuf,chinese_position_num*2);
	
	send_chinese_sms(g_ucCurrentNum,g_ucGPRSCmdBuff,chinese_position_num*2);
	//SendMsg_UCS2(g_ucCurrentNum,"60A8597D",strlen("60A8597D"));

	chinesesmsflag = 0;  // 中文短信处理标志
	startchinesesms = 0;
}

void SetGPRS(void)
{
	u16 message_id; 
	u8 *ptmp;
	//u8 status=message_SUCCESS; 
	gprs_message_head_struct   tmp_head;

	if(analysis(g_ucGPRSCmdBuff, GPRSCmdCount) == 0)    //检验不正确，校验的过程已经把数据转移正确了
	{
		g_ucGPRSCmdMissionFlag = 0;
		//ClrGPRSCmdBuff();
		memset(g_ucGPRSCmdBuff,0,sizeof(g_ucGPRSCmdBuff));
		cmd_GENERAL_RESPONSION(message_ERROR);
		return;
	}
	#if 0
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
	#endif
	//tmp_head = (gprs_message_head_struct *)(&bigbuf.big.g_ucGPRSCmdBuff[1]);
	memcpy((u8*)&tmp_head, (u8 *)(&g_ucGPRSCmdBuff[0]),sizeof(tmp_head));//把消息头提取出来
		//交换高低字节
	swapHLchar((u8*)&(tmp_head.attribute),sizeof (tmp_head.attribute));
	swapHLchar((u8*)&(tmp_head.id),sizeof (tmp_head.id));
	//swapHLchar((u8*)&(tmp_head.serial_number),sizeof (tmp_head.serial_number)); //反正还要发回去，就不转了
	if(tmp_head.serial_number == flowwater_form_table) //流水号不对,  这个是判断tmp_head.serial_number里的流水号是上一条的，不是这一条的澹垮??
	{
		cmd_GENERAL_RESPONSION(message_ERROR);
		return;
	}
	flowwater_form_table = tmp_head.serial_number;//流水号只有这一处有赋值
	message_id_from_table = tmp_head.id;
   //bigbuf.big.g_ucGPRSCmdBuff[1+sizeof(gprs_message_head)]*0x100+bigbuf.big.g_ucGPRSCmdBuff[2++sizeof(gprs_message_head)];
	switch(message_id_from_table)
	{
		case  SET_TERMINAL_PARAMETER:
			cmd_SET_TERMINAL_PARAMETER();
			break;
		case  INQUIRE_TERMINAL_PARAMETER:
			cmd_INQUIRE_TERMINAL_PARAMETER();
			break;			
		case  SET_GPRS_SERVICER:
			cmd_SET_GPRS_SERVICER(); //感觉服务器的设置已经有对应的指令了，这个就做成空空函数
			break;
		case  INQUIRE_ONE_TERMINAL_PARAMETER:
			cmd_INQUIRE_ONE_TERMINAL_PARAMETER();
			break;
		case INQUIRE_POSITION:
			cmd_INQUIRE_POSITION();
			break;
		case  CHINESE_POSITION_rece:
			cmd_CHINESE_POSITION_rece();
			break;	
		case  OIL_BREAK:
			cmd_OIL_BREAK();
			break;
		case  TEMPORARY_POSTION:
			cmd_TEMPORARY_POSTION();
			break;	
		default:
			break;
	}
// 根据消息ID　区分具体任务
	
	g_ucGPRSCmdMissionFlag = 0;
}
//搜索B562 ,能调过０数据
u8* strstrb562(u8*pointer,u16 len)
{
    u16 i;
    u8* ptmp = pointer;
    for(i=0; i<len; i++)
    {
	 if((*ptmp == 0xBA) &&(*(ptmp+1) == 0xCE))
	 return ptmp;
	 ptmp ++;
    }
    return NULL;
}

u8 in_agps= 0;


//wei,20160827 查找0D 0A子函数
u8* intstr(u8 *pointer,u16 len,u8 first,u8 second)
{
    u16 i;
    u8* ptmp = pointer;
    for(i=0; i<len; i++)
    {
	 if((*ptmp == first) &&(*(ptmp+1) == second))
        return ptmp;
	 ptmp ++;
    }
    return NULL;
}

#define get_GC65_lat_len 5
#define get_GC65_lon_len 6
//u8 get_GC65_lat[get_GC65_lat_len+1] = {0};
//u8 get_GC65_lon[get_GC65_lon_len+1] = {0};
u8 get_GC65_LL_Flag = 0;
u8 get_lac_gps(u8 *lat,u8 *lon)
{
    u8 *p_temp = NULL;
//    u8 get_GC65_lat[get_GC65_lat_len+1] = {0};
//    u8 get_GC65_lon[get_GC65_lon_len+1] = {0};

    IWDG_ReloadCounter();//ljw,20160906防止在收7E数据
    while(proGPRSCmdFlag==1);//ljw,20160906防止在收7E数据
    g_ucSim900Status = BUSY;
    ddi_uart_Write(&UART1_ctl_t,"AT+QCELLLOC=1\r\n",strlen("AT+QCELLLOC=1\r\n"));
	WaitGSMOK2(500);//ljw20160903for space   WaitGSMOK();    
//    delayms(1500);//wei,20160621,for check GPRS status at condition AT+QIMUX=1
    p_temp = strstr((char *)g_ucSim900Buff,"+QCELLLOC:");
	if(p_temp != NULL)
	{
//      memcpy(get_GC65_lon,p_temp+11,6);
      memcpy(lon,p_temp+11,6); 
      p_temp = strstr((char *)g_ucSim900Buff,",");
      memcpy(lat,p_temp+1,5);
      g_ucSim900Status = IDLE;
      get_GC65_LL_Flag = 0x5a;
	  return 1;
	}
    get_GC65_LL_Flag = 0x00;
    g_ucSim900Status = IDLE;
    return 0;
}
//A-GPS 发送缓冲区的数据长度
//wei,zhu 返回:获取AGPS数据成功，返回1，否则返回0
u16  AGPS_rec_len = 0;//ljw,20161125,串口中收到的APGS辅助数据的总长度
u8 test_a_gps(void)
{
    u32 i;
//    u8 *p1 = NULL;//ljw,20160827,APGS数据优化
//    u8 *p2 = NULL;//ljw,20160827,APGS数据优化
    u16 temp_len_end= 0;//ljw,20160827,APGS数据优化
//    u16 temp_len_0d = 0;//ljw,20160827,APGS数据优化
    u16 j = 0;//ljw,20160827,APGS数据优化
    u16 offset = 0;//ljw,20161202,环形指针发送接收到的AGPS数据的便宜地址
    u8 get_GC65_lat[get_GC65_lat_len+1] = {0};
    u8 get_GC65_lon[get_GC65_lon_len+1] = {0};

#ifdef ZHONGKEWEI_GPS //ljw,20161125,中科微的GPS模块
    
//    u8 temp_send_AGPS[100] = "\"cmd=full;user=miccozhu@163.com;pwd=Fht1230;";
//    srtcat(temp_send_AGPS);
	delayms(2000);
	//g_ucGPRSDataBuff
	//memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
    //wei,20160827   cmd=full;cmd=eph;

    if(!get_lac_gps(get_GC65_lat,get_GC65_lon))//ljw,20160903测试GC65是否能获得经纬度
        get_lac_gps(get_GC65_lat,get_GC65_lon);
    //delayms(2000);//ljw,20160907 测试GC65是否能获得经纬度
//    srtcat(temp_send_AGPS,);
//wei,	memcpy(g_ucGPRSDataBuff,"cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n",
//    memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;\n",	
//    strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;\n"));
    memset(g_ucGPRSDataBuff,0x00,sizeof(g_ucGPRSDataBuff));//ljw,2060907
    
//ljw,20160920 eph-full    memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;",	
//ljw,20160920 eph-full    strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;"));
    if(get_GC65_LL_Flag == 0x00)
    {
        memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;",
        strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;"));
        strcat(g_ucGPRSDataBuff,"\n");
    }
    else
    {
        memcpy(g_ucGPRSDataBuff,"cmd=full;user=miccozhu@163.com;pwd=Fht1230;",
        strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;"));
        strcat(g_ucGPRSDataBuff,"lat=");
        strcat(g_ucGPRSDataBuff,get_GC65_lat);
        strcat(g_ucGPRSDataBuff,";");
        strcat(g_ucGPRSDataBuff,"lon=");
        strcat(g_ucGPRSDataBuff,get_GC65_lon);
        strcat(g_ucGPRSDataBuff,";");
        strcat(g_ucGPRSDataBuff,"alt=100;\n");
    }
#endif  //ljw,20161125,中科微的GPS模块
    
#ifdef UM220_GPS  //ljw,20161125,UM220
#if 0 //ljw,20161202,这个原来是使用不主动上报数据的
    in_agps = 0x5a;//ljw,20161128,请求APGS之前清空GSM缓存
    for(j=0;j<10;j++)//ljw,20161128,请求APGS之前清空GSM缓存
    {
        if(!GC65_QIRD())
        {
            break;
        }
        else
        if(GPRSCmdCount<1500)
            break;
    }
    in_agps = 0;
#endif    
    if(GPS_module == UM220)
    {
        memset(g_ucGPRSDataBuff,0x00,sizeof(g_ucGPRSDataBuff));//ljw,2060907
        memcpy(g_ucGPRSDataBuff,"x=874675487@qq.com:qfsGBL",strlen("x=874675487@qq.com:qfsGBL"));
    }
    else
    {
        if(!get_lac_gps(get_GC65_lat,get_GC65_lon))//ljw,20160903测试GC65是否能获得经纬度
            get_lac_gps(get_GC65_lat,get_GC65_lon);
        //delayms(2000);//ljw,20160907 测试GC65是否能获得经纬度
        //    srtcat(temp_send_AGPS,);
        //wei,	memcpy(g_ucGPRSDataBuff,"cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n",
        //    memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;\n",	
        //    strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;\n"));
        memset(g_ucGPRSDataBuff,0x00,sizeof(g_ucGPRSDataBuff));//ljw,2060907
    
        //ljw,20160920 eph-full    memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;",	
        //ljw,20160920 eph-full    strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;"));
        if(get_GC65_LL_Flag == 0x00)
        {
            memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;",
            strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;"));
            strcat(g_ucGPRSDataBuff,"\n");
        }
        else
        {
            memcpy(g_ucGPRSDataBuff,"cmd=full;user=miccozhu@163.com;pwd=Fht1230;",
            strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;"));
            strcat(g_ucGPRSDataBuff,"lat=");
            strcat(g_ucGPRSDataBuff,get_GC65_lat);
//            strcat(g_ucGPRSDataBuff,";");
            strcat(g_ucGPRSDataBuff,";lon=");
            strcat(g_ucGPRSDataBuff,get_GC65_lon);
//            strcat(g_ucGPRSDataBuff,";");
            strcat(g_ucGPRSDataBuff,";alt=100;\n");
        }
    }
    
//    strcat(g_ucGPRSDataBuff,"\n");

#endif //ljw,20161125,UM220
    GetAvailableGPSInfoV();
    if(dingweied_flag)
        return 1;
//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n"));
	bit_flag |=(0x03<<0);//(0b00000011<<0);
//wei,20160907 测完要删	SendGprs_Index(strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n"),(u8)AGPS_SERVERS);//wei
	if(strlen(g_ucGPRSDataBuff) <150)//ljw,20160907加长度判断
        SendGprs_Index(strlen(g_ucGPRSDataBuff),(u8)AGPS_SERVERS);//wei
    else //ljw,20160907加长度判断
    {
        memset(g_ucGPRSDataBuff,0x00,sizeof(g_ucGPRSDataBuff));//ljw,2060907
        bit_flag &=(~(0x03<<0));//(~(0b00000011<<0));
        return 0;
    }
    bit_flag &=(~(0x03<<0));//(~(0b00000011<<0));
#if 0

//接收到的数据格式如下
//+QIRDI: 0,1,0  //提示收到了GPRS数据

//+QIRD: 195.34.89.144:46434,UDP,200

//u-blox a-gps server (c) 1997-2009 u-blox AG
//Content-Length: 1144

//Content-Type: application/ubx　　　　//144个左右

//礲0駢l?&爢媓b1h?訧9?
//+QIRD: 195.34.89.144:46434,UDP,200
//	9}訧G荝?�@9G0礲1h?覫?(堑b1h

//+QIRD: 195.34.89.144:46434,UDP,200
//○
//鷪
//~訧??I?鯅??.???+QIRD: 195.34.89.144:46434,UDP,200
//礲1h?訧9?\x�?

//+QIRD: 195.34.89.144:46434,UDP,200
//'?礲1h?T礲1h
//+QIRD: 195.34.89.144:46434,UDP,200
//瓻々?訧
//j礲H
//+QIRD: 195.34.89.144:46434,UDP,43

//OK
//OK

#endif

     in_agps = 0x5a;
     fB5 = 0;//wei.20160601
     AGPS_ov = 0;//wei.20160601
     GPRSCmdCount = 0;
     memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
     delayms(1);

#ifdef ZHONGKEWEI_GPS //ljw,20161125,中科微的GPS模块    
	for(i=15000; i>0; i-- )
	{
		//if ((fB5&(1<<1)) == (1<<1))
		if((fB5 >= 2))
		{
            if((g_ucGPRSCmdBuffback[2]+10) < agps_len_to_send)
            {
                p1 = intstr(g_ucGPRSCmdBuffback,agps_len_to_send,0x0D,0x0a);
                if(p1 != NULL)
                {
                    if((p1-g_ucGPRSCmdBuffback) >= (g_ucGPRSCmdBuffback[2]+10))//ljw,20160827
                        agps_len_to_send = (g_ucGPRSCmdBuffback[2]+10);
                    p2 = intstr((p1+1),(agps_len_to_send-(p1-g_ucGPRSCmdBuffback)),0x0D,0x0a);
                    if(p2 !=NULL)
                    {
                        temp_len_end = (u8)(agps_len_to_send-(p1-g_ucGPRSCmdBuffback));//wei,20160827计算搬移长度
                        temp_len_0d =(u8)(p2-p1);
                        for(j=0;j<(agps_len_to_send-(p2-g_ucGPRSCmdBuffback));j++)
                        {
                            p1[j+2] =p1[temp_len_0d+2];
                        }
                        agps_len_to_send = (g_ucGPRSCmdBuffback[2]+10);
                    }
                }
            }
			ddi_uart_Write(&UART2_ctl_t,(const u8*)g_ucGPRSCmdBuffback,agps_len_to_send);
			agps_len_to_send = 0;
			fB5 --;
		}
		if ((1000< AGPS_ov))
		{
			ddi_uart_Write(&UART2_ctl_t,(const u8*)g_ucGPRSCmdBuff,GPRSCmdCount);	
			fB5 = 0;
			break;
		}
		delayms(1);
	}
    in_agps = 0;
	if (i<10)
	 return 0;
	else
	 return 1;
#endif //ljw,20161125,中科微的GPS模块 
    
#ifdef UM220_GPS  //ljw,20161125,UM220
    for(i=15000;i>0;i--)
    {
#if 0 //2800
        if(fB5 ==AGPS_RE_END)
        {
            temp_len_0d = GPRSCmdCount;
            if(temp_len_0d < 500) //接收到的数据太短，认为有错误
            {
                in_agps = 0;
                return 0;
            }
            ddi_uart_Write(&UART2_ctl_t,(const u8*)g_ucGPRSCmdBuff,temp_len_0d);        
            fB5 = 0;
        }
#endif       
#if 0 //ljw,20161202,这个原来是使用不主动上报数据的
        if(strstr(g_ucGPRSCmdBuff,"+QIRDI:"))
        {
            delayms(10);
            for(j=0;j<3;j++)
            {
                if(GC65_QIRD())
                {
                    temp_len_0d = GPRSCmdCount;
                    ddi_uart_Write(&UART2_ctl_t,(const u8*)g_ucGPRSCmdBuff,temp_len_0d);
                    //delayms(3000);
                    fB5 = 0;
                    if(temp_len_0d < 1300)
                        break;
                }
                //delayms(1000);
            }
            in_agps = 0;
            return 1;
        }
#endif
#if 1//ljw,20161126方案一
        if(fB5 == GC65_RECE)
        {
//            delayms(1);
            offset = 0;
            for(j =0;j<= AGPS_rec_len;j++)
            {
                while(j >= AGPS_rec_len)//(temp_len_end >= AGPS_rec_len)&&
                {
//                    if(temp_len_end < AGPS_rec_len)
//                        break;
                    if(fB5 == AGPS_RE_END)
                    {
                        in_agps = 0;
                        if(AGPS_rec_len<500)
                            return 0;
                        else
                            return 1;
                    }
                }
               // temp_len_end = AGPS_rec_len;
                ddi_uart_Write(&UART2_ctl_t,((const u8*)(g_ucGPRSCmdBuff+offset)),1);
                offset++;
                if(offset> 380)
                    offset = 0;
            }
        }
#endif //ljw,20161126 方案一        
        delayms(1);
    }
    in_agps = 0;
	if (i<10)
	 return 0;
	else
	 return 1;
#endif //ljw,20161125,UM220
}
#if 0 //ljw,20161125 for um220
u8 test_a_gps(void)
{
    u32 i;
    u8 *p1 = NULL;//ljw,20160827,APGS数据优化
    u8 *p2 = NULL;//ljw,20160827,APGS数据优化
    u16 temp_len_end= 0;//ljw,20160827,APGS数据优化
    u16 temp_len_0d = 0;//ljw,20160827,APGS数据优化
    u16 j = 0;//ljw,20160827,APGS数据优化
    
    u8 get_GC65_lat[get_GC65_lat_len+1] = {0};
    u8 get_GC65_lon[get_GC65_lon_len+1] = {0};
//    u8 temp_send_AGPS[100] = "\"cmd=full;user=miccozhu@163.com;pwd=Fht1230;";
//    srtcat(temp_send_AGPS);
	delayms(2000);
	//g_ucGPRSDataBuff
	//memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
    //wei,20160827   cmd=full;cmd=eph;
    
    if(!get_lac_gps(get_GC65_lat,get_GC65_lon))//ljw,20160903测试GC65是否能获得经纬度
        get_lac_gps(get_GC65_lat,get_GC65_lon);
    //delayms(2000);//ljw,20160907 测试GC65是否能获得经纬度
//    srtcat(temp_send_AGPS,);
//wei,	memcpy(g_ucGPRSDataBuff,"cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n",
//    memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;\n",	
//    strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;\n"));
    memset(g_ucGPRSDataBuff,0x00,sizeof(g_ucGPRSDataBuff));//ljw,2060907
    
//ljw,20160920 eph-full    memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;",	
//ljw,20160920 eph-full    strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;"));
    if(get_GC65_LL_Flag == 0x00)
    {
        memcpy(g_ucGPRSDataBuff,"cmd=eph;user=miccozhu@163.com;pwd=Fht1230;",
        strlen("cmd=eph;user=miccozhu@163.com;pwd=Fht1230;"));
        strcat(g_ucGPRSDataBuff,"\n");
    }
    else
    {
        memcpy(g_ucGPRSDataBuff,"cmd=full;user=miccozhu@163.com;pwd=Fht1230;",
        strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;"));
        strcat(g_ucGPRSDataBuff,"lat=");
        strcat(g_ucGPRSDataBuff,get_GC65_lat);
        strcat(g_ucGPRSDataBuff,";");
        strcat(g_ucGPRSDataBuff,"lon=");
        strcat(g_ucGPRSDataBuff,get_GC65_lon);
        strcat(g_ucGPRSDataBuff,";");
        strcat(g_ucGPRSDataBuff,"alt=100;\n");
    }
    GetAvailableGPSInfoV();
    if(dingweied_flag)
        return 1;
//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n"));
	bit_flag |=(0x03<<0);//(0b00000011<<0);
//wei,20160907 测完要删	SendGprs_Index(strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n"),(u8)AGPS_SERVERS);//wei
	if(strlen(g_ucGPRSDataBuff) <150)//ljw,20160907加长度判断
        SendGprs_Index(strlen(g_ucGPRSDataBuff),(u8)AGPS_SERVERS);//wei
    else //ljw,20160907加长度判断
    {
        memset(g_ucGPRSDataBuff,0x00,sizeof(g_ucGPRSDataBuff));//ljw,2060907
        bit_flag &=(~(0x03<<0));//(~(0b00000011<<0));
        return 0;
    }
    bit_flag &=(~(0x03<<0));//(~(0b00000011<<0));
#if 0

//接收到的数据格式如下
//+QIRDI: 0,1,0  //提示收到了GPRS数据

//+QIRD: 195.34.89.144:46434,UDP,200

//u-blox a-gps server (c) 1997-2009 u-blox AG
//Content-Length: 1144

//Content-Type: application/ubx　　　　//144个左右

//礲0駢l?&爢媓b1h?訧9?
//+QIRD: 195.34.89.144:46434,UDP,200
//	9}訧G荝?�@9G0礲1h?覫?(堑b1h

//+QIRD: 195.34.89.144:46434,UDP,200
//○
//鷪
//~訧??I?鯅??.???+QIRD: 195.34.89.144:46434,UDP,200
//礲1h?訧9?\x�?

//+QIRD: 195.34.89.144:46434,UDP,200
//'?礲1h?T礲1h
//+QIRD: 195.34.89.144:46434,UDP,200
//瓻々?訧
//j礲H
//+QIRD: 195.34.89.144:46434,UDP,43

//OK
//OK

#endif

     in_agps = 0x5a;
     fB5 = 0;//wei.20160601
     AGPS_ov = 0;//wei.20160601
     GPRSCmdCount = 0;
     memset(g_ucGPRSCmdBuff,0,GPRS_BUFF_SIZE);
     delayms(1);

	for(i=15000; i>0; i-- )
	{	
		//if ((fB5&(1<<1)) == (1<<1))
		if((fB5 >= 2))
		{
            if((g_ucGPRSCmdBuffback[2]+10) < agps_len_to_send)
            {
                p1 = intstr(g_ucGPRSCmdBuffback,agps_len_to_send,0x0D,0x0a);
                if(p1 != NULL)
                {
                    if((p1-g_ucGPRSCmdBuffback) >= (g_ucGPRSCmdBuffback[2]+10))//ljw,20160827
                        agps_len_to_send = (g_ucGPRSCmdBuffback[2]+10);
                    p2 = intstr((p1+1),(agps_len_to_send-(p1-g_ucGPRSCmdBuffback)),0x0D,0x0a);
                    if(p2 !=NULL)
                    {
                        temp_len_end = (u8)(agps_len_to_send-(p1-g_ucGPRSCmdBuffback));//wei,20160827计算搬移长度
                        temp_len_0d =(u8)(p2-p1);
                        for(j=0;j<(agps_len_to_send-(p2-g_ucGPRSCmdBuffback));j++)
                        {
                            p1[j+2] =p1[temp_len_0d+2];
                        }
                        agps_len_to_send = (g_ucGPRSCmdBuffback[2]+10);
                    }
                }
            }
			ddi_uart_Write(&UART2_ctl_t,(const u8*)g_ucGPRSCmdBuffback,agps_len_to_send);
			agps_len_to_send = 0;
			fB5 --;
		}
		if ((1000< AGPS_ov)  )
		{
			ddi_uart_Write(&UART2_ctl_t,(const u8*)g_ucGPRSCmdBuff,GPRSCmdCount);	
			fB5 = 0;
			break;
		}
		delayms(1);
	}

       in_agps = 0;
	if (i<10)
	 return 0;
	else
	 return 1;
}
#endif  //ljw,20161125 for um220


#if 0
void uploadLAC(void)
{
	gprs_message_head_struct head;
	u8* ptmp = NULL;
	u8* p1 = NULL;
	u16 sendlen;
	u8 tmpbuf[8];
	u8 len;
	u8 tmp;
	u8 i;
	u32 Lac=0;
	u32 LacId=0;
	u32 tmpMobileCompany;


	ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r",9);
	WaitGSMOK();
	IWDG_ReloadCounter();
	ptmp = (unsigned char *)strstr((char *)g_ucSim900Buff,"\"");
	ptmp += 1;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
	memcpy(tmpbuf,ptmp,len);
	for(i=0; i<len; i++)
	{
		if ((tmpbuf[i] >= '0') && (tmpbuf[i] <='9'))
		{			
			Lac = Lac*0x10 + tmpbuf[i]-'0';			
		}
		else
		if ((tmpbuf[i] >= 'A') && (tmpbuf[i] <='F'))
		{			
			Lac = Lac*0x10 + tmpbuf[i]-'A'+10;
		}
	}	

	ptmp = (unsigned char *)strstr((char *)p1,",\"");
	ptmp += 2;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
	
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
	memcpy(tmpbuf,ptmp,len);
	for(i=0; i<len; i++)
	{
		if ((tmpbuf[i] >= '0') && (tmpbuf[i] <='9'))
		{
			LacId = LacId*0x10 + tmpbuf[i]-'0';
		}
		else
		if ((tmpbuf[i] >= 'A') && (tmpbuf[i] <='F'))
		{
			LacId = LacId*0x10 + tmpbuf[i]-'A'+10;
		}
	}

	head.id = MOBILE_LOCATION_SEND;
	head.serial_number = flowwater++;
	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
	head.attribute = 12;

	tmpMobileCompany = MobileCompany;
		//??????μí×??ú
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));
	swapHLchar((u8*)&(Lac),sizeof (Lac));
	swapHLchar((u8*)&(LacId),sizeof (LacId));
	swapHLchar((u8*)&(tmpMobileCompany),sizeof (tmpMobileCompany));

		//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	ptmp = &(g_ucGPRSDataBuff[sizeof(head)]);
	
	memcpy(ptmp,(u8*)&tmpMobileCompany,sizeof(tmpMobileCompany));
	ptmp += 4;
	memcpy(ptmp,&Lac,4);
	ptmp += 4;
	memcpy(ptmp,&LacId,4);
		
	sendlen = constructor(g_ucGPRSDataBuff, 12+sizeof(head));

	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei
}
#endif

//ljw,20160906字符串查找函数，加长度判断
//char *strstr_len(u8 *s1,u8 *s2,u16 len)
//{
//    int len2;
//    if(!(len2=strlen(s2)))//?????s2?????,??strlen??????,??????
//        return(char*)s1;
//    if(len < strlen(s1))
//        return NULL;
//    for(;*s1;++s1)
//    {
//        if(*s1==*s2 && strncmp(s1,s2,len2)==0)
//        return(char*)s1;
//    }
//    return NULL;
//}
//wei,20160822,for gsm id
#if 1
//ljw,20160908 test reset flag u8 erro_flag = 0;//ljw,20160906,测试复位错误标志量   测完要删
position_message_struct Deal_GSM_LAC(position_message_struct position_in)
//ljw,20160906测完要删u8 Deal_GSM_LAC(void)//ljw,20160906测完要删 
{
//wei,20160822	gprs_message_head_struct head;
	u8* ptmp = NULL;
	u8* p1 = NULL;
//wei,20160822	u16 sendlen;
	u8 tmpbuf[8];
	u8 len;
	u8 tmp;
	u8 i;
	u32 Lac=0;
	u32 LacId=0;
	u32 tmpMobileCompany;
    
    position_message_struct position = position_in;//wei
    
    get_ID();//ljw,20160918,使用子函数获取小区ID
    position.latitude = Lac_g;
    position.longitude = LacId_g;
//ljw,20160908 test reset flag    erro_flag =0;//ljw,20160906,测试复位错误标志量   测完要删
#ifdef GSM_ID_OLD //ljw,20160918,使用子函数获取小区ID    
	ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r",9);
	WaitGSMOK1();      //ljw20160903for space   WaitGSMOK();
	IWDG_ReloadCounter();
	ptmp = (unsigned char *)strstr((char *)g_ucSim900Buff,"\"");
//ljw,20160908 test reset flag    erro_flag =1;//ljw,20160906,测试复位错误标志量   测完要删
	ptmp += 1;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
//ljw,20160908 test reset flag    erro_flag =2;//ljw,20160906,测试复位错误标志量   测完要删
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len<=8) //ljw,20160906加长度判断
    {
        memcpy(tmpbuf,ptmp,len);
        for(i=0; i<len; i++)
        {
            if ((tmpbuf[i] >= '0') && (tmpbuf[i] <='9'))
            {
                Lac = Lac*0x10 + tmpbuf[i]-'0';			
            }
            else
            if ((tmpbuf[i] >= 'A') && (tmpbuf[i] <='F'))
            {
                Lac = Lac*0x10 + tmpbuf[i]-'A'+10;
            }
        }
        position.latitude = Lac;//wei,20160822
    }
    else position.latitude = Lac_b;
	
    
	ptmp = (unsigned char *)strstr((char *)p1,",\"");
//ljw,20160908 test reset flag    erro_flag =3;//ljw,20160906,测试复位错误标志量   测完要删
	ptmp += 2;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
//ljw,20160908 test reset flag	erro_flag =4;//ljw,20160906,测试复位错误标志量   测完要删
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len<=8)
    {
        memcpy(tmpbuf,ptmp,len);
//ljw,20160908 test reset flag        erro_flag =5;//ljw,20160906,测试复位错误标志量   测完要删
        for(i=0; i<len; i++)
        {
            if ((tmpbuf[i] >= '0') && (tmpbuf[i] <='9'))
            {
                LacId = LacId*0x10 + tmpbuf[i]-'0';
            }
            else
            if ((tmpbuf[i] >= 'A') && (tmpbuf[i] <='F'))
            {
                LacId = LacId*0x10 + tmpbuf[i]-'A'+10;
            }
        }
    position.longitude = LacId;//wei,20160822
    }
    else  position.longitude = LacID_b;
#endif	//ljw,20160918,使用子函数获取小区ID
    
//wei,20160822	head.id = MOBILE_LOCATION_SEND;
//wei,20160822	head.serial_number = flowwater++;
//wei,20160822	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
//wei,20160822	head.attribute = 12;

//	tmpMobileCompany = MobileCompany;
    position.speed = (u16)MobileCompany;//wei,20160822
    position.altitude = 0x0000;
    position.direction = 0x0000;
    RTC2Position_time(&position.time[0]);//wei,20160825,取系统的RTC时间
//ljw,20160908 test reset flag    erro_flag = 6;//ljw,20160906,测试复位错误标志量   测完要删
//    position.time[0] = RTC_ByteToBcd2((u8)(timer.w_year-2000));
//    position.time[1] = RTC_ByteToBcd2(timer.w_month);
//    position.time[2] = RTC_ByteToBcd2(timer.w_date);
//    position.time[3] = RTC_ByteToBcd2(timer.hour);
//    position.time[4] = RTC_ByteToBcd2(timer.min);
//    position.time[5] = RTC_ByteToBcd2(timer.sec);
//ljw,20160906测完要删    return position;
    return position;

#if 0  
		//??????μí×??ú
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));
	swapHLchar((u8*)&(Lac),sizeof (Lac));
	swapHLchar((u8*)&(LacId),sizeof (LacId));
	swapHLchar((u8*)&(tmpMobileCompany),sizeof (tmpMobileCompany));

		//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	ptmp = &(g_ucGPRSDataBuff[sizeof(head)]);

	memcpy(ptmp,(u8*)&tmpMobileCompany,sizeof(tmpMobileCompany));
	ptmp += 4;
	memcpy(ptmp,&Lac,4);
	ptmp += 4;
	memcpy(ptmp,&LacId,4);

	sendlen = constructor(g_ucGPRSDataBuff, 12+sizeof(head));

	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei
#endif
}
#endif


//wei,20160918,for 上传小区号获取中文位置
void uploadLAC_InquireChinese(void)
{
	gprs_message_head_struct head;
	u8* ptmp = NULL;
	u8* p1 = NULL;
	u16 sendlen;
	u8 tmpbuf[8];
	u8 len;
//wei for warning	u8 tmp;
	u8 i;
	u32 Lac=0;
	u32 LacId=0;
	u32 tmpMobileCompany;

    get_ID();//ljw,20160918,使用子函数获取小区ID
    Lac = Lac_g;
    LacId = LacId_g;
#ifdef GSM_ID_OLD //ljw,20160918,使用子函数获取小区ID
	ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r",9);
    WaitGSMOK1();
//ljw,20160918	WaitGSMOK();
	IWDG_ReloadCounter();
	ptmp = (unsigned char *)strstr((char *)g_ucSim900Buff,"\"");
	ptmp += 1;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len>8)//ljw,20160918测完要删
        return ;
	memcpy(tmpbuf,ptmp,len);
	for(i=0; i<len; i++)
	{
		if ((tmpbuf[i] >= '0') && (tmpbuf[i] <='9'))
		{			
			Lac = Lac*0x10 + tmpbuf[i]-'0';			
		}
		else
			if ((tmpbuf[i] >= 'A') && (tmpbuf[i] <='F'))
		{			
			Lac = Lac*0x10 + tmpbuf[i]-'A'+10;
		}
	}

	ptmp = (unsigned char *)strstr((char *)p1,",\"");
	ptmp += 2;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
	
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len>8)//ljw,20160918测完要删
        return ;
	memcpy(tmpbuf,ptmp,len);
	for(i=0; i<len; i++)
	{
		if ((tmpbuf[i] >= '0') && (tmpbuf[i] <='9'))
		{			
			LacId = LacId*0x10 + tmpbuf[i]-'0';			
		}
		else
			if ((tmpbuf[i] >= 'A') && (tmpbuf[i] <='F'))
		{			
			LacId = LacId*0x10 + tmpbuf[i]-'A'+10;
		}
	}
#endif	//ljw,20160918,使用子函数获取小区ID
	head.id = CHINESE_POSITION_send;//wei,20160723,  0x0f88
	head.serial_number = flowwater++;
	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
	head.attribute = 13;//wei,20160723,消息体属性 长度1+4+4+4

	tmpMobileCompany = MobileCompany;
		//??????μí×??ú
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));
	swapHLchar((u8*)&(Lac),sizeof (Lac));
	swapHLchar((u8*)&(LacId),sizeof (LacId));
	swapHLchar((u8*)&(tmpMobileCompany),sizeof (tmpMobileCompany));

		//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	ptmp = &(g_ucGPRSDataBuff[sizeof(head)]);
	
    *ptmp = 0x02; //wei,20160723, 1-GPS，2-小区位置
    ptmp++;//wei,20160723,
    
	memcpy(ptmp,(u8*)&tmpMobileCompany,sizeof(tmpMobileCompany));
	ptmp += 4;
	memcpy(ptmp,&Lac,4);
	ptmp += 4;
	memcpy(ptmp,&LacId,4);
		
//wei	sendlen = constructor(g_ucGPRSDataBuff, 12+sizeof(head));
    sendlen = constructor(g_ucGPRSDataBuff, 13+sizeof(head));
    
//	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei	
}


//ljw,20160918,获取GSM小区ID
volatile u32 Lac_g =0x00;
volatile u32 LacId_g = 0x00;
void get_ID(void)
{
    u8* ptmp = NULL;
	u8* p1 = NULL;
    u8 tmpbuf[8];
	u8 len;
    u8 i;
    u32 Lac =0x00;
    u32 LacId = 0x00;
    
    ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r",9);
	WaitGSMOK1();      //ljw20160903for space   WaitGSMOK();
	IWDG_ReloadCounter();
	ptmp = (unsigned char *)strstr((char *)g_ucSim900Buff,"\"");
//ljw,20160908 test reset flag    erro_flag =1;//ljw,20160906,测试复位错误标志量   测完要删
	ptmp += 1;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
//ljw,20160908 test reset flag    erro_flag =2;//ljw,20160906,测试复位错误标志量   测完要删
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len<=8) //ljw,20160906加长度判断
    {
        memcpy(tmpbuf,ptmp,len);
        for(i=0; i<len; i++)
        {
            if ((tmpbuf[i] >= '0') && (tmpbuf[i] <='9'))
            {
                Lac = Lac*0x10 + tmpbuf[i]-'0';			
            }
            else
            if ((tmpbuf[i] >= 'A') && (tmpbuf[i] <='F'))
            {
                Lac = Lac*0x10 + tmpbuf[i]-'A'+10;
            }
        }
        Lac_g = Lac;
    }
    else Lac_g = Lac_b;
    
	ptmp = (unsigned char *)strstr((char *)p1,",\"");
//ljw,20160908 test reset flag    erro_flag =3;//ljw,20160906,测试复位错误标志量   测完要删
	ptmp += 2;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
//ljw,20160908 test reset flag	erro_flag =4;//ljw,20160906,测试复位错误标志量   测完要删
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len<=8)
    {
        memcpy(tmpbuf,ptmp,len);
//ljw,20160908 test reset flag        erro_flag =5;//ljw,20160906,测试复位错误标志量   测完要删
        for(i=0; i<len; i++)
        {
            if ((tmpbuf[i] >= '0') && (tmpbuf[i] <='9'))
            {
                LacId = LacId*0x10 + tmpbuf[i]-'0';
            }
            else
            if ((tmpbuf[i] >= 'A') && (tmpbuf[i] <='F'))
            {
                LacId = LacId*0x10 + tmpbuf[i]-'A'+10;
            }
        }
        LacId_g = LacId;
    }
    else  LacId_g = LacID_b;
}



