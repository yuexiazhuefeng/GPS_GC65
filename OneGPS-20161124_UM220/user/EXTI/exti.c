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
 


#include "exti.h"
#include "Timer.h"
typedef struct _speed
{
      u8 pnum;
      u16  counter_speed[5];
      u16 backup_speed;	  
}SPEED_STRUCT; 
 
u32  count_speed=0;

SPEED_STRUCT speed;

//PE8
//初始化Pe8为中断输入.
void EXTIX_Init(void)
{
	u8 i;
  	GPIO_InitTypeDef GPIO_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	for(i=0; i<5;i++) 
		speed.counter_speed[i] = 0;
	speed.pnum = 0;
	speed.backup_speed = 0;


	Timer5_Init();

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO,ENABLE);

  //初始化 PE8 速度输入
  	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);


  //PE8中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource8);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器



  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//抢占优先级3， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x08;					//子优先级8
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
  

}
//一般最灵敏的速度传感器每圈100，对应的距离2米，限定300 km/h  (4166hz),最低时速 1km/h(13.88hz)
//count_speed 脉冲计数器，单位是1/50000 秒	，最大计数值对应的频率：0.762，最小：50000hz
 //uint16_t TIM_GetCounter(TIM_TypeDef* TIMx)
void EXTI9_5_IRQHandler(void)
{
	u32 tmp_speedcount=0;
	u8 i;
	u16 tmp;

  	if(EXTI_GetITStatus(EXTI_Line8) != RESET)	  //检查指定的EXTI0线路触发请求发生与否
	{	

		tmp = TIM_GetCounter(TIM5);
		
		speed.counter_speed[speed.pnum] = tmp-speed.backup_speed;
		speed.backup_speed = tmp;
		if((speed.pnum ++) >=5 )
			speed.pnum = 0;

		for (i =0; i<5; i++) 
			tmp_speedcount += speed.counter_speed[i];
		tmp_speedcount /= 5;
		
		count_speed = tmp_speedcount;
				
	}
	EXTI_ClearITPendingBit(EXTI_Line8);  //清除EXTI0线路挂起位
}
 


