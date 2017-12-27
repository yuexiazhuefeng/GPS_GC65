/***************************************************************************
 * @Filename        -  
 * 
 * Copyright ?zzg Technology Ltd, 2015. All rights reserved.
 * 
 * @Brief           - 
 *      This file 
 *      
 * 
 * 											   
 * @Revision        - 1.1.1.1 $
 * @Date            - 2015/03/11 11:50:35 $
 * @Author          - zzg $
 * @Project         - 
 *
 *
 * @Revision History- 20150514 12:00发送位置的改好了其它短信的处理还要继续搞，短信的接收，解析
 * @Log             - 　20150515: at first LED all flash .串口设置时不能勾选发送新航
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
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "led.h"
#include "SmsCmd.h"
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

extern const u8 DEVICEID[];
extern const u8 PASSWD[];
extern const u8 IPADDR[];  //"218.22.32.154"//	"220.178.0.58"
extern const u8 IPPORT[];  //"4095" (公司老协议 ) //"6000" //	9668(部标协议)
extern  const unsigned char  g_ucVERSION[18] ;

extern unsigned char g_ucGPRSCmdBuff[GPRS_BUFF_SIZE];	// GPRS 命令缓冲区


extern unsigned char g_ucGPRSCmdBuffback[GPRS_BUFF_SIZE];
void PrintATF(void)
{
	u8 tmp_sn[12];


	ddi_uart_Write(&UART2_ctl_t,"\r\nIP:",strlen("\r\nIP:")); 
	//delayms(50);
	ddi_uart_Write(&UART2_ctl_t,system_para.g_ucIPAddress,strlen(system_para.g_ucIPAddress));
	//delayms(100);
	ddi_uart_Write(&UART2_ctl_t,"\r\n端口:",strlen("\r\n端口:")); 
	//delayms(50);
	ddi_uart_Write(&UART2_ctl_t,system_para.g_ucUDPPort,strlen(system_para.g_ucUDPPort));
	//delayms(100);
	ddi_uart_Write(&UART2_ctl_t,"\r\nMID:",strlen("\r\nMID:")); 
	//delayms(50);
	//void hextoascii(unsigned char * srcdata,unsigned char * decdata,unsigned short int len)
	hextoascii(system_para.g_ucDeviceID,tmp_sn,6);
	ddi_uart_Write(&UART2_ctl_t,&tmp_sn[1],11);
	//delayms(100);
	ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
	//delayms(20);
}

//ljw,20160928
void ddi_send_Number_registed_Failed(void)
{
    ddi_uart_Write(&UART2_ctl_t,"The Specified Number registed Failed!\r\n",40);
}

void InitialSetting(void)
{
	unsigned char temp[21] = {'*',',','o','k','#',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char i,j;
	//unsigned char *buf;
	unsigned char tmp[16];
	unsigned char setflag = 0;
	unsigned char *  t1;
	unsigned char tmp_sn[12];
	unsigned char smsbuf[160];
    static unsigned char paraset_flag = 0;//wei,20160819,替换while(g_ucInitSettingFlag == 1)中的g_ucInitSettingFlag
    u32 temp_InitialSetting_timer = 0;


	ddi_uart_Write(&UART2_ctl_t,(u8*)g_ucVERSION,17);
	//ddi_uart_Write(&UART2_ctl_t,"版本发布日期 :2015年09月07日",sizeof("版本发布日期 :2015年08月27日"));
	//delayms(500); //wait for send end
	ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
	ddi_uart_Write(&UART2_ctl_t,"udp setup command support\r\n",strlen("udp setup command support\r\n")); 
	ddi_uart_Write(&UART2_ctl_t,"udp dns support\r\n",strlen("udp dns support\r\n")); 
	ddi_uart_Write(&UART2_ctl_t,"ring power wakeup support\r\n",strlen("ring power wakeup support\r\n")); 	
	ddi_uart_Write(&UART2_ctl_t,"Loading user's settings...\r\n",strlen("Loading user's settings...\r\n")); 

	PrintATF();
	ddi_uart_Write(&UART2_ctl_t,"user settings initialized\r\n",strlen("user settings initialized\r\n")); 
	ddi_uart_Write(&UART2_ctl_t,"Press '%%%%%%%%%%' to enter the setup mode\r\n",
		                        strlen("Press '%%%%%%%%%%' to enter the setup mode\r\n")); 
	//..............................
	//ddi_uart_Write(&UART2_ctl_t,"Modem power uping",strlen("Modem power uping")); 

    EN_initialize = 1;
	//if(g_ucSysInitialized == SYS_UNINITI)
	//改成了什么样都可以设置参数
	if(1)
	{
		for(i=0; i<40; i++)    // 50 
		{
			delayms(100);
			ddi_uart_Write(&UART2_ctl_t,".",1);
			memcpy(g_ucGPRSCmdBuff,g_ucGPRSCmdBuffback,40);
			if (strstr((char *)g_ucGPRSCmdBuff, "%%%%%%%%%%"))
			{
                paraset_flag = 0x5a;
				ddi_uart_Write(&UART2_ctl_t,"\r\nEnter system set mode,please set them!!!\r\n",strlen("\r\nEnter system set mode,please set them!!!\r\n")); 
				break;
			}
		}		

		//ddi_uart_Write(&UART2_ctl_t,"%%%%%%%%%%",10); 		// 下位机发送同步字符 上位机收到该字符同样下发该字符 
		//delayms(500);		// 下位机收到该字符后将 g_ucInitSettingFlag 标志置位

//wei,20160819,		while(g_ucInitSettingFlag == 1)	// 下位机在检测到该标志后 进入初始设置循环 
        temp_InitialSetting_timer = secondcounter;//wei,20160819,参数设置定时器
        while(paraset_flag == 0x5a)
		{
            ////wei,20161221 ddi_uart_Write(&UART2_ctl_t,"TEST USART2...OK",strlen("TEST USART2...OK"));//wei,20161221 
			IWDG_ReloadCounter();
            if(secondcounter-temp_InitialSetting_timer >300)//wei,20160819,参数设置定时器 5min
            {
                g_ucInitSettingFlag = 0;
                paraset_flag = 0;
                break ;
            }
			memcpy(g_ucGPRSCmdBuff,g_ucGPRSCmdBuffback,sizeof(g_ucGPRSCmdBuffback));
			//delayms(100);

			if (strstr((char *)g_ucGPRSCmdBuff, "%%%%%%%%%%"))
			{
				g_ucGPRSCmdMissionFlag = 1;
				setflag = 1;//通过该临时变量，在该子函数末尾判断是否要清除g_ucGPRSCmdBuffback[]
			}
			else if(g_ucGPRSCmdMissionFlag == 1)
			{
				if(strstr((char *)g_ucGPRSCmdBuff,"%")||(strstr((char *)g_ucGPRSCmdBuff,"<")))
				{
					//delayms(100); //wait for send end
					delayms(200); //wait for send end
                    memcpy(g_ucGPRSCmdBuff,g_ucGPRSCmdBuffback,200);
					if (strstr((char *)g_ucGPRSCmdBuff,"<SETM*"))	
					{
						memcpy(g_ucSim900Buff,g_ucGPRSCmdBuffback,200);
						Cmd_SETM();
						memset (smsbuf,0,sizeof(smsbuf));
						sub_cxcs(smsbuf);
						delayms(10);
                        
						ddi_uart_Write(&UART2_ctl_t,smsbuf,strlen(smsbuf));
						//delayms(100);
						ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"<CXCS>"))	
					{
						memset (smsbuf,0,sizeof(smsbuf));
						sub_cxcs(smsbuf);
						ddi_uart_Write(&UART2_ctl_t,smsbuf,strlen(smsbuf));
						//delayms(100);
						ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"ID"))
					{
						hextoascii(system_para.g_ucDeviceID,tmp_sn,6);
						ddi_uart_Write(&UART2_ctl_t,&tmp_sn[1],11);
						//delayms(100);
						ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
						//delayms(20);
  				 	    
					     setflag = 1;
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,",2D,"))	
					{
                        t1 = (unsigned char*  )(strstr((char *)g_ucGPRSCmdBuff,"%") + 15+5);
                        //i = t1-g_ucGPRSCmdBuff;
                        //判断号码是否有效
						for(i =0; i<11; i++)
                        {
                            if ((*(t1+i)>'9') ||(*(t1+i)<'0'))
                            {
                                //delayms(100); //wait for send end
                                setflag = 1;
                                break;
                            }
                        }
						if (i < 11)
						{
							ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
							setflag = 1;							
						}  
						else
						{
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

					         //sys_Init();
						  	hextoascii(system_para.g_ucDeviceID,tmp_sn,6);
							ddi_uart_Write(&UART2_ctl_t,&tmp_sn[1],11);
						  //delayms(100);
						      ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
						  // delayms(20); //wait for send end
						   setflag = 1;
						}
					}

					if(strstr((char *)g_ucGPRSCmdBuff,"SR"))
					{
                        t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"SR") + 2;
						if ('?' == *t1)
						{
							ddi_uart_Write(&UART2_ctl_t,system_para.g_ucIPAddress,strlen(system_para.g_ucIPAddress));
	  				 	    ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n"));
						}
						else
						{
							//清空原IP地址
							//
							i = strlen((char *)g_ucGPRSCmdBuff) -3;  //%SR 计算IP地址长度
//WEI							if((i>16) ||(i<7))
							if((i>15) ||(i<7))//WEI
							{
								ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
								//delayms(100); //wait for send end
								setflag = 1;
							}
							else
							{
								for(j=0 ; j<IPADD_LEN ; j++)//wei for(j=0 ; j<16 ; j++)
								{
								//wei	system_para.g_ucIPAddress[i] = 0;
									system_para.g_ucIPAddress[j] = 0;
								}
								memcpy((char *)system_para.g_ucIPAddress,(char *)(g_ucGPRSCmdBuff+3),i);
								//IIC_MWriteData(EEP_IP_ADDR,system_para.g_ucIPAddress,16);
								Store_System_Para();
								//sys_Init();							
								ddi_uart_Write(&UART2_ctl_t,"IP set ok!\r\n",strlen("IP set ok!\r\n"));
							}	
						}
						setflag = 1;						
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"PT"))
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"PT")+1])
                                                t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"PT") + 2;
						if ('?' == *t1)
						{
							  if(strlen(system_para.g_ucUDPPort) <= sizeof(system_para.g_ucUDPPort))
								  ddi_uart_Write(&UART2_ctl_t,system_para.g_ucUDPPort,strlen(system_para.g_ucUDPPort));
							  else
							  	ddi_uart_Write(&UART2_ctl_t,system_para.g_ucUDPPort,sizeof(system_para.g_ucUDPPort));
					
							//delayms(500); //wait for send end
	  				 	        ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
						}
						else
						{
							//判断号码是否有效
							j = strlen((char *)g_ucGPRSCmdBuff) -3;  // 计算端口长度
							for(i =0; i<j; i++)
						      {
								if ((*(t1+i)>'9') ||(*(t1+i)<'0'))
								{
									//ddi_uart_Write(&UART2_ctl_t,"Wrong number!",strlen("Wrong number!"));
									//delayms(100); //wait for send end
									setflag = 1;
									break;
									//continue;
								}
	
									
						      }
							if ((i < 4) ||(i>5))
							{
								ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
								setflag = 1;
							} 
							else
							{
								//清空原端口
								//
								for(i=0 ; i<5 ; i++)
								{
								   system_para.g_ucUDPPort[i] = 0;
								}
								strncpy((char *)system_para.g_ucUDPPort,(char *)(g_ucGPRSCmdBuff+3),i);
								//IIC_MWriteData(EEP_UDP_PORT,system_para.g_ucUDPPort,5);
								Store_System_Para();
								//sys_Init();							
								
								ddi_uart_Write(&UART2_ctl_t,"PORT set ok!\r\n",strlen("PORT set ok!\r\n"));
							}	
						}
						setflag = 1;
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"EXCLUSIVE_IP")) //FHTIP 5A: 设置成固定FHT的ｉｐ，fhtip 00:随意IP
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"HT")+1])
						//THIS IS A ERROR ,system_para.g_usGPRSUpLoadAternationTime IS NOT g_usGPRSUpLoadTime
                                      t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"EXCLUSIVE_IP") + 12;
		#if 0				

	#else
	                                //system_para.g_usGPRSUpLoadAternationTime
						if ('?' == *t1)
						{
							if(FHTIP != system_para.g_ucSysMode_S)
							{
							//delayms(500); //wait for send end
  				 	                   ddi_uart_Write(&UART2_ctl_t,"NOT EXCLUSIVE_IP!\r\n",strlen("NOT EXCLUSIVE_IP!\r\n")); 							
							}
							else
							{
								 ddi_uart_Write(&UART2_ctl_t,"EXCLUSIVE_IP!!! \r\n",strlen("EXCLUSIVE_IP!!! \r\n")); 			
							}
						}
						else
						{
							if (('5'==*t1) && ('A' == *(t1+1)))
							{
								system_para.g_ucSysMode_S = 0x5a;

							}
							else 
							if((('0'==*t1) && ('0' == *(t1+1))))
							{
								system_para.g_ucSysMode_S = 0;
							}							
							//IIC_MWriteData(EEP_GPRSUpLoadAternationTime,(u8 *)(&system_para.g_usGPRSUpLoadAternationTime),2);
							Store_System_Para();

							//sys_Init();							
							if(FHTIP != system_para.g_ucSysMode_S)
							{
							//delayms(500); //wait for send end
  				 	                   ddi_uart_Write(&UART2_ctl_t,"NOT EXCLUSIVE_IP mode set ok!\r\n",strlen("NOT EXCLUSIVE_IP mode set ok!\r\n")); 							
							}
							else
							{
								 ddi_uart_Write(&UART2_ctl_t,"EXCLUSIVE_IP mode set ok!\r\n",strlen("EXCLUSIVE_IP mode set ok!\r\n")); 			
							}
							
						}
						setflag = 1;
	
	#endif
					}
					
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"HT"))
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"HT")+1])
						//THIS IS A ERROR ,system_para.g_usGPRSUpLoadAternationTime IS NOT g_usGPRSUpLoadTime
                                      t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"HT") + 2;
		#if 0				

	#else
	                                //system_para.g_usGPRSUpLoadAternationTime
						if ('?' == *t1)
						{
							if (system_para.HeartAternationTime>999)
							{
							     system_para.HeartAternationTime = 999;
							}
							//if (system_para.g_usGPRSUpLoadAternationTime/100)
							tmp[0] = 	system_para.HeartAternationTime/100 + '0';
							tmp[1] = ((system_para.HeartAternationTime%100) /10 ) + '0';
							tmp[2] = ((system_para.HeartAternationTime%10)) + '0';
							
							ddi_uart_Write(&UART2_ctl_t,tmp,3);
							//delayms(500); //wait for send end
  				 	        ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n"));
						}
						else
						{
							j = strlen((char *)g_ucGPRSCmdBuff) -3;  // 计算端口长度
							for(i =0; i<j; i++)
                            {
                                if ((*(t1+i)>'9') ||(*(t1+i)<'0'))
                                {
                                    ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
                                    //delayms(100); //wait for send end
                                    setflag = 1;
                                    break;
                                    //continue;
                                }
                            }
							
							system_para.HeartAternationTime = 0;
							for(i=0; i<3; i++)
							{
							    system_para.HeartAternationTime *= 10;
								system_para.HeartAternationTime += *(t1+i) -'0';
							}	
							 if(system_para.HeartAternationTime <10) //wei,20160603强制心跳间隔不能小于10s
				                        system_para.HeartAternationTime = 10; //wei,20160603
							 if(system_para.HeartAternationTime >300) //wei,20160603强制心跳间隔不能小于10s
				                        system_para.HeartAternationTime = 300; //wei,20160603
							//IIC_MWriteData(EEP_GPRSUpLoadAternationTime,(u8 *)(&system_para.g_usGPRSUpLoadAternationTime),2);
							Store_System_Para();
							//sys_Init();							
							
							//delayms(500); //wait for send end
							ddi_uart_Write(&UART2_ctl_t,"HT set ok!\r\n",strlen("HT set ok!\r\n"));

						}
						setflag = 1;	
	#endif
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"VER"))
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"VER")+1])
                        t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"VER") + 3;
						if('?' == *t1)
						{
							ddi_uart_Write(&UART2_ctl_t,(u8*)g_ucVERSION,17);
							//delayms(500); //wait for send end
	  				 	        ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
							setflag = 1;
						}
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"ATF"))
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"ATF")+1])
                        t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"ATF") + 3;
						if ('?' == *t1)
						{
							PrintATF();		
						}
						else
						{
							PrintATF();
							//Cmd_Initial();
						}
						setflag = 1;	
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"AP"))
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"AP")+1])
                        t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"AP") + 2;
						if ('?' == *t1)
						{
							ddi_uart_Write(&UART2_ctl_t,system_para.g_ucNetAPN,sizeof (system_para.g_ucNetAPN));
							//delayms(500); //wait for send end
                            ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
							setflag = 1;
						}
						else
						{
							i = strlen((char *)g_ucGPRSCmdBuff) -3;  // 计算APN长度
//wei							if ((i>20) ||(i<3))
//wei							if ((i>19) ||(i<3)) //wei
                            if ((i>NETAPN_LEN-1) ||(i<3)) //wei
							{
								ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
									//delayms(100); //wait for send end
								setflag = 1;
							}
							else	
							{	//清空原端口
								//
								memset(system_para.g_ucNetAPN,0,NETAPN_LEN);
								strncpy((char *)system_para.g_ucNetAPN,(char *)(g_ucGPRSCmdBuff+3),i);
								//IIC_MWriteData(EEP_IP_APN,system_para.g_ucNetAPN,sizeof(system_para.g_ucNetAPN));
								Store_System_Para();
								//sys_Init();									
								ddi_uart_Write(&UART2_ctl_t,"AP set ok!\r\n",strlen("AP set ok!\r\n"));
							}
						}
					}
//wei,20160706for ANP: USERNAME USERPASSWORD <
                    else
					if (strstr((char *)g_ucGPRSCmdBuff,"UN"))//wei,20160706,USERNAME (GPRS)
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"AP")+1])
                        t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"UN") + 2;
						if ('?' == *t1)
						{
							//system_para.g_ucNetUser
                            ddi_uart_Write(&UART2_ctl_t,system_para.g_ucNetUser,sizeof (system_para.g_ucNetUser));
							//delayms(500); //wait for send end
                            ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
							setflag = 1;
						}
						else
						{
							i = strlen((char *)g_ucGPRSCmdBuff) -3;  // 计算APN长度
//wei							if ((i>20) ||(i<3))
//wei							if ((i>19) ||(i<3)) //wei
                            if ((i>NETUSER_LEN-1) ||(i<3)) //wei
							{
								ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
									//delayms(100); //wait for send end
								setflag = 1;
							}
							else	
							{	//清空原端口
								//
								memset(system_para.g_ucNetUser,0,NETUSER_LEN);
								strncpy((char *)system_para.g_ucNetUser,(char *)(g_ucGPRSCmdBuff+3),i);
								//IIC_MWriteData(EEP_IP_APN,system_para.g_ucNetAPN,sizeof(system_para.g_ucNetAPN));
								Store_System_Para();
								//sys_Init();									
								ddi_uart_Write(&UART2_ctl_t,"UN set ok!\r\n",strlen("UN set ok!\r\n"));
							}
						}
					}
                    else
					if (strstr((char *)g_ucGPRSCmdBuff,"PW"))//wei,20160706,NET PSE(GPRS)
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"AP")+1])
                        t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"PW") + 2;
						if ('?' == *t1)
						{
							//system_para.g_ucNetUser
                            ddi_uart_Write(&UART2_ctl_t,system_para.g_ucNetPsw,sizeof (system_para.g_ucNetPsw));
							//delayms(500); //wait for send end
                            ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
							setflag = 1;
						}
						else
						{
							i = strlen((char *)g_ucGPRSCmdBuff) -3;  // 计算APN长度
//wei							if ((i>20) ||(i<3))
//wei							if ((i>19) ||(i<3)) //wei
                            if ((i>NETPSW_LEN-1) ||(i<3)) //wei
							{
								ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
									//delayms(100); //wait for send end
								setflag = 1;
							}
							else	
							{	//清空原端口
								//
								memset(system_para.g_ucNetPsw,0,NETPSW_LEN);
								strncpy((char *)system_para.g_ucNetPsw,(char *)(g_ucGPRSCmdBuff+3),i);
								//IIC_MWriteData(EEP_IP_APN,system_para.g_ucNetAPN,sizeof(system_para.g_ucNetAPN));
								Store_System_Para();
								//sys_Init();									
								ddi_uart_Write(&UART2_ctl_t,"PW set ok!\r\n",strlen("PW set ok!\r\n"));
							}
						}
					}
//wei,20160706for ANP: USERNAME USERPASSWORD >

					else
#if 0						
					if (strstr((char *)g_ucGPRSCmdBuff,"moto"))
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"AP")+1])
                                                t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"moto") + 5;
						if ('?' == *t1)
						{

							if (!power_mode)
								ddi_uart_Write(&UART2_ctl_t,"electric bike",sizeof ("electric bike"));
							else
								ddi_uart_Write(&UART2_ctl_t,"motorcycle",sizeof ("motorcycle"));
							//delayms(500); //wait for send end
  				 	             ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 
							setflag = 1;
						}
						else
						{
                                             if ('0' == *t1)
								power_mode = 0;  // 电动车
							else if ('1' == *t1)
								power_mode = 0x5a;
							//IIC_MWriteData(EEP_POWER_MODE,&power_mode,sizeof(power_mode));
							ddi_uart_Write(&UART2_ctl_t,"moto set ok!\r\n",strlen("moto set ok!\r\n"));


						}
						
				
	
					}					
					
					else
#endif						
					if (strstr((char *)g_ucGPRSCmdBuff,"OT"))
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"OT")+1])
                                                t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"OT") + 2;
						if ('?' == *t1)
						{
							//ddi_uart_Write(&UART2_ctl_t,"CMNET",strlen("CMNET"));
							setflag = 1;
						}
						else
						{

							ddi_uart_Write(&UART2_ctl_t,"OT set ok\r\n",strlen("OT set ok\r\n"));
  				 	               // ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n")); 

						}
				
	
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"RC"))
					{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"RC")+1])
                                                t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"RC") + 2;

						g_ucInitSettingFlag = 0;
                        paraset_flag = 0;//wei,20160819,替换while(g_ucInitSettingFlag == 1)中的g_ucInitSettingFlag
						system_para.g_ucSysInitialized = SYS_INITIED;
						Store_System_Para();
						ddi_uart_Write(&UART2_ctl_t,"Enter application!!\r\n",strlen("Enter application!!\r\n"));
						//delayms(500); //wait for send end
						//return;
						setflag = 1;
						break;
						 
						/*
						if ('?' == *t1)
						{
							//ddi_uart_Write(&UART2_ctl_t,"CMNET",strlen("CMNET"));
							setflag = 1;
						}
						*/	
					}
					
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"RST"))
					{

					//把其它数据写成默认值

						// 系统参数初始化
						system_para.g_ucSysInitialized = SYS_INITIED;

						Store_System_Para();
						sys_Init();
                        
                        GSM_Power(FALSE); //wei,20160608看门狗复位前，给GSM断电
//ljw,20160908提高效率                        while(1); //等看门狗复位系统// wait for watch dog rst	
 					    for(;;);
						setflag = 1;
					}
					else
					if (strstr((char *)g_ucGPRSCmdBuff,"CSQ"))
					{
						//NUM		
						setflag = 1;
	
					}
                    //ljw,20160927 for wulian
                    else
					if(strstr((char *)g_ucGPRSCmdBuff,"NUM"))
					{
                        setflag = 1;
                        if(strstr((char *)g_ucGPRSCmdBuff,"%DELNUM"))//ljw zhu,20160927 没有删除指令，是设置指令
                        {
                            t1 = strstr((char *)g_ucGPRSCmdBuff,"%DELNUM");                            
                            if(GPRSCmdCount == strlen(t1))
                            {
                                for(i=0 ; i<2 ; i++)  
                                {
                                    if((GPRSCmdCount != 7)&&(!strncmp((char *)system_para.g_ucRegNum[i]+2,(char *)(t1+7),GPRSCmdCount-7)))  // 在授权号码中找到要删除的号码
                                    {                                        
                                        memset((u8 *)(&system_para.g_ucRegNum[i]),0x00,sizeof(system_para.g_ucRegNum[i]));
                                        system_para.g_ucRegNum[i][0] = 'e';//ljw ,20160927 strncpy((char *)system_para.g_ucRegNum[i],"e   00000000000",15);        // 删除内存中授权号码
                                        Store_System_Para();
                                        //sys_Init();                                        
                                        if(system_para.g_ucRegNum[i][0] == EMPTY)
                                        {
                                            ddi_uart_Write(&UART2_ctl_t,"The Specified Register Number has been delete Successful!\r\n",59);
                                        }
                                        else
                                        {
                                            memset((u8 *)(&system_para.g_ucRegNum[i]),0x00,sizeof(system_para.g_ucRegNum[i]));
                                            system_para.g_ucRegNum[i][0] = 'e';
                                            //ljw,20160927 for wulian strncpy((char *)system_para.g_ucRegNum[i],"e   00000000000",15);        // 删除内存中授权号码
                                            ddi_uart_Write(&UART2_ctl_t,"The Specified Register Number has been delete Fail!\r\n",53);
                                            //delayms(500); //wait for send end
                                        }
                                        break;
                                    }
                                }
                                if(i>=2)
                                   ddi_uart_Write(&UART2_ctl_t,"The Specified Register Number Error!\r\n",38); 
                            }
                        }
                        else if(strstr((char *)g_ucGPRSCmdBuff,"%NUM"))
                        {
                            t1 = strstr((char *)g_ucGPRSCmdBuff,"%NUM");
                            if(GPRSCmdCount == strlen(t1))//
                            {
                                if(GPRSCmdCount < (PHNUM_LEN+1)) //LJW,20160928不要做字符判断 &&(isdigtit_len(t1+4,GPRSCmdCount-4)))
                                {
                                    for(i=0 ; i<2 ; i++)
                                    {
                                        if(system_para.g_ucRegNum[i][0] != FULL)
                                        {
                                            //strncpy(system_para.g_ucRegNum[i]+4,temp,11); // 从短信命令中拷贝授权号码到授权号码缓冲区(包括号码类型和结束标记)
                                            for(j=0 ; j<GPRSCmdCount-4 ; j++)
                                            {
                                                system_para.g_ucRegNum[i][2+j] = *(t1+4+j);
                                            }
                                            system_para.g_ucRegNum[i][1] = GPRSCmdCount-4;
                                            system_para.g_ucRegNum[i][0] = FULL;
                                            Store_System_Para();
                                            //sys_Init();
                                            
                                            if(!strncmp((char *)(system_para.g_ucRegNum[i]+2),(char *)t1+4,GPRSCmdCount-4))
                                            {
                                                ddi_uart_Write(&UART2_ctl_t,"The Specified Number has been registed Successful!\r\n",52);
                                            }
                                            else
                                            {
                                                ddi_uart_Write(&UART2_ctl_t,"The Specified Number registed Failed!\r\n",40);
                                            }
                                            //delayms(500); //wait for send end
                                            break;
                                        }
                                    } 
                                    if(i >= 2)
                                    {
                                        ddi_uart_Write(&UART2_ctl_t,"The Register Number is Full!\r\n",30);
                                    }
                                }
                                else ddi_uart_Write(&UART2_ctl_t,"The Specified Number registed Failed!\r\n",40);
                            }
                        }
				}
                //ljw,20160927for wulian
