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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 0xffff; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 1440-1; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 

	TIM_Cmd(TIM5, ENABLE);  //使能TIMx外设
							 
}


void Timer3_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 72; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 1000-1; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;  //先占优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;  //从优先级8级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}

void Timer3_Deinit(void)
{
	TIM_Cmd(TIM3, DISABLE);  //使能TIMx外设
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); //时钟使能

}

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。115200的波特率，每个字节有11bit，等4个字节的超时时间
//psc：时钟预分频数36000000/115200 = 312.5
//这里使用的是定时器4!,5ms 
void Timer4_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 600; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 600; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM4, //TIM2
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;  //先占优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 9;  //从优先级8级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
							 
}
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。115200的波特率，每个字节有11bit，等4个字节的超时时间
//psc：时钟预分频数36000000/115200 = 312.5
//这里使用的是定时器2!
void Timer2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =(psc-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM2, //TIM2
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  1; //  //4;  //先占优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//8;  //从优先级8级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
							 
}

//每毫秒中断一次
void TIM3_IRQHandler(void)   //TIM3中断
{

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		   TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
	       //    if(delayms_counter > 0)
			delayms_counter ++;
	}
}

#if 0
//定时频率200HZ 5ms
void TIM4_IRQHandler(void)   //TIM5中断
{

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		   TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 

	}
}	
#endif


//定时频率11520/4
void TIM2_IRQHandler(void)   //TIM2中断
{
	u8  err1;
	static u8  flag_red=0;
	static u8  flag_yellow=0;
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		   TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 

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
		   //判断每个串口的接收数据是否大于16，大了就发送消息给应用
		   //若小于等于16 而大于零，判断超时计数器是否到零，到了也发消息
		   //好像到一更好点 
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













