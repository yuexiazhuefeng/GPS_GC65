/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : LMS330DLC_driver.h
* Author             : MSH Application Team
* Author             : Abhishek Anand, Fabio Tota
* Version            : $Revision:$
* Date               : $Date:$
* Description        : Descriptor Header for LMS330DLC_driver.c driver file
*
* HISTORY:
* Date        | Modification                                | Author
* 24/06/2011  | Initial Revision                            | Fabio Tota
* 07/06/2012  | Support for multiple drivers in the same program | Abhishek Anand
*
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LMS330DLC_DRIVER__H
#define __LMS330DLC_DRIVER__H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/


//these could change accordingly with the architecture

#ifndef __ARCHDEP__TYPES
#define __ARCHDEP__TYPES

typedef unsigned char u8_t;
typedef unsigned short int u16_t;
typedef short int i16_t;
typedef signed char i8_t;

#endif /*__ARCHDEP__TYPES*/

typedef u8_t LMS330DLC_IntPinConf_t;
typedef u8_t LMS330DLC_Axis_t;
typedef u8_t LMS330DLC_Int1Conf_t;


//define structure
#ifndef __SHARED__TYPES
#define __SHARED__TYPES

typedef enum {
  MEMS_SUCCESS				=		0x01,
  MEMS_ERROR				=		0x00	
} status_t;

typedef enum {
  MEMS_ENABLE				=		0x01,
  MEMS_DISABLE				=		0x00	
} State_t;

typedef struct {
  i16_t AXIS_X;
  i16_t AXIS_Y;
  i16_t AXIS_Z;
} AxesRaw_t;

#endif /*__SHARED__TYPES*/

typedef enum {  
  LMS330DLC_ODR_1Hz		        =		0x01,		
  LMS330DLC_ODR_10Hz            =		0x02,
  LMS330DLC_ODR_25Hz		        =		0x03,
  LMS330DLC_ODR_50Hz		        =		0x04,
  LMS330DLC_ODR_100Hz		        =		0x05,	
  LMS330DLC_ODR_200Hz		        =		0x06,
  LMS330DLC_ODR_400Hz		        =		0x07,
  LMS330DLC_ODR_1620Hz_LP		        =		0x08,
  LMS330DLC_ODR_1344Hz_NP_5367HZ_LP       =		0x09	
} LMS330DLC_ODR_t;

typedef enum {
  LMS330DLC_POWER_DOWN       =		0x00,
  LMS330DLC_LOW_POWER 			=		0x01,
  LMS330DLC_NORMAL			=		0x02
} LMS330DLC_Mode_t;

//High-pass filter mode configuration
typedef enum {
  LMS330DLC_HPM_NORMAL_MODE_RES           =               0x00,
  LMS330DLC_HPM_REF_SIGNAL                =               0x01,
  LMS330DLC_HPM_NORMAL_MODE               =               0x02,
  LMS330DLC_HPM_AUTORESET_INT             =               0x03
} LMS330DLC_HPFMode_t;

//High-pass filter cut-off frequency configuration
typedef enum {
  LMS330DLC_HPFCF_0                       =               0x00,
  LMS330DLC_HPFCF_1                       =               0x01,
  LMS330DLC_HPFCF_2                       = 		0x02,
  LMS330DLC_HPFCF_3                       =               0x03
} LMS330DLC_HPFCutOffFreq_t;

//CTRL_REG4_A description
typedef enum {
  LMS330DLC_FULLSCALE_2                   =               0x00,
  LMS330DLC_FULLSCALE_4                   =               0x01,
  LMS330DLC_FULLSCALE_8                   =               0x02,
  LMS330DLC_FULLSCALE_16                  =               0x03
} LMS330DLC_Fullscale_t;

//Big/little endian data selection.
typedef enum {
  LMS330DLC_BLE_LSB			=		0x00,
  LMS330DLC_BLE_MSB			=		0x01
} LMS330DLC_Endianess_t;


typedef enum {
  LMS330DLC_FIFO_BYPASS_MODE              =               0x00,
  LMS330DLC_FIFO_MODE                     =               0x01,
  LMS330DLC_FIFO_STREAM_MODE              =               0x02,
  LMS330DLC_FIFO_TRIGGER_MODE             =               0x03,
  LMS330DLC_FIFO_DISABLE                  =               0x04
} LMS330DLC_FifoMode_t;

