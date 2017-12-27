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

//ljw,20160903 �𶯵���u32 Impact_time=0;
u8 Impact_time_LED = 0;//ljw,20160903,LED100ms��ر�
u8 Impact_time_conut = 0;//ljw,20160903,�𶯴�������
u8 Impact_Alarm_flag = 0;//ljw,20160903�����˺����Ч�𶯲�����0x5a,���𶯣�  0x00,����
u8 Impact_Alarm_8s = 0;//ljw,20160903
//�ж����˺�һ�������ж�
u8 flag_start_duage_g_ucAlarm=0;
u32 duage_g_ucAlarm_counter=0;

// acc ����֮���2���ӣ���������Ч���ϴ��񶯱�־
u8  start_accoff = 0;
u32 accoff_counter = 0; 
u8 shake_gate = 0x5a;
u8 alarm_in_flag = 0;

//  һСʱ��ʱʱ�䵽��Ҫ�����������
u8 flag_mileage_save=0;
//ljw,20160920 NOLocalCount>=120��ҪGPS������ u32 NOLocalCount=0;

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
//ljw,20160908���Ч��     for(;;)
    {
        GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);//LJW,20160904 �쳣ָʾ��
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
static u8 proGPRSCmdFlag_counter=0; //ljw,20160906�Ż����ڽ��ճ���
void SysTick_Handler(void)
{
  static  u8 tencounter=0;
//ljw,20160906�Ż����ڽ��ճ���  static u8 proGPRSCmdFlag_counter=0; //ljw,20160906�Ż����ڽ��ճ���
	static long long backup_mileage_64 = 0;//wei.20160524 for mileage

  if(++tencounter >= 10)//wei,zhu 1s
  {
	tencounter =  0;
	secondcounter ++;
	lowbattcount ++;
	gps_copy = 0x5a;
//    if(test_LIS3DH_timer++>2) //ljw,20160913,������ ����Ҫɾ)
//        test_LIS3DH_timer = 2;
//wei20610601
//		if(test_reset_time++>600)
//			test_reset_time=601;
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
	 if (!get_SHAKE_status())  //�ж��ǲ��ǵ͵�ƽ
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
		 if((system_para.HeartAternationTime <10)||(system_para.HeartAternationTime >300)) //wei,20160603ǿ����������10s-300s֮��ǿ��ΪĬ��ֵ
		    system_para.HeartAternationTime = default_HeartAternationTime; //wei,20160603
	      back_g_usHeartAternationTime = secondcounter;
		   
	     g_ucHeartUpdateFlag  = 0x5a; 

	}
	//��ʱ�ϴ�
	if (((T_P_time > 1) && ((secondcounter-back_g_usGPRSUpLoadAternationTime) >= T_P_g_usGPRSUpLoadAternationTime)) \
		|| ((secondcounter-back_g_usGPRSUpLoadAternationTime) >= system_para.g_usGPRSUpLoadAternationTime))
	{
	       back_g_usGPRSUpLoadAternationTime = secondcounter;
		 // ��ֹproGPRSCmdFlagһֱ��  
#if 0 //ljw,20160906,�Ż����ڽ��գ���ֹ7E��־һֱ
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
#endif //ljw,20160906,�Ż����ڽ��գ���ֹ7E��־һֱ
	     g_ucUpdateFlag  = 0x5a;
	}
	//��������ʱ��
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
	//����gsm 120�붼�ղ�������������Ϣ,������GSM.
	server_noack_counter ++;
	if (server_ack_flag)
	{
		server_noack_counter = 0;
		server_ack_flag = 0;
	}
	if (server_noack_counter > 240)  //120
		rst_gsm_flag = 0x5a;

      // ÿ5���ӱ���һ����̣�ȡֵ300�����Դ�25��
    //  if (secondcounter %3601 == 0)

#if 0 //wei,ע�͵�  20160829
    if (secondcounter %300 == 0) // ���ԡ�ȡֵ2�룬��һ��
    {
		flag_mileage_save = 0x5a;
	}
#endif
	//����ʱ��ÿ���һ���������Ҫ����ԭ�����ϴ����
	if (T_P_time > 0)
	{
		T_P_time --;
	}
	//20160129 dingweied_flag�ĳ�dingwei_flag
//ljw,20160920 NOLocalCount>=120��ҪGPS������
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
		//acc ���ܴ򿪣�״̬����ACC�źŵ�ʱ��ż������
//wei 20160524,���ͳ��
	if((system_para.g_bACCon) &&(g_ucACCFlag)&&(dingwei_flag))
	{
		if((mileage_tmp>0)&&(mileage_tmp<=2200)) //wei 20160525,���ٶȼӸ��Ϸ�ֵ�ж�	
			wzw_mileage.mileage_64 += mileage_tmp;
		else mileage_tmp = 0;  //�ٶȷǷ���������
//		if(wzw_mileage.mileage_64-backup_mileage_64 >= 7200)  //wei,20160523,����2����ʹ���̡�
        if(wzw_mileage.mileage_64-backup_mileage_64 >= 3600)  //wei,20160523,����1����ʹ���̡�
		{
			flag_mileage_save = 0x5a;
			backup_mileage_64 = wzw_mileage.mileage_64;
	    }
		//mileage_tmp = 0;
	}
//wei 20160524,���ͳ��
	
		
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
// ʮ�������ж�һ�Σ����Ź���λʱ����20�롣������֤��û�и�λ�������ͨ��rtc
//ι�������ٴν������ߡ���ִ�д�������sleeptime*6��ʱ�򣬹ر�rtc��������������ģʽ��
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
#if 1 //ljw,20160903���𶯹���     
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�            
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
#endif //ljw,20160903���𶯹���             
			 if(ring_start_flag == 0x5a)//�жϵ绰�����
			 {
				 ring_counter ++;
				 if (ring_counter >= 300)  //140
				 {
				   ring_start_flag = 0;
				   if (!get_GSM_RING_status())
						 ring_sms = RING;
				   else
						 ring_sms = SMS;
				   proGPRSCmdFlag = 0;// 20150819:?��ʱ�����־һֱ�У������Ǻܶ���ŵ�ʱ�򣬵����޷���ʱ�ش�
				 }
			 }
			if(((delayms_counter-duage_g_ucAlarm_counter) >= 1000)&&(flag_start_duage_g_ucAlarm))//wei,zhu,20160712
			{
				if(get_ALARM_status())
				{
					g_ucAlarm = 0x5a;
                    back_g_usGPRSUpLoadTime = secondcounter; //����ʱ�����¼���
				}
				flag_start_duage_g_ucAlarm = 0;
				duage_g_ucAlarm_counter = 0;
			}
			if (delayms_counter%50 == 0)  //�婖������һ�ν�������
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
//ljw,20160928����������־��					alarm_in_flag = 0;
					alarm_counter = 0;
                    alarm_count = 0;//wei,���������Ǵ�
				}
	//wei,20160524 for ACC check
                
