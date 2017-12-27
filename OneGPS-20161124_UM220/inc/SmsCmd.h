
#ifndef _I_SMS_
#define _I_SMS_

#define maxsms   10 
#define COMPANY_FLAG          ("fht")
typedef enum smsenum
{
	position=0,
	init,
	newnum,
	delnum,
	delall,
	shownum,
	setpsw,
	net,
	setuptime,
	imei,
	devinfo,
	readspeedlimit,
	speedlimit,
	nolimit,
	cellid,
	mid,
	mode,
	period,
	acc,
	autosms,
	duration,
	sleeptime,
	chinese_position,
	relay,
	shocks
}_smsenum;

#define PHNUM_LEN 32
typedef struct smsdata
{       
    unsigned char count;
	unsigned char _smscommand[3];

//wei,20160708      unsigned char _smscontent[3][60];
    unsigned char _smscontent[3][130];//wei,20160708

//ljw,20160924for wulian	unsigned char mobilenum[3][11];
    unsigned char mobilenum[3][PHNUM_LEN];//ljw,20160924for wulian
} _smsdata;

extern _smsdata smsdatareg;

void Cmd_Position(void); 

void Cmd_LocaLimit(void);	
void Cmd_ReadSpeedLimit(void);
void Cmd_SetNetPsw(void);
void Cmd_SetMode(void);
void Cmd_Setmid(void);
void Cmd_SetNetUser(void)  ;
void Cmd_Contrack(void); 	
void Cmd_Initial(void);		
void Cmd_NewNum(void);		
void Cmd_DelNum(void);		
void Cmd_SetPsw(void);		
void Cmd_SetIPAddr(void); 
void Cmd_SetAPN(void);
void Cmd_SetIPPort(void); 
void Cmd_SetUpTime(void);
void Cmd_CallNum(void);
void Cmd_ShowRegNum(void);
void Cmd_SetCall_ANum(void);
void Cmd_SetCall_BNum(void);
void Cmd_EditRegNum(void);
void Cmd_SetTimePrieod(void);
void Cmd_SetWakeTime(void);
void Cmd_GPRSMode(void);
void Cmd_IMEI(void);
void Cmd_TrackMode(void);
void Cmd_DeviceInfo(void);
void Cmd_SpeedLimit(void);
void Cmd_CellID(void);
void Cmd_ACC(void);

void Cmd_ENAUTOSMS(void);
void Cmd_Duration(void);
void Cmd_SleepTime(void);
//Cmd_ENAUTOSMS
void analysis_sms(char * ptmp);
void setSMS(void);

#endif
