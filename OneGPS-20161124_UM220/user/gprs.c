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
u16 flowwater=0; 	  //�ն���ˮ��  ���Լ�		   
u16 flowwater_form_table;   // ��ƽ̨�յ�����ˮ��
u16 message_id_from_table; // ��ƽ̨�յ�����ϢID	
//15656049332
//  u8 mbnumber[6]={0x01,0x56,0x56,0x04,0x93,0x32};
////ljw,20160902,������Ҫɾ u8 mbnumber[6]={0x01,0x98,0x00,0x00,0x20,0x12};
/***********************************************************************
 * Function prototypes of global functions imported.  
 **********************************************************************/
//�����ϴ����������ǣ���ʾ��Ҫ������
 u16 T_P_g_usGPRSUpLoadAternationTime;

//����ʱ��
 u32 T_P_time;
/***********************************************************************
 * Data definitions of global data exported.  
 **********************************************************************/
//ljw,20161126 extern unsigned char GPRSCmdCount;
//LJW,20161124,�������Ŀǰֻ���������в��������õ��ˡ� extern unsigned char g_ucGPRSCmdBuffback[GPRS_BUFF_SIZE];
extern unsigned char g_ucGPRSCmdBuffback[GPRS_BACK_BUFF_SIZE]; //LJW,20161124,�������Ŀǰֻ���������в��������õ��ˡ�
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
//�����ߵ��ֽڣ�ֻ�����ֻ�˫��
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

//Э����ô��ģʽ(big-endian)�������ֽ����������ֺ�˫�֡�
//˫��(DWORD)�Ĵ���Լ�����ȴ��ݸ�24 λ��Ȼ�󴫵ݸ�16 λ���ٴ��ݸ߰�λ,��󴫵ݵͰ�λ��
//ÿ����Ϣ�ɱ�ʶλ����Ϣͷ����Ϣ���У������ɣ���Ϣ�ṹͼ��ͼ1 ��ʾ��
//  ��ʶλ(0X7E)|   ��Ϣͷ |��Ϣ�� |������| ��ʶλ(0X7E)
//��У���롢��Ϣͷ�Լ���Ϣ���г���0x7e����Ҫ����ת�崦��ת��
//���������£�
//0x7e <��������> 0x7d �����һ��0x02��
//0x7d <��������> 0x7d �����һ��0x01��


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


//��Ϣ�����Ը�ʽ�ṹͼ��ͼ2 ��ʾ��
//15     14  |  13   |   12   11    10           |        9    8   7     6     5    4    3   2   1   0
//����  | �ְ�| ���ݼ��ܷ�ʽ |              ��Ϣ�峤��
//���ݼ��ܷ�ʽ��
//����bit10~bit12 Ϊ���ݼ��ܱ�ʶλ��
//����������λ��Ϊ0����ʾ��Ϣ�岻���ܣ�
//��������10 λΪ1����ʾ��Ϣ�徭��RSA �㷨���ܣ�
//��������������

//�ְ���
//����Ϣ�������е�13 λΪ1 ʱ��ʾ��Ϣ��Ϊ����Ϣ�����зְ����ʹ�������ְ���
//Ϣ����Ϣ����װ�����������13 λΪ0������Ϣͷ������Ϣ����װ���ֶΡ�

// ��Ϣ����װ������
// ��ʼ�ֽ�| �ֶ�              | �������� |  ������Ҫ��
// 0                |   ��Ϣ�ܰ���| WORD              |����Ϣ�ְ�����ܰ���
// 2                   | �����          |WORD               |��1 ��ʼ


//У����
//У����ָ����Ϣͷ��ʼ��ͬ��һ�ֽ����ֱ��У����ǰһ���ֽڣ�ռ��һ���ֽڡ�

//���ղ���Э��ת�壬����0x7d 0x02->0x7e �� 0x7d 0x01 ->0x7d;
//��0x7e��һ���ֽڿ�ʼ�ͺ�һ���ֽ����ֱ��У���룬�����У������ͬ��������ȷ��



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
   k = number-2; //ȥͷβ����0X7E
   j = 0; //ת�� ������

   for ( i=0; i<k;  i++ )//ת��
   {
         if(0x7d == *(tmp+i+j+1))   // ����0x7e�ĵ�һ���ַ�
         {
			if (0x02 == *(tmp+i+j+2))
            {
			   *(tmp+i) = 0x7e;	
         	j ++;
					k --; //?????? ���������Ż��									
			}
			else
			if (0x01 == *(tmp+i+j+2))
            {
			   *(tmp+i) = 0x7d;	
				 			j ++;
			      k --; //?????? ���������Ż��				
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
   for ( i=0; i<(k-1);  i++ )//����У����
   {
		sum ^= *(tmp+i);  //��ת����У��
   }	
   if (sum != *(tmp+k-1))
   {
	return 0;
   }
   else
    return k;


}


// ���ݷ�װ
unsigned short constructor(unsigned char* buffer,unsigned short number)
{
   volatile unsigned char* tmp = buffer;
   u8 tmp_buf[150];
   unsigned short i,j=0,k;
   volatile short sum=0 ;
   for ( i=0; i<number;  i++ )
   {
		sum ^= *(tmp+i);  //��ת����У��

   }	
   *(buffer+number) = sum;
   number ++;
   k = number; 
   
   tmp_buf[0] = 0x7e; //��־��
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
   tmp_buf[j] = 0x7e; //��־��
   memset(buffer,0,number);   
   memcpy(buffer,tmp_buf,j+1);
  
   return j+1;
}

//wei
/*****************
����˵����
������:void  SendGprs_Index(u16 len,u8 index)
len:���͵����ݳ���
index:�������ݵ�ͨ��
*****************/
void  SendGprs_Index(u16 len,u8 index)
{
	//volatile unsigned char g_ucGPRSDataBuff_hex[160];	
	int i;
	
	//memset((void*)g_ucGPRSDataBuff_hex,0,sizeof(g_ucGPRSDataBuff_hex));
	//hextoascii(g_ucGPRSDataBuff,(void*)g_ucGPRSDataBuff_hex,strlen((char *)g_ucGPRSDataBuff));
//wei,20160709,��ֹ��˯������ʱ��û��˯��
    if ( (system_para.g_bACCon &&(!g_ucACCFlag)) || (!system_para.g_bACCon))
    {
        if( (g_ucGotoSleepFlag) &&(system_para.g_ucPowerMode==EXTRA_LOW_POWER_MODE))//wei,20160709,����˯����������Ҫ�ڼ���������
            return ;
    }
//wei,20160709,��ֹ��˯������ʱ��û��˯��

//wei 	if (SEND_FAIL == (UDPSendData((char*)g_ucGPRSDataBuff,len)))
	if (SEND_FAIL == (UDPSendData_Index((unsigned char*)g_ucGPRSDataBuff,len,index)))
	{
		//DebugOut("\r[ ------SEND_FAIL ==UDPSendData------- ]\r\n",0);
			 for(i=0; i<100; i++ )
			 {
					if(g_ucSMSCmdMissionFlag == 1)
						return;							
//wei					delayms(50); 
					delayms(100);  //wei ��ɵȴ�10s
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
		      //�ر�GPRS led
			GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);  
#if 0		
			while(	GetGPRSStatus() != GPRS_CONNECTED)
			{
				if(i>3)
				{	
					DebugOut("  PowerOff SIM900.\r\n",0);
					P1 |= Pn6_OUTPUT_1;  	// �ر�GSM
					SoftDelay_ms(3000);  
					P1 &= ~Pn6_OUTPUT_1;  	// ����POWERkey
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
            //20130906 ������ʱ���Ѻ�ģ�����������������ط�ֱ��ͨ�����Ź���λ
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
					PMK4 = 1;  //��ֹring�ж�
					// PM1 |= (1<<0|1<<1); //UART0 RX TX AS INPUT
					PU3 &= ~(1<<3);
					PU1 &= ~(1<<1);

					P1 |= Pn6_OUTPUT_1;  	// �ر�GSM
					SoftDelay_ms(4000);  
					P1 &= ~Pn6_OUTPUT_1;  	// ����POWERkey
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
				 //20130906 ������ʱ���Ѻ�ģ�����������������ط�ֱ��ͨ�����Ź���λ
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
                
//wei,20160709                    GSM_Power(FALSE); //wei,20160608���Ź���λǰ����GSM�ϵ�
//wei,20160709                    while(1); //�ȿ��Ź���λϵͳ//20130906 ������ʱ���Ѻ�ģ�����������������ط�ֱ��ͨ�����Ź���λ					
                dial_status = 0;//wei,20160709,����־����
                GSM_reset();//wei,20160616����GSM
                StartGPRS();//wei,20160617����GSM
                if(dial_status ==0)
                {
                    ddi_uart_Write(&UART1_ctl_t,"SEND_index err, dog reset...",strlen("SEND_index err, dog reset..."));
                    GSM_Power(FALSE); //wei,20160608���Ź���λǰ����GSM�ϵ�
                    while(1); //�ȿ��Ź���λϵͳ//20130906 ������ʱ���Ѻ�ģ�����������������ط�ֱ��ͨ�����Ź���λ
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
		      //�ر�GPRS led
			GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);  
#if 0		
			while(	GetGPRSStatus() != GPRS_CONNECTED)
			{
				if(i>3)
				{	
					DebugOut("  PowerOff SIM900.\r\n",0);
					P1 |= Pn6_OUTPUT_1;  	// �ر�GSM
					SoftDelay_ms(3000);  
					P1 &= ~Pn6_OUTPUT_1;  	// ����POWERkey
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
                        
                        
                        //20130906 ������ʱ���Ѻ�ģ�����������������ط�ֱ��ͨ�����Ź���λ
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
                                    PMK4 = 1;  //��ֹring�ж�
                                    // PM1 |= (1<<0|1<<1); //UART0 RX TX AS INPUT
                                    PU3 &= ~(1<<3);
                                    PU1 &= ~(1<<1);
									 
                                    P1 |= Pn6_OUTPUT_1;  	// �ر�GSM
                                    SoftDelay_ms(4000);  
                                    P1 &= ~Pn6_OUTPUT_1;  	// ����POWERkey
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
				 //20130906 ������ʱ���Ѻ�ģ�����������������ط�ֱ��ͨ�����Ź���λ
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
                                
                      GSM_Power(FALSE); //wei,20160608���Ź���λǰ����GSM�ϵ�
                      while(1); //�ȿ��Ź���λϵͳ//20130906 ������ʱ���Ѻ�ģ�����������������ط�ֱ��ͨ�����Ź���λ				
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


// γ�ȣ�������Ҫ����ת���ɶȷ���ĸ�ʽ�����㷽��������յ���γ���ǣ�4546.40891
// 4546.40891/100=45.4640891����ֱ�Ӷ���45��, 4546.40891�C45*100=46.40891, ����ֱ�Ӷ���46��
// 46.40891�C46 =0.40891*60=24.5346����24��, ����γ���ǣ�45��46��24��
// ����
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
//ljw,20160922for С�������λ	ptmp1 = (const u8 *)strstr(ptmp,",");//
//ljw,20160922for С�������λ	len = ptmp1-ptmp;
	//if (len >= 4)
	{
		asciitohex((unsigned char *)ptmp, (unsigned char *) &tmpbuf, 2);  
		tmp += tmpbuf*500/3;  // 2500/9;    // 0.xx*60*1000000/3600;
//ljw,20160922for С�������λ		asciitohex((unsigned char *)(ptmp+2), (unsigned char *) &tmpbuf, len-2);  //�д���5����
        asciitohex((unsigned char *)(ptmp+2), (unsigned char *)(&tmpbuf),2);  //�д���5����
		tmp += tmpbuf*5/3;    // 0.00xx*60 *1000000/3600;
	}
//	asciitohex(src+7, (unsigned char *) &tmpbuf, 2);  
//	fen += tmpbuf;	

//	tmp += fen*100/60

      return tmp;
}
// ����dddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������)
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

// 0~359, ����ֱ��ʹ��asciitohex��Ҫÿһ�ֽڷֿ�
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

	// ʹ��GMT+8ʱ�����漰�����ڵĸ���
    tmp = (*(dec+3)/16)*10 +(*(dec+3))%16+8;
	if (tmp  >= 24)
	{
		*(dec+3) = ((tmp-24)/10)*16+(tmp-24)%10;
		//dd
		//tmp = *(dec+2) +1;//BCD��
		tmp_dd = (*(dec+2)/16)*10 +(*(dec+2))%16 +1; // hex��
		//mm  ����
		if (*(dec+1) == 2)
		{  
			//����
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
					// �귭ת
					if (((*(dec+1)/16) == 1)&&((*(dec+1)%16) == 2))
					{
						*dec += 1;
						// ��ĸ�λ��9
						if ((*(dec)%16) ==10)
						{
							tmp =  (*dec /16)*10 +(*dec%16);
							*(dec) = (tmp/10)*16 + (tmp %16);
						}
						*(dec+1) = 1; //��Ϊ
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

      if (src >100000)  //�˿� һ�㶼�ǣ�λ��
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

//flow_from_table: ƽ̨��������ˮ�ţ����Ϊ0XFFFF?�Ͳ���ƽ̨��ˮ��
u8  ACC_GPS_flag = 0;
u8  ACC_GSM_flag = 0;
//ljw,20160908 test reset flag extern u8 erro_flag ;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
//position_message_struct position_0200;//ljw,20160906����Ҫɾ

#if 1//ljw,20160906 ���Ե�����λ����
static u32 latitude_b = 0;//ljw,20160824for ACC
static u32 longitude_b = 0;//ljw,20160824for ACC    
static u32 Lac_b = 0;//ljw,20160824for ACC
static u32 LacID_b = 0;//ljw,20160824for ACC
#endif   //ljw,20160906 ���Ե�����λ����
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
	u8 additional_beyond[3]={0x11,0x01,0}; //���ٸ����ֶ�
	u8 add_flag=0;
    u32 tmp_mileage = wzw_mileage.mileage_64 /3600;

//	wzw_mileage.mileage_64 += 36000;//wei,������̣���Ҫע��
//ljw,20160908 test reset flag    erro_flag = 0;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
//ljw,20160906 ���Ե�����λ����    memset((u8 *)(&position_0200),0x00,sizeof(position_0200));//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ

	position.alarm = 0;
	position.status = 0;
	position.altitude = 0;
//ljw,20160906for space	while(GPRSCmdFlag == 1);
    if(send_gps_gsm_flag == GPS_FLAG)
    {
        position = GPS_Deal( position);  //wei,20160820 ����GPS�ӳ���        
    }
    else
    {
       position = Deal_GSM_LAC( position);  //wei,20160820 ����GPS�ӳ���
//ljw,20160906����Ҫɾ          Deal_GSM_LAC();//ljw,20160906����Ҫɾ 
    }
//ljw,20160908 test reset flag    erro_flag = 50;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
#if 0  //wei,20160820 ����GPS�ӳ���
////////////////////wei
	//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
	// ����ʱ��
	if (dingwei_flag == 0x5a)
		i = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
	else
		i = (unsigned char *)strstr((char *)backup_g_ucNeo6QBuff,",");
	i++;
	memcpy((char *)(tmp_time+6),(char *)(i+1),6);	// ����ʱ��
	i +=6;
	i = (unsigned char *)strstr((char *)i,",");

	if(('A' ==*(i+1) ) &&(dingwei_flag))
	{
		position.status |= EXCURSION_POSITION_STATUS_ORIENTATION; 
	}
	// ����GPS��Ϣ���ñ�־
	i = (unsigned char *)strstr((char *)(i+1),",");
	// ���� γ�ȣ�
	position.latitude = asciitolatitude(i+1);
	i = (unsigned char *)strstr((char *)(i+1),",");
	if ('S' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_SOURTH; 
	}
	i = (unsigned char *)strstr((char *)(i+1),",");
	// ���� ���ȣ�
	position.longitude = asciitolatitude(i+1); //asciitolongitude(i+1);
	i = (unsigned char *)strstr((char *)(i+1),",");
	//position.status = 0;
	if ('W' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_WEST; 
	}
	i = (unsigned char *)strstr((char *)(i+1),",");
	
//	if( 1 )//(strstr((char *)g_ucNeo6QBuff,",A,"))  // �Ѿ��жϹ���
	{
			// ���� �ٶ�
			ptmp= (unsigned char *)strstr((char *)(i),".");

			position.speed = asciitospeed(i+1,ptmp-i-1);
			
			//��ֹGPS�����г����ر�������
			if (dingwei_flag  != 0x5a)  
			{
				position.speed = 0;
			}
			if((position.speed > system_para.g_ucMaxSpeed*10) &&(system_para.g_ucMaxSpeed>0))//wei zhu,20160804���ٱ���
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
			// ���� �Ƕ�

			position.direction = asciitodirection(i+1,ptmp-i-1);
			//��ֹGPS�����г����ر�������
			if (dingwei_flag  != 0x5a)
			{
				position.direction = 0;
			}
			i = (unsigned char *)strstr((char *)(i+1),",");
			memcpy((char *)(tmp_time),(char *)(i+1),6);	// ��������
//wei,   DDMMYYHHmmSS
			asciitodate(tmp_time,position.time);
//wei,for rtc <
            if (dingwei_flag == 0x5a)
            {
                memcpy(time_gps_rtc,position.time,6);//wei,20160810,��ʱ�俽����time_gps_rtc��������RTCʱ���׼
                backsecondforRTC = secondcounter;
                RTC_timeavalibe_flag = 0x50;//wei,20160812 0x0a,ʱ����Ч  0x50,ʱ����Ч
            }
//wei,for rtc >
	}
/////////////////////wei,
#if 0 //wei,20160804 for space
      else
      	{
      		tmp = '0';
		strncat((char *)g_ucGPRSDataBuff,(char *)(&tmp),1);  // ���� �ٶ���Ϣ
		strcat((char *)g_ucGPRSDataBuff,",");
		strncat((char *)g_ucGPRSDataBuff,(char *)(&tmp),1);  // ���� �ٶ���Ϣ
		strcat((char *)g_ucGPRSDataBuff,",");
		i = (unsigned char *)strstr((char *)(i+1),",");
	}
#endif //wei,20160804 for space
#endif
  //ljw,20160904���ٱ����жϷŻ���<
    if((position.speed > system_para.g_ucMaxSpeed*10)&&(system_para.g_ucMaxSpeed>0)&&(send_gps_gsm_flag == GPS_FLAG))//wei zhu,20160804���ٱ���
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
//ljw,20160908 test reset flag    erro_flag =7;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
  //ljw,20160904���ٱ����жϷŻ���>
    memset(g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
    
    if(send_gps_gsm_flag == GSM_FLAG)//wei,20160822
        head.id = MOBILE_LOCATION_SEND;
    else head.id = gprsid;//wei,20160822
	if((flow_from_table == 0xffff)||((head.id == MOBILE_LOCATION_SEND)&&(flow_from_table == 0xffff)))
	{
		head.attribute = sizeof(position)+6;  // ���������ͳ�ơ�����01��********,Ҳ��Ҫ�ߵ�λ�仯
        if((ACC_GPS_flag != 0x5a)&&(ACC_GSM_flag != 0x5a)&&(system_para.g_bACCon&&(g_ucACCFlag !=0x5a)))//GPS
        {//wei,20160825,��ACC��ĵ�һ��λ�ô���
            if(head.id == MOBILE_LOCATION_SEND)//wei,���һ���ǻ�վ��λ
            {
                ACC_GSM_flag = 0x5a;
                ACC_GPS_flag = 0x00;
                Lac_b = position.latitude;	
                LacID_b = position.longitude;
            }
            else //wei,���һ����GPS��λ
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
        if(ACC_GPS_flag ==0x5a)//wei,20160825,��GPS��������
        {
            position.latitude = latitude_b;	
            position.longitude = longitude_b;
            if(dingwei_flag != 0x5a)
                RTC2Position_time(&position.time[0]);//wei,20160825,���GPS����λ��ȡϵͳ��RTCʱ��
            position.speed = 0x0000;//wei,20160822
            position.altitude = 0x0000;
            position.direction = 0x0000;
        }
        else if(ACC_GSM_flag ==0x5a)//wei,20160825����վ��������
        {
            position.latitude = Lac_b;	
            position.longitude = LacID_b;
            position.direction = 0x0000;
        }
        
//        else //wei,20160825�����Ϸ�λ��
//        {
//            ACC_GPS_flag = 0x00;
//            ACC_GSM_flag = 0x00;
//        }
	}
	else //����
	{
        head.attribute = sizeof(position)+6;
        if(head.id != MOBILE_LOCATION_SEND)//ljw,20160908��������վ���ݲ�Ҫ��Ӧ����ˮ�ţ�GPSҪ��
            head.attribute +=2;
		
         ACC_GPS_flag = 0x00;
         ACC_GSM_flag = 0x00;
	}
//    if(T_P_time > 1)//wei,20160825,��ʱ׷��
//    {
//        ACC_GPS_flag = 0x00;
//        ACC_GSM_flag = 0x00;
//    }
	if (add_flag)
	{
		head.attribute += sizeof(additional_beyond);
	}
//ljw,20160908 test reset flag	erro_flag =8;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	head.serial_number = flowwater++;
	//tmp_time[0]='0';
	//memcpy(&tmp_time[1],g_ucCurrentNum,sizeof(g_ucCurrentNum));
	
	//asciitohex (tmp_time,head.mbphonenum,12);
	memcpy((u8*)(head.mbphonenum),(u8*)(system_para.g_ucDeviceID),sizeof(system_para.g_ucDeviceID));
//ljw,20160908 test reset flag    erro_flag = 9;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	if (g_ucAlarm)
	{
	//	if(voltage >power_battary)
		if( ((BatteryStatusGet()>50)))
	//20150907: ��ѹ���ڵĵ�·������û����ӵ�Դ������½���������
			position.alarm |= EXCURSION_POSITION_ALARM_ALARM;
		else
			position.alarm &= ~EXCURSION_POSITION_ALARM_ALARM;//�������󴥷���������
		g_ucAlarm = 0;		
	}

//wei	if (get_ACC_CHECK_status()&&system_para.g_bACCon)
	if ((g_ucACCFlag ==0x5a)&&system_para.g_bACCon) //wei,20160524,ACC�޸�
	{
        position.status |= EXCURSION_POSITION_STATUS_ACC;
	}

//ljw,20160903,�𶯼��˹���	if ((Impact_flag) && (shake_gate))
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
    if((Impact_Alarm_flag==0xa5)&&(shake_gate)&&(g_ucACCFlag != 0x5a))
	{
        position.alarm |= EXCURSION_POSITION_ALARM_SHAKE;
        Impact_Alarm_flag = 0x0a;//ljw,20160903,�𶯼��˹���,���鷢��ȥ��
//ljw,20160903,�𶯼��˹���	          Impact_flag = 0;
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

	//�����ߵ��ֽ�
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
		g_ucGPRSDataBuff[sizeof(head)+sizeof(position)] =  0x01; // ���ͳ�Ƶ�ID
		g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+1] =  0x04; // ���ͳ�Ƶĳ���
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
		g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+2] =  0x01; // ���ͳ�Ƶ�ID
		g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+3] =  0x04; // ���ͳ�Ƶĳ���
		memcpy(&g_ucGPRSDataBuff[sizeof(head)+sizeof(position)+4],(u8*)&tmp_mileage,sizeof(tmp_mileage));  
		  
		//sendlen = constructor(g_ucGPRSDataBuff,sizeof(position)+sizeof(head)+2);
		if (add_flag)//wei,zhu,20160804���٣���ӳ��ٸ�����Ϣ
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
//ljw,20160908 test reset flagerro_flag =10;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	//
	// ���� CMD00
	//
//ljw,20160906for space 	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei
//ljw,20160908 test reset flagerro_flag =11;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
    //ljw,20160903 for space	DebugOut("\r[ -------------------- GPRS Data Send OK! --------------------- ]\r\n",0);

//ljw,20160903 for space	DebugOut("\r[ ------------ User ",0);
//ljw,20160903 for space	DebugOut(g_ucGPRSDataBuff+8,3);
//ljw,20160903 for space	DebugOut(" Request GPRS Data.Send OK! ------------ ]\r\n",0);
}

//wei,20160805 GPS��ȡ����<


u8 time_gps_rtc[6]={0};//wei,20160809,YYMMDDHHmmSS ����GPS��ʱ�䣬��ΪRTCʱ��Ļ�׼
u32 backsecondforRTC = 0;//wei,20160809, RTC��ʼ��ʱ�ĵ�
u8 RTC_timeavalibe_flag = 0x0a;//wei,20160812 0x0a,ʱ����Ч  0x50,ʱ����Ч
//YYMMDDHHmmSS 0x16 0x08 0x10 0x10 0x50 0x54
//             0    1    2    3    4    5
//#if 0

//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{
    if(year%4==0) //�����ܱ�4����
    {
        if(year%100==0) 
        {
            if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
            else return 0;   
        }
        else return 1;   
    }
    else return 0;	
}
 			   
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�	  
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u32 RTC_seccount = 0;//wei,20160810,������ʱ���붼ת�����룬�ڼ��߹���������
tm timer;
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 seccount=0;
    if(syear<2000||syear>2099)return 1;	   
    for(t=2000;t<syear;t++)	//��������ݵ��������
    {
        if(Is_Leap_Year(t))seccount+=31622400;//�����������
        else seccount+=31536000;			  //ƽ���������
    }
    smon-=1;
    for(t=0;t<smon;t++)	   //��ǰ���·ݵ����������
    {
        seccount+=(u32)mon_table[t]*86400;//�·����������
        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������	   
    }
    seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ���������� 
    seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;	 //����������
    seccount+=sec;//�������Ӽ���ȥ
    RTC_seccount=seccount+(secondcounter - backsecondforRTC);//�õ��������е�ֵ(������)
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
	
    temp=RTC_seccount/86400;   //�õ�����(��������Ӧ��)
    
	if(daycnt!=temp)//����һ����
	{
		daycnt=temp;
		temp1 = 2000;	//��GPS��ȡ�����꣬��ʼ
		while(temp>=365)
		{
			if(Is_Leap_Year(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else {temp1++;break;}  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}
		timer.w_year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(Is_Leap_Year(timer.w_year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break;
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		timer.w_month=temp1+1;//�õ��·�
		timer.w_date=temp+1;  //�õ����� 
	}
	temp=RTC_seccount%86400;     //�õ�������   	   
	timer.hour=temp/3600;     //Сʱ
	timer.min=(temp%3600)/60; //����	
	timer.sec=(temp%3600)%60; //����
//	timer.week=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);//��ȡ����   
	return 0;
}
#if 1  //wei,��GPS�����ó�������
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
	u8 additional_beyond[3]={0x11,0x01,0}; //���ٸ����ֶ�
	u8 add_flag=0;
    
    position.alarm = 0;
	position.status = 0;
	position.altitude = 0;
    
    IWDG_ReloadCounter();//ljw,20160904��ֹ����ĵȴ���λ
//ljw,20160906for space	while(GPRSCmdFlag == 1);

	//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
	// ����ʱ��
#ifndef GPS_OLD //ljw,20160904�¸ĵ�GPS
    
    if(dingweied_flag) //ljw,20160904ֻҪ�Ƕ���Ϊ���ṹ�������������
    {
        position.latitude = asciitolatitude(gps_ascii.Lat);           //��ȡ����
        if('S' == gps_ascii.uLat)                                      //��ȡ���ȱ�־
        {
            position.status |= EXCURSION_POSITION_STATUS_SOURTH; 
        }
        position.longitude = asciitolatitude(gps_ascii.Lon);           //��ȡγ��
        if ('W' == gps_ascii.uLon)                                     //��ȡγ�ȱ�־
        {
            position.status |= EXCURSION_POSITION_STATUS_WEST; 
        }
        i = gps_ascii.Spd;
        ptmp = (unsigned char *)strstr((char *)(i),".");
//        if(ptmp-i >0)&&(ptmp-i <=3)//ljw,20160907�ٶ�ֵ�����ж�
        position.speed = asciitospeed(i,ptmp-i);//asciitospeed(gps_ascii.Spd,strlen((const u8 *)gps_ascii.Spd)); 
//        else position.speed = 0;
        if(position.speed > 2200)//ljw,20160928�ٶȴ���220��Ϊ��Ч
            position.speed = 0;
        //��ȡ�ٶ�
        //���ٶ��ж�
        i = gps_ascii.Cog;
        ptmp = (unsigned char *)strstr((char *)(i),".");        
        position.direction = asciitodirection(gps_ascii.Cog,ptmp-i);//ljw strlen((u8 *)gps_ascii.Cog)
        if(position.direction > 359)
            position.direction = 0;
        
        position.status |= EXCURSION_POSITION_STATUS_ORIENTATION;//ljw,20160920 ֻҪ��������GPS���ݣ���һ���ö�λ��NOLocalCount>=120��������Ϊ10�� 
        if(('A' ==gps_ascii.status ) &&(dingwei_flag))                //��ȡ��λ��־
        {
            memcpy((char *)(tmp_time+6),(char *)(gps_ascii.UTCtime),6);	  // ����ʱ��.             
            memcpy((char *)(tmp_time),(char *)(gps_ascii.Date),6);	// ��������
            asciitodate(tmp_time,position.time);
//ljw,20160920 NOLocalCount>=120��������Ϊ0��            position.status |= EXCURSION_POSITION_STATUS_ORIENTATION;
            memcpy(time_gps_rtc,position.time,6);//wei,20160810,��ʱ�俽����time_gps_rtc��������RTCʱ���׼
            backsecondforRTC = secondcounter;
            RTC_timeavalibe_flag = 0x50;//wei,20160812 0x0a,ʱ����Ч  0x50,ʱ����Ч
        }
        else 
        {
            position.speed = 0;//δ��λ���ٶ�ǿ��Ϊ��
            position.direction = 0;//δ��λ���Ƕ�ǿ��Ϊ��
            RTC_Get();//wei,20160810 ����RTCʱ��
            RTC2Position_time(&position.time[0]);//wei,20160825,ȡϵͳ��RTCʱ��            
        }
#if 0 //ljw,20160904���ٱ����жϷŵ�ԭ����send��������
        //���ٱ���
        if((position.speed > system_para.g_ucMaxSpeed*10) &&(system_para.g_ucMaxSpeed>0))//wei zhu,20160804���ٱ���
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
#endif //ljw,20160904���ٱ����жϷŵ�ԭ����send��������
    }
#endif
#ifdef GPS_OLD  //ljw,20160904�޸�GPS��ȡ�������ŵ�һ���̶��ĺ�����һ������ȡ�ŵ��ṹ��gps_ascii����
	if(dingwei_flag == 0x5a)
    {
        i = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
        i++;
        memcpy((char *)(tmp_time+6),(char *)(i),6);	// ����ʱ��
    }
	else
		i = (unsigned char *)strstr((char *)backup_g_ucNeo6QBuff,",");	
	i +=6;
	i = (unsigned char *)strstr((char *)i,",");

	if(('A' ==*(i+1) ) &&(dingwei_flag))
	{
		position.status |= EXCURSION_POSITION_STATUS_ORIENTATION; 
	}
	// ����GPS��Ϣ���ñ�־
	i = (unsigned char *)strstr((char *)(i+1),",");
	// ���� γ�ȣ�
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
	// ���� ���ȣ�
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
	
//wei	if( 1 )//(strstr((char *)g_ucNeo6QBuff,",A,"))  // �Ѿ��жϹ���
//wei	{
        // ���� �ٶ�
        ptmp= (unsigned char *)strstr((char *)(i),".");
        position.speed = asciitospeed(i+1,ptmp-i-1);			
        //��ֹGPS�����г����ر�������
        if(dingwei_flag  != 0x5a)//wei...||((system_para.g_bACCon)&&(!g_ucACCFlag)))
        {
            position.speed = 0;
        }
        if((position.speed > system_para.g_ucMaxSpeed*10) &&(system_para.g_ucMaxSpeed>0))//wei zhu,20160804���ٱ���
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
        
        // ���� �Ƕ�
        position.direction = asciitodirection(i+1,ptmp-i-1);
        //��ֹGPS�����г����ر�������
        if (dingwei_flag  != 0x5a)  
        {
            position.direction = 0;
        }
        i = (unsigned char *)strstr((char *)(i+1),",");
        if(dingwei_flag == 0x5a)
        {
            memcpy((char *)(tmp_time),(char *)(i+1),6);	// ��������
            asciitodate(tmp_time,position.time);
        }
        else
        {
            RTC_Get();//wei,20160810 ����RTCʱ��
            RTC2Position_time(&position.time[0]);//wei,20160825,ȡϵͳ��RTCʱ��
//            position.time[0] = RTC_ByteToBcd2((u8)(timer.w_year-2000));
//            position.time[1] = RTC_ByteToBcd2(timer.w_month);
//            position.time[2] = RTC_ByteToBcd2(timer.w_date);
//            position.time[3] = RTC_ByteToBcd2(timer.hour);
//            position.time[4] = RTC_ByteToBcd2(timer.min);
//            position.time[5] = RTC_ByteToBcd2(timer.sec);
        }
        if (dingwei_flag == 0x5a)//���»�׼ʱ��
        {
            memcpy(time_gps_rtc,position.time,6);//wei,20160810,��ʱ�俽����time_gps_rtc��������RTCʱ���׼
            backsecondforRTC = secondcounter;
            RTC_timeavalibe_flag = 0x50;//wei,20160812 0x0a,ʱ����Ч  0x50,ʱ����Ч
        }
//wei	}
#endif //ljw,20160904�޸�GPS��ȡ�������ŵ�һ���̶��ĺ�����һ������ȡ�ŵ��ṹ��gps_ascii����
        

    return position ;
}
//wei,20160825,ȡϵͳ��RTCʱ��
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
u8 time_ungps_flag =0x00;//wei,20160805,ʱ����Ч��־���˱�־��Ҳ���Բ�Ҫ��ֱ����dingweied_flag�����
void Time_Deal(void);
{
    u32 temp_second =0 ;
}
#endif
//wei,20160805 GPS��ȡ����>
/*
**-----------------------------------------------------------------------------
**
**	����:
**		�����ϴ�����
**
**	����:
**		��
**
**	����:
**		��
**
**-----------------------------------------------------------------------------
*/
u8 send_gps_gsm_flag = 0;//wei,20160822 0x5a,GSM  0xa5,GPS
void cmd_0200(void)
{
//ljw,20160906for space	while(GPRSCmdFlag == 1);
	gps_gate = 0;
//wei,20160825	if(((!GetAvailableGPSInfoV())&&(dingweied_flag == 0))||(NOLocalCount>=120))
//ljw,20160920û��Ҫ    GetAvailableGPSInfoV_N(2);//ljw,20160920
    if((ACC_GPS_flag !=0x5a)&&(dingwei_flag == 0))//(((!GetAvailableGPSInfoV())&&(dingweied_flag == 0))||(NOLocalCount>=10)))//ljw,20160920 NOLocalCount>=120��������Ϊ10��
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
        ACC_GSM_flag = 0x00;//wei,20160825,��GSM���ͱ�־����
    }
	send_location_gprs(0xffff,POSITION_REPORT);
	gps_gate = 1;
}

// ��������
void cmd_0002(void)
{
	u16 sendlen;
	gprs_message_head_struct head;
	head.id = HEARTBEAT;
	head.serial_number = flowwater++;

	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
	head.attribute = 0;
	
	//�����ߵ��ֽ�
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


// status : 0���ɹ�/ȷ�ϣ�1��ʧ�ܣ�2����Ϣ����3����֧��
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
			//�����ߵ��ֽ�
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
    swapHLchar((u8*)&(message_id_from_table),sizeof(message_id_from_table));//ljw,20161217,�޸��ն�ͨ��Ӧ��
	memcpy(ptmp, &message_id_from_table,sizeof(message_id_from_table));
	ptmp += sizeof(message_id_from_table);
	*ptmp = status1;


	
	sendlen = constructor(g_ucGPRSDataBuff, 5+sizeof(head));

//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei


}


//�����ն˲���
void cmd_SET_TERMINAL_PARAMETER(void)
{
	u32 parameter_id,u32tmp;
	u8 parameter_num;
	u8* ptmp;
	u8 i,j;
	u16 sendlen;
	volatile u8 recelen=0;

	ptmp = & g_ucGPRSCmdBuff[12];
	parameter_num = *ptmp; 	//�������ֽ���
//	if (parameter_num > 1)��//��ֹһ�	
      // parameter_id = bigbuf.big.g_ucGPRSCmdBuff[1]*0x100+bigbuf.big.g_ucGPRSCmdBuff[2];
      ptmp ++;//   = 8; //bigbuf.big.g_ucGPRSCmdBuff[12]
	for(i =0; i<parameter_num;)	
	{
		//parameter_id = *(u32 *)ptmp ; //�������˫�ֶ���Ļ�����߻ᱨHARDFAULT
		memcpy((u8*)&parameter_id,ptmp,sizeof(parameter_id));
		//�����ߵ��ֽ�
		swapHLchar((u8*)&(parameter_id),sizeof (parameter_id));
		
		ptmp +=4;
		//i +=4;
		switch(parameter_id)
		{
			case  ALTERNATION_HEARTBEAT:
				//system_para.HeartAternationTime = *(u32*)(ptmp);
				ptmp ++;
				memcpy((u8*)&system_para.HeartAternationTime,ptmp,sizeof(system_para.HeartAternationTime));
				//�����ߵ��ֽ�
				swapHLchar((u8*)&(system_para.HeartAternationTime),sizeof (system_para.HeartAternationTime));
				 if(system_para.HeartAternationTime <10) //wei,20160603ǿ�������������С��10s
				     system_para.HeartAternationTime = 10; //wei,20160603
				 if(system_para.HeartAternationTime >300) //wei,20160603ǿ�������������С��10s
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
			case  UDP_PORT: //��������HEX��Ҫת�ɣ������ַ�
				
				recelen = *ptmp;
				ptmp ++;
				if (FHTIP != system_para.g_ucSysMode_S)
				{
					memset(system_para.g_ucUDPPort,0,5);
					//u32tmp = *(u32*)(ptmp);
					memcpy((u8*)&u32tmp,ptmp,sizeof(u32tmp));
					
					//�����ߵ��ֽ�
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
				
				//�����ߵ��ֽ�
				swapHLchar((u8*)&(system_para.ALTERNATION_IN_SLEEPTime),sizeof (system_para.ALTERNATION_IN_SLEEPTime));
				
				ptmp += recelen;
				i ++;			
				break;

			case  ALTERNATION_IN_ALARM:
				recelen = *ptmp;
				ptmp ++;				
				//system_para.ALTERNATION_IN_ALARMTime = *(u32*)(ptmp);
				memcpy((u8*)&system_para.ALTERNATION_IN_ALARMTime,ptmp,sizeof(system_para.ALTERNATION_IN_ALARMTime));
				
				//�����ߵ��ֽ�
				swapHLchar((u8*)&(system_para.ALTERNATION_IN_ALARMTime),sizeof (system_para.ALTERNATION_IN_ALARMTime));
				ptmp += recelen;
				i ++;	

			case  ALTERNATION_DEFAULT_POSITION:  
				recelen = *ptmp;
				ptmp ++;				
//				system_para.g_usGPRSUpLoadAternationTime = *(u32*)(ptmp);
				memcpy((u8*)&system_para.g_usGPRSUpLoadAternationTime,ptmp,sizeof(system_para.g_usGPRSUpLoadAternationTime));

				//�����ߵ��ֽ�
				swapHLchar((u8*)&(system_para.g_usGPRSUpLoadAternationTime),sizeof (system_para.g_usGPRSUpLoadAternationTime));
				ptmp += recelen;
				i ++;					
				break;
						
			case  HIGH_SPEED:
				recelen = *ptmp;
				ptmp ++;				
				//system_para.g_ucMaxSpeed = (u8)(*(u32*)(ptmp));
				memcpy((u8*)&system_para.g_ucMaxSpeed,ptmp,sizeof(system_para.g_ucMaxSpeed));
				
				//�����ߵ��ֽ�
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
			for(time=0; time<10 ; time++)    // 20151017: ip �л����ɹ��������ſ���
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
                    break ;//wei,20160617,��ֹ������ʱ���ˣ�ȴ�޷����ߣ����ڲ���ѭ������
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
                GSM_Power(FALSE); //wei,20160608���Ź���λǰ����GSM�ϵ�
                while(1); //�ȿ��Ź���λϵͳ
			}
		}
	}
}

//Ӧ���ǰ����ԵĲ���һ���͸�������
void cmd_INQUIRE_TERMINAL_PARAMETER(void)
{
	u32 parameter_id;
	u8 parameter_num;
	//u8* ptmp; //�����
	//u8 i;
	gprs_message_head_struct head;
	u8* ptmp_send;
	u16 sendnum;
	u16 sendlen;
	u32 u32tmp;

	head.id = INQUIRE_TERMINAL_PARAMETER_RESPONSION;
	head.serial_number = flowwater++;
	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
//	head.attribute = 5;   //һ��ʼ��֪����Ҫ��ͳ�����˲���

	//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	//memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	ptmp_send = &(g_ucGPRSDataBuff[sizeof(head)]);
	memcpy(ptmp_send, &flowwater_form_table,sizeof(flowwater_form_table));
	//ptmp_send += sizeof(flowwater_form_table);
	sendnum = sizeof(flowwater_form_table);


	parameter_num = 10; 	//��������

     
	//�������ϸ�Ĳ�����Ŀ
      *(ptmp_send+sendnum++) = parameter_num;
	 // sendnum ++;
	

//��ѯ�ն˲���Ӧ��
// ��ϢID��0x0104��
// ��ѯ�ն˲���Ӧ����Ϣ�����ݸ�ʽ����16��
// ��16 ��ѯ�ն˲���Ӧ����Ϣ�����ݸ�ʽ
// ��ʼ�ֽ�                     �ֶ�             ��������              ������Ҫ��
// 0                           Ӧ����ˮ��            WORD                        ��Ӧ���ն˲�����ѯ��Ϣ����ˮ��
// 2                         Ӧ���������           BYTE
// 3                               �������б�                                        �������ʽ�Ͷ������10

			
	//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
	u32tmp = ALTERNATION_HEARTBEAT;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);
	//�����ߵ��ֽ�
	swapHLchar(ptmp_send+sendnum,4);
	 
	sendnum += 4; 
	*(ptmp_send+sendnum) = 4;
	//u32tmp = 4;
	//memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	//�����ߵ��ֽ�
	//swapHLchar(ptmp_send+sendnum,4);	
	sendnum ++;
	//*(u32*)(ptmp_send+sendnum) = system_para.HeartAternationTime ; 
	u32tmp = system_para.HeartAternationTime ;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);
	//�����ߵ��ֽ�
	swapHLchar(ptmp_send+sendnum,4);	
	sendnum += 4; 
	


	//*(u32*)(ptmp_send+sendnum) = APN;
	u32tmp = APN;
	memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);	
	//�����ߵ��ֽ�
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
	
		//�����ߵ��ֽ�
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
	
	//�����ߵ��ֽ�
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
	
	//�����ߵ��ֽ�
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
	
	//�����ߵ��ֽ�
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
		//�����ߵ��ֽ�
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));	  
      memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	  
	sendlen = constructor(g_ucGPRSDataBuff, sendnum+sizeof(head));

//ljw,20160906for space	while(GPRSCmdFlag == 1);
//wei	SendGprs(sendlen);
	SendGprs_Index(sendlen,(u8)FHT_SERVERS);//wei
}

//��ϢID��0x8105��
//�ն˿�����Ϣ�����ݸ�ʽ����17��
//��17 �ն˿�����Ϣ�����ݸ�ʽ
//19
//��ʼ�ֽ�    �ֶ�      ��������    ������Ҫ��
//0          ������        BYTE    �ն˿���������˵������18
// 1         �������       STRING  ���������ʽ���������������ÿ���ֶ�֮�����
//��ǡ�;���ָ���ÿ��STRING �ֶ��Ȱ�GBK ���봦
//����������Ϣ
//��18 �ն˿���������˵��
//������ ������� ������Ҫ��
// 1 ���������ʽ����19 ��������������֮����ð�Ƿֺŷָ���ָ�����£���URL ��ַ;���ŵ�
//����;�����û���;��������;��ַ;TCP �˿�;UDP �˿�;������ID; Ӳ��
//�汾;�̼��汾; ���ӵ�ָ��������ʱ�ޡ�����ĳ��������ֵ����ſ�
// 2 ���������ʽ����19 �����ն�����ָ��������������֮����ð�Ƿֺŷָ�������ָ�����£�
//�����ӿ���;���ƽ̨��Ȩ��;���ŵ�����;�����û���;��������;��
//ַ;TCP �˿�;UDP �˿�;���ӵ�ָ��������ʱ�ޡ�����ĳ��������ֵ����
//�ſգ������ӿ���ֵΪ1�����޺�̲���
// 3 �� �ն˹ػ�
// 4 �� �ն˸�λ
//5 �� �ն˻ָ���������
//6 �� �ر�����ͨ��
//7 �� �ر���������ͨ��



void cmd_SET_GPRS_SERVICER(void)
{



}
//��ѯ�����б�
//��ʼ�ֽ� �ֶ�                   ��������                        ������Ҫ��
// 0                ��������              BYTE                             ��������Ϊn
// 1               ����ID �б�           BYTE[4*n]                       ����˳�����У��硰����ID1 ����ID2......����IDn����
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
//	head.attribute = 5;   //һ��ʼ��֪����Ҫ��ͳ�����˲���

	//g_ucGPRSDataBuff as send buf
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	//memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));
	ptmp_send = &(g_ucGPRSDataBuff[sizeof(head)]);
	memcpy(ptmp_send, &flowwater_form_table,sizeof(flowwater_form_table));
	//ptmp_send += sizeof(flowwater_form_table);
	sendnum = sizeof(flowwater_form_table);


	ptmp = & g_ucGPRSCmdBuff[12];
//	parameter_num = *ptmp; 	//��������
//	if (parameter_num > 1)��//��ֹһ�	
      // parameter_id = bigbuf.big.g_ucGPRSCmdBuff[1]*0x100+bigbuf.big.g_ucGPRSCmdBuff[2];
      parameter_num = *ptmp; //�������ϸ�Ĳ�����Ŀ
      *(ptmp_send+sendnum++) = parameter_num;
	// sendnum ++;
      ptmp ++;
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// �о�����ķ��ͺͽ��յĻ�������Ļ�����
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

//��ѯ�ն˲���Ӧ��
// ��ϢID��0x0104��
// ��ѯ�ն˲���Ӧ����Ϣ�����ݸ�ʽ����16��
// ��16 ��ѯ�ն˲���Ӧ����Ϣ�����ݸ�ʽ
// ��ʼ�ֽ�                     �ֶ�             ��������              ������Ҫ��
// 0                           Ӧ����ˮ��            WORD                        ��Ӧ���ն˲�����ѯ��Ϣ����ˮ��
// 2                         Ӧ���������           BYTE
// 3                               �������б�                                        �������ʽ�Ͷ������10

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
				
				//�����ߵ��ֽ�
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
				//�����ߵ��ֽ�
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
				//�����ߵ��ֽ�
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
				//�����ߵ��ֽ�
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
				//�����ߵ��ֽ�
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
				//�����ߵ��ֽ�
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
				
				sendnum ++;				
				 //*(u32*)(ptmp_send+sendnum) = system_para.ALTERNATION_IN_SLEEPTime ;
				u32tmp = system_para.ALTERNATION_IN_SLEEPTime; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					 
				//�����ߵ��ֽ�
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4;
				break;

			case  ALTERNATION_IN_ALARM:
				//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
				u32tmp = ALTERNATION_IN_ALARM; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//�����ߵ��ֽ�
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
				sendnum ++;					
				//*(u32*)(ptmp_send+sendnum) = system_para.ALTERNATION_IN_ALARMTime  ;
				u32tmp = system_para.ALTERNATION_IN_ALARMTime  ;
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					

				//�����ߵ��ֽ�
				swapHLchar(ptmp_send+sendnum,4);				
				sendnum += 4;

			case  ALTERNATION_DEFAULT_POSITION:  
				//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
				u32tmp = ALTERNATION_DEFAULT_POSITION; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//�����ߵ��ֽ�
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
				sendnum ++;					
				//*(u32*)(ptmp_send+sendnum) = system_para.g_usGPRSUpLoadAternationTime ;
				u32tmp = system_para.g_usGPRSUpLoadAternationTime ;
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//�����ߵ��ֽ�
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4;
				break;
						
			case  HIGH_SPEED:
				//*(u32*)(ptmp_send+sendnum) = ALTERNATION_HEARTBEAT;
				u32tmp = HIGH_SPEED; 
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//�����ߵ��ֽ�
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4; 
				*(ptmp_send+sendnum) = 4;
				sendnum ++;					
				//*(u32*)(ptmp_send+sendnum) = system_para.g_ucMaxSpeed ;
				u32tmp = system_para.g_ucMaxSpeed ;
				memcpy(ptmp_send+sendnum,(u8*)&u32tmp,4);					
				//�����ߵ��ֽ�
				swapHLchar(ptmp_send+sendnum,4);				

				sendnum += 4;
				break;

			default:
				break;


		}		
		
		
	}
      head.attribute = sendnum;
		//�����ߵ��ֽ�
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
//ljw,20160920 NOLocalCount>=120��������Ϊ10��    if(((!GetAvailableGPSInfoV())&&(dingweied_flag == 0))||(NOLocalCount>=120))//ljw,20160823,������
	
    if(dingwei_flag == 0)
    {
		//wei 20160607cmd_GENERAL_RESPONSION(message_FALSE);
	  	//wei 20160607cmd_0002();
//wei,20160822	  	uploadLAC();//wei,20160607,�г�����Ӧ������ʱ��û��GPS����վ
        send_gps_gsm_flag = GSM_FLAG;//wei,20160822
//wei,20160823		gps_gate = 1;
//wei,20160823	  	return;
	}
    else send_gps_gsm_flag = GPS_FLAG;//wei,20160822
	//head.id = POSITION_REPORT;
	send_location_gprs(flowwater_form_table,INQUIRE_POSITION_RESPONSION);
	gps_gate = 1;
}

//����MOTO��λ����˵���Ͷϵ綼ֻ��һ���ߣ�ƽֻ̨������·�Ŀ���
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
		//���ű���	
	   
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
//ljw20160910������һ�λ�ȡ��żȻ�Խϴ�		if(!GetAvailableGPSInfoV())
	{
        send_gps_gsm_flag = GSM_FLAG;
//ljw	  	cmd_0002();
		gps_gate = 1;
//ljw	  	return;
	}
    else
    {
        send_gps_gsm_flag = GPS_FLAG;
        ACC_GSM_flag = 0x00;//wei,20160825,��GSM���ͱ�־����
    }
	send_location_gprs(flowwater_form_table,OIL_BREAK_RESPONSION);
	gps_gate = 1;
}
// 7E 82 02 00 06 01 85 51 09 00 21 02 8E 00 0A 00 00 07 08 F2 7E 
//7E 82 02 00 06 01 85 51 09 00 21 02 89 00 0A 00 00 07 08 F5 7E 
//�������ٹ��ܣ�����ָ���ϴ�ʱ��������ָ��ʱ�䳤�ȣ��������ʱ��ͻ�ָ���֮ǰ��ʱ����
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



