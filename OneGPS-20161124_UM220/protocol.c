void dumpmem(unsigned char* addr1,unsigned short length)
{
	unsigned short i;
		for (i=0; i<length; i++)
		{
			if ( *(addr1+i)< 0x10)
			RETAILMSG(1, (TEXT("0%x"),*(addr1+i)));
			else
			RETAILMSG(1, (TEXT("%x"),*(addr1+i)));

		}



}
//按照部标协议转义，遇到0x7d 0x02->0x7e 、 0x7d 0x01 ->0x7d;
//从0x7e后一个字节开始和后一个字节异或，直到校验码，如果和校验码相同，就是正确的
unsigned short analysis_0x7d(unsigned char* buffer,unsigned short number)
{
   unsigned char* tmp = buffer;
   unsigned short i,j=0,k;
   short sum=0, check_sum;
   BOOL flag = FALSE;
   if ( *tmp == 0X7E );
   else
	   return FALSE;
	 if (*(tmp+number-1) != 0x7e)
	 	return FALSE;  
   k = number-2; //去头尾两个0X7E
   j = 0;
   sum = *(buffer+1);
   for ( i=0; i<k;  i++ )
   {
	   	 sum ^= *(tmp+i+2);

		 if(0x7d == *(tmp+i+1))
		 {
			j ++;
			k ++;
			flag = TRUE;
			continue;
		 }
		 else
		 if (i >= 3+j)
		 {
			 if (!flag)
			 {
				 revebuf[i-3-j] = *(tmp+i+1); // copy the true data to revebuf
				 flag = FALSE;
			 }
			 else
				 revebuf[i-3-j] = (*(tmp+i+1))^0x1b; // copy the true data to revebuf
		 }

   }

   k ++;
   if (0x1b == *(tmp+k))
   {
	   check_sum = ((*(tmp+k+1))^0x1b)*0x100;
	   j = 1;
	   if (0x1b == *(tmp+k+2))
		   check_sum += ((*(tmp+k+3))^0x1b);
	   else
		   check_sum += *(tmp+k+2);

   }
   else
   {
	   check_sum = (*(tmp+k))*0x100;
	   if (0x1b == *(tmp+k+1))
		   check_sum += ((*(tmp+k+2))^0x1b);
	   else
		   check_sum += *(tmp+k+1);
   }
   if (check_sum == sum)
	   ;
	else 
	{
	   dumpmem(buffer,number);
	   return FALSE;
	}
	if ( *(tmp+number-1) == 0X7E )
		return TRUE;
	else
	{
		return FALSE;
	}

}
unsigned short ret;
unsigned short analysis02(unsigned char* buffer,unsigned short number)
{
	unsigned char* tmp = buffer;
	unsigned int i,k ,j=0;
	short sum=0, check_sum;
	BOOL flag = FALSE;
	if ( *tmp == 0X7E );
	else
		return FALSE;
	//tmp++;
    k = 515; //data 512 + 1 addr + 1 commond + 1 SN;
	j = 0;
	for ( i=0; i<(k); i ++ )
	{
		sum += *(tmp+i+1);
#if 1
		if(0x1b == *(tmp+i+1))
		{

			k ++;
			j ++;
			flag = TRUE;

			continue;
		}
#endif

		
		 if (i > (2+j))
		 {
			 if (!flag)
			 {
				revebuf[i-3-j] = *(tmp+i+1); // copy the true data to revebuf
				
			 }
			 else
			 {
				revebuf[i-3-j] = (*(tmp+i+1))^0x1b; // copy the true data to revebuf
				flag = FALSE;
			 }
		 }
		 else
		 {
			if (flag )
			  flag = FALSE;
		 }


	}
	ret = j; //被转义的字节数

	k ++;
	if (0x1b == *(tmp+k))
	{
		check_sum = ((*(tmp+k+1))^0x1b)*0x100;
		j = 1;
		if (0x1b == *(tmp+k+2))
			check_sum += ((*(tmp+k+3))^0x1b);
		else
			check_sum += *(tmp+k+2);

	}
	else
	{
		check_sum = (*(tmp+k))*0x100;
		if (0x1b == *(tmp+k+1))
			check_sum += ((*(tmp+k+2))^0x1b);
		else
			check_sum += *(tmp+k+1);
	}
#if 1
	if (check_sum == sum)
		;
	else 
	{
		RETAILMSG(1, (L"ln245" ));
		return FALSE;
	}
	if ( *(tmp+number-1) == 0X7E )
		return TRUE;
	else
	{
		RETAILMSG(1, (L"ln252" ));
		return FALSE;
	}
#endif


}
unsigned short analysis00(unsigned char* buffer,unsigned short number)
{
	unsigned char* tmp = buffer;
	unsigned short i,j=0,k;
	short sum=0, check_sum;
	if ( *tmp == 0X7E );
	else
		return FALSE;
	if (number >= 9)
	{
	  k = 5; //真阳的协议还有2Byte图片的字节数
	}
	else
		k = 2; //现在的908上使用的协议无图片字节数。
	for ( i=0; i<k;  i++ )
	{
		sum += *(tmp+i+1);
		if(0x1b == *(tmp+i+1))
		{
			j ++;
			k ++;
			continue;
		}
	}
	k ++;
	j = 0;
	if (0x1b == *(tmp+k))
	{
		check_sum = ((*(tmp+k+1))^0x1b)*0x100;
		j = 1;
		if (0x1b == *(tmp+k+2))
		 check_sum += ((*(tmp+k+3))^0x1b);
		else
		 check_sum += *(tmp+k+2);

	}
	else
	{
		check_sum = (*(tmp+k))*0x100;
		if (0x1b == *(tmp+k+1))
			check_sum += ((*(tmp+k+2))^0x1b);
		else
			check_sum += *(tmp+k+1);
	}

	if (check_sum == sum)
		;
	else 
	{
		RETAILMSG(1, (L"ln308" ));
		return FALSE;
	}
	if ( *(tmp+number-1) == 0X7E )
		return TRUE;
	else
	{
		RETAILMSG(1, (L"ln315" ));
		return FALSE;
	}

}