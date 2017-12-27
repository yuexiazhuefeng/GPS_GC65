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
   	   		   

/***********************************************************************
 * Function prototypes of global functions imported.  
 **********************************************************************/

/***********************************************************************
 * Data definitions of global data exported.  
 **********************************************************************/


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
 


#include "timer.h"

#include "Ddi_uart.h"
#include "ring.h"
#include "LED.h"

u32 delayms_counter;


//capture speed:50khz
void Timer5_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 0xffff; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 1440-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 

	TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx����
							 
}


void Timer3_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 72; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 1000-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;  //��ռ���ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;  //�����ȼ�8��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}

void Timer3_Deinit(void)
{
	TIM_Cmd(TIM3, DISABLE);  //ʹ��TIMx����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); //ʱ��ʹ��

}

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��115200�Ĳ����ʣ�ÿ���ֽ���11bit����4���ֽڵĳ�ʱʱ��
//psc��ʱ��Ԥ��Ƶ��36000000/115200 = 312.5
//����ʹ�õ��Ƕ�ʱ��4!,5ms 
void Timer4_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 600; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 600; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM4, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;  //��ռ���ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 9;  //�����ȼ�8��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����
							 
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��115200�Ĳ����ʣ�ÿ���ֽ���11bit����4���ֽڵĳ�ʱʱ��
//psc��ʱ��Ԥ��Ƶ��36000000/115200 = 312.5
//����ʹ�õ��Ƕ�ʱ��2!
void Timer2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =(psc-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM2, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  1; //  //4;  //��ռ���ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//8;  //�����ȼ�8��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
							 
}

//ÿ�����ж�һ��
void TIM3_IRQHandler(void)   //TIM3�ж�
{

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		   TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	       //    if(delayms_counter > 0)
			delayms_counter ++;
	}
}

#if 0
//��ʱƵ��200HZ 5ms
void TIM4_IRQHandler(void)   //TIM5�ж�
{

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		   TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 

	}
}	
#endif


