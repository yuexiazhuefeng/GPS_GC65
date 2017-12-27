#include "ddi_Norflash.h"
#include "main.h" 

/*
　　关于里程：是随位置信息上发的，在附加字段里。因为做了不定位就使用上一次数据的方法，不定位时（隧道，涵洞，地下车库），可能误记
所以只有打开ACC功能，ACC信号有效时才做统计，低于5KM/H就不记录。每秒把速度做累加（节），(n*1814/3600 )/100 ，上传是１／１０ｋｍ为单位。
要记录了，因为存储，要开一个专用区，和系统参数分开，防止ｆｌａｓｈ坏掉。一直在线的每小时存一次，或电压低时存，检测到外接电源断了也要存。
ｆｌａｓｈ的寿命10000次，到时候可能要做个平衡区。1024字节是一块，使用两块做，其中一块做备份，每次写的时候都写两块。每一块的第一个ｄｗｏｒｄ作为标志0xa5（一字节）和擦除次数（三个字节）

原来的平衡区是这样的。因为只保存一个数据，简化一下。只留下数据，初始化是擦除成０ｘｆｆ，每个数据DWORD占有４字节。直接寻找最大数，０ｘｆｆｆｆｆｆｆ要忽略
另外恢复出厂设置时也要把平衡区擦除掉。
如果使用两块备份的，先检查第一快，要是能读出来，就比较第二块的的数据，两块不一样，用第一块的
填充到第二块，如果第一块不正确，第二块正确，就使用第二块的数据，同时把数据拷贝到第一块中。
     写数据时保证，两块都是好的，同时写两块的数据
mileage_ADDRESS
*/

/**********************************************************************
//说明: 一块flash 内容复制到另一块中，每块的大小是1024字节
//参数1:src_ad，源地址，dec_ad,目的地址
//返回:　是否成功
**********************************************************************/
u8 flash_copy(u32 src_ad, u32 dec_ad)
{
	uint32_t Address = 0x00;

	volatile u32 *ptmp=NULL;
	volatile u8 i;
	
	 /* Unlock the Flash to enable the flash control register access *************/ 
	FLASH_Unlock();

	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 

  	/* Erase the FLASH pages */
	if (FLASH_ErasePage((uint32_t)dec_ad)!= FLASH_COMPLETE)
	{
	 /* Error occurred while sector erase. 
	     User can add here some code to deal with this error  */
	  return 0xff;
	}

  	/* Program the user Flash area word by word */
	   Address  = (uint32_t)dec_ad;

	  while (Address  <  (uint32_t)(dec_ad+1024))   // 一块FLASH的长度为1024
	  {
	    if (FLASH_ProgramWord(Address, *ptmp++) == FLASH_COMPLETE)
	    {
	      Address +=4;
		// ptmp ++;
	    }
	    else
	    { 
	      /* Error occurred while writing data in Flash memory. 
	         User can add here some code to deal with this error */
		return 0xff; 
	    }
	  }

     /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
       FLASH_Lock(); 


       /* Check if the programmed data is OK 
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/

	return 0;

}

u8 mileage_flash_init(void)
{
//	volatile u32 *ptmp=NULL;
//	volatile u32 *ptmp_system_para;
	volatile u8 i;
	
	 /* Unlock the Flash to enable the flash control register access *************/ 
	FLASH_Unlock();

	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 

  	/* Erase the FLASH pages */
	if (FLASH_ErasePage(mileage_ADDRESS)!= FLASH_COMPLETE)
	{
	 /* Error occurred while sector erase. 
	     User can add here some code to deal with this error  */
	  return 0xff;
	}

  	/* Erase the FLASH pages */
	if (FLASH_ErasePage(backup_mileage_ADDRESS)!= FLASH_COMPLETE)
	{
	 /* Error occurred while sector erase. 
	     User can add here some code to deal with this error  */
	  return 0xff;
	}


	wzw_mileage.flag = 0xa5;
	wzw_mileage.high_erasure_number = 0;
	wzw_mileage.low_erasure_number = 0;
	wzw_mileage.location = 1;
	wzw_mileage.mileage_64= 0;

  	/* Program the user Flash area word by word */
	//Address = dec_ad;


	if (!FLASH_ProgramWord((uint32_t)mileage_ADDRESS, *(uint32_t*)(&wzw_mileage)) == FLASH_COMPLETE)
	{

	  /* Error occurred while writing data in Flash memory. 
	     User can add here some code to deal with this error */
		return 0xff; 
	}

	if (!FLASH_ProgramWord((uint32_t)backup_mileage_ADDRESS, *(uint32_t*)&wzw_mileage) == FLASH_COMPLETE)
	{

	  /* Error occurred while writing data in Flash memory. 
	     User can add here some code to deal with this error */
		return 0xff; 
	}

     /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
   FLASH_Lock(); 

	 return 0;

}

