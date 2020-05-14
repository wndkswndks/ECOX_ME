/*C***************************************************************************
* $RCSfile: eeprom.c
*-----------------------------------------------------------------------------
* PURPOSE: Control eeprom by I2C bus interface 
* This file contains the eeprom.c source code
*----------------------------------------------------------------------------
* RELEASE:      $Name: eeprom.c     
* Edited  By    $Id: sslim
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include 	"includes.h"

/*_____ D E C L A R A T I O N ______________________________________________*/


/*_____ F U C T I O N ______________________________________________________*/
void write_eeprom_BYTE(BYTE, BYTE);
void write_eeprom_WORD(BYTE, WORD);
void write_eeprom_LONG(BYTE, LONG);
void write_eeprom(BYTE, BYTE);

BYTE read_eeprom_BYTE(BYTE);
WORD read_eeprom_WORD(BYTE);
LONG read_eeprom_LONG(BYTE);
BYTE read_eeprom(BYTE);


void start_eeprom(void);
void stop_eeprom(void);
void eeprom_init(void);
BYTE sout_eeprom(BYTE);
BYTE sin_eeprom(void);


/*_____ V A R I A B L E ____________________________________________________*/
BYTE	read_byte = 0;
WORD	read_word = 0;
LONG	read_long = 0;

BYTE	temp_val = 0;
BYTE	temp_ADD = 0;
WORD	temp_cal = 0;


/*==========================================================================*/


/*F**************************************************************************
* NAME:     Delay
* PURPOSE:
*****************************************************************************/
void Delay(long dval)
{
	while(--dval);
}

/*F********************************ee******************************************
* NAME:     set_in
* PURPOSE:
*****************************************************************************/
void set_in(void)	{ 	SDA_IN;	}

/*F**************************************************************************
* NAME:     set_out
* PURPOSE:
*****************************************************************************/
void set_out(void)	{	SDA_OUT;	Delay(DELY);	}

/*F**************************************************************************
* NAME:     set_sda
* PURPOSE:
*****************************************************************************/
void set_sda(void)	{	set_in();	Delay(DELY);	}

/*F**************************************************************************
* NAME:     clr_sda
* PURPOSE:
*****************************************************************************/
void clr_sda(void)	{	set_out();	SDA = OFF;	}

/*F**************************************************************************
* NAME:     set_scl
* PURPOSE:
*****************************************************************************/
void set_scl(void)	{	SCL=ON;		Delay(DELY);	}

/*F**************************************************************************
* NAME:     clr_scl
* PURPOSE:
*****************************************************************************/
void clr_scl(void)	{	SCL=OFF;	Delay(DELY);	}
	
/*F**************************************************************************
* NAME:     check_sda
* PURPOSE:
*****************************************************************************/
BYTE check_sda(void)
{
	if(SDA_CH)	return(1);
	else		return(0);
}

/*F**************************************************************************
* NAME:     start_eeprom
* PURPOSE:
*****************************************************************************/
void start_eeprom(void)
{
	set_sda();	set_scl();
	
	if(check_sda()==0)
		return;
	
	clr_sda();	clr_scl();
}

/*F**************************************************************************
* NAME:     stop_eeprom
* PURPOSE:
*****************************************************************************/
void stop_eeprom(void)
{
	clr_sda();	set_scl();	set_sda();
}

/*F**************************************************************************
* NAME:     init_eeprom
* PURPOSE:
*****************************************************************************/
void eeprom_init(void)
{
	SWP = 1;
	set_out();
	set_sda();
	set_scl();
	set_in();
	while(check_sda()==0)
	{
		Delay(DELY);
		set_out();
		clr_scl();
		stop_eeprom();
		set_in();
	}
	set_out();
	stop_eeprom();
}

/*F**************************************************************************
* NAME:     sout_eeprom
* PURPOSE: 1byte의 값을 내보내는 함수
*****************************************************************************/
BYTE sout_eeprom(BYTE val1)
{
	int cnt;
	for(cnt=0;cnt<8;cnt++)
	{
		if(val1&0x80)		//1000 0000 & 0000 1010 = 0000 0000  MSB mask
			set_sda();
		else	
		clr_sda();
		set_scl();	
		clr_scl();
		val1=(val1<<1);
	}

	set_in();
	set_scl();

	if(check_sda())
	{
		clr_scl();
		set_out();
		return(1);
	}
	else
	{
		clr_scl();
		set_out();
		return(0);
	}
}		

/*F**************************************************************************
* NAME:     sin_eeprom
* PURPOSE: 1byte의 값을 입력받는 함수
*****************************************************************************/
BYTE sin_eeprom(void)
{
	WORD cnt;
	BYTE val2;
	set_in();
	for(cnt=0;cnt<8;cnt++)
	{
		set_scl();	
		val2=(val2<<1);	
		val2=(val2&0xfe);	
		if(check_sda())
			val2=(val2|0x01);	
		clr_scl();
	}
	set_out();
	return(val2);
}