//��ϢID 0x0f00 ��Ϣ�� Ҫ�� ���� (1�ֽ�,1 GPS ; 2 С��)��С���Ļ� :����С����Ŀ��(1�ֽ�)С�����,С��ID;
//                                                     ���gps,ֻ�����Ⱥ�γ��,1�ֽڵ�״̬�ֽ�(����,��γ)
//

//�������յ����ݺ�,�϶��᷵��.�ն˿��ԶԷ������·������ݲ���Ӧ��
//�������ظ���ϢID 0x8f00 �������UCS2���������λ��


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
//ljw20160910������һ�λ�ȡ��żȻ�Խϴ�		if(!GetAvailableGPSInfoV())
	{
//ljw,20160918	  	cmd_0002();
	  	//��վ��λ��Ԥ����
 		gps_gate = 1;
        //wei,20160723��վ��ȡ����λ����Ϣ
    uploadLAC_InquireChinese();//wei,20160723,ʹ�û�վ
//wei,20160723��վ��ȡ����λ����Ϣ
		return;
	}
	head.attribute = 10; //
	//�����ߵ��ֽ�
	swapHLchar((u8*)&(head.attribute),sizeof (head.attribute));
	swapHLchar((u8*)&(head.id),sizeof (head.id));
	swapHLchar((u8*)&(head.serial_number),sizeof (head.serial_number));
	
	//g_ucGPRSDataBuff
	memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
	memcpy(g_ucGPRSDataBuff,(u8*)&head,sizeof(head));

	position.status = 0;

	// ���� GPS��λ
	//tmp = 1;
	g_ucGPRSDataBuff[sizeof(head)] = 1;
	
	//$GPRMC,,164819.000,A,3112.1940,N,11700.0000,W,049.0,000.0,151113,000.0,E*66
	// ����ʱ��
	i = (unsigned char *)strstr((char *)g_ucNeo6QBuff,",");