// 把里程数据写进flash,如果找到最大数据，但是紧跟其后的是非0XFF怎么办 ?
u8 mileage_read(u32 address)
{
   volatile u32* pdwordtmp = (u32*)address;
   volatile u32 max=0,u32tmp=0;
   volatile u16 i;

   wzw_mileage.mileage_64 = 0;
   wzw_mileage.location = 1;
   memcpy((u8*)&wzw_mileage,(u8*)address,4);
   for (i=1; i<255; i++)
   {
		pdwordtmp ++;
		u32tmp = *(u32*)pdwordtmp; 
        if( (u32tmp != 0xffffffff) && ((u32tmp >= max)))
        {
            max = u32tmp;
            wzw_mileage.mileage_64 = max*3600;
            wzw_mileage.location = (i+1);
        }
    }
   return 0;
}


// 里程统计
//初始化,没规划的现规划，规划好的就读出来，放到内存中
void mileage_init(void)
{
	 //u8* pchartmp = NULL;
    u8 firstflag = 0;
    u32 tmp;
    tmp = *(u32*)mileage_ADDRESS;
    if(((tmp&0xff) == 0xa5)&&((tmp&0xffff0000) != 0xffff0000)&&(((tmp &0xff00) != 0xff00))) //标志正确
    {
        //  pchartmp = (u8*)backup_mileage_ADDRESS;
        tmp = *(u32*)backup_mileage_ADDRESS;
        if (((tmp&0xff) !=  0xa5) || ((tmp&0xffff0000) == 0xffff0000)  || (((tmp &0xff00) == 0xff00))) //标志正确
        {
            //	把mileage_ADDRESS->backup_mileage_ADDRESS
            if (flash_copy(mileage_ADDRESS,backup_mileage_ADDRESS))
            {
                if (flash_copy(mileage_ADDRESS,backup_mileage_ADDRESS))
                    return; //flash  error!
            }
        }
    //否则认为是正确的
    }
    else
    {
        tmp = *(u32*)backup_mileage_ADDRESS;
        if (((tmp&0xff) ==  0xa5) && ((tmp&0xffff0000) != 0xffff0000) &&  (((tmp &0xff00) != 0xff00))) //标志正确
        {
            // backup_mileage_ADDRESS->   mileage_ADDRESS
            if (flash_copy(backup_mileage_ADDRESS,mileage_ADDRESS))
            {
                if (flash_copy(backup_mileage_ADDRESS,mileage_ADDRESS))
                    return; //flash  error!
            }
        }
        else
        {
            //	说明是刚生产的，需要把标志写到两块中，擦除次数清零
            firstflag = 0x5a;
            if(mileage_flash_init())
            {
                return;
            }
        }
    }
   //读出位置和里程值
    if (!firstflag)
    {
        mileage_read(mileage_ADDRESS);
    }
}

