#include "ddi_Norflash.h"
#include "main.h" 

/*
����������̣�����λ����Ϣ�Ϸ��ģ��ڸ����ֶ����Ϊ���˲���λ��ʹ����һ�����ݵķ���������λʱ����������������³��⣩���������
����ֻ�д�ACC���ܣ�ACC�ź���Чʱ����ͳ�ƣ�����5KM/H�Ͳ���¼��ÿ����ٶ����ۼӣ��ڣ���(n*1814/3600 )/100 ���ϴ��ǣ����������Ϊ��λ��
Ҫ��¼�ˣ���Ϊ�洢��Ҫ��һ��ר��������ϵͳ�����ֿ�����ֹ�����軵����һֱ���ߵ�ÿСʱ��һ�Σ����ѹ��ʱ�棬��⵽��ӵ�Դ����ҲҪ�档
�����������10000�Σ���ʱ�����Ҫ����ƽ������1024�ֽ���һ�飬ʹ��������������һ�������ݣ�ÿ��д��ʱ��д���顣ÿһ��ĵ�һ�����������Ϊ��־0xa5��һ�ֽڣ��Ͳ��������������ֽڣ�

ԭ����ƽ�����������ġ���Ϊֻ����һ�����ݣ���һ�¡�ֻ�������ݣ���ʼ���ǲ����ɣ�����棬ÿ������DWORDռ�У��ֽڡ�ֱ��Ѱ���������������������Ҫ����
����ָ���������ʱҲҪ��ƽ������������
���ʹ�����鱸�ݵģ��ȼ���һ�죬Ҫ���ܶ��������ͱȽϵڶ���ĵ����ݣ����鲻һ�����õ�һ���
��䵽�ڶ��飬�����һ�鲻��ȷ���ڶ�����ȷ����ʹ�õڶ�������ݣ�ͬʱ�����ݿ�������һ���С�
     д����ʱ��֤�����鶼�Ǻõģ�ͬʱд���������
mileage_ADDRESS
*/

/**********************************************************************
//˵��: һ��flash ���ݸ��Ƶ���һ���У�ÿ��Ĵ�С��1024�ֽ�
//����1:src_ad��Դ��ַ��dec_ad,Ŀ�ĵ�ַ
//����:���Ƿ�ɹ�
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

	  while (Address  <  (uint32_t)(dec_ad+1024))   // һ��FLASH�ĳ���Ϊ1024
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

// ���������д��flash,����ҵ�������ݣ����ǽ��������Ƿ�0XFF��ô�� ?
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


// ���ͳ��
//��ʼ��,û�滮���ֹ滮���滮�õľͶ��������ŵ��ڴ���
void mileage_init(void)
{
	 //u8* pchartmp = NULL;
    u8 firstflag = 0;
    u32 tmp;
    tmp = *(u32*)mileage_ADDRESS;
    if(((tmp&0xff) == 0xa5)&&((tmp&0xffff0000) != 0xffff0000)&&(((tmp &0xff00) != 0xff00))) //��־��ȷ
    {
        //  pchartmp = (u8*)backup_mileage_ADDRESS;
        tmp = *(u32*)backup_mileage_ADDRESS;
        if (((tmp&0xff) !=  0xa5) || ((tmp&0xffff0000) == 0xffff0000)  || (((tmp &0xff00) == 0xff00))) //��־��ȷ
        {
            //	��mileage_ADDRESS->backup_mileage_ADDRESS
            if (flash_copy(mileage_ADDRESS,backup_mileage_ADDRESS))
            {
                if (flash_copy(mileage_ADDRESS,backup_mileage_ADDRESS))
                    return; //flash  error!
            }
        }
    //������Ϊ����ȷ��
    }
    else
    {
        tmp = *(u32*)backup_mileage_ADDRESS;
        if (((tmp&0xff) ==  0xa5) && ((tmp&0xffff0000) != 0xffff0000) &&  (((tmp &0xff00) != 0xff00))) //��־��ȷ
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
            //	˵���Ǹ������ģ���Ҫ�ѱ�־д�������У�������������
            firstflag = 0x5a;
            if(mileage_flash_init())
            {
                return;
            }
        }
    }
   //����λ�ú����ֵ
    if (!firstflag)
    {
        mileage_read(mileage_ADDRESS);
    }
}

// ���������д��flash
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
      //�����ַ��һ������һ�����ݣ��Ͳ�������
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
	      //д���ʼ����־�Ͳ�������
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
//ljw,20160831,�����жϷ��� 	if (FLASH_ProgramWord(Address, tmp) == FLASH_COMPLETE)
    if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)
	{
		  /* Error occurred while writing data in Flash memory. 
		     User can add here some code to deal with this error */
        if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)//ljw,20160831�Ż���̴洢
        {
            wzw_mileage.location++;//ljw,20160831�Ż���̴洢
            return 0xff;
        }
	}
	// д�����У����
	if(*(u32*)Address != tmp)
	{
//ljw,20160831,�����жϷ���		if (FLASH_ProgramWord(Address, tmp) == FLASH_COMPLETE)
        wzw_mileage.location++;//ljw,20160831�Ż���̴洢
        Address = mileage_ADDRESS+wzw_mileage.location*4;//ljw,20160831�Ż���̴洢
        if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)
		{
			  /* Error occurred while writing data in Flash memory. 
			     User can add here some code to deal with this error */
			return 0xff; 
		}
        else if(*(u32*)Address != tmp)//ljw,20160831�Ż���̴洢
            return 0xff; //ljw,20160831�Ż���̴洢
	}
	 FLASH_Lock(); 

	 /* Unlock the Flash to enable the flash control register access *************/ 
	FLASH_Unlock();


	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 	
      Address = backup_mileage_ADDRESS+wzw_mileage.location*4;
      
//ljw,20160831,�����жϷ���	if(FLASH_ProgramWord(Address, tmp) == FLASH_COMPLETE)
    if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)
	{
		  /* Error occurred while writing data in Flash memory. 
		     User can add here some code to deal with this error */
        if(FLASH_ProgramWord(Address, tmp) != FLASH_COMPLETE)//ljw,20160831�Ż���̴洢
        {
            wzw_mileage.location++;//ljw,20160831�Ż���̴洢
            return 0xff;
        }
	}
	// д�����У����
	if(*(u32*)Address != tmp)
	{
        wzw_mileage.location++;//ljw,20160831�Ż���̴洢
        Address = backup_mileage_ADDRESS+wzw_mileage.location*4;//ljw,20160831�Ż���̴洢
		if(FLASH_ProgramWord(Address, tmp) == FLASH_COMPLETE)
		{
			  /* Error occurred while writing data in Flash memory. 
			     User can add here some code to deal with this error */
			return 0xff; 
		}
        else if(*(u32*)Address != tmp)//ljw,20160831�Ż���̴洢
            return 0xff; //ljw,20160831�Ż���̴洢
	}
     /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    FLASH_Lock(); 
	wzw_mileage.location++;
	return 0;
}