//ljw,20160902�޸���ǰ�Ĵ�������ݵĽ���	i++;
	//memcpy((char *)(tmp_time+6),(char *)(i+1),6);	// ����ʱ��
	i +=6;
	i = (unsigned char *)strstr((char *)i,",");

	//if('A' ==*(i+1) )
	{
	//	position.status |= EXCURSION_POSITION_STATUS_ORIENTATION; 
	}
	// ����GPS��Ϣ���ñ�־
	i = (unsigned char *)strstr((char *)(i+1),",");
	// ���� γ�ȣ�
	position.latitude = asciitolatitude(i+1);
	swapHLchar((u8*)&(position.latitude),sizeof (position.latitude));
	memcpy((u8*)&g_ucGPRSDataBuff[sizeof(head)+1],  (u8*)&(position.latitude),4 );
	i = (unsigned char *)strstr((char *)(i+1),",");
	if ('S' == *(i+1))
	{
		position.status |= EXCURSION_POSITION_STATUS_SOURTH; 
	}
	i = (unsigned char *)strstr((char *)(i+1),",");
	// ���� ���ȣ�
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


// 0X8F00    ��Ϣ�壺���ݳ��ȣ�2���ֽڣ���λ�����ݣ�N���ֽڣ�UNICODE���룩��
void cmd_CHINESE_POSITION_rece(void)
{	
	u16 chinese_position_num;
	u8* ptmp;
	u8 i,j;
	
	ptmp = &g_ucGPRSCmdBuff[12];
	chinese_position_num = (*ptmp++)*0xff; 	//�������ֽ���
//ljw,20160907 ����Ҫɾ    chinese_position_num = (*ptmp++)<<8; 	//�������ֽ���   //ljw,20160907 ����Ҫɾ
	chinese_position_num += (*ptmp++); 	//�������ֽ���

      //��������ѯʧ�ܾ�ֱ���˳�
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

	chinesesmsflag = 0;  // ���Ķ��Ŵ����־
	startchinesesms = 0;
}

