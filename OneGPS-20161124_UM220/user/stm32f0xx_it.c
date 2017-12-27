/**
  ******************************************************************************
  * @file    SysTick/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "main.h"
#include "led.h"

/** @addtogroup STM32F0_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup SysTick_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern __IO uint16_t CCR3_Val;
extern __IO uint16_t CCR4_Val;
extern unsigned char volatile proGPRSCmdFlag;
u16 capture = 0;
u32 delayms_counter=0;
u32 secondcounter = 0;
//??????,?????0x5a, ????????
u8 Impact_flag = 0;
u8 send_alarm_flag = 0;

u8  ring_start_flag =0;
u32 ring_counter=0;
u32 back_g_usHeartAternationTime=0;
u32 back_g_usGPRSUpLoadAternationTime=0;
u32 back_g_usGPRSUpLoadTime=0;
u16 RTC_ALARM_timer=0;

//ljw,20160903 震动调整u32 Impact_time=0;
u8 Impact_time_LED = 0;//ljw,20160903,LED100ms后关闭
u8 Impact_time_conut = 0;//ljw,20160903,震动次数过滤
u8 Impact_Alarm_flag = 0;//ljw,20160903，过滤后的有效震动产生，0x5a,有震动，  0x00,无震动
u8 Impact_Alarm_8s = 0;//ljw,20160903
//中断来了后一秒再做判断
u8 flag_start_duage_g_ucAlarm=0;
u32 duage_g_ucAlarm_counter=0;

// acc 关了之后才2分钟，才能振动有效，上传振动标志
u8  start_accoff = 0;
u32 accoff_counter = 0; 
u8 shake_gate = 0x5a;
u8 alarm_in_flag = 0;

//  一小时定时时间到，要保存里程数据
u8 flag_mileage_save=0;
//ljw,20160920 NOLocalCount>=120不要GPS过滤了 u32 NOLocalCount=0;

extern void RTC_Config(void);

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
    u32 i ;
    while(1)
//ljw,20160908提高效率     for(;;)
    {
        GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);//LJW,20160904 异常指示灯
        for(i=0;i<0x4FFFF;i++);
        GPIO_ResetBits(LED3_GPIO_PORT,LED3_PIN);
        for(i=0;i<0x4FFFF;i++);
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
static u8 proGPRSCmdFlag_counter=0; //ljw,20160906优化串口接收程序
void SysTick_Handler(void)
{
  static  u8 tencounter=0;
//ljw,20160906优化串口接收程序  static u8 proGPRSCmdFlag_counter=0; //ljw,20160906优化串口接收程序
	static long long backup_mileage_64 = 0;//wei.20160524 for mileage

  if(++tencounter >= 10)//wei,zhu 1s
  {
	tencounter =  0;
	secondcounter ++;
	lowbattcount ++;
	gps_copy = 0x5a;
//    if(test_LIS3DH_timer++>2) //ljw,20160913,测试震动 测完要删)
//        test_LIS3DH_timer = 2;
//wei20610601
//		if(test_reset_time++>600)
//			test_reset_time=601;
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
	 if (!get_SHAKE_status())  //判断是不是低电平
       {
          if(SHACK_Init_Flag_Time++>=5)
		 BitFlag.SHACK_Init_Flag = 0x03;
       }
	 else 
	 {
          SHACK_Init_Flag_Time = 0;
	    BitFlag.SHACK_Init_Flag = 0;  
	 }
#endif
//wei20160601
	if (chinesesmsflag&&startchinesesms)
	{
		startchinesesms ++;
	}

	if((secondcounter-back_g_usHeartAternationTime) >= system_para.HeartAternationTime)
	{
		 if((system_para.HeartAternationTime <10)||(system_para.HeartAternationTime >300)) //wei,20160603强制心跳不在10s-300s之间强制为默认值
		    system_para.HeartAternationTime = default_HeartAternationTime; //wei,20160603
	      back_g_usHeartAternationTime = secondcounter;
		   
	     g_ucHeartUpdateFlag  = 0x5a; 

	}
	//定时上传
	if (((T_P_time > 1) && ((secondcounter-back_g_usGPRSUpLoadAternationTime) >= T_P_g_usGPRSUpLoadAternationTime)) \
		|| ((secondcounter-back_g_usGPRSUpLoadAternationTime) >= system_para.g_usGPRSUpLoadAternationTime))
	{
	       back_g_usGPRSUpLoadAternationTime = secondcounter;
		 // 防止proGPRSCmdFlag一直有  
#if 0 //ljw,20160906,优化串口接收，防止7E标志一直
		 if (proGPRSCmdFlag)
		 {
		 	proGPRSCmdFlag_counter ++;
			if (proGPRSCmdFlag_counter>3)
			{
				proGPRSCmdFlag_counter = 0;
				proGPRSCmdFlag = 0;
			}
		 }
		 else
		 	proGPRSCmdFlag_counter = 0;
#endif //ljw,20160906,优化串口接收，防止7E标志一直
	     g_ucUpdateFlag  = 0x5a;
	}
	//工作持续时间
	if((secondcounter-back_g_usGPRSUpLoadTime) >= system_para.g_usGPRSUpLoadTime*60)
	{
	     back_g_usGPRSUpLoadTime = secondcounter;
	     g_ucGotoSleepFlag  = 0x5a;
	}

	if (start_accoff  && (secondcounter-accoff_counter >= 120)	)
	{
			accoff_counter= 0;
			shake_gate = 0x5a;
			start_accoff = 0;
	}
	//增加gsm 120秒都收不到服务器的消息,就重启GSM.
	server_noack_counter ++;
	if (server_ack_flag)
	{
		server_noack_counter = 0;
		server_ack_flag = 0;
	}
	if (server_noack_counter > 240)  //120
		rst_gsm_flag = 0x5a;

      // 每5分钟保存一次里程，取值300，可以存25年
    //  if (secondcounter %3601 == 0)

#if 0 //wei,注释掉  20160829
    if (secondcounter %300 == 0) // 测试　取值2秒，存一次
    {
		flag_mileage_save = 0x5a;
	}
#endif
	//跟踪时间每秒减一，减到零就要换成原来的上传间隔
	if (T_P_time > 0)
	{
		T_P_time --;
	}
	//20160129 dingweied_flag改成dingwei_flag
//ljw,20160920 NOLocalCount>=120不要GPS过滤了
//	if (0 == dingwei_flag)
//	{
//		NOLocalCount++;
//		if (NOLocalCount>0xf0000000)
//			NOLocalCount = 0xf0000000;
//	}
//	else
//	{
//		NOLocalCount = 0;
//	}
		//acc 功能打开，状态是有ACC信号的时候才计算里程
//wei 20160524,里程统计
	if((system_para.g_bACCon) &&(g_ucACCFlag)&&(dingwei_flag))
	{
		if((mileage_tmp>0)&&(mileage_tmp<=2200)) //wei 20160525,给速度加个合法值判断	
			wzw_mileage.mileage_64 += mileage_tmp;
		else mileage_tmp = 0;  //速度非法，则清零
//		if(wzw_mileage.mileage_64-backup_mileage_64 >= 7200)  //wei,20160523,超过2公里就存里程。
        if(wzw_mileage.mileage_64-backup_mileage_64 >= 3600)  //wei,20160523,超过1公里就存里程。
		{
			flag_mileage_save = 0x5a;
			backup_mileage_64 = wzw_mileage.mileage_64;
	    }
		//mileage_tmp = 0;
	}
//wei 20160524,里程统计
	
		
  }
  

  
	

}


/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/
/**
  * @brief  This function handles RTC Auto wake-up interrupt request.
  * @param  None
  * @retval None
  */