typedef enum {
  LMS330DLC_TRIG_INT1                     =		0x00,
  LMS330DLC_TRIG_INT2 			=		0x01
} LMS330DLC_TrigInt_t;

typedef enum {
  LMS330DLC_SPI_4_WIRE                    =               0x00,
  LMS330DLC_SPI_3_WIRE                    =               0x01
} LMS330DLC_SPIMode_t;

typedef enum {
  LMS330DLC_X_ENABLE                      =               0x01,
  LMS330DLC_X_DISABLE                     =               0x00,
  LMS330DLC_Y_ENABLE                      =               0x02,
  LMS330DLC_Y_DISABLE                     =               0x00,
  LMS330DLC_Z_ENABLE                      =               0x04,
  LMS330DLC_Z_DISABLE                     =               0x00    
} LMS330DLC_AXISenable_t;

typedef enum {
  LMS330DLC_INT1_6D_4D_DISABLE            =               0x00,
  LMS330DLC_INT1_6D_ENABLE                =               0x01,
  LMS330DLC_INT1_4D_ENABLE                =               0x02 
} LMS330DLC_INT_6D_4D_t;

typedef enum {
  LMS330DLC_UP_SX                         =               0x44,
  LMS330DLC_UP_DX                         =               0x42,
  LMS330DLC_DW_SX                         =               0x41,
  LMS330DLC_DW_DX                         =               0x48,
  LMS330DLC_TOP                           =               0x60,
  LMS330DLC_BOTTOM                        =               0x50
} LMS330DLC_POSITION_6D_t;

typedef enum {
  LMS330DLC_INT_MODE_OR                   =               0x00,
  LMS330DLC_INT_MODE_6D_MOVEMENT          =               0x01,
  LMS330DLC_INT_MODE_AND                  =               0x02,
  LMS330DLC_INT_MODE_6D_POSITION          =               0x03  
} LMS330DLC_Int1Mode_t;


//interrupt click response
//  b7 = don't care   b6 = IA  b5 = DClick  b4 = Sclick  b3 = Sign  
//  b2 = z      b1 = y     b0 = x
typedef enum {
LMS330DLC_DCLICK_Z_P                      =               0x24,
LMS330DLC_DCLICK_Z_N                      =               0x2C,
LMS330DLC_SCLICK_Z_P                      =               0x14,
LMS330DLC_SCLICK_Z_N                      =               0x1C,
LMS330DLC_DCLICK_Y_P                      =               0x22,
LMS330DLC_DCLICK_Y_N                      =               0x2A,
LMS330DLC_SCLICK_Y_P                      =               0x12,
LMS330DLC_SCLICK_Y_N			=		0x1A,
LMS330DLC_DCLICK_X_P                      =               0x21,
LMS330DLC_DCLICK_X_N                      =               0x29,
LMS330DLC_SCLICK_X_P                      =               0x11,
LMS330DLC_SCLICK_X_N                      =               0x19,
LMS330DLC_NO_CLICK                        =               0x00
} LMS330DLC_Click_Response; 

//TODO: start from here and manage the shared macros etc before this

/* Exported constants --------------------------------------------------------*/

#ifndef __SHARED__CONSTANTS
#define __SHARED__CONSTANTS

#define MEMS_SET                                        0x01
#define MEMS_RESET                                      0x00

#endif /*__SHARED__CONSTANTS*/


//Register Definition
#define LMS330DLC_WHO_AM_I				0x0F  // device identification register

// CONTROL REGISTER 1
#define LMS330DLC_CTRL_REG1				0x20
#define LMS330DLC_ODR_BIT				        BIT(4)
#define LMS330DLC_LPEN					BIT(3)
#define LMS330DLC_ZEN					BIT(2)
#define LMS330DLC_YEN					BIT(1)
#define LMS330DLC_XEN					BIT(0)

//CONTROL REGISTER 2
#define LMS330DLC_CTRL_REG2				0x21
#define LMS330DLC_HPM     				BIT(6)
#define LMS330DLC_HPCF					BIT(4)
#define LMS330DLC_FDS					BIT(3)
#define LMS330DLC_HPCLICK					BIT(2)
#define LMS330DLC_HPIS2					BIT(1)
#define LMS330DLC_HPIS1					BIT(0)