//��ʱƵ��11520/4
void TIM2_IRQHandler(void)   //TIM2�ж�
{
	u8  err1;
	static u8  flag_red=0;
	static u8  flag_yellow=0;
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		   TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 

		    if((LED_yellow.flash) && (LED_yellow.power))
		   {
		   	if (LED_yellow.counter>1)
				LED_yellow.counter --;
			if (LED_yellow.counter == 1)
			{
				LED_yellow.counter = LED_yellow.freq;
				if (flag_yellow)
				{
					flag_yellow = 0;
					LED_On(2);
				}
				else
				{
					flag_yellow = 1;
					LED_Off(2);
				}
			}

		   }
		   if ((LED_red.flash) && (LED_red.power))
		   {
		   	if (LED_red.counter>1)
				LED_red.counter --;
			if (LED_red.counter == 1)
			{
				LED_red.counter = LED_red.freq;
				if (flag_red)
				{
					flag_red = 0;
					LED_On(4);
				}
				else
				{
					flag_red = 1;
					LED_Off(4);
				}
			}

		   }		  
		   //�ж�ÿ�����ڵĽ��������Ƿ����16�����˾ͷ�����Ϣ��Ӧ��
		   //��С�ڵ���16 �������㣬�жϳ�ʱ�������Ƿ��㣬����Ҳ����Ϣ
		   //����һ���õ� 
		   if(UART1_ctl_t.RTimeOver ) 
		   {

			   if( UART1_ctl_t.Ro > 1)
			      UART1_ctl_t.Ro --;
			   if (2 == UART1_ctl_t.Ro) 
			   {

			//		if ((RING_COUNT((RING_T*)(UART1_ctl_t.pRbuf))))// && (0 == UART1_ctl_t.RMsgLock))
					{
						OSFlagPost(g_pFlagsEvent,(OS_FLAGS)(FLAGS_USERCOM1_RX),OS_FLAG_SET,&err1);
						UART1_ctl_t.ComId = 0x5a;
					//	UART1_ctl_t.Ro = 0;
					}

			   }
		   }
		   if(UART2_ctl_t.RTimeOver ) 
		   {
		   	   if( UART2_ctl_t.Ro > 1)
			      UART2_ctl_t.Ro --;
			   if (2 == UART2_ctl_t.Ro) 
			   {
				//	if ((RING_COUNT((RING_T*)(UART2_ctl_t.pRbuf))))// && (0 == UART2_ctl_t.RMsgLock))
					{
						OSFlagPost(g_pFlagsEvent,(OS_FLAGS)(FLAGS_USERCOM2_RX),OS_FLAG_SET,&err1);
						UART2_ctl_t.ComId = 0x5a;
					//	UART2_ctl_t.Ro = 0;
					}
			   }
		   }	   
		   if(UART3_ctl_t.RTimeOver ) 
		   {
			   if( UART3_ctl_t.Ro > 1)   
				  UART3_ctl_t.Ro --;
			   if (2 == UART3_ctl_t.Ro) 
			   {

			//		if ((RING_COUNT((RING_T*)(UART3_ctl_t.pRbuf))))// && (0 == UART3_ctl_t.RMsgLock))
					{
						OSFlagPost(g_pFlagsEvent,(OS_FLAGS)(FLAGS_USERCOM3_RX),OS_FLAG_SET,&err1);
						UART3_ctl_t.ComId = 0x5a;
				//		UART3_ctl_t.Ro = 0;
					}


			   }
		   }
		   if(UART4_ctl_t.RTimeOver ) 
		   {
			   if( UART4_ctl_t.Ro > 1)			   
			      UART4_ctl_t.Ro --;
#if 0			   
			   if (7 == UART4_ctl_t.Ro) 
			   {
					if (RING_COUNT((RING_T*)(UART4_ctl_t.pRbuf)))
					{
						
						if (0 == UART4_ctl_t.RMsgLock)
						{
							OSFlagPost(g_pFlagsEvent,(OS_FLAGS)(FLAGS_USERCOM4_RX),OS_FLAG_SET,&err1);
							UART4_ctl_t.ComId = 0x5a;
						}	
					}
			   }
#endif
			   if (2 ==  UART4_ctl_t.Ro) 
			   {
#if 0
			   		if (RING_COUNT((RING_T*)(UART4_ctl_t.pRbuf)))
					{
						OSFlagPost(g_pFlagsEvent,(OS_FLAGS)(FLAGS_USERCOM4_RX),OS_FLAG_SET,&err1);
						UART4_ctl_t.Ro = 0;
					}
#else
				//	if ((RING_COUNT((RING_T*)(UART4_ctl_t.pRbuf))))// && (0 == UART4_ctl_t.RMsgLock))
					{
						OSFlagPost(g_pFlagsEvent,(OS_FLAGS)(FLAGS_USERCOM4_RX),OS_FLAG_SET,&err1);
						UART4_ctl_t.ComId = 0x5a;
					//	UART4_ctl_t.Ro = 0;
					}
#endif

			   }
			   
		   }
		   if(UART5_ctl_t.RTimeOver ) 
		   {
			   if( UART5_ctl_t.Ro > 1)
			     UART5_ctl_t.Ro --;
			   if (2 == UART5_ctl_t.Ro) 
			   {
#if 0
			   		if (RING_COUNT((RING_T*)(UART5_ctl_t.pRbuf)))
					{
						OSFlagPost(g_pFlagsEvent,(OS_FLAGS)(FLAGS_USERCOM5_RX),OS_FLAG_SET,&err1);
					}
#else
  				//	if ((RING_COUNT((RING_T*)(UART5_ctl_t.pRbuf))))// && (0 == UART5_ctl_t.RMsgLock))
					{
						OSFlagPost(g_pFlagsEvent,(OS_FLAGS)(FLAGS_USERCOM5_RX),OS_FLAG_SET,&err1);
						UART5_ctl_t.ComId = 0x5a;
					//	UART5_ctl_t.Ro = 0;
					}
#endif
			   }
		   }

		}
}