#if 0 //ljw,20160927					
                    else
					if(strstr((char *)g_ucGPRSCmdBuff,"NUM"))
					{                       
//                        if(GPRSCmdCount ==strlen(g_ucGPRSCmdBuff))//ljw,20160927for wulian
//                        {                        
                            t1 = (unsigned char *)strstr((char *)g_ucGPRSCmdBuff,"NUM")+3;
                               //判断号码是否有效
                            for(i =0; i<11; i++)
                            {
                                if ((*(t1+i)>'9') ||(*(t1+i)<'0'))
                                {
                                    //ddi_uart_Write(&UART2_ctl_t,"Wrong number!",strlen("Wrong number!"));
                                    //delayms(100); //wait for send end
                                    setflag = 1;
                                    break;                                
                                }
                            }
                            if(i < 11)
                            {
                                ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
                                setflag = 1;							
                            }
                            else
                            {
                                if (!strstr((char *)g_ucGPRSCmdBuff,"DEL"))//ljw zhu,20160927 没有删除指令，是设置指令
                                {
                                    for(i=0 ; i<2 ; i++)
                                    {
                                        if(system_para.g_ucRegNum[i][0] != FULL)
                                        {
                                            //strncpy(system_para.g_ucRegNum[i]+4,temp,11); // 从短信命令中拷贝授权号码到授权号码缓冲区(包括号码类型和结束标记)
                                            for(j=0 ; j<11 ; j++)
                                            {
                                                system_para.g_ucRegNum[i][4+j] = *(t1+j);
                                            }
                                            system_para.g_ucRegNum[i][0] = FULL;   // 

                                            Store_System_Para();
                                            //sys_Init();							
                                            
                                            if(!strncmp((char *)(system_para.g_ucRegNum[i]+4),(char *)t1,11))
                                            {
                                                ddi_uart_Write(&UART2_ctl_t,"The Specified Number has been registed Successful!\r\n",52);
                                            }
                                            else
                                            {
                                                ddi_uart_Write(&UART2_ctl_t,"The Specified Number registed Failed!\r\n",40);
                                            }
                                            //delayms(500); //wait for send end
                                            setflag = 1;
            
                                            break;
                                            }
                                        } 
                                        if(i >= 2)
                                        {
                                            ddi_uart_Write(&UART2_ctl_t,"The Register Number is Full!\r\n",30);
                                        }							
                                    }
                            else
                            {
                                for(i=0 ; i<2 ; i++)  
                                {
                                    if(!strncmp((char *)system_para.g_ucRegNum[i]+4,(char *)t1,11))  // 在授权号码中找到要删除的号码
                                    {
                                        strncpy((char *)system_para.g_ucRegNum[i],"e   00000000000",15);        // 删除内存中授权号码
                                        Store_System_Para();
                                        //sys_Init();							
                                        
                                        if(system_para.g_ucRegNum[i][0] == EMPTY)
                                        {
                                            ddi_uart_Write(&UART2_ctl_t,"The Specified Register Number has been delete Successful!\r\n",59);
                                        }
                                        else
                                        {
                                            strncpy((char *)system_para.g_ucRegNum[i],"e   00000000000",15);        // 删除内存中授权号码
                                            ddi_uart_Write(&UART2_ctl_t,"The Specified Register Number has been delete Fail!\r\n",53);
                                            //delayms(500); //wait for send end
                                        }
                                        setflag = 1;
                                        break;
                                    }
                                }
                            }
                             delayms(500); //wait for send end
                            setflag = 1;
                        }
//                    }//ljw,20160927for wulian
				}