/*F**************************************************************************
* NAME:     ack_eeprom
* PURPOSE:
*****************************************************************************/
void ack_eeprom(void)
{
	clr_sda();	set_scl();	clr_scl();
}
/*F**************************************************************************
* NAME:     nak_eeprom
* PURPOSE:
*****************************************************************************/
void nak_eeprom(void)
{
	set_sda();	set_scl();	clr_scl();
}
	
/*F**************************************************************************
* NAME:     write_eeprom
* PURPOSE:
*****************************************************************************/
void write_eeprom(BYTE addr,BYTE dat)
{
	SWP = 0;
	set_imask_ccr(1);		// ccr : condition code register ?? define된 코드는 어디에??
   	start_eeprom();
	if(sout_eeprom(0xa0)==0)
	{
		if(sout_eeprom(addr)==0)
		{
			if(sout_eeprom(dat)==0)	stop_eeprom();
			else					stop_eeprom();
		}
		else	stop_eeprom();
	}
	else stop_eeprom();
	set_imask_ccr(0);
	SWP = 1;
}

/*F**************************************************************************
* NAME:     read_eeprom
* PURPOSE:
*****************************************************************************/
BYTE read_eeprom(BYTE addr)
{
	BYTE i;
	set_imask_ccr(1);
	start_eeprom();
	if(sout_eeprom(0xa0)==0)
	{
		if(sout_eeprom(addr)==0);
		else
		{
			stop_eeprom();
			set_imask_ccr(0);
			return(0);
		}
	}
	else
	{
		stop_eeprom();
		set_imask_ccr(0);
		return(0);
	}
	start_eeprom();
	if(sout_eeprom(0xa1)==0);
	else
	{	
		stop_eeprom();
		set_imask_ccr(0);
		return(0);
	}
	i = sin_eeprom();
	nak_eeprom();
	stop_eeprom();
	set_imask_ccr(0);
	return(i);
}

/*F**************************************************************************
* NAME:     write_eeprom_BYTE
* PURPOSE:
*****************************************************************************/
void write_eeprom_BYTE(BYTE address, BYTE data)
{
	write_eeprom(address,(BYTE)((data) & 0xff));	dly_ms(15);
}

/*F**************************************************************************
* NAME:     write_eeprom_WORD
* PURPOSE:
*****************************************************************************/
void write_eeprom_WORD(BYTE address, WORD data)
{
	write_eeprom(address,  (BYTE)((data >> 8) 	& 0x00ff));	dly_ms(15);
	write_eeprom(address+2,(BYTE)((data) 		& 0x00ff));	dly_ms(15);
}

/*F**************************************************************************
* NAME:     write_eeprom_LONG
* PURPOSE:
*****************************************************************************/
void write_eeprom_LONG(BYTE address, LONG data)
{
	write_eeprom(address,  (BYTE)((data >> 24) 	& 0x000000ff));	dly_ms(15);
	write_eeprom(address+2,(BYTE)((data >> 16) 	& 0x000000ff));	dly_ms(15);
	write_eeprom(address+4,(BYTE)((data >> 8)	& 0x000000ff));	dly_ms(15);
	write_eeprom(address+6,(BYTE)((data)		& 0x000000ff));	dly_ms(15);
}
/*F**************************************************************************
* NAME:     read_eeprom_BYTE
* PURPOSE:
*****************************************************************************/
BYTE read_eeprom_BYTE(BYTE address)
{
	BYTE data;

	data = (BYTE)read_eeprom(address) & 0xff;
  	return data;
}

/*F**************************************************************************
* NAME:     read_eeprom_WORD
* PURPOSE:
*****************************************************************************/
WORD read_eeprom_WORD(BYTE address)
{
	WORD data;

   	data	=  (((WORD)read_eeprom(address)	<< 8)	& 0xff00);
	data	|= (((WORD)read_eeprom(address+2) ) 	& 0x00ff);
	return data;
}

/*F**************************************************************************
* NAME:     read_eeprom_LONG
* PURPOSE:
*****************************************************************************/
LONG read_eeprom_LONG(BYTE address)
{
	LONG data;

   	data	=  (((LONG)read_eeprom(address  )	<< 24)	& 0xff000000);
	data	|= (((LONG)read_eeprom(address+2)	<< 16)	& 0x00ff0000);
   	data	|= (((LONG)read_eeprom(address+4)	<<  8)	& 0x0000ff00);
	data	|= (((LONG)read_eeprom(address+6)        )	& 0x000000ff);
	return data;
}