void SetGPRS(void)
{
	u16 message_id; 
	u8 *ptmp;
	//u8 status=message_SUCCESS; 
	gprs_message_head_struct   tmp_head;

	if(analysis(g_ucGPRSCmdBuff, GPRSCmdCount) == 0)    //���鲻��ȷ��У��Ĺ����Ѿ�������ת����ȷ��
	{
		g_ucGPRSCmdMissionFlag = 0;
		//ClrGPRSCmdBuff();
		memset(g_ucGPRSCmdBuff,0,sizeof(g_ucGPRSCmdBuff));
		cmd_GENERAL_RESPONSION(message_ERROR);
		return;
	}
	#if 0
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
	#endif
	//tmp_head = (gprs_message_head_struct *)(&bigbuf.big.g_ucGPRSCmdBuff[1]);
	memcpy((u8*)&tmp_head, (u8 *)(&g_ucGPRSCmdBuff[0]),sizeof(tmp_head));//����Ϣͷ��ȡ����
		//�����ߵ��ֽ�
	swapHLchar((u8*)&(tmp_head.attribute),sizeof (tmp_head.attribute));
	swapHLchar((u8*)&(tmp_head.id),sizeof (tmp_head.id));
	//swapHLchar((u8*)&(tmp_head.serial_number),sizeof (tmp_head.serial_number)); //������Ҫ����ȥ���Ͳ�ת��
	if(tmp_head.serial_number == flowwater_form_table) //��ˮ�Ų���,  ������ж�tmp_head.serial_number�����ˮ������һ���ģ�������һ����壿�??
	{
		cmd_GENERAL_RESPONSION(message_ERROR);
		return;
	}
	flowwater_form_table = tmp_head.serial_number;//��ˮ��ֻ����һ���и�ֵ
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
			cmd_SET_GPRS_SERVICER(); //�о��������������Ѿ��ж�Ӧ��ָ���ˣ���������ɿտպ���
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
// ������ϢID�����־�������
	
	g_ucGPRSCmdMissionFlag = 0;
}
//����B562 ,�ܵ���������
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