// 把里程数据写进flash
u8 mileage_write(void)
{
	volatile uint32_t  Address = 0x00;
	u32 tmp;

	// FLASH_Status FLASHStatus = FLASH_COMPLETE;
	//volatile u32 *ptmp=NULL;
	//volatile u32 *ptmp_system_para=NULL;
	volatile u8 i;
	
	 /* Unlock the Flash to enable the flash control register access *************/ 
	FLASH_Unlock();

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
      //如果地址到一块的最后一个数据，就擦除整块
    if(wzw_mileage.location == 0xff)
    {
	  	/* Erase the FLASH pages */
		if (FLASH_ErasePage(mileage_ADDRESS)!= FLASH_COMPLETE)
		{
		 /* Error occurred while sector erase. 
		     User can add here some code to deal with this error  */
		  return 0xff;
		}
		FLASH_Lock();

		 /* Unlock the Flash to enable the flash control register access *************/ 
		FLASH_Unlock();
		/* Clear pending flags (if any) */  
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
		/* Erase the FLASH pages */
		if (FLASH_ErasePage(backup_mileage_ADDRESS)!= FLASH_COMPLETE)
		{
		 /* Error occurred while sector erase. 
		     User can add here some code to deal with this error  */
		  return 0xff;
		}
		
		wzw_mileage.low_erasure_number ++;
		if (wzw_mileage.low_erasure_number == 0)
		{
			wzw_mileage.high_erasure_number ++;
		}
		FLASH_Lock();

		 /* Unlock the Flash to enable the flash control register access *************/ 
		FLASH_Unlock();
		/* Clear pending flags (if any) */  
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 		
	      //写入初始化标志和擦除次数
		if(!FLASH_ProgramWord(mileage_ADDRESS, *(u32*)&wzw_mileage) == FLASH_COMPLETE)
		{
		  /* Error occurred while writing data in Flash memory. 
		     User can add here some code to deal with this error */
			return 0xff; 
		}
		 FLASH_Lock(); 

		 /* Unlock the Flash to enable the flash control register access *************/ 
		FLASH_Unlock();

		/* Clear pending flags (if any) */  
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
		if (!FLASH_ProgramWord(backup_mileage_ADDRESS, *(u32*)&wzw_mileage) == FLASH_COMPLETE)
		{
		  /* Error occurred while writing data in Flash memory. 
		     User can add here some code to deal with this error */
			return 0xff; 
		}		
		wzw_mileage.location = 1; 
    }
  	/* Program the user Flash area word by word */
      Address = mileage_ADDRESS+wzw_mileage.location*4;

	tmp = wzw_mileage.mileage_64/3600;
	FLASH_Lock();

	 /* Unlock the Flash to enable the flash control register access *************/ 
	FLASH_Unlock();

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);      
//ljw,20160831,这里判断反了 	if (FLASH_ProgramWord(Address, tmp) == FLASH_COMPLETE)
    if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)
	{
		  /* Error occurred while writing data in Flash memory. 
		     User can add here some code to deal with this error */
        if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)//ljw,20160831优化里程存储
        {
            wzw_mileage.location++;//ljw,20160831优化里程存储
            return 0xff;
        }
	}
	// 写完读出校验下
	if(*(u32*)Address != tmp)
	{
//ljw,20160831,这里判断反了		if (FLASH_ProgramWord(Address, tmp) == FLASH_COMPLETE)
        wzw_mileage.location++;//ljw,20160831优化里程存储
        Address = mileage_ADDRESS+wzw_mileage.location*4;//ljw,20160831优化里程存储
        if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)
		{
			  /* Error occurred while writing data in Flash memory. 
			     User can add here some code to deal with this error */
			return 0xff; 
		}
        else if(*(u32*)Address != tmp)//ljw,20160831优化里程存储
            return 0xff; //ljw,20160831优化里程存储
	}
	 FLASH_Lock(); 

	 /* Unlock the Flash to enable the flash control register access *************/ 
	FLASH_Unlock();


	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 	
      Address = backup_mileage_ADDRESS+wzw_mileage.location*4;
      
//ljw,20160831,这里判断反了	if(FLASH_ProgramWord(Address, tmp) == FLASH_COMPLETE)
    if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)
	{
		  /* Error occurred while writing data in Flash memory. 
		     User can add here some code to deal with this error */
        if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)//ljw,20160831优化里程存储
        {
            wzw_mileage.location++;//ljw,20160831优化里程存储
            return 0xff;
        }
	}
	// 写完读出校验下
	if(*(u32*)Address != tmp)
	{
        wzw_mileage.location++;//ljw,20160831优化里程存储
        Address = backup_mileage_ADDRESS+wzw_mileage.location*4;//ljw,20160831优化里程存储
		if(FLASH_ProgramWord(Address, tmp) == FLASH_COMPLETE)
		{
			  /* Error occurred while writing data in Flash memory. 
			     User can add here some code to deal with this error */
			return 0xff; 
		}
        else if(*(u32*)Address != tmp)//ljw,20160831优化里程存储
            return 0xff; //ljw,20160831优化里程存储
	}
     /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    FLASH_Lock(); 
	wzw_mileage.location++;
	return 0;
}