//CONTROL REGISTER 3
#define LMS330DLC_CTRL_REG3				0x22
#define LMS330DLC_I1_CLICK				BIT(7)
#define LMS330DLC_I1_AOI1					BIT(6)
//#define LMS330DLC_I1_AOI2				        BIT(5)
#define LMS330DLC_I1_DRDY1				BIT(4)
#define LMS330DLC_I1_DRDY2				BIT(3)
#define LMS330DLC_I1_WTM					BIT(2)
#define LMS330DLC_I1_ORUN					BIT(1)

//CONTROL REGISTER 6
#define LMS330DLC_CTRL_REG6				0x25
#define LMS330DLC_I2_CLICK				BIT(7)
#define LMS330DLC_I2_INT1					BIT(6)
#define LMS330DLC_I2_BOOT         			BIT(4)
#define LMS330DLC_H_LACTIVE				BIT(1)

//TEMPERATURE CONFIG REGISTER
#define LMS330DLC_TEMP_CFG_REG				0x1F
#define LMS330DLC_ADC_PD				        BIT(7)
#define LMS330DLC_TEMP_EN					BIT(6)

//CONTROL REGISTER 4
#define LMS330DLC_CTRL_REG4				0x23

#define LMS330DLC_BLE					BIT(6)
#define LMS330DLC_FS					BIT(4)
#define LMS330DLC_HR					BIT(3)
#define LMS330DLC_ST       				BIT(1)
#define LMS330DLC_SIM					BIT(0)

//CONTROL REGISTER 5
#define LMS330DLC_CTRL_REG5				0x24
#define LMS330DLC_BOOT                                    BIT(7)
#define LMS330DLC_FIFO_EN                                 BIT(6)
#define LMS330DLC_LIR_INT1                                BIT(3)
#define LMS330DLC_D4D_INT1                                BIT(2)

//REFERENCE/DATA_CAPTURE
#define LMS330DLC_REFERENCE_REG		                0x26
#define LMS330DLC_REF		                	BIT(0)

//STATUS_REG_AXIES
#define LMS330DLC_STATUS_REG				0x27
#define LMS330DLC_ZYXOR                                   BIT(7)
#define LMS330DLC_ZOR                                     BIT(6)
#define LMS330DLC_YOR                                     BIT(5)
#define LMS330DLC_XOR                                     BIT(4)
#define LMS330DLC_ZYXDA                                   BIT(3)
#define LMS330DLC_ZDA                                     BIT(2)
#define LMS330DLC_YDA                                     BIT(1)
#define LMS330DLC_XDA                                     BIT(0)

//STATUS_REG_AUX
#define LMS330DLC_STATUS_AUX				0x07

//INTERRUPT 1 CONFIGURATION
#define LMS330DLC_INT1_CFG				0x30
#define LMS330DLC_ANDOR                                   BIT(7)
#define LMS330DLC_INT_6D                                  BIT(6)
#define LMS330DLC_ZHIE                                    BIT(5)
#define LMS330DLC_ZLIE                                    BIT(4)
#define LMS330DLC_YHIE                                    BIT(3)
#define LMS330DLC_YLIE                                    BIT(2)
#define LMS330DLC_XHIE                                    BIT(1)
#define LMS330DLC_XLIE                                    BIT(0)

//FIFO CONTROL REGISTER
#define LMS330DLC_FIFO_CTRL_REG                           0x2E
#define LMS330DLC_FM                                      BIT(6)
#define LMS330DLC_TR                                      BIT(5)
#define LMS330DLC_FTH                                     BIT(0)

//CONTROL REG3 bit mask
#define LMS330DLC_CLICK_ON_PIN_INT1_ENABLE                0x80
#define LMS330DLC_CLICK_ON_PIN_INT1_DISABLE               0x00
#define LMS330DLC_I1_INT1_ON_PIN_INT1_ENABLE              0x40
#define LMS330DLC_I1_INT1_ON_PIN_INT1_DISABLE             0x00
#define LMS330DLC_I1_INT2_ON_PIN_INT1_ENABLE              0x20
#define LMS330DLC_I1_INT2_ON_PIN_INT1_DISABLE             0x00
#define LMS330DLC_I1_DRDY1_ON_INT1_ENABLE                 0x10
#define LMS330DLC_I1_DRDY1_ON_INT1_DISABLE                0x00
#define LMS330DLC_I1_DRDY2_ON_INT1_ENABLE                 0x08
#define LMS330DLC_I1_DRDY2_ON_INT1_DISABLE                0x00
#define LMS330DLC_WTM_ON_INT1_ENABLE                      0x04
#define LMS330DLC_WTM_ON_INT1_DISABLE                     0x00
#define LMS330DLC_INT1_OVERRUN_ENABLE                     0x02
#define LMS330DLC_INT1_OVERRUN_DISABLE                    0x00