#endif //ljw,20160927for wulian
				else
				if (strstr((char *)g_ucGPRSCmdBuff,"MODE"))
				{
						//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"MODE")+4])
                                                t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"MODE") + 4;
						if ('?' == *t1)
						{
								switch(system_para.g_ucPowerMode)
								{
									case ALLONLINE_MODE:
										ddi_uart_Write(&UART2_ctl_t,"allonline\r\n",strlen("allonline\r\n"));
										break;
#if 0//wei,20160803 for space<
									case LOW_POWER_MODE:
										ddi_uart_Write(&UART2_ctl_t,"lowpower\r\n",strlen("lowpower\r\n"));
										break;
#endif //wei,20160803 for space>
									case EXTRA_LOW_POWER_MODE:
										ddi_uart_Write(&UART2_ctl_t,"elowpower\r\n",strlen("elowpower\r\n"));
										break;
									default:
										break;										
								}								
						}
						else
						{
							if(strstr((char *)g_ucGPRSCmdBuff,"elowpower"))
							{
								system_para.g_ucPowerMode = EXTRA_LOW_POWER_MODE;
								i = 0x5a;
							}
#if 0//wei,20160803 for space<
							else
							if(strstr((char *)g_ucGPRSCmdBuff,"lowpower"))
							{
								 i = 0x5a;
								system_para.g_ucPowerMode = LOW_POWER_MODE;
							}
#endif //wei,20160803 for space>
							else								
							if(strstr((char *)g_ucGPRSCmdBuff,"allonline"))
							{
								  i =0x5a;
								system_para.g_ucPowerMode = ALLONLINE_MODE;
							}	
							else
							{
								i = 0xa5;
							}
							if (i == 0x5a)
							{
								Store_System_Para();
							//sys_Init();							


								ddi_uart_Write(&UART2_ctl_t,"Set mode ok!\r\n",strlen("Set mode ok!\r\n"));
							}
							else
							if (i == 0xa5)
							{
								ddi_uart_Write(&UART2_ctl_t,"Wrong mode !\r\n",strlen("Wrong mode !\r\n"));
							}
						}
						setflag = 1;
				}
				else
				if (strstr((char *)g_ucGPRSCmdBuff,"AUTOSMS"))
				{

                                t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"AUTOSMS") + 7;
					if ('?' == *t1)
					{


						if (system_para.g_bAUTOSMSon)
						{
							ddi_uart_Write(&UART2_ctl_t,"AUTOSMS is on!\r\n",strlen("AUTOSMS is on!\r\n"));
						
						}
						else
						{
							ddi_uart_Write(&UART2_ctl_t,"AUTOSMS is off!\r\n",strlen("AUTOSMS is off!\r\n"));
	
						}
						
					}
					else
					{
					      if  (  strstr((char *)t1,"on") )
					      	{
							system_para.g_bAUTOSMSon = 1;
							ddi_uart_Write(&UART2_ctl_t,"Set AUTOSMS ok!\r\n",strlen("Set AUTOSMS ok!\r\n"));
					      	}
						else
						   if  (  strstr((char *)t1,"off") )
					      	{
							system_para.g_bAUTOSMSon = 0;
							ddi_uart_Write(&UART2_ctl_t,"Cancel AUTOSMS ok!\r\n",strlen("Cancel AUTOSMS ok!\r\n"));
					      	}
						
						Store_System_Para();
						//sys_Init();							
				      

						  
					}
					// delayms(500); //wait for send end
					setflag = 1;

				}
				else
				if (strstr((char *)g_ucGPRSCmdBuff,"ACC"))
				{
					//if ('?' == g_ucGPRSCmdBuff[strstr((char *)g_ucGPRSCmdBuff,"AUTOSMS")+7])
                                        t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"ACC") + 3;
					if ('?' == *t1)
					{

						if (system_para.g_bACCon)
						{
							ddi_uart_Write(&UART2_ctl_t,"ACC MODE is on!\r\n",strlen("ACC MODE is on!\r\n"));
						
						}
						else
						{
							ddi_uart_Write(&UART2_ctl_t,"ACC MODE is off!\r\n",strlen("ACC MODE is off!\r\n"));
	
						}
						
					}
					else
					{
					      if  (  strstr((char *)t1,"on") )
					      	{
							system_para.g_bACCon = 1;
							ddi_uart_Write(&UART2_ctl_t,"ACC MODE is on!\r\n",strlen("ACC MODE is on!\r\n"));
					      	}
						else
						   if  (  strstr((char *)t1,"off") )
					      	{
							system_para.g_bACCon = 0;
							ddi_uart_Write(&UART2_ctl_t,"ACC MODE is off!\r\n",strlen("ACC MODE is off!\r\n"));
					      	}
						
						Store_System_Para();
						//sys_Init();							
					       
						  
					}
					// delayms(500); //wait for send end
					setflag = 1;

				}

                else
				if (strstr((char *)g_ucGPRSCmdBuff,"SETPASSWORD"))
				{ 

						
						t1 = (unsigned char*)strstr((char *)g_ucGPRSCmdBuff,"SETPASSWORD") + strlen("SETPASSWORD");
						if ('?' == *t1)
						{

							ddi_uart_Write(&UART2_ctl_t,system_para.g_ucPassWord,sizeof (system_para.g_ucPassWord));
							delayms(500); //wait for send end
                            ddi_uart_Write(&UART2_ctl_t,"\r\n",strlen("\r\n"));
						}
						else
						{
							for(i =0; i<6; i++)
						      {
								if ((*(t1+i)>'9') ||(*(t1+i)<'0'))
								{
									ddi_uart_Write(&UART2_ctl_t,"Wrong number!\r\n",strlen("Wrong number!\r\n"));
									setflag = 1;
									break;
								}
						      }
							if(i == 6)
							{
								memcpy((char *)temp, (char *)system_para.g_ucPassWord, 6);
								strncpy((char *)system_para.g_ucPassWord,t1,6);	
								Store_System_Para();
								ddi_uart_Write(&UART2_ctl_t,"Set SETPASSWORD ok!\r\n",strlen("Set SETPASSWORD ok!\r\n"));
							}
						}
						//delayms(500); //wait for send end
						setflag = 1;
                    }								
                    setflag = 1;
			    }
			}  // while(g_ucInitSettingFlag == 1)
			
			if(setflag)
			{
			   //ClrGPRSCmdBuff(); //处理完一帧数据就清空缓冲去，等下一阵数据
                //wei,20161221 if(GPRS_BUFF_SIZE<=strlen(g_ucGPRSCmdBuffback))
                   //wei,20161221 memset(g_ucGPRSCmdBuffback,0x00,strlen(g_ucGPRSCmdBuffback));
                //wei,20161221 else   
                    memset(g_ucGPRSCmdBuffback,0x00,GPRS_BACK_BUFF_SIZE);
                GPRSCmdCount = 0;
			   //wei memset(g_ucGPRSCmdBuffback,0,40);
			   setflag = 0;
			}
			IWDG_ReloadCounter();			
		}
	}

	EN_initialize = 0;
  	ddi_uart_Write(&UART2_ctl_t,"  System start now!\r\n",strlen("  System start now!\r\n"));
	//delayms(500);

}