//wei,20160827 ����0D 0A�Ӻ���
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

    IWDG_ReloadCounter();//ljw,20160906��ֹ����7E����
    while(proGPRSCmdFlag==1);//ljw,20160906��ֹ����7E����
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
//A-GPS ���ͻ����������ݳ���
//wei,zhu ����:��ȡAGPS���ݳɹ�������1�����򷵻�0
u16  AGPS_rec_len = 0;//ljw,20161125,�������յ���APGS�������ݵ��ܳ���
u8 test_a_gps(void)
{
    u32 i;
//    u8 *p1 = NULL;//ljw,20160827,APGS�����Ż�
//    u8 *p2 = NULL;//ljw,20160827,APGS�����Ż�
    u16 temp_len_end= 0;//ljw,20160827,APGS�����Ż�
//    u16 temp_len_0d = 0;//ljw,20160827,APGS�����Ż�
    u16 j = 0;//ljw,20160827,APGS�����Ż�
    u16 offset = 0;//ljw,20161202,����ָ�뷢�ͽ��յ���AGPS���ݵı��˵�ַ
    u8 get_GC65_lat[get_GC65_lat_len+1] = {0};
    u8 get_GC65_lon[get_GC65_lon_len+1] = {0};

#ifdef ZHONGKEWEI_GPS //ljw,20161125,�п�΢��GPSģ��
    
//    u8 temp_send_AGPS[100] = "\"cmd=full;user=miccozhu@163.com;pwd=Fht1230;";
//    srtcat(temp_send_AGPS);
	delayms(2000);
	//g_ucGPRSDataBuff
	//memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
    //wei,20160827   cmd=full;cmd=eph;

    if(!get_lac_gps(get_GC65_lat,get_GC65_lon))//ljw,20160903����GC65�Ƿ��ܻ�þ�γ��
        get_lac_gps(get_GC65_lat,get_GC65_lon);
    //delayms(2000);//ljw,20160907 ����GC65�Ƿ��ܻ�þ�γ��
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
#endif  //ljw,20161125,�п�΢��GPSģ��
    
#ifdef UM220_GPS  //ljw,20161125,UM220
#if 0 //ljw,20161202,���ԭ����ʹ�ò������ϱ����ݵ�
    in_agps = 0x5a;//ljw,20161128,����APGS֮ǰ���GSM����
    for(j=0;j<10;j++)//ljw,20161128,����APGS֮ǰ���GSM����
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
        if(!get_lac_gps(get_GC65_lat,get_GC65_lon))//ljw,20160903����GC65�Ƿ��ܻ�þ�γ��
            get_lac_gps(get_GC65_lat,get_GC65_lon);
        //delayms(2000);//ljw,20160907 ����GC65�Ƿ��ܻ�þ�γ��
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
//wei,20160907 ����Ҫɾ	SendGprs_Index(strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n"),(u8)AGPS_SERVERS);//wei
	if(strlen(g_ucGPRSDataBuff) <150)//ljw,20160907�ӳ����ж�
        SendGprs_Index(strlen(g_ucGPRSDataBuff),(u8)AGPS_SERVERS);//wei
    else //ljw,20160907�ӳ����ж�
    {
        memset(g_ucGPRSDataBuff,0x00,sizeof(g_ucGPRSDataBuff));//ljw,2060907
        bit_flag &=(~(0x03<<0));//(~(0b00000011<<0));
        return 0;
    }
    bit_flag &=(~(0x03<<0));//(~(0b00000011<<0));
#if 0

//���յ������ݸ�ʽ����
//+QIRDI: 0,1,0  //��ʾ�յ���GPRS����

//+QIRD: 195.34.89.144:46434,UDP,200

//u-blox a-gps server (c) 1997-2009 u-blox AG
//Content-Length: 1144

//Content-Type: application/ubx��������//144������

//�b0�l?&�����hb1h?�I9?
//+QIRD: 195.34.89.144:46434,UDP,200
//	9}�IG�R?���@9G0�b1h?�I?(ǵb1h

//+QIRD: 195.34.89.144:46434,UDP,200
//��
//��
//~�I??I?��??.???+QIRD: 195.34.89.144:46434,UDP,200
//�b1h?�I9?\x�?

//+QIRD: 195.34.89.144:46434,UDP,200
//'?�b1h?T�b1h
//+QIRD: 195.34.89.144:46434,UDP,200
//�E��?�I
//j�bH
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

#ifdef ZHONGKEWEI_GPS //ljw,20161125,�п�΢��GPSģ��    
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
                        temp_len_end = (u8)(agps_len_to_send-(p1-g_ucGPRSCmdBuffback));//wei,20160827������Ƴ���
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
#endif //ljw,20161125,�п�΢��GPSģ�� 
    
#ifdef UM220_GPS  //ljw,20161125,UM220
    for(i=15000;i>0;i--)
    {
#if 0 //2800
        if(fB5 ==AGPS_RE_END)
        {
            temp_len_0d = GPRSCmdCount;
            if(temp_len_0d < 500) //���յ�������̫�̣���Ϊ�д���
            {
                in_agps = 0;
                return 0;
            }
            ddi_uart_Write(&UART2_ctl_t,(const u8*)g_ucGPRSCmdBuff,temp_len_0d);        
            fB5 = 0;
        }
#endif       
#if 0 //ljw,20161202,���ԭ����ʹ�ò������ϱ����ݵ�
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
#if 1//ljw,20161126����һ
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
#endif //ljw,20161126 ����һ        
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
    u8 *p1 = NULL;//ljw,20160827,APGS�����Ż�
    u8 *p2 = NULL;//ljw,20160827,APGS�����Ż�
    u16 temp_len_end= 0;//ljw,20160827,APGS�����Ż�
    u16 temp_len_0d = 0;//ljw,20160827,APGS�����Ż�
    u16 j = 0;//ljw,20160827,APGS�����Ż�
    
    u8 get_GC65_lat[get_GC65_lat_len+1] = {0};
    u8 get_GC65_lon[get_GC65_lon_len+1] = {0};
//    u8 temp_send_AGPS[100] = "\"cmd=full;user=miccozhu@163.com;pwd=Fht1230;";
//    srtcat(temp_send_AGPS);
	delayms(2000);
	//g_ucGPRSDataBuff
	//memset (g_ucGPRSDataBuff,0,sizeof(g_ucGPRSDataBuff));
    //wei,20160827   cmd=full;cmd=eph;
    
    if(!get_lac_gps(get_GC65_lat,get_GC65_lon))//ljw,20160903����GC65�Ƿ��ܻ�þ�γ��
        get_lac_gps(get_GC65_lat,get_GC65_lon);
    //delayms(2000);//ljw,20160907 ����GC65�Ƿ��ܻ�þ�γ��
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
//wei,20160907 ����Ҫɾ	SendGprs_Index(strlen("cmd=full;user=miccozhu@163.com;pwd=Fht1230;lat=31.84;lon=117.29;pacc=1000\n"),(u8)AGPS_SERVERS);//wei
	if(strlen(g_ucGPRSDataBuff) <150)//ljw,20160907�ӳ����ж�
        SendGprs_Index(strlen(g_ucGPRSDataBuff),(u8)AGPS_SERVERS);//wei
    else //ljw,20160907�ӳ����ж�
    {
        memset(g_ucGPRSDataBuff,0x00,sizeof(g_ucGPRSDataBuff));//ljw,2060907
        bit_flag &=(~(0x03<<0));//(~(0b00000011<<0));
        return 0;
    }
    bit_flag &=(~(0x03<<0));//(~(0b00000011<<0));
#if 0

//���յ������ݸ�ʽ����
//+QIRDI: 0,1,0  //��ʾ�յ���GPRS����

//+QIRD: 195.34.89.144:46434,UDP,200

//u-blox a-gps server (c) 1997-2009 u-blox AG
//Content-Length: 1144

//Content-Type: application/ubx��������//144������

//�b0�l?&�����hb1h?�I9?
//+QIRD: 195.34.89.144:46434,UDP,200
//	9}�IG�R?���@9G0�b1h?�I?(ǵb1h

//+QIRD: 195.34.89.144:46434,UDP,200
//��
//��
//~�I??I?��??.???+QIRD: 195.34.89.144:46434,UDP,200
//�b1h?�I9?\x�?

//+QIRD: 195.34.89.144:46434,UDP,200
//'?�b1h?T�b1h
//+QIRD: 195.34.89.144:46434,UDP,200
//�E��?�I
//j�bH
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
                        temp_len_end = (u8)(agps_len_to_send-(p1-g_ucGPRSCmdBuffback));//wei,20160827������Ƴ���
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
		//??????�̨���??��
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

//ljw,20160906�ַ������Һ������ӳ����ж�
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
//ljw,20160908 test reset flag u8 erro_flag = 0;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
position_message_struct Deal_GSM_LAC(position_message_struct position_in)
//ljw,20160906����Ҫɾu8 Deal_GSM_LAC(void)//ljw,20160906����Ҫɾ 
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
    
    get_ID();//ljw,20160918,ʹ���Ӻ�����ȡС��ID
    position.latitude = Lac_g;
    position.longitude = LacId_g;
//ljw,20160908 test reset flag    erro_flag =0;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
#ifdef GSM_ID_OLD //ljw,20160918,ʹ���Ӻ�����ȡС��ID    
	ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r",9);
	WaitGSMOK1();      //ljw20160903for space   WaitGSMOK();
	IWDG_ReloadCounter();
	ptmp = (unsigned char *)strstr((char *)g_ucSim900Buff,"\"");
//ljw,20160908 test reset flag    erro_flag =1;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	ptmp += 1;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
//ljw,20160908 test reset flag    erro_flag =2;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len<=8) //ljw,20160906�ӳ����ж�
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
//ljw,20160908 test reset flag    erro_flag =3;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	ptmp += 2;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
//ljw,20160908 test reset flag	erro_flag =4;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len<=8)
    {
        memcpy(tmpbuf,ptmp,len);
//ljw,20160908 test reset flag        erro_flag =5;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
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
#endif	//ljw,20160918,ʹ���Ӻ�����ȡС��ID
    
//wei,20160822	head.id = MOBILE_LOCATION_SEND;
//wei,20160822	head.serial_number = flowwater++;
//wei,20160822	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
//wei,20160822	head.attribute = 12;

//	tmpMobileCompany = MobileCompany;
    position.speed = (u16)MobileCompany;//wei,20160822
    position.altitude = 0x0000;
    position.direction = 0x0000;
    RTC2Position_time(&position.time[0]);//wei,20160825,ȡϵͳ��RTCʱ��
//ljw,20160908 test reset flag    erro_flag = 6;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
//    position.time[0] = RTC_ByteToBcd2((u8)(timer.w_year-2000));
//    position.time[1] = RTC_ByteToBcd2(timer.w_month);
//    position.time[2] = RTC_ByteToBcd2(timer.w_date);
//    position.time[3] = RTC_ByteToBcd2(timer.hour);
//    position.time[4] = RTC_ByteToBcd2(timer.min);
//    position.time[5] = RTC_ByteToBcd2(timer.sec);
//ljw,20160906����Ҫɾ    return position;
    return position;

#if 0  
		//??????�̨���??��
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


//wei,20160918,for �ϴ�С���Ż�ȡ����λ��
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

    get_ID();//ljw,20160918,ʹ���Ӻ�����ȡС��ID
    Lac = Lac_g;
    LacId = LacId_g;
#ifdef GSM_ID_OLD //ljw,20160918,ʹ���Ӻ�����ȡС��ID
	ddi_uart_Write(&UART1_ctl_t,"AT+CREG?\r",9);
    WaitGSMOK1();
//ljw,20160918	WaitGSMOK();
	IWDG_ReloadCounter();
	ptmp = (unsigned char *)strstr((char *)g_ucSim900Buff,"\"");
	ptmp += 1;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len>8)//ljw,20160918����Ҫɾ
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
    if(len>8)//ljw,20160918����Ҫɾ
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
#endif	//ljw,20160918,ʹ���Ӻ�����ȡС��ID
	head.id = CHINESE_POSITION_send;//wei,20160723,  0x0f88
	head.serial_number = flowwater++;
	memcpy((u8*)head.mbphonenum,(u8*)system_para.g_ucDeviceID,sizeof(system_para.g_ucDeviceID));
	head.attribute = 13;//wei,20160723,��Ϣ������ ����1+4+4+4

	tmpMobileCompany = MobileCompany;
		//??????�̨���??��
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
	
    *ptmp = 0x02; //wei,20160723, 1-GPS��2-С��λ��
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


//ljw,20160918,��ȡGSMС��ID
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
//ljw,20160908 test reset flag    erro_flag =1;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	ptmp += 1;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
//ljw,20160908 test reset flag    erro_flag =2;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len<=8) //ljw,20160906�ӳ����ж�
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
//ljw,20160908 test reset flag    erro_flag =3;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	ptmp += 2;
	p1 = (unsigned char *)strstr((char *)ptmp,"\"");
//ljw,20160908 test reset flag	erro_flag =4;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = p1-ptmp;
    if(len<=8)
    {
        memcpy(tmpbuf,ptmp,len);
//ljw,20160908 test reset flag        erro_flag =5;//ljw,20160906,���Ը�λ�����־��   ����Ҫɾ
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



