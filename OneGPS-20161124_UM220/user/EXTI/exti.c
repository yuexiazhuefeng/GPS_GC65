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
//��ʼ��Pe8Ϊ�ж�����.
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

  //��ʼ�� PE8 �ٶ�����
  	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);


  //PE8�ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource8);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���



  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//��ռ���ȼ�3�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x08;					//�����ȼ�8
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
  

}
//һ�����������ٶȴ�����ÿȦ100����Ӧ�ľ���2�ף��޶�300 km/h  (4166hz),���ʱ�� 1km/h(13.88hz)
//count_speed �������������λ��1/50000 ��	��������ֵ��Ӧ��Ƶ�ʣ�0.762����С��50000hz
 //uint16_t TIM_GetCounter(TIM_TypeDef* TIMx)
void EXTI9_5_IRQHandler(void)
{
	u32 tmp_speedcount=0;
	u8 i;
	u16 tmp;

  	if(EXTI_GetITStatus(EXTI_Line8) != RESET)	  //���ָ����EXTI0��·�������������
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
	EXTI_ClearITPendingBit(EXTI_Line8);  //���EXTI0��·����λ
}
 


