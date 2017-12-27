/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : LMS330DLC_driver.c
* Author             : MSH Application Team
* Author             : Fabio Tota
* Version            : $Revision:$
* Date               : $Date:$
* Description        : LMS330DLC driver file
*                      
* HISTORY:
* Date               |	Modification                    |	Author
* 24/06/2011         |	Initial Revision                |	Fabio Tota
* 11/06/2012         |	Support for multiple drivers in the same program |	Abhishek Anand

********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include <includes.h> 

#include "stm32f0xx.h"
#include "lsm330dlc_driver.h"
#include "L3Gx.h"
#include <math.h>

//SDO_A SET 0,SO address is 0X32 
#define acceleration_address   0x32 //0x30


#define scale_acc 1
#define scale_ms 1



//IIC所有操作函数
extern void IIC_lsm330_Init(void);                //初始化IIC的IO口				 
extern void IIC_lsm330_Start(void);				//发送IIC开始信号
extern void IIC_lsm330_Stop(void);	  			//发送IIC停止信号
extern void IIC_lsm330_Send_Byte(u8 txd);			//IIC发送一个字节
extern u8 IIC_lsm330_Read_Byte(unsigned char ack);//IIC读取一个字节
extern u8 IIC_lsm330_Wait_Ack(void); 				//IIC等待ACK信号
extern void IIC_lsm330_Ack(void);					//IIC发送ACK信号
extern void IIC_lsm330_NAck(void);				//IIC不发送ACK信号

