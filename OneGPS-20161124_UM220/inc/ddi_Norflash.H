#ifndef _ddi_Norflash_h
#define _ddi_Norflash_h




/**********************************************************************
//说明	:初始化平衡区
//参数	:平衡区数据结构
//返回	:

**********************************************************************/
void mileage_init(void);


// 把里程数据写进flash
unsigned char mileage_write(void);

#endif