//CONTROL REG6 bit mask
#define LMS330DLC_CLICK_ON_PIN_INT2_ENABLE                0x80
#define LMS330DLC_CLICK_ON_PIN_INT2_DISABLE               0x00
#define LMS330DLC_I2_INT1_ON_PIN_INT2_ENABLE              0x40
#define LMS330DLC_I2_INT1_ON_PIN_INT2_DISABLE             0x00
#define LMS330DLC_I2_INT2_ON_PIN_INT2_ENABLE              0x20
#define LMS330DLC_I2_INT2_ON_PIN_INT2_DISABLE             0x00
#define LMS330DLC_I2_BOOT_ON_INT2_ENABLE                  0x10
#define LMS330DLC_I2_BOOT_ON_INT2_DISABLE                 0x00
#define LMS330DLC_INT_ACTIVE_HIGH                         0x00
#define LMS330DLC_INT_ACTIVE_LOW                          0x02

//INT1_CFG bit mask
#define LMS330DLC_INT1_AND                                0x80
#define LMS330DLC_INT1_OR                                 0x00
#define LMS330DLC_INT1_ZHIE_ENABLE                        0x20
#define LMS330DLC_INT1_ZHIE_DISABLE                       0x00
#define LMS330DLC_INT1_ZLIE_ENABLE                        0x10
#define LMS330DLC_INT1_ZLIE_DISABLE                       0x00
#define LMS330DLC_INT1_YHIE_ENABLE                        0x08
#define LMS330DLC_INT1_YHIE_DISABLE                       0x00
#define LMS330DLC_INT1_YLIE_ENABLE                        0x04
#define LMS330DLC_INT1_YLIE_DISABLE                       0x00
#define LMS330DLC_INT1_XHIE_ENABLE                        0x02
#define LMS330DLC_INT1_XHIE_DISABLE                       0x00
#define LMS330DLC_INT1_XLIE_ENABLE                        0x01
#define LMS330DLC_INT1_XLIE_DISABLE                       0x00

//INT1_SRC bit mask
#define LMS330DLC_INT1_SRC_IA                             0x40
#define LMS330DLC_INT1_SRC_ZH                             0x20
#define LMS330DLC_INT1_SRC_ZL                             0x10
#define LMS330DLC_INT1_SRC_YH                             0x08
#define LMS330DLC_INT1_SRC_YL                             0x04
#define LMS330DLC_INT1_SRC_XH                             0x02
#define LMS330DLC_INT1_SRC_XL                             0x01

//INT1 REGISTERS
#define LMS330DLC_INT1_THS                                0x32
#define LMS330DLC_INT1_DURATION                           0x33

//INTERRUPT 1 SOURCE REGISTER
#define LMS330DLC_INT1_SRC				0x31

//FIFO Source Register bit Mask
#define LMS330DLC_FIFO_SRC_WTM                            0x80
#define LMS330DLC_FIFO_SRC_OVRUN                          0x40
#define LMS330DLC_FIFO_SRC_EMPTY                          0x20
  
//INTERRUPT CLICK REGISTER
#define LMS330DLC_CLICK_CFG				0x38
//INTERRUPT CLICK CONFIGURATION bit mask
#define LMS330DLC_ZD_ENABLE                               0x20
#define LMS330DLC_ZD_DISABLE                              0x00
#define LMS330DLC_ZS_ENABLE                               0x10
#define LMS330DLC_ZS_DISABLE                              0x00
#define LMS330DLC_YD_ENABLE                               0x08
#define LMS330DLC_YD_DISABLE                              0x00
#define LMS330DLC_YS_ENABLE                               0x04
#define LMS330DLC_YS_DISABLE                              0x00
#define LMS330DLC_XD_ENABLE                               0x02
#define LMS330DLC_XD_DISABLE                              0x00
#define LMS330DLC_XS_ENABLE                               0x01
#define LMS330DLC_XS_DISABLE                              0x00