extern void IIC_lsm330_Write_One_Byte(u8 daddr,u8 addr,u8 data);
extern u8 IIC_lsm330_Read_One_Byte(u8 daddr,u8 addr);	 
extern void L3gx_init(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/*******************************************************************************
* Function Name		: LMS330DLC_ReadReg
* Description		: Generic Reading function. It must be fullfilled with either
*			: I2C or SPI reading functions					
* Input			: Register Address
* Output		: Data REad
* Return		: None
*******************************************************************************/
u8_t LMS330DLC_ReadReg(u8_t Reg, u8_t* Data) {
  
  //To be completed with either I2c or SPI reading function
	u8 temp=0;		  	    																 
       IIC_lsm330_Start();  
	IIC_lsm330_Send_Byte(acceleration_address);	   //发送写命令
	IIC_lsm330_Wait_Ack();
	IIC_lsm330_Send_Byte(Reg);//发送高地址
	IIC_lsm330_Wait_Ack();		 

	IIC_lsm330_Start();  	 	   
	IIC_lsm330_Send_Byte(acceleration_address+1);           //进入接收模式			   
	IIC_lsm330_Wait_Ack();	 
      temp=IIC_lsm330_Read_Byte(0);		   
      IIC_lsm330_Stop();//产生一个停止条件	    
	*Data = temp;
  return 1;
}


/*******************************************************************************
* Function Name		: LMS330DLC_WriteReg
* Description		: Generic Writing function. It must be fullfilled with either
*			: I2C or SPI writing function
* Input			: Register Address, Data to be written
* Output		: None
* Return		: None
*******************************************************************************/
u8_t LMS330DLC_WriteReg(u8_t WriteAddr, u8_t Data) {
  
       IIC_lsm330_Start();  

	IIC_lsm330_Send_Byte(acceleration_address);	    //发送写命令
	IIC_lsm330_Wait_Ack();
	IIC_lsm330_Send_Byte(WriteAddr);//发送高地址
	IIC_lsm330_Wait_Ack();		 
									   
	IIC_lsm330_Send_Byte(Data);     //发送字节							   
	IIC_lsm330_Wait_Ack();  		    	   
       IIC_lsm330_Stop();//产生一个停止条件 
	//delayms(10);	 
  return 1;
}
/**********************************************************************
//说明:  设置碰撞阀值和碰撞持续时间
//参数1 accelero:  碰撞阀值，分辨率0.1g 范围0~79
//参数2 time: 碰撞持续时间，分辨率4ms, 范围0~38
//返回: 0 为失败；1 为成功
**********************************************************************/
u8_t set_Impact(u8_t accelero,u8_t time)
{    
	u8 ret=1;
	u16 tmp1;
	u8 tmp2,t1;
	if( accelero > 79)
		return 0;
	if (time>38)
		time = 38;
	  LMS330DLC_AccSetThreshold(accelero); //　对应关系0.1G为单位
	  LMS330DLC_AccSetIrqDuration(time);  //与ｍｓ的对应关系 4MS为单位

	return ret;
}


AxesRaw_t data11;

//ljw,20160912测试震动芯片   #if 0
#if 1 //ljw,20160912测试震动芯片
u16 get_Pitch(void) 
{
	u8 devid;
	u16 Pitch1;
	float Pitch,Q,T,K;    
	 short value;

      LMS330DLC_GetAccAxesRaw(&data11);

	  //分别是加速度X,Y,Z的原始数据，13位的
	  Q=(float)(data11.AXIS_X)*1.953125; //计算出实际的加速度值
	  T=(float)(data11.AXIS_Y)*1.953125;
	  K=(float)(data11.AXIS_Z)*1.953125; 
	  Q=-Q;
	//  Roll=(float)(((atan2(K,Q)*180)/3.14159265)-90);    //X轴角度值  
	  Pitch=(float)(((atan2(K,T)*180)/3.14159265)-90);  //Y轴角度值       
	  if (Pitch<0)
	     Pitch =-Pitch;	
	  
         Pitch1 = (u16)(Pitch);        //延时               

	return Pitch;
}
#endif
extern u8 Impact_flag;
	volatile u16 jiaodu;
void LMS330DLC_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	volatile unsigned char  tmp;
	
	IIC_lsm330_Init();//初始化IIC管脚PF6--SCL  PF7--SDA
    
	if (!LMS330DLC_GetWHO_AM_I((u8_t*)&tmp))
	{
		tmp ++;
		tmp --;
	}

	if(tmp == 0x33)
	{
		tmp ++;
		tmp --;
	}
#if 0	

#else
	
    LMS330DLC_SetODR(LMS330DLC_ODR_400Hz); //LMS330DLC_ODR_200Hz//单位是５ｍｓ
       //wei,400Hz
	LMS330DLC_SetMode(LMS330DLC_LOW_POWER); //LMS330DLC_NORMAL

  //set Fullscale
//ljw,20160914测完恢复       LMS330DLC_SetFullScale(LMS330DLC_FULLSCALE_4);// LMS330DLC_FULLSCALE_8  正负８ｇ
    LMS330DLC_SetFullScale(LMS330DLC_FULLSCALE_4);//ljw,20160914测完要删
		//设FS1,FS0为01,即为+/-4G
  //set axis Enable
    LMS330DLC_SetAxis(LMS330DLC_X_ENABLE | LMS330DLC_Y_ENABLE|LMS330DLC_Z_ENABLE);

	LMS330DLC_SetBLE(LMS330DLC_BLE_LSB);//LSB小端模式，高数据位存在高地址

	LMS330DLC_HPFClickEnable(MEMS_DISABLE);
    LMS330DLC_HPFAOI1Enable(MEMS_DISABLE);
    LMS330DLC_HPFAOI2Enable(MEMS_DISABLE);
	LMS330DLC_SetHPFMode(LMS330DLC_HPM_NORMAL_MODE_RES);
	LMS330DLC_SetHPFCutOFF(LMS330DLC_HPFCF_3);
	LMS330DLC_SetFilterDataSel(MEMS_DISABLE);

//ljw,20160914	LMS330DLC_SetIntConfiguration(LMS330DLC_INT1_OR|LMS330DLC_INT1_ZHIE_ENABLE| LMS330DLC_INT1_YHIE_ENABLE|LMS330DLC_INT1_YLIE_DISABLE|LMS330DLC_INT1_XHIE_ENABLE|LMS330DLC_INT1_XLIE_DISABLE);
	LMS330DLC_SetIntConfiguration(LMS330DLC_INT1_ZHIE_ENABLE|LMS330DLC_INT1_ZLIE_DISABLE|\
                                  LMS330DLC_INT1_YHIE_ENABLE|LMS330DLC_INT1_YLIE_DISABLE|\
                                  LMS330DLC_INT1_XHIE_ENABLE|LMS330DLC_INT1_XLIE_DISABLE);
    LMS330DLC_SetIntMode(LMS330DLC_INT_MODE_OR); //只要有一个都中断
//ljw,20160914    LMS330DLC_SetIntMode(LMS330DLC_INT_MODE_6D_MOVEMENT); //只要有一个都中断

//ljw,20160914    LMS330DLC_SetInt1Threshold (0x10);//ljw,20160914,测完要删
//ljw,20160914    LMS330DLC_SetInt1Duration(0x02);
	  LMS330DLC_AccSetThreshold(40); //79　对应关系0.1G为单位
	  LMS330DLC_AccSetIrqDuration(5);  //38 与ｍｓ的对应关系 4MS为单位

	//LMS330DLC_SetClickCFG(LMS330DLC_ZS_ENABLE|LMS330DLC_YS_ENABLE|LMS330DLC_XS_ENABLE);
	//LMS330DLC_SetClickTHS(1);
	//LMS330DLC_SetClickLIMIT(1);
	// LMS330DLC_Int1LatchEnable(DISABLE); 
	// LMS330DLC_SetInt6D4DConfiguration(LMS330DLC_INT1_6D_4D_DISABLE); //LMS330DLC_INT1_4D_ENABLE
	 LMS330DLC_ResetInt1Latch();
	 LMS330DLC_FIFOModeEnable(LMS330DLC_FIFO_DISABLE);
	 LMS330DLC_SetTriggerInt(LMS330DLC_TRIG_INT1);
#endif
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	//U24(20)-U8(55)  INT1A      PD8
	//U24(19)-U8(57)  INT2A      PD10
	//正反转2 GPIO_Pin_15 GPE15  
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//WEI	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //WEI,20160612,震动中断脚设为内部上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PB5输入无上拉50MHz


  	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//开启复用时钟
	
	// 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);
	/* Connect Button EXTI Line to Button GPIO Pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//LMS330DLC_SetInt1Pin(LMS330DLC_CLICK_ON_PIN_INT1_ENABLE|LMS330DLC_I1_INT1_ON_PIN_INT1_ENABLE); //AOI1 interrupt on INT1_A.
	LMS330DLC_SetInt1Pin(LMS330DLC_CLICK_ON_PIN_INT1_DISABLE | LMS330DLC_I1_INT1_ON_PIN_INT1_ENABLE |              
	            LMS330DLC_I1_INT2_ON_PIN_INT1_DISABLE | LMS330DLC_I1_DRDY1_ON_INT1_DISABLE | LMS330DLC_I1_DRDY2_ON_INT1_DISABLE |
                    LMS330DLC_WTM_ON_INT1_DISABLE | LMS330DLC_INT1_OVERRUN_DISABLE   ) ;

	//set_Impact(60,37);
    LMS330DLC_SetInt2Pin(LMS330DLC_INT_ACTIVE_LOW);//wei,20160801,配置中断默认电平为高，中断时为低
}



/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : LMS330DLC_GetWHO_AM_I
* Description    : Read identification code by WHO_AM_I register
* Input          : Char to empty by Device identification Value
* Output         : None
* Return         : Status [value of FSS]
*******************************************************************************/
status_t LMS330DLC_GetWHO_AM_I(u8_t* val){
  
  if( !LMS330DLC_ReadReg(LMS330DLC_WHO_AM_I, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetStatusAUX
* Description    : Read the AUX status register
* Input          : Char to empty by status register buffer
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetStatusAUX(u8_t* val) {
  
  if( !LMS330DLC_ReadReg(LMS330DLC_STATUS_AUX, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}




/*******************************************************************************
* Function Name  : LMS330DLC_SetODR
* Description    : Sets LMS330DLC Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetODR(LMS330DLC_ODR_t ov){
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG1, &value) )
    return MEMS_ERROR;
  
  value &= 0x0f;
  value |= ov<<LMS330DLC_ODR_BIT; //左移4位
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG1, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetTemperature
* Description    : Sets LMS330DLC Output Temperature
* Input          : MEMS_ENABLE, MEMS_DISABLE
* Output         : None
* Note           : For Read Temperature by LMS330DLC_OUT_AUX_3, LMS330DLC_SetADCAux and LMS330DLC_SetBDU 
				   functions must be ENABLE
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetTemperature(State_t state){
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_TEMP_CFG_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xBF;
  value |= state<<LMS330DLC_TEMP_EN;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_TEMP_CFG_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}




/*******************************************************************************
* Function Name  : LMS330DLC_SetMode
* Description    : Sets LMS330DLC Operating Mode
* Input          : Modality (LMS330DLC_NORMAL, LMS330DLC_LOW_POWER, LMS330DLC_POWER_DOWN)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
//只是设置功耗模式，高分辨率使能失能
status_t LMS330DLC_SetMode(LMS330DLC_Mode_t md) {
  u8_t value;
  u8_t value2;
  static   u8_t ODR_old_value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG1, &value) )
    return MEMS_ERROR;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG4, &value2) )
    return MEMS_ERROR;
  
  if((value & 0xF0)==0) //ODR3-ODR0,全为0,则为power down模式
    value = value | (ODR_old_value & 0xF0); //if it comes from POWERDOWN  
  
  switch(md) {
    
  case LMS330DLC_POWER_DOWN:  //设为低功耗模式
    ODR_old_value = value;
    value &= 0x0F;
    break;
    
  case LMS330DLC_NORMAL:
    value &= 0xF7;//设置LPen位为0, 设为正常模式
    value |= (MEMS_RESET<<LMS330DLC_LPEN);
    value2 &= 0xF7;//清除HR位,失能高分辨率
    value2 |= (MEMS_SET<<LMS330DLC_HR);   //set HighResolution_BIT,使能高分辨率
    break;
    
  case LMS330DLC_LOW_POWER:		
    value &= 0xF7;
    value |=  (MEMS_SET<<LMS330DLC_LPEN);//LPen为置为1,进入low power mode
    value2 &= 0xF7;
    value2 |= (MEMS_RESET<<LMS330DLC_HR); //reset HighResolution_BIT,清除高分辨率位
    break;
    
  default:
    return MEMS_ERROR;
  }
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG1, value) )
    return MEMS_ERROR;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG4, value2) )
    return MEMS_ERROR;  
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetAxis
* Description    : Enable/Disable LMS330DLC Axis
* Input          : LMS330DLC_X_ENABLE/DISABLE | LMS330DLC_Y_ENABLE/DISABLE | LMS330DLC_Z_ENABLE/DISABLE
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetAxis(LMS330DLC_Axis_t axis) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG1, &value) )
    return MEMS_ERROR;
  value &= 0xF8;
  value |= (0x07 & axis);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG1, value) )
    return MEMS_ERROR;   
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetFullScale
* Description    : Sets the LMS330DLC FullScale
* Input          : LMS330DLC_FULLSCALE_2/LMS330DLC_FULLSCALE_4/LMS330DLC_FULLSCALE_8/LMS330DLC_FULLSCALE_16
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetFullScale(LMS330DLC_Fullscale_t fs) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xCF;	//清除FS1,FS0位
  value |= (fs<<LMS330DLC_FS); //左移4位
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}





/*******************************************************************************
* Function Name  : LMS330DLC_SetBLE
* Description    : Set Endianess (MSB/LSB)
* Input          : BLE_LSB / BLE_MSB
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetBLE(LMS330DLC_Endianess_t ble) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xBF;	
  value |= (ble<<LMS330DLC_BLE);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_HPFClick
* Description    : Enable/Disable High Pass Filter for click
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_HPFClickEnable(State_t hpfe) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFB;
  value |= (hpfe<<LMS330DLC_HPCLICK);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_HPFAOI1
* Description    : Enable/Disable High Pass Filter for AOI on INT_1
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_HPFAOI1Enable(State_t hpfe) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFE;
  value |= (hpfe<<LMS330DLC_HPIS1);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_HPFAOI2
* Description    : Enable/Disable High Pass Filter for AOI on INT_2
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_HPFAOI2Enable(State_t hpfe) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFD;
  value |= (hpfe<<LMS330DLC_HPIS2);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetHPFMode
* Description    : Set High Pass Filter Modality
* Input          : LMS330DLC_HPM_NORMAL_MODE_RES/LMS330DLC_HPM_REF_SIGNAL/
				   LMS330DLC_HPM_NORMAL_MODE/LMS330DLC_HPM_AUTORESET_INT
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetHPFMode(LMS330DLC_HPFMode_t hpm) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0x3F;
  value |= (hpm<<LMS330DLC_HPM);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetHPFCutOFF
* Description    : Set High Pass CUT OFF Freq
* Input          : HPFCF [0,3]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetHPFCutOFF(LMS330DLC_HPFCutOffFreq_t hpf) {
  u8_t value;
  
  if (hpf > 3)
    return MEMS_ERROR;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xCF;
  value |= (hpf<<LMS330DLC_HPCF);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetFilterDataSel
* Description    : Set Filter Data Selection bypassed or sent to FIFO OUT register
* Input          : MEMS_SET, MEMS_RESET
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetFilterDataSel(State_t state) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xF7;
  value |= (state<<LMS330DLC_FDS);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetInt1Pin
* Description    : Set Interrupt1 pin Function
* Input          :  LMS330DLC_CLICK_ON_PIN_INT1_ENABLE/DISABLE    | LMS330DLC_I1_INT1_ON_PIN_INT1_ENABLE/DISABLE |              
                    LMS330DLC_I1_INT2_ON_PIN_INT1_ENABLE/DISABLE  | LMS330DLC_I1_DRDY1_ON_INT1_ENABLE/DISABLE    |              
                    LMS330DLC_I1_DRDY2_ON_INT1_ENABLE/DISABLE     | LMS330DLC_WTM_ON_INT1_ENABLE/DISABLE         |           
                    LMS330DLC_INT1_OVERRUN_ENABLE/DISABLE  
* example        : SetInt1Pin(LMS330DLC_CLICK_ON_PIN_INT1_ENABLE | LMS330DLC_I1_INT1_ON_PIN_INT1_ENABLE |              
                    LMS330DLC_I1_INT2_ON_PIN_INT1_DISABLE | LMS330DLC_I1_DRDY1_ON_INT1_ENABLE | LMS330DLC_I1_DRDY2_ON_INT1_ENABLE |
                    LMS330DLC_WTM_ON_INT1_DISABLE | LMS330DLC_INT1_OVERRUN_DISABLE   ) 
* Note           : To enable Interrupt signals on INT1 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetInt1Pin(LMS330DLC_IntPinConf_t pinConf) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG3, &value) )
    return MEMS_ERROR;
  
  value &= 0x00;
  value |= pinConf;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetInt2Pin
* Description    : Set Interrupt2 pin Function
* Input          : LMS330DLC_CLICK_ON_PIN_INT2_ENABLE/DISABLE   | LMS330DLC_I2_INT1_ON_PIN_INT2_ENABLE/DISABLE |               
                   LMS330DLC_I2_INT2_ON_PIN_INT2_ENABLE/DISABLE | LMS330DLC_I2_BOOT_ON_INT2_ENABLE/DISABLE |                   
                   LMS330DLC_INT_ACTIVE_HIGH/LOW
* example        : LMS330DLC_SetInt2Pin(LMS330DLC_CLICK_ON_PIN_INT2_ENABLE/DISABLE | LMS330DLC_I2_INT1_ON_PIN_INT2_ENABLE/DISABLE |               
                   LMS330DLC_I2_INT2_ON_PIN_INT2_ENABLE/DISABLE | LMS330DLC_I2_BOOT_ON_INT2_ENABLE/DISABLE |                   
                   LMS330DLC_INT_ACTIVE_HIGH/LOW)
* Note           : To enable Interrupt signals on INT2 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetInt2Pin(LMS330DLC_IntPinConf_t pinConf) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG6, &value) )
    return MEMS_ERROR;
  
  value &= 0x00;
  value |= pinConf;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG6, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}  

#if 0

/*******************************************************************************
* Function Name  : LMS330DLC_SetClickCFG
* Description    : Set Click Interrupt config Function
* Input          : LMS330DLC_ZD_ENABLE/DISABLE | LMS330DLC_ZS_ENABLE/DISABLE  | LMS330DLC_YD_ENABLE/DISABLE  | 
                   LMS330DLC_YS_ENABLE/DISABLE | LMS330DLC_XD_ENABLE/DISABLE  | LMS330DLC_XS_ENABLE/DISABLE 
* example        : LMS330DLC_SetClickCFG( LMS330DLC_ZD_ENABLE | LMS330DLC_ZS_DISABLE | LMS330DLC_YD_ENABLE | 
                               LMS330DLC_YS_DISABLE | LMS330DLC_XD_ENABLE | LMS330DLC_XS_ENABLE)
* Note           : You MUST use all input variable in the argument, as example
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetClickCFG(u8_t status) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CLICK_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0xC0;
  value |= status;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CLICK_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}  


/*******************************************************************************
* Function Name  : LMS330DLC_SetClickTHS
* Description    : Set Click Interrupt threshold
* Input          : Click-click Threshold value [0-127]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetClickTHS(u8_t ths) {
  
  if(ths>127)     
    return MEMS_ERROR;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CLICK_THS, ths) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LMS330DLC_SetClickLIMIT
* Description    : Set Click Interrupt Time Limit
* Input          : Click-click Time Limit value [0-127]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetClickLIMIT(u8_t t_limit) {
  
  if(t_limit>127)     
    return MEMS_ERROR;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_TIME_LIMIT, t_limit) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LMS330DLC_SetClickLATENCY
* Description    : Set Click Interrupt Time Latency
* Input          : Click-click Time Latency value [0-255]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetClickLATENCY(u8_t t_latency) {
  
  if( !LMS330DLC_WriteReg(LMS330DLC_TIME_LATENCY, t_latency) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LMS330DLC_SetClickWINDOW
* Description    : Set Click Interrupt Time Window
* Input          : Click-click Time Window value [0-255]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetClickWINDOW(u8_t t_window) {
  
  if( !LMS330DLC_WriteReg(LMS330DLC_TIME_WINDOW, t_window) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetClickResponse
* Description    : Get Click Interrupt Response by CLICK_SRC REGISTER
* Input          : char to empty by Click Response Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetClickResponse(u8_t* res) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CLICK_SRC, &value) ) 
    return MEMS_ERROR;
  
  value &= 0x7F;
  
  if((value & LMS330DLC_IA)==0) {        
    *res = LMS330DLC_NO_CLICK;     
    return MEMS_SUCCESS;
  }
  else {
    if (value & LMS330DLC_DCLICK){
      if (value & LMS330DLC_CLICK_SIGN){
        if (value & LMS330DLC_CLICK_Z) {
          *res = LMS330DLC_DCLICK_Z_N;   
          return MEMS_SUCCESS;
        }
        if (value & LMS330DLC_CLICK_Y) {
          *res = LMS330DLC_DCLICK_Y_N;   
          return MEMS_SUCCESS;
        }
        if (value & LMS330DLC_CLICK_X) {
          *res = LMS330DLC_DCLICK_X_N;   
          return MEMS_SUCCESS;
        }
      }
      else{
        if (value & LMS330DLC_CLICK_Z) {
          *res = LMS330DLC_DCLICK_Z_P;   
          return MEMS_SUCCESS;
        }
        if (value & LMS330DLC_CLICK_Y) {
          *res = LMS330DLC_DCLICK_Y_P;   
          return MEMS_SUCCESS;
        }
        if (value & LMS330DLC_CLICK_X) {
          *res = LMS330DLC_DCLICK_X_P;   
          return MEMS_SUCCESS;
        }
      }       
    }
    else{
      if (value & LMS330DLC_CLICK_SIGN){
        if (value & LMS330DLC_CLICK_Z) {
          *res = LMS330DLC_SCLICK_Z_N;   
          return MEMS_SUCCESS;
        }
        if (value & LMS330DLC_CLICK_Y) {
          *res = LMS330DLC_SCLICK_Y_N;   
          return MEMS_SUCCESS;
        }
        if (value & LMS330DLC_CLICK_X) {
          *res = LMS330DLC_SCLICK_X_N;   
          return MEMS_SUCCESS;
        }
      }
      else{
        if (value & LMS330DLC_CLICK_Z) {
          *res = LMS330DLC_SCLICK_Z_P;   
          return MEMS_SUCCESS;
        }
        if (value & LMS330DLC_CLICK_Y) {
          *res = LMS330DLC_SCLICK_Y_P;   
          return MEMS_SUCCESS;
        }
        if (value & LMS330DLC_CLICK_X) {
          *res = LMS330DLC_SCLICK_X_P;   
          return MEMS_SUCCESS;
        }
      }
    }
  }
  return MEMS_ERROR;
} 


/*******************************************************************************
* Function Name  : LMS330DLC_Int1LatchEnable
* Description    : Enable Interrupt 1 Latching function
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_Int1LatchEnable(State_t latch) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  value &= 0xF7;
  value |= latch<<LMS330DLC_LIR_INT1;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}
#endif

/*******************************************************************************
* Function Name  : LMS330DLC_ResetInt1Latch
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_ResetInt1Latch(void) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_INT1_SRC, &value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetIntConfiguration
* Description    : Interrupt 1 Configuration (without LMS330DLC_6D_INT)
* Input          : LMS330DLC_INT1_AND/OR | LMS330DLC_INT1_ZHIE_ENABLE/DISABLE | LMS330DLC_INT1_ZLIE_ENABLE/DISABLE...
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetIntConfiguration(LMS330DLC_Int1Conf_t ic) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_INT1_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0x40; 
  value |= ic;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 

     
/*******************************************************************************
* Function Name  : LMS330DLC_SetIntMode
* Description    : Interrupt 1 Configuration mode (OR, 6D Movement, AND, 6D Position)
* Input          : LMS330DLC_INT_MODE_OR, LMS330DLC_INT_MODE_6D_MOVEMENT, LMS330DLC_INT_MODE_AND, 
				   LMS330DLC_INT_MODE_6D_POSITION
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetIntMode(LMS330DLC_Int1Mode_t int_mode) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_INT1_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0x3F; 
  value |= (int_mode<<LMS330DLC_INT_6D);
  
  if( !LMS330DLC_WriteReg(LMS330DLC_INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

    
/*******************************************************************************
* Function Name  : LMS330DLC_SetInt6D4DConfiguration
* Description    : 6D, 4D Interrupt Configuration
* Input          : LMS330DLC_INT1_6D_ENABLE, LMS330DLC_INT1_4D_ENABLE, LMS330DLC_INT1_6D_4D_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetInt6D4DConfiguration(LMS330DLC_INT_6D_4D_t ic) {
  u8_t value;
  u8_t value2;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_INT1_CFG, &value) )
    return MEMS_ERROR;
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG5, &value2) )
    return MEMS_ERROR;
  
  if(ic == LMS330DLC_INT1_6D_ENABLE){
    value &= 0xBF; 
    value |= (MEMS_ENABLE<<LMS330DLC_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_DISABLE<<LMS330DLC_D4D_INT1);
  }
  
  if(ic == LMS330DLC_INT1_4D_ENABLE){
    value &= 0xBF; 
    value |= (MEMS_ENABLE<<LMS330DLC_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_ENABLE<<LMS330DLC_D4D_INT1);
  }
  
  if(ic == LMS330DLC_INT1_6D_4D_DISABLE){
    value &= 0xBF; 
    value |= (MEMS_DISABLE<<LMS330DLC_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_DISABLE<<LMS330DLC_D4D_INT1);
  }
  
  if( !LMS330DLC_WriteReg(LMS330DLC_INT1_CFG, value) )
    return MEMS_ERROR;
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG5, value2) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_Get6DPosition
* Description    : 6D, 4D Interrupt Position Detect
* Input          : Byte to empty by POSITION_6D_t Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_Get6DPosition(u8_t* val){
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_INT1_SRC, &value) )
    return MEMS_ERROR;
  
  value &= 0x7F;
  
  switch (value){
  case LMS330DLC_UP_SX:   
    *val = LMS330DLC_UP_SX;    
    break;
  case LMS330DLC_UP_DX:   
    *val = LMS330DLC_UP_DX;    
    break;
  case LMS330DLC_DW_SX:   
    *val = LMS330DLC_DW_SX;    
    break;
  case LMS330DLC_DW_DX:   
    *val = LMS330DLC_DW_DX;    
    break;
  case LMS330DLC_TOP:     
    *val = LMS330DLC_TOP;      
    break;
  case LMS330DLC_BOTTOM:  
    *val = LMS330DLC_BOTTOM;   
    break;
  }
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetInt1Threshold
* Description    : Sets Interrupt 1 Threshold
* Input          : Threshold = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetInt1Threshold(u8_t ths) {
  if (ths > 127)
    return MEMS_ERROR;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_INT1_THS, ths) )
    return MEMS_ERROR;    
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetInt1Duration
* Description    : Sets Interrupt 1 Duration
* Input          : Duration value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetInt1Duration(LMS330DLC_Int1Conf_t id) {
  
  if (id > 127)
    return MEMS_ERROR;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_INT1_DURATION, id) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_FIFOModeEnable
* Description    : Sets Fifo Modality
* Input          : LMS330DLC_FIFO_DISABLE, LMS330DLC_FIFO_BYPASS_MODE, LMS330DLC_FIFO_MODE, 
				   LMS330DLC_FIFO_STREAM_MODE, LMS330DLC_FIFO_TRIGGER_MODE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_FIFOModeEnable(LMS330DLC_FifoMode_t fm) {
  u8_t value;  
  
  if(fm == LMS330DLC_FIFO_DISABLE) { 
    if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1F;
    value |= (LMS330DLC_FIFO_BYPASS_MODE<<LMS330DLC_FM);                     
    
    if( !LMS330DLC_WriteReg(LMS330DLC_FIFO_CTRL_REG, value) )           //fifo mode bypass
      return MEMS_ERROR;   
    if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;    
    
    if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG5, value) )               //fifo disable
      return MEMS_ERROR;   
  }
  
  if(fm == LMS330DLC_FIFO_BYPASS_MODE)   {  
    if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LMS330DLC_FIFO_EN;
    
    if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;  
    if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LMS330DLC_FM);                     //fifo mode configuration
    
    if( !LMS330DLC_WriteReg(LMS330DLC_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LMS330DLC_FIFO_MODE)   {
    if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LMS330DLC_FIFO_EN;
    
    if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;  
    if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LMS330DLC_FM);                      //fifo mode configuration
    
    if( !LMS330DLC_WriteReg(LMS330DLC_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LMS330DLC_FIFO_STREAM_MODE)   {  
    if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LMS330DLC_FIFO_EN;
    
    if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;   
    if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LMS330DLC_FM);                      //fifo mode configuration
    
    if( !LMS330DLC_WriteReg(LMS330DLC_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LMS330DLC_FIFO_TRIGGER_MODE)   {  
    if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LMS330DLC_FIFO_EN;
    
    if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;    
    if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LMS330DLC_FM);                      //fifo mode configuration
    
    if( !LMS330DLC_WriteReg(LMS330DLC_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetTriggerInt
* Description    : Trigger event liked to trigger signal INT1/INT2
* Input          : LMS330DLC_TRIG_INT1/LMS330DLC_TRIG_INT2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetTriggerInt(LMS330DLC_TrigInt_t tr) {
  u8_t value;  
  
  if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xDF;
  value |= (tr<<LMS330DLC_TR); 
  
  if( !LMS330DLC_WriteReg(LMS330DLC_FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_SetWaterMark
* Description    : Sets Watermark Value
* Input          : Watermark = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetWaterMark(u8_t wtm) {
  u8_t value;
  
  if(wtm > 31)
    return MEMS_ERROR;  
  
  if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xE0;
  value |= wtm; 
  
  if( !LMS330DLC_WriteReg(LMS330DLC_FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

  
/*******************************************************************************
* Function Name  : LMS330DLC_GetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetStatusReg(u8_t* val) {
  if( !LMS330DLC_ReadReg(LMS330DLC_STATUS_REG, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetStatusBIT
* Description    : Read the status register BIT
* Input          : LMS330DLC_STATUS_REG_ZYXOR, LMS330DLC_STATUS_REG_ZOR, LMS330DLC_STATUS_REG_YOR, LMS330DLC_STATUS_REG_XOR,
                   LMS330DLC_STATUS_REG_ZYXDA, LMS330DLC_STATUS_REG_ZDA, LMS330DLC_STATUS_REG_YDA, LMS330DLC_STATUS_REG_XDA, 
				   LMS330DLC_DATAREADY_BIT
				   val: Byte to be filled with the status bit	
* Output         : status register BIT
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetStatusBit(u8_t statusBIT, u8_t* val) {
  u8_t value;  
  
  if( !LMS330DLC_ReadReg(LMS330DLC_STATUS_REG, &value) )
    return MEMS_ERROR;
  
  switch (statusBIT){
  case LMS330DLC_STATUS_REG_ZYXOR:     
    if(value &= LMS330DLC_STATUS_REG_ZYXOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  case LMS330DLC_STATUS_REG_ZOR:       
    if(value &= LMS330DLC_STATUS_REG_ZOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  case LMS330DLC_STATUS_REG_YOR:       
    if(value &= LMS330DLC_STATUS_REG_YOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }                                 
  case LMS330DLC_STATUS_REG_XOR:       
    if(value &= LMS330DLC_STATUS_REG_XOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }     
  case LMS330DLC_STATUS_REG_ZYXDA:     
    if(value &= LMS330DLC_STATUS_REG_ZYXDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LMS330DLC_STATUS_REG_ZDA:       
    if(value &= LMS330DLC_STATUS_REG_ZDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LMS330DLC_STATUS_REG_YDA:       
    if(value &= LMS330DLC_STATUS_REG_YDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LMS330DLC_STATUS_REG_XDA:       
    if(value &= LMS330DLC_STATUS_REG_XDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }                                  
    
  }
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetAccAxesRaw
* Description    : Read the Acceleration Values Output Registers
* Input          : buffer to empity by AxesRaw_t Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetAccAxesRaw(AxesRaw_t* buff) {
  i16_t value;
  u8_t *valueL = (u8_t *)(&value);
  u8_t *valueH = ((u8_t *)(&value)+1);

  if( !LMS330DLC_ReadReg(LMS330DLC_OUT_X_L, valueL) )
    return MEMS_ERROR;

  if( !LMS330DLC_ReadReg(LMS330DLC_OUT_X_H, valueH) )
    return MEMS_ERROR;

  buff->AXIS_X = value;

  if(!LMS330DLC_ReadReg(LMS330DLC_OUT_Y_L, valueL))
    return MEMS_ERROR;

  if( !LMS330DLC_ReadReg(LMS330DLC_OUT_Y_H, valueH) )
    return MEMS_ERROR;

  buff->AXIS_Y = value;

  if( !LMS330DLC_ReadReg(LMS330DLC_OUT_Z_L, valueL) )
    return MEMS_ERROR;

  if( !LMS330DLC_ReadReg(LMS330DLC_OUT_Z_H, valueH) )
    return MEMS_ERROR;

  buff->AXIS_Z = value;

  return MEMS_SUCCESS; 
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetInt1Src
* Description    : Reset Interrupt 1 Latching function
* Input          : Char to empty by Int1 source value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetInt1Src(u8_t* val) {
  
  if( !LMS330DLC_ReadReg(LMS330DLC_INT1_SRC, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetInt1SrcBit
* Description    : Reset Interrupt 1 Latching function
* Input          : statusBIT: LMS330DLC_INT_SRC_IA, LMS330DLC_INT_SRC_ZH, LMS330DLC_INT_SRC_ZL.....
*                  val: Byte to be filled with the status bit
* Output         : None
* Return         : Status of BIT [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetInt1SrcBit(u8_t statusBIT, u8_t* val) {
  u8_t value;  
   
  if( !LMS330DLC_ReadReg(LMS330DLC_INT1_SRC, &value) )
      return MEMS_ERROR;
   
  if(statusBIT == LMS330DLC_INT1_SRC_IA){
    if(value &= LMS330DLC_INT1_SRC_IA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LMS330DLC_INT1_SRC_ZH){
    if(value &= LMS330DLC_INT1_SRC_ZH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LMS330DLC_INT1_SRC_ZL){
    if(value &= LMS330DLC_INT1_SRC_ZL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LMS330DLC_INT1_SRC_YH){
    if(value &= LMS330DLC_INT1_SRC_YH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LMS330DLC_INT1_SRC_YL){
    if(value &= LMS330DLC_INT1_SRC_YL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  if(statusBIT == LMS330DLC_INT1_SRC_XH){
    if(value &= LMS330DLC_INT1_SRC_XH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LMS330DLC_INT1_SRC_XL){
    if(value &= LMS330DLC_INT1_SRC_XL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetFifoSourceReg
* Description    : Read Fifo source Register
* Input          : Byte to empty by FIFO source register value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetFifoSourceReg(u8_t* val) {
  
  if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_SRC_REG, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetFifoSourceBit
* Description    : Read Fifo WaterMark source bit
* Input          : statusBIT: LMS330DLC_FIFO_SRC_WTM, LMS330DLC_FIFO_SRC_OVRUN, LMS330DLC_FIFO_SRC_EMPTY
*				   val: Byte to fill  with the bit value
* Output         : None
* Return         : Status of BIT [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_GetFifoSourceBit(u8_t statusBIT,  u8_t* val){
  u8_t value;  
  
  if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_SRC_REG, &value) )
    return MEMS_ERROR;
  
  
  if(statusBIT == LMS330DLC_FIFO_SRC_WTM){
    if(value &= LMS330DLC_FIFO_SRC_WTM){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LMS330DLC_FIFO_SRC_OVRUN){
    if(value &= LMS330DLC_FIFO_SRC_OVRUN){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  if(statusBIT == LMS330DLC_FIFO_SRC_EMPTY){
    if(value &= statusBIT == LMS330DLC_FIFO_SRC_EMPTY){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LMS330DLC_GetFifoSourceFSS
* Description    : Read current number of unread samples stored in FIFO
* Input          : Byte to empty by FIFO unread sample value
* Output         : None
* Return         : Status [value of FSS]
*******************************************************************************/
status_t LMS330DLC_GetFifoSourceFSS(u8_t* val){
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_FIFO_SRC_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0x1F;
  
  *val = value;
  
  return MEMS_SUCCESS;
}

      
/*******************************************************************************
* Function Name  : LMS330DLC_SetSPIInterface
* Description    : Set SPI mode: 3 Wire Interface OR 4 Wire Interface
* Input          : LMS330DLC_SPI_3_WIRE, LMS330DLC_SPI_4_WIRE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LMS330DLC_SetSPIInterface(LMS330DLC_SPIMode_t spi) {
  u8_t value;
  
  if( !LMS330DLC_ReadReg(LMS330DLC_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xFE;
  value |= spi<<LMS330DLC_SIM;
  
  if( !LMS330DLC_WriteReg(LMS330DLC_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/**
* @brief Sets threshold for acceleration.
* @param nData: Acceleration threshold.
*        This parameter is a uint16_t.
* @param xIRQLine: IRQ threshold mask to be set.
*        This parameter is a @ref LSMAIrqLine .
* @retval None
*/
void LMS330DLC_AccSetThreshold(int16_t nData)
{
  uint8_t ctrl4 ;
  uint8_t accThs = nData; //wei,lhl,20160822<<LMS330DLC_FS)

  /* Read the register content */
//  LMS330DLC_ReadReg(LMS330DLC_CTRL_REG4,&ctrl4);

//  /* switch the sensitivity value set in the CRTL4*/
//  switch(ctrl4&0x30)
//  {
//      case LMS330DLC_FULLSCALE_2:
//          accThs = (uint8_t)((float)(nData/16)*LSM_Acc_Sensitivity_2g);
//      break;
//      case LMS330DLC_FULLSCALE_4:
//          accThs = (uint8_t)((float)(nData/16)*LSM_Acc_Sensitivity_4g);
//      break;
//      case LMS330DLC_FULLSCALE_8:
//          accThs = (uint8_t)((float)(nData/16)*LSM_Acc_Sensitivity_8g);
//      break;
//      case LMS330DLC_FULLSCALE_16:
//          accThs = (uint8_t)((float)(nData/16)*LSM_Acc_Sensitivity_16g);
//      break;
//  }
  /* Ensure that the MSb is 0 */
  accThs &= 0x7F;
  if(accThs>127)
      accThs = 50;
  LMS330DLC_SetInt1Threshold (accThs);
}

/**
* @brief  Returns the output data rate.
* @param  None.
* @retval Datarate in Hz.
*         This parameter is an uint16_t.
*/
uint16_t LMS330DLC_AccGetDataRate(void)
{
  uint8_t tmpReg;

  /* Read the register content */
  LMS330DLC_ReadReg( LMS330DLC_CTRL_REG1,&tmpReg);
  /* ..mask it */
  tmpReg &= 0xF0;

  /* return the correspondent value */
  switch(tmpReg>>LMS330DLC_ODR_BIT){
  case LMS330DLC_ODR_1Hz:
    return 1;
  case LMS330DLC_ODR_10Hz:
    return 10;
  case LMS330DLC_ODR_25Hz:
    return 25;
  case LMS330DLC_ODR_50Hz:
    return 50;
  case LMS330DLC_ODR_100Hz:
    return 100;
  case LMS330DLC_ODR_200Hz:
    return 200;
  case LMS330DLC_ODR_400Hz:
    return 400;
  case LMS330DLC_ODR_1620Hz_LP:
    return 1620;
  case LMS330DLC_ODR_1344Hz_NP_5367HZ_LP:
    return 1344;
  }

  return 0;
}
/**
* @brief Sets the minimum duration of an IRQ to be recognized.
* @param cDuration: Duration expressed in ms.
*        This parameter is a uint8_t .
* @param xIRQLine: IRQ duration to be set.
*        This parameter is a @ref LSMAIrqLine .
* @retval None
*/
void LMS330DLC_AccSetIrqDuration(uint8_t cDuration)
{
  uint8_t tempReg;

  /* Get datarate for time register value computation */
  uint16_t nDatarate = LMS330DLC_AccGetDataRate();

  /* Compute the duration register value */
  tempReg=(uint8_t)((float)cDuration/1000*nDatarate);

  LMS330DLC_SetInt1Duration(tempReg);
}

#if 0
/**
 * @defgroup Accelerometer_Private_Functions             Accelerometer Private Functions
 * @{
 */

/**
* @brief  Set configuration of Linear Acceleration measurement of LSM303DLHC
* @param  pxLSMAccInitStruct : pointer to a LSMAccInit structure that contains the configuration setting for the Accelerometer LSM303DLH.
* @retval None.
* @details
* <b>Example:</b>
* @code
*  LSMAccInit LSMAccInitStructure;
*
*  LSMAccInitStructure.xPowerMode = LSM_NORMAL_MODE;
*  LSMAccInitStructure.xOutputDataRate = LSM_ODR_400_HZ;
*  LSMAccInitStructure.xEnabledAxes= LSM_ALL_AXES_EN;
*  LSMAccInitStructure.xFullScale = LSM_FS_2G;
*  LSMAccInitStructure.xDataUpdate = LSM_CONTINUOS_UPDATE;
*  LSMAccInitStructure.xEndianness = LSM_BIG_ENDIAN;
*  LSMAccInitStructure.xHighResolution = LSM_ENABLE;
*
*  Lsm303dlhcAccConfig(&LSMAccInitStructure);
* @endcode
*/
void LMS330DLC_AccConfig(LSMAccInit* pxLSMAccInitStruct)
{
  uint8_t CTRL1 = 0x00, CTRL4  =  0x00;

  /* Read the CTRL4 register content */
  Lsm303dlhcAccI2CByteRead(&CTRL4, LSM_A_CTRL4_REG_ADDR);

  /* Compute the register values */
  CTRL1 |= (uint8_t) ((uint8_t)pxLSMAccInitStruct->xPowerMode | (uint8_t)pxLSMAccInitStruct->xOutputDataRate | (uint8_t)pxLSMAccInitStruct->xEnabledAxes);
  CTRL4 |= (uint8_t) ((uint8_t)pxLSMAccInitStruct->xFullScale | (uint8_t)pxLSMAccInitStruct->xDataUpdate | (uint8_t)pxLSMAccInitStruct->xEndianness);
  if(pxLSMAccInitStruct->xHighResolution == LSM_ENABLE)
  {
    CTRL4 |= 0x08;
  }
  else
  {
    CTRL4 &= 0xF0;
  }

  /* Write the computed values on registers */
  Lsm303dlhcAccI2CByteWrite(&CTRL1, LSM_A_CTRL1_REG_ADDR);
  Lsm303dlhcAccI2CByteWrite(&CTRL4, LSM_A_CTRL4_REG_ADDR);

}





/**
* @brief  Set configuration of  Internal High Pass Filter of  LSM303DLHC for the linear acceleration
* @param  pxLSMAccFilterInitStruct : pointer to a LSMAccFilterInit structure that
*         contains the configuration setting for the LSM303DLHC.
* @retval None
* @details
* <b>Example:</b>
* @code
*  LSMAccFilterInit LSMAccFilterInitStructure;
*
*  LSMAccFilterInitStructure.xHPF=LSM_DISABLE;
*  LSMAccFilterInitStructure.xHPF_Mode=LSM_HPFM_NORMAL;
*  LSMAccFilterInitStructure.cHPFReference=0x00;
*  LSMAccFilterInitStructure.xHPFCutOff=LSM_HPCF_16;
*  LSMAccFilterInitStructure.xHPFClick=LSM_DISABLE;
*  LSMAccFilterInitStructure.xHPFAOI2=LSM_DISABLE;
*  LSMAccFilterInitStructure.xHPFAOI1=LSM_DISABLE;
*
*  Lsm303dlhcAccFilterConfig(&LSMAccFilterInitStructure);
* @endcode
*/
void Lsm303dlhcAccFilterConfig(LSMAccFilterInit* pxLSMAccFilterInitStruct)
{
  uint8_t CTRL2 = 0x00;
  uint8_t REF  =  0x00;

  /* Compute the register values */
  CTRL2 |= (uint8_t) ((uint8_t)pxLSMAccFilterInitStruct->xHPF_Mode| (uint8_t)pxLSMAccFilterInitStruct->xHPFCutOff);
  if(pxLSMAccFilterInitStruct->xHPF == LSM_ENABLE)
  {
    CTRL2 |= 0x08;
  }
  else
  {
    CTRL2 &= 0xF7;
  }
  if(pxLSMAccFilterInitStruct->xHPFClick == LSM_ENABLE)
  {
    CTRL2 |= 0x04;
  }
  else
  {
    CTRL2 &= 0xFB;
  }
  if(pxLSMAccFilterInitStruct->xHPFAOI2 == LSM_ENABLE)
  {
    CTRL2 |= 0x02;
  }
  else
  {
    CTRL2 &= 0xFD;
  }
  if(pxLSMAccFilterInitStruct->xHPFAOI1 == LSM_ENABLE)
  {
    CTRL2 |= 0x01;
  }
  else
  {
    CTRL2 &= 0xFE;
  }

  REF |= (uint8_t) (pxLSMAccFilterInitStruct->cHPFReference);

  /* Write the computed values on registers */
  Lsm303dlhcAccI2CByteWrite(&CTRL2, LSM_A_CTRL2_REG_ADDR);
  Lsm303dlhcAccI2CByteWrite(&REF, LSM_A_REFERENCE_REG_ADDR);
}


#endif


/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