// 十秒闹铃中断一次，看门狗复位时间是20秒。这样保证在没有复位的情况下通过rtc
//喂狗，并再次进入休眠。等执行次数到了sleeptime*6的时候，关闭rtc，进入正常工作模式，
void RTC_IRQHandler(void)
{
    RTC_InitTypeDef   RTC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
	RTC_TimeTypeDef   RTC_TimeStructure;
	RTC_AlarmTypeDef  RTC_AlarmStructure;

	NVIC_InitTypeDef NVIC_InitStructure;

	if (RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		RTC_ALARM_timer ++;
		/* Clear the Alarm A Pending Bit */
		RTC_ClearITPendingBit(RTC_IT_ALRA);

		/* Clear EXTI line17 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line17);
		//GSMLedOn();
	}  
}

/**
  * @brief  This function handles External lines 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_1_IRQHandler(void)
{
  if(EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) != RESET)
  {
    /* Clear the USER Button EXTI line pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }
}
/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
extern volatile u32 TimingDelay ;
void TIM3_IRQHandler(void)
{
  static u8 alarm_counter=0;
	static u8 ACC_counter_ON=0;//WEI,20160524for acc check
	static u8 ACC_counter_Off=0;//WEI,20160524for acc check
  if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare3(TIM3, capture + CCR3_Val);
  }
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //?????TIM??????:TIM ??? 
	{
		   TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //??TIMx???????:TIM ??? 
			 delayms_counter ++;
            if(TimingDelay--<=1)
                TimingDelay = 0;
#if 1 //ljw,20160903加震动过滤     
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪            
//ljw			 if (delayms_counter-Impact_time > 100)
//ljw		 {
//ljw				SHAKELedOff();
//ljw			 }
             if(Impact_time_LED++>100)//ljw
             {
                Impact_time_LED = 101;
                SHAKELedOff();
             }
#endif
#endif //ljw,20160903加震动过滤             
			 if(ring_start_flag == 0x5a)//判断电话或短信
			 {
				 ring_counter ++;
				 if (ring_counter >= 300)  //140
				 {
				   ring_start_flag = 0;
				   if (!get_GSM_RING_status())
						 ring_sms = RING;
				   else
						 ring_sms = SMS;
				   proGPRSCmdFlag = 0;// 20150819:?有时这个标志一直有，尤其是很多短信的时候，导致无法定时回传
				 }
			 }
			if(((delayms_counter-duage_g_ucAlarm_counter) >= 1000)&&(flag_start_duage_g_ucAlarm))//wei,zhu,20160712
			{
				if(get_ALARM_status())
				{
					g_ucAlarm = 0x5a;
                    back_g_usGPRSUpLoadTime = secondcounter; //工作时间重新计算
				}
				flag_start_duage_g_ucAlarm = 0;
				duage_g_ucAlarm_counter = 0;
			}
			if (delayms_counter%50 == 0)  //五〇毫秒检测一次紧急报警
			{
				if(get_ALARM_status())
				{
					alarm_counter ++ ;
					if (alarm_counter > 40 )
					{
						alarm_counter = 0;
						alarm_in_flag = 0x5a;
					}
				}
				else
				{
//ljw,20160928紧急报警标志量					alarm_in_flag = 0;
					alarm_counter = 0;
                    alarm_count = 0;//wei,紧急报警记次
				}
	//wei,20160524 for ACC check
                
//ljw,20160920把ACC使能开关做为大开关				if((system_para.g_bACCon)&&(get_ACC_CHECK_status()))
                if(system_para.g_bACCon)
                {
                    if(get_ACC_CHECK_status())
                    {
                        ACC_counter_Off = 0;
                        if(ACC_counter_ON++>=3)
                        {
                            ACC_counter_ON = 4;
                            g_ucACCFlag = 0x5a;
                            start_accoff = 0;//ljw,20160901,ACC判断从中断里面移动到这里，增加过滤
                            back_g_usGPRSUpLoadTime = secondcounter; //工作时间重新计算
                        }
                    }
                    else
                    {
                        ACC_counter_ON = 0;
                        if(ACC_counter_Off++>=3)
                        {
                            ACC_counter_Off = 4;
                            if(g_ucACCFlag)
                            {
                                g_ucACCFlag = 0;                        
                             //ljw,20160901,ACC判断从中断里面移动到这里，增加过滤<
                                start_accoff = 0x5a;
                                accoff_counter = secondcounter;
                                shake_gate = 0;
                             //ljw,20160901,ACC判断从中断里面移动到这里，增加过滤>
                                flag_mileage_save = 0x5a;//ljw,20160909,ACC关的时候触发一次存储动作。
                            }
                        }
                    }
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
                    if(Impact_flag)
                    {
                        if(Impact_time_conut++ > 20)//20*50ms
                        {
                            Impact_time_conut = 0;
                            if(Impact_flag == 3)//ljw,20160903,1s内有五个震动，就认为震动是有效的
                            {
                                Impact_flag = 1;
                                if((Impact_Alarm_flag == 0)&&(shake_gate == 0x5a)&&(g_ucACCFlag == 0))
                                    Impact_Alarm_flag = 0x5a;
                            }
                            else Impact_flag = 1;
                            if((Impact_Alarm_flag)&&(Impact_Alarm_8s++>8))
                            {
                                Impact_Alarm_flag = 0;
                                Impact_flag = 0;
                            }
                        }
                    }
#endif
                }
				
                //ljw,20160906优化串口程序<
                if(proGPRSCmdFlag)
                {
                    proGPRSCmdFlag_counter ++;
                    if (proGPRSCmdFlag_counter>40)
                    {
                        proGPRSCmdFlag_counter = 0;
                        proGPRSCmdFlag = 0;
                    }
                }
                else
                    proGPRSCmdFlag_counter = 0;
                //ljw,20160906优化串口程序>
	//wei,20160524 for ACC check
			}
#ifdef ZHONGKEWEI_GPS //ljw,20161125,中科微的GPS模块  
			if (fB5)
			{
				AGPS_ov ++;
			}
#endif //ljw,20161125,中科微的GPS模块 
#ifdef UM220_GPS//ljw,20161125,for um220
            if(fB5 == GC65_RECE)  //ljw,20161128,  ||(fB5 == AGPS_START_SEND))
            {
                if(AGPS_ov++ >200)
                    fB5 = AGPS_RE_END;
            }
#endif
	}
}

u16 shake_num = 0; //wei
void EXTI4_15_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        /* Clear the USER Button EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line5);
#ifdef ENSHOCK  //LJW,20161122 震动功能裁剪
        if(get_SHAKE_status())    //pb5
        {
//            if((BitFlag.SHACK_Init_Flag == 0x02)&&(!entersleepflag))
//            return ;
            SHAKELedOn();
//            //		shake_num++; //wei
//            if((system_para.g_bACCon) &&(!get_ACC_CHECK_status()))
//            {
//ljw,20160903,改成记次                Impact_flag = 0x5a;
                if(Impact_flag++>2)
                {
                    Impact_flag = 3;
                    Impact_Alarm_8s = 0;
                }
//            }
            back_g_usGPRSUpLoadTime = secondcounter; //工作时间重新计算
//ljw,20160903            Impact_time = delayms_counter;
            Impact_time_LED = 0;//ljw,20160903,不用32位的那种减方式了，太费RAM
            if(entersleepflag)
            {
                send_alarm_flag = 0x5a; //在振动，发送报警短信
                rtc_wakeup_timer = (system_para.g_usSleepTime*6) +2;   //弱国在休眠状态，立马退出，唤醒起来
            }
        }
#endif
    }
    if(EXTI_GetITStatus(EXTI_Line8) != RESET)  //劫警
    {
        /* Clear the USER Button EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line8);
        if (get_ALARM_status())    //pA8
        {
            flag_start_duage_g_ucAlarm = 0x5a;
            duage_g_ucAlarm_counter = secondcounter;
//wei,20160712,这里就从新计时不好，放大消抖程序里面做            back_g_usGPRSUpLoadTime = secondcounter; //工作时间重新计算
            if(entersleepflag)
            {
                send_alarm_flag = 0x5a; //有劫警，发送报警短信
                rtc_wakeup_timer = (system_para.g_usSleepTime*6) +2;   //弱国在休眠状态，立马退出，唤醒起来
            }
        }
    }

    if(EXTI_GetITStatus(EXTI_Line12) != RESET)//PA12 ACC检测
    {
        /* Clear the USER Button EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line12);
        if((entersleepflag)&&(get_ACC_CHECK_status())&&(system_para.g_bACCon))
        {
//ljw,20160903ACC不要短信报警            send_alarm_flag = 0x5a;
            rtc_wakeup_timer = (system_para.g_usSleepTime*6) +2;   //弱国在休眠状态，立马退出，唤醒起来
        }
#if 0 //ljw,20160902,ACC关的动作放到定时器里面做，增加过滤
        //这段主要用来计算ACC关了有没有超过2min
        if( (!get_ACC_CHECK_status()) && (system_para.g_bACCon))
        {
            start_accoff = 0x5a;
            accoff_counter= secondcounter;
            shake_gate = 0;
        }
        else
        {
            start_accoff = 0;
        }
#endif //ljw,20160902,ACC关的动作放到定时器里面做，增加过滤        
    }
   
    if(EXTI_GetITStatus(EXTI_Line15) != RESET) //PB15
    {
        /* Clear the USER Button EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line15);
        if (!get_GSM_RING_status())
        {
            // 定时140ms,然后再判断
            ring_start_flag = 0x5a;
            ring_counter = 0;
            if(entersleepflag)
            {
                rtc_wakeup_timer = (system_para.g_usSleepTime*6) +2;   //弱国在休眠状态，立马退出，唤醒起来
            }
            //　　　　//影瞎　工作时间延长　　　
            //                 // 20150513: 时间需要调整
            entersleepflag = 0;
            if(dial_status == 1) //wei,20160617判断一下拨号状态，如果拨号没有成功，就不要延时，
            {                    //防止机器无限制的拨号，将机器电量耗完。               
                back_g_usGPRSUpLoadTime = secondcounter; //工作时间重新计算			
            }
        }
    }  
}


 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