//INTERRUPT CLICK SOURCE REGISTER
#define LMS330DLC_CLICK_SRC                               0x39
//INTERRUPT CLICK SOURCE REGISTER bit mask
#define LMS330DLC_IA                                      0x40
#define LMS330DLC_DCLICK                                  0x20
#define LMS330DLC_SCLICK                                  0x10
#define LMS330DLC_CLICK_SIGN                              0x08
#define LMS330DLC_CLICK_Z                                 0x04
#define LMS330DLC_CLICK_Y                                 0x02
#define LMS330DLC_CLICK_X                                 0x01

//Click-click Register
#define LMS330DLC_CLICK_THS                               0x3A
#define LMS330DLC_TIME_LIMIT                              0x3B
#define LMS330DLC_TIME_LATENCY                            0x3C
#define LMS330DLC_TIME_WINDOW                             0x3D

//OUTPUT REGISTER
#define LMS330DLC_OUT_X_L					0x28
#define LMS330DLC_OUT_X_H					0x29
#define LMS330DLC_OUT_Y_L					0x2A
#define LMS330DLC_OUT_Y_H					0x2B
#define LMS330DLC_OUT_Z_L					0x2C
#define LMS330DLC_OUT_Z_H					0x2D



//STATUS REGISTER bit mask
#define LMS330DLC_STATUS_REG_ZYXOR                        0x80    // 1	:	new data set has over written the previous one
							// 0	:	no overrun has occurred (default)	
#define LMS330DLC_STATUS_REG_ZOR                          0x40    // 0	:	no overrun has occurred (default)
							// 1	:	new Z-axis data has over written the previous one
#define LMS330DLC_STATUS_REG_YOR                          0x20    // 0	:	no overrun has occurred (default)
							// 1	:	new Y-axis data has over written the previous one
#define LMS330DLC_STATUS_REG_XOR                          0x10    // 0	:	no overrun has occurred (default)
							// 1	:	new X-axis data has over written the previous one
#define LMS330DLC_STATUS_REG_ZYXDA                        0x08    // 0	:	a new set of data is not yet avvious one
                                                        // 1	:	a new set of data is available 
#define LMS330DLC_STATUS_REG_ZDA                          0x04    // 0	:	a new data for the Z-Axis is not availvious one
                                                        // 1	:	a new data for the Z-Axis is available
#define LMS330DLC_STATUS_REG_YDA                          0x02    // 0	:	a new data for the Y-Axis is not available
                                                        // 1	:	a new data for the Y-Axis is available
#define LMS330DLC_STATUS_REG_XDA                          0x01    // 0	:	a new data for the X-Axis is not available

#define LMS330DLC_DATAREADY_BIT                           LMS330DLC_STATUS_REG_ZYXDA


//STATUS AUX REGISTER bit mask
#define LMS330DLC_STATUS_AUX_321OR                         0x80
#define LMS330DLC_STATUS_AUX_3OR                           0x40
#define LMS330DLC_STATUS_AUX_2OR                           0x20
#define LMS330DLC_STATUS_AUX_1OR                           0x10
#define LMS330DLC_STATUS_AUX_321DA                         0x08
#define LMS330DLC_STATUS_AUX_3DA                           0x04
#define LMS330DLC_STATUS_AUX_2DA                           0x02
#define LMS330DLC_STATUS_AUX_1DA                           0x01

#define LMS330DLC_MEMS_I2C_ADDRESS			        0x33

//FIFO REGISTERS
#define LMS330DLC_FIFO_CTRL_REG			        0x2E
#define LMS330DLC_FIFO_SRC_REG			        0x2F


/* Exported macro ------------------------------------------------------------*/

#ifndef __SHARED__MACROS

#define __SHARED__MACROS
#define ValBit(VAR,Place)         (VAR & (1<<Place))
#define BIT(x) ( (x) )

#endif /*__SHARED__MACROS*/

#define LSM_Acc_Sensitivity_2g     1.0    /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     0.5    /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     0.25   /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    0.0834 /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */




/* Exported functions --------------------------------------------------------*/
//Sensor Configuration Functions
status_t LMS330DLC_SetODR(LMS330DLC_ODR_t ov);
status_t LMS330DLC_SetMode(LMS330DLC_Mode_t md);
status_t LMS330DLC_SetAxis(LMS330DLC_Axis_t axis);
status_t LMS330DLC_SetFullScale(LMS330DLC_Fullscale_t fs);
status_t LMS330DLC_SetBDU(State_t bdu);
status_t LMS330DLC_SetBLE(LMS330DLC_Endianess_t ble);
//status_t LMS330DLC_SetSelfTest(LMS330DLC_SelfTest_t st);
status_t LMS330DLC_SetTemperature(State_t state);
status_t LMS330DLC_SetADCAux(State_t state);

//Filtering Functions
status_t LMS330DLC_HPFClickEnable(State_t hpfe);
status_t LMS330DLC_HPFAOI1Enable(State_t hpfe);
status_t LMS330DLC_HPFAOI2Enable(State_t hpfe);
status_t LMS330DLC_SetHPFMode(LMS330DLC_HPFMode_t hpf);
status_t LMS330DLC_SetHPFCutOFF(LMS330DLC_HPFCutOffFreq_t hpf);
status_t LMS330DLC_SetFilterDataSel(State_t state);

//Interrupt Functions
status_t LMS330DLC_SetInt1Pin(LMS330DLC_IntPinConf_t pinConf);
status_t LMS330DLC_SetInt2Pin(LMS330DLC_IntPinConf_t pinConf);
status_t LMS330DLC_Int1LatchEnable(State_t latch);
status_t LMS330DLC_ResetInt1Latch(void);
status_t LMS330DLC_SetIntConfiguration(LMS330DLC_Int1Conf_t ic);
status_t LMS330DLC_SetInt1Threshold(u8_t ths);
status_t LMS330DLC_SetInt1Duration(LMS330DLC_Int1Conf_t id);
status_t LMS330DLC_SetIntMode(LMS330DLC_Int1Mode_t ic);
status_t LMS330DLC_SetClickCFG(u8_t status);
status_t LMS330DLC_SetInt6D4DConfiguration(LMS330DLC_INT_6D_4D_t ic);
status_t LMS330DLC_GetInt1Src(u8_t* val);
status_t LMS330DLC_GetInt1SrcBit(u8_t statusBIT, u8_t* val);

//FIFO Functions
status_t LMS330DLC_FIFOModeEnable(LMS330DLC_FifoMode_t fm);
status_t LMS330DLC_SetWaterMark(u8_t wtm);
status_t LMS330DLC_SetTriggerInt(LMS330DLC_TrigInt_t tr);
status_t LMS330DLC_GetFifoSourceReg(u8_t* val);
status_t LMS330DLC_GetFifoSourceBit(u8_t statusBIT, u8_t* val);
status_t LMS330DLC_GetFifoSourceFSS(u8_t* val);

//Other Reading Functions
status_t LMS330DLC_GetStatusReg(u8_t* val);
status_t LMS330DLC_GetStatusBit(u8_t statusBIT, u8_t* val);
status_t LMS330DLC_GetStatusAUXBit(u8_t statusBIT, u8_t* val);
status_t LMS330DLC_GetStatusAUX(u8_t* val);
status_t LMS330DLC_GetAccAxesRaw(AxesRaw_t* buff);
//status_t LMS330DLC_GetAuxRaw(LMS330DLC_Aux123Raw_t* buff);
status_t LMS330DLC_GetClickResponse(u8_t* val);
status_t LMS330DLC_GetTempRaw(i8_t* val);
status_t LMS330DLC_GetWHO_AM_I(u8_t* val);
status_t LMS330DLC_Get6DPosition(u8_t* val);

//Generic
status_t LMS330DLC_SetClickTHS(u8_t ths) ;
status_t LMS330DLC_SetClickLIMIT(u8_t t_limit);
 u8_t LMS330DLC_ReadReg(u8_t Reg, u8_t* Data);
 u8_t LMS330DLC_WriteReg(u8_t Reg, u8_t Data);
 void LMS330DLC_AccSetIrqDuration(uint8_t cDuration);
void LMS330DLC_AccSetThreshold(int16_t nData);
extern u8_t set_Impact(u8_t accelero,u8_t time);//wei,20160803

#endif /* __LMS330DLC_DRIVER__H */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/