/*F**************************************************************************
* NAME:     eeprom_test()
* PURPOSE:
*****************************************************************************/
void eeprom_test()
{
	
	BYTE addr = 0;
	BYTE value[256];
	
	BYTE i, in;


	StartTimeCheck_10us("write_eeprom 256");
	for(i = 0; i < 5; i++)
	{
		write_eeprom_BYTE(i, i+60);
	}
	EndTimeCheck_10us();


	StartTimeCheck_10us("read_eeprom 256");

		value[0] = read_eeprom_BYTE(0);
		value[1] = read_eeprom_BYTE(1);
		value[2] = read_eeprom_BYTE(2);
		value[3] = read_eeprom_BYTE(3);
		value[4] = read_eeprom_BYTE(4);
		
	// read_eeprom_BYTE()의 경우 for문에서 오동작 : 입력매개변수 오동작			
	// UartPrintf(), PCPuts()의 경우도 같은 증상, for문시 인터럽트 중첩(?)으로 안되는듯
	// for문등에 위함수 사용 제외!!
	for(i = 0; i < 5; i++);
	{
		// value[0] = read_eeprom_BYTE(0);
		// value[1] = read_eeprom_BYTE(1);
		// value[2] = read_eeprom_BYTE(2);
		// value[3] = read_eeprom_BYTE(3);
		// value[4] = read_eeprom_BYTE(4);

		value[i] = read_eeprom_BYTE(i);
		UartPrintf("i : %d, \r\n", i);
		dly_ms(100);
	}
	EndTimeCheck_10us();



		UartPrintf("value[0] = %u \r\n", value[0]);
		UartPrintf("value[1] = %u \r\n", value[1]);
		UartPrintf("value[2] = %u \r\n", value[2]);
		UartPrintf("value[3] = %u \r\n", value[3]);
		UartPrintf("value[4] = %u \r\n", value[4]);
		UartPrintf("value[5] = %u \r\n", value[5]);
		UartPrintf("value[6] = %u \r\n", value[6]);
		UartPrintf("value[7] = %u \r\n", value[7]);
		UartPrintf("value[8] = %u \r\n", value[8]);
		UartPrintf("value[9] = %u \r\n", value[9]);

	// 이전 소스
	/*
	BYTE 	write_value_BYTE_1 = 0;	
	BYTE 	write_value_BYTE_2 = 0;	
	WORD 	write_value_WORD_1 = 0;	
	WORD 	write_value_WORD_2 = 0;
	LONG	wirte_value_LONG_1 = 0;
	LONG	wirte_value_LONG_2 = 0;
	
	BYTE 	read_value_BYTE_1 = 0;	
	BYTE 	read_value_BYTE_2 = 0;	
	WORD 	read_value_WORD_1 = 0;	
	WORD 	read_value_WORD_2 = 0;
	LONG	read_value_LONG_1 = 0;
	LONG	read_value_LONG_2 = 0;
	
	char temp[64] = {0};
	

			write_value_BYTE_1 = 255;
			write_value_BYTE_2 = 256;
			write_value_WORD_1 = 32767;
			write_value_WORD_2 = 32768;
			wirte_value_LONG_1 = 2147483647;
			wirte_value_LONG_2 = 2147483648;
			
	StartTimeCheck_10us("write eeprom : 1");
			write_eeprom_BYTE(0, write_value_BYTE_1);
	EndTimeCheck_10us();
			write_eeprom_BYTE(2, write_value_BYTE_2);

			write_eeprom_WORD(100, write_value_WORD_1);
			write_eeprom_WORD(105, write_value_WORD_2);
			write_eeprom_LONG(300, wirte_value_LONG_1);
			write_eeprom_LONG(310, wirte_value_LONG_2);
			
			
	StartTimeCheck_10us("read eeprom : 1");
			read_value_BYTE_1	= read_eeprom_BYTE(0);
	EndTimeCheck_10us();
			read_value_BYTE_2	= read_eeprom_BYTE(2);
			read_value_WORD_1	= read_eeprom_WORD(100);
			read_value_WORD_2	= read_eeprom_WORD(105);
			read_value_LONG_1	= read_eeprom_LONG(300);
			read_value_LONG_2	= read_eeprom_LONG(310);
			
			PCPrintf("EEPROM VALUE 1 => [%u]\r\n",read_value_BYTE_1);
			PCPrintf("EEPROM VALUE 2 => [%u]\r\n",read_value_BYTE_2);
			PCPrintf("EEPROM VALUE 3 => [%u]\r\n",read_value_WORD_1);
			PCPrintf("EEPROM VALUE 4 => [%u]\r\n",read_value_WORD_2);
			PCPrintf("EEPROM VALUE 5 => [%ld]\r\n",read_value_LONG_1);
			PCPrintf("EEPROM VALUE 6 => [%ld]\r\n",read_value_LONG_2);
	*/
}