//ljw,20160920��ACCʹ�ܿ�����Ϊ�󿪹�				if((system_para.g_bACCon)&&(get_ACC_CHECK_status()))
                if(system_para.g_bACCon)
                {
                    if(get_ACC_CHECK_status())
                    {
                        ACC_counter_Off = 0;
                        if(ACC_counter_ON++>=3)
                        {
                            ACC_counter_ON = 4;
                            g_ucACCFlag = 0x5a;
                            start_accoff = 0;//ljw,20160901,ACC�жϴ��ж������ƶ���������ӹ���
                            back_g_usGPRSUpLoadTime = secondcounter; //����ʱ�����¼���
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
                             //ljw,20160901,ACC�жϴ��ж������ƶ���������ӹ���<
                                start_accoff = 0x5a;
                                accoff_counter = secondcounter;
                                shake_gate = 0;
                             //ljw,20160901,ACC�жϴ��ж������ƶ���������ӹ���>
                                flag_mileage_save = 0x5a;//ljw,20160909,ACC�ص�ʱ�򴥷�һ�δ洢������
                            }
                        }
                    }
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
                    if(Impact_flag)
                    {
                        if(Impact_time_conut++ > 20)//20*50ms
                        {
                            Impact_time_conut = 0;
                            if(Impact_flag == 3)//ljw,20160903,1s��������𶯣�����Ϊ������Ч��
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
				
                //ljw,20160906�Ż����ڳ���<
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
                //ljw,20160906�Ż����ڳ���>
	//wei,20160524 for ACC check
			}
#ifdef ZHONGKEWEI_GPS //ljw,20161125,�п�΢��GPSģ��  
			if (fB5)
			{
				AGPS_ov ++;
			}
#endif //ljw,20161125,�п�΢��GPSģ�� 
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
#ifdef ENSHOCK  //LJW,20161122 �𶯹��ܲü�
        if(get_SHAKE_status())    //pb5
        {
//            if((BitFlag.SHACK_Init_Flag == 0x02)&&(!entersleepflag))
//            return ;
            SHAKELedOn();
//            //		shake_num++; //wei
//            if((system_para.g_bACCon) &&(!get_ACC_CHECK_status()))
//            {
//ljw,20160903,�ĳɼǴ�                Impact_flag = 0x5a;
                if(Impact_flag++>2)
                {
                    Impact_flag = 3;
                    Impact_Alarm_8s = 0;
                }
//            }
            back_g_usGPRSUpLoadTime = secondcounter; //����ʱ�����¼���
//ljw,20160903            Impact_time = delayms_counter;
            Impact_time_LED = 0;//ljw,20160903,����32λ�����ּ���ʽ�ˣ�̫��RAM
            if(entersleepflag)
            {
                send_alarm_flag = 0x5a; //���񶯣����ͱ�������
                rtc_wakeup_timer = (system_para.g_usSleepTime*6) +2;   //����������״̬�������˳�����������
            }
        }
#endif
    }
    if(EXTI_GetITStatus(EXTI_Line8) != RESET)  //�پ�
    {
        /* Clear the USER Button EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line8);
        if (get_ALARM_status())    //pA8
        {
            flag_start_duage_g_ucAlarm = 0x5a;
            duage_g_ucAlarm_counter = secondcounter;
//wei,20160712,����ʹ��¼�ʱ���ã��Ŵ���������������            back_g_usGPRSUpLoadTime = secondcounter; //����ʱ�����¼���
            if(entersleepflag)
            {
                send_alarm_flag = 0x5a; //�нپ������ͱ�������
                rtc_wakeup_timer = (system_para.g_usSleepTime*6) +2;   //����������״̬�������˳�����������
            }
        }
    }

    if(EXTI_GetITStatus(EXTI_Line12) != RESET)//PA12 ACC���
    {
        /* Clear the USER Button EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line12);
        if((entersleepflag)&&(get_ACC_CHECK_status())&&(system_para.g_bACCon))
        {
//ljw,20160903ACC��Ҫ���ű���            send_alarm_flag = 0x5a;
            rtc_wakeup_timer = (system_para.g_usSleepTime*6) +2;   //����������״̬�������˳�����������
        }
#if 0 //ljw,20160902,ACC�صĶ����ŵ���ʱ�������������ӹ���
        //�����Ҫ��������ACC������û�г���2min
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
#endif //ljw,20160902,ACC�صĶ����ŵ���ʱ�������������ӹ���        
    }
   
    if(EXTI_GetITStatus(EXTI_Line15) != RESET) //PB15
    {
        /* Clear the USER Button EXTI line pending bit */
        EXTI_ClearITPendingBit(EXTI_Line15);
        if (!get_GSM_RING_status())
        {
            // ��ʱ140ms,Ȼ�����ж�
            ring_start_flag = 0x5a;
            ring_counter = 0;
            if(entersleepflag)
            {
                rtc_wakeup_timer = (system_para.g_usSleepTime*6) +2;   //����������״̬�������˳�����������
            }
            //��������//ӰϹ������ʱ���ӳ�������
            //                 // 20150513: ʱ����Ҫ����
            entersleepflag = 0;
            if(dial_status == 1) //wei,20160617�ж�һ�²���״̬���������û�гɹ����Ͳ�Ҫ��ʱ��
            {                    //��ֹ���������ƵĲ��ţ��������������ꡣ               
                back_g_usGPRSUpLoadTime = secondcounter; //����ʱ�����¼���			
            }
        }
    }  
}


 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
