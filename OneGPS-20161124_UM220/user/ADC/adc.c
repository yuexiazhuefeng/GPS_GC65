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
 


 
#include <stdio.h>
#include "main.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"

/* Private variables ---------------------------------------------------------*/
float AD_value;
vu16 ADC_ConvertedValue;


/**
  * @brief  ADC1 channel5 configuration
  * @param  None
  * @retval None
  */
void ADC1_Config(void)
{
  ADC_InitTypeDef          ADC_InitStructure;
  GPIO_InitTypeDef         GPIO_InitStructure;
 
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  
  /* Configure ADC Channel11 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  /* ADC1 Configuration *******************************************************/
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits*/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode =  ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // ADC_ExternalTrigConvEdge_Rising;    
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  /* Convert the ADC1 Channel 11 with 239.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_5 , ADC_SampleTime_239_5Cycles);   

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);

  /* Enable the auto delay feature */    
  ADC_WaitModeCmd(ADC1, ENABLE); 
  
  /* Enable the Auto power off mode */
  ADC_AutoPowerOffCmd(ADC1, ENABLE); 
  
  /* Enable ADCperipheral[PerIdx] */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
 
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
}
			  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(void)   
{
      volatile u16 i;
  	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
      ADC_StartOfConversion(ADC1);
      i ++;
      i --;
	  /* Wait the ADC_FLAG_EOSMP falg */
      while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); 
      i ++;
      i --;	  
	ADC_StopOfConversion(ADC1);
      i ++;
      i --;
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}


u16 GetVoltage(void)
{
	u32 dwSamplePool[12];
	u32 dwMax = 0, dwMin = 0, dwAdcTmp = 0;
	u8 i;

	  /* filter routine, read adc for 7 times totally */
	  for(i=0; i<12; i++) {
	    /* sampling */
	    dwSamplePool[i] = Get_Adc();
	    if(dwSamplePool[i] > 0) dwMin = dwSamplePool[i];
	    if(dwSamplePool[i] < 0xfff) dwMax = dwSamplePool[i];
	    
	    dwAdcTmp += dwSamplePool[i];
	  }
	  for(i=0; i<12; i++) {

	    if(dwSamplePool[i] < dwMin) dwMin = dwSamplePool[i];
	    if(dwSamplePool[i] > dwMax) dwMax = dwSamplePool[i];
	    
      }

	  /* throw one max and one min values before generates a average result */
	dwAdcTmp -= (dwMin+dwMax); 
	dwAdcTmp *= 330;   //3300/10
	dwAdcTmp /=195;  //4096/21=195

	return (u16)(dwAdcTmp);

}

