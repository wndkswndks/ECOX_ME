/*C***************************************************************************
* $RCSfile: serial.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the serial library
*----------------------------------------------------------------------------
* RELEASE:      $Name: serial     
* Edited  By    $Id: Joony  
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include	"includes.h"


/*_____ D E F I N E ________________________________________________________*/
#define RX_RING_BUF_SIZE 20


/*_____ D E C L A R A T I O N ______________________________________________*/
typedef struct {
 char buf[RX_RING_BUF_SIZE];
 int head;
 int tail;
} RX_RING;


/*_____ F U C T I O N ______________________________________________________*/
static void receive_rx0();
static void receive_rx2();
static void receive_rx4();
static void receive_rx5();

static void com0_putchar(char);
static void com2_putchar(char);
static void com4_putchar(char);
static void com5_putchar(char);

// not used
static void com0_BufClear();
static void com2_BufClear();
static void com4_BufClear();
static void com5_BufClear();



void MP3Puts(char *);
void LCDPuts(char *);
void PCPuts(char *);
void GenPuts(char *);

int  com0_BufGetSize();
int  com2_BufGetSize();
int  com4_BufGetSize();
int  com5_BufGetSize();

void com0_BufPutData(char *);
void com2_BufPutData(char *);
void com4_BufPutData(char *);
void com5_BufPutData(char *);

void com0_BufGetData(char *);
void com2_BufGetData(char *);
void com4_BufGetData(char *);
void com5_BufGetData(char *);

void enable_serial(void);
void serial_init(void);
void UartPrintf(char *,...);
void PCPrintf(char *, ...);
void LCDPrintf(char *, ...);
void GenPrintf(char *, ...);
void MP3Printf(char *, ...);
void ASCII2Version(char *);
void Change_ASC(char);


/*_____ V A R I A B L E ____________________________________________________*/


//////  Ring Buffer
static char rx_buf0[RX_RING_BUF_SIZE][20];
static char rx_buf2[RX_RING_BUF_SIZE][20];
static char rx_buf4[RX_RING_BUF_SIZE][20];
static char rx_buf5[RX_RING_BUF_SIZE][20];

static int m_nHead0,m_nTail0;
static int m_nHead2,m_nTail2;
static int m_nHead4,m_nTail4;
static int m_nHead5,m_nTail5;

static RX_RING rx0_ring;
static RX_RING rx2_ring;
static RX_RING rx4_ring;
static RX_RING rx5_ring;

////// todo : local variables
static BYTE rx0_start;
static BYTE rx0_data_count;
static char rx0_data[20];

static BYTE rx2_start;
static BYTE rx2_data_count;
static char rx2_data[20];

// static BYTE rx4_start;
// static BYTE rx4_data_count;
// static char rx4_data[20]; 

static BYTE rx5_start;
static BYTE rx5_data_count;
static char rx5_data[20];

/*F**************************************************************************
* NAME:     enable_serial
* PURPOSE:	
*****************************************************************************/
void enable_serial(void)
{
	MSTP.CRB.BIT._SCI0 = 0;							//Takes SCI0 out of Module Stop Mode
	MSTP.CRB.BIT._SCI2 = 0;							//Takes SCI2 out of Module Stop Mode 
	MSTP.CRB.BIT._SCI4 = 0;							//Takes SCI4 out of Module Stop Mode  
	MSTP.CRB.BIT._SCI5 = 0;							//Takes SCI5 out of Module Stop Mode 
}
/*F**************************************************************************
* NAME:     serial_init
* PURPOSE:	
*****************************************************************************/
void serial_init (void) 
{
	enable_serial();
	
	P2.ICR.BIT.B2		= 1;						// P22(TxD0) Receive 
	P2.ICR.BIT.B1		= 1;						// P21(RxD0) Receive
		
	SCI0.SCR.BYTE		= 0; 							
	SCI0.SMR.BYTE		= 0x00;						//Async, 8-data, No-parity, 1-stop, N-multi, CLK 	
	SCI0.SCMR.BIT.SMIF 	= 0;						// Smart Card interface mode select                 
	SCI0.BRR			= 10;						//MCU baud rate set : 57600(osc=20MHz)
//	SCI0.BRR = 32;									//MCU baud rate set : 19200(osc=20MHz)
// 	SCI0.BRR = 64;									//MCU baud rate set : 9600(osc=20MHz)
             
  	SCI0.SCR.BIT.TE		= 1;      					//txd enable
    SCI0.SCR.BIT.RE		= 1;      					//rxd enable
    SCI0.SCR.BIT.TIE	= 0;     					//txd interript enable    
    SCI0.SCR.BIT.RIE	= 1;    				 		//rxd interript enable   

	SCI0.SCR.BIT.MPIE	= 0;						//Transmit-data-empty Interrupt Disable			
	SCI0.SCR.BIT.TEIE	= 0;						//Transmit End Interrupt Enable 					
	SCI0.SSR.BIT.TDRE	= 0;						//Clear TDR Empty flag
	 	
	
	P1.ICR.BIT.B0		= 1;                        // P10(TxD2) Receive 
	P1.ICR.BIT.B1		= 1;                        // P11(RxD2) Receive 
		
	SCI2.SCR.BYTE		= 0; 							
	SCI2.SMR.BYTE		= 0x00;						//Async, 8-data, No-parity, 1-stop, N-multi, CLK 	
	SCI2.SCMR.BIT.SMIF 	= 0;						// Smart Card interface mode select                 
	SCI2.BRR 			= 32;						//MCU baud rate set : 19200(osc=20MHz) //57600
            
  	SCI2.SCR.BIT.TE		= 1;      					//txd enable
    SCI2.SCR.BIT.RE		= 1;      					//rxd enable
    SCI2.SCR.BIT.TIE	= 0;     					//txd interript enable    
    SCI2.SCR.BIT.RIE	= 1;    				 		//rxd interript enable   

	SCI2.SCR.BIT.MPIE	= 0;						//Transmit-data-empty Interrupt Disable			
	SCI2.SCR.BIT.TEIE	= 0;						//Transmit End Interrupt Enable 					
	SCI2.SSR.BIT.TDRE	= 0;						//Clear TDR Empty flag 
		
	
	P6.ICR.BIT.B0		= 1;						// P60(TxD4) Receive 
	P6.ICR.BIT.B1		= 1;						// P61(RxD4) Receive 
	
	SCI4.SCR.BYTE		= 0; 							
	SCI4.SMR.BYTE		= 0x00;						//Async, 8-data, No-parity, 1-stop, N-multi, CLK 	
	SCI4.SCMR.BIT.SMIF 	= 0;						// Smart Card interface mode select                 
//	SCI4.BRR = 10;									//MCU baud rate set : 115200(osc=20MHz)
	SCI4.BRR			= 32;									//MCU baud rate set : 19200(osc=20MHz)
// 	SCI4.BRR = 64;									//MCU baud rate set : 9600(osc=20MHz)
           
  	SCI4.SCR.BIT.TE		= 1;      					//txd enable
    SCI4.SCR.BIT.RE		= 1;      					//rxd enable
    SCI4.SCR.BIT.TIE	= 0;     					//txd interript enable    
    SCI4.SCR.BIT.RIE	= 1;    				 		//rxd interript enable   

	SCI4.SCR.BIT.MPIE	= 0;						//Transmit-data-empty Interrupt Disable			
	SCI4.SCR.BIT.TEIE	= 0;						//Transmit End Interrupt Enable 					
	SCI4.SSR.BIT.TDRE	= 0;						//Clear TDR Empty flag
	 										
	
	PF.ICR.BIT.B5		= 1;						// PF5(TxD5) Receive 
	PF.ICR.BIT.B6		= 1;						// PF6(RxD5) Receive 
	
	SCI5.SCR.BYTE		= 0; 							
	SCI5.SMR.BYTE		= 0x00;						//Async, 8-data, No-parity, 1-stop, N-multi, CLK 	
	SCI5.SCMR.BIT.SMIF 	= 0;						// Smart Card interface mode select                 
//	SCI5.BRR = 10;									//MCU baud rate set : 115200(osc=20MHz)
//	SCI5.BRR = 32;									//MCU baud rate set : 19200(osc=20MHz)
//  SCI5.BRR = 64;									//MCU baud rate set : 9600(osc=20MHz)
 	SCI5.BRR = 16;									//MCU baud rate set : 38400(osc=20MHz)
            
  	SCI5.SCR.BIT.TE		= 1;      					//txd enable
    SCI5.SCR.BIT.RE		= 1;      					//rxd enable
    SCI5.SCR.BIT.TIE	= 0;     					//txd interript enable    
    SCI5.SCR.BIT.RIE	= 1;   				 		//rxd interript enable   

	SCI5.SCR.BIT.MPIE 	= 0;						//Transmit-data-empty Interrupt Disable			
	SCI5.SCR.BIT.TEIE 	= 0;						//Transmit End Interrupt Enable 					
	SCI5.SSR.BIT.TDRE 	= 0;
}
/*F**************************************************************************
* NAME:     MP3 : com0_puts
* PURPOSE:	
*****************************************************************************/
void MP3Puts(char *s)	
{
	while(*s)
	{
		com0_putchar(*s);
		s++;
	}
} 

/*F**************************************************************************
* NAME:     com0_putchar
* PURPOSE:	
*****************************************************************************/
void com0_putchar(char c)	
{
	while(SCI0.SSR.BIT.TDRE == 0);
	SCI0.TDR = c;
	SCI0.SSR.BIT.TDRE = 0;
}
/*F**************************************************************************
* NAME:     receive_rx0
* PURPOSE:	rx0_data[20],rx0_data_count,rx0_start
*****************************************************************************/
void receive_rx0()
{
	char c;		
	if(rx0_ring.head != rx0_ring.tail)
	{
		c = rx0_ring.buf[rx0_ring.tail++];
		if(rx0_ring.tail == RX_RING_BUF_SIZE) rx0_ring.tail=0;
		if(c>64 && c<91) c += 32;				// change to lowercase
		if( (c == '[') || (c == '!') )
		{
			rx0_start = ON;
			rx0_data_count = 0;
			memset(rx0_data,0,20);
		}
		else if(rx0_start == ON && ( (c == ']') || (c == 0x0D) ) )	// 0x0D = Carriage Return
		{			
			rx0_start = OFF;			
			com0_BufPutData(rx0_data);			// put data to command ring buffer
		}			
		else if(rx0_start == ON)
		{
			rx0_data[rx0_data_count++] = c;
		}		
	}	
}

// vector 144 ERI0 SCI0
__interrupt(vect=144) void INT_ERI0_SCI0(void)
{ 
  	SCI0.SSR.BIT.ORER = 0;
  	SCI0.SSR.BIT.FER  = 0;
  	SCI0.SSR.BIT.PER  = 0; 
  	SCI0.SCR.BIT.RE   = 1;      // rxd enable
    SCI0.SCR.BIT.RIE  = 1;    	// rxd interript enable     
} 
   
// vector 145 RXI0 SCI0
__interrupt(vect=145) void INT_RXI0_SCI0(void)
{          
	char c;                  
	SCI0.SSR.BIT.RDRF=0;         // receive data register full bit clear; 
	c = SCI0.RDR;   	         // receive data register read;

	rx0_ring.buf[rx0_ring.head++] = c;
	 
	if(rx0_ring.head == RX_RING_BUF_SIZE) rx0_ring.head = 0;
	
	receive_rx0();	
}
/*F**************************************************************************
* NAME:     LCD : com2_puts
* PURPOSE:	
*****************************************************************************/
void LCDPuts(char *s)	
{	
	while(*s)
	{		
		com2_putchar(*s);
		//메인보드에서 터치패널로 보내는 tml명령어를 출력하기 위함
		com4_putchar(*s);
		s++;		
	}	
} 
/*F**************************************************************************
* NAME:     com2_putchar
* PURPOSE:	
*****************************************************************************/
void com2_putchar(char c)	
{	
	while(SCI2.SSR.BIT.TDRE == 0);	
	SCI2.TDR = c;	
	SCI2.SSR.BIT.TDRE = 0;	
}
/*F**************************************************************************
* NAME:     receive_rx2
* PURPOSE:	rx2_data[20],rx2_data_count,rx2_start
*****************************************************************************/
void receive_rx2()
{
	char c;			
	if(rx2_ring.head != rx2_ring.tail)
	{				
		c = rx2_ring.buf[rx2_ring.tail++];		
		if(rx2_ring.tail == RX_RING_BUF_SIZE) rx2_ring.tail=0;
		if(c>64 && c<91) c += 32;
		if(c == '[')
		{
			rx2_start = ON;
			rx2_data_count = 0;
			memset(rx2_data,0,20);
		}
		else if(rx2_start == ON && c == ']')
		{					
			rx2_start = OFF;			
			com2_BufPutData(rx2_data);
		}
		else if(rx2_start == ON)
		{
			rx2_data[rx2_data_count++] = c;
		}				
	}	
}
// vector 152 ERI2 SCI2
__interrupt(vect=152) void INT_ERI2_SCI2(void)
{ 
  	SCI2.SSR.BIT.ORER = 0;
  	SCI2.SSR.BIT.FER  = 0;
  	SCI2.SSR.BIT.PER  = 0; 
  	SCI2.SCR.BIT.RE   = 1;      //rxd enable
    SCI2.SCR.BIT.RIE  = 1;    	//rxd interript enable     
}

// vector 153 RXI2 SCI2
__interrupt(vect=153) void INT_RXI2_SCI2(void)
{          
	char c;                  
	SCI2.SSR.BIT.RDRF=0;         	// receive data register full bit clear; 
	c = SCI2.RDR;   	         	//receive data register read;		
	
	rx2_ring.buf[rx2_ring.head++] = c;
	 
	if(rx2_ring.head == RX_RING_BUF_SIZE) rx2_ring.head = 0;
	
	receive_rx2();	
}
/*F**************************************************************************
* NAME:     PC : PCPuts
* PURPOSE:	PC Serial
*****************************************************************************/
void PCPuts(char *s)	
{	
	while(*s)
	{		
		com4_putchar(*s);
		s++;		
	}	
} 	 	 
/*F**************************************************************************
* NAME:     com4_putchar
* PURPOSE:	
*****************************************************************************/
void com4_putchar(char c)	
{	
	while(SCI4.SSR.BIT.TDRE == 0);	
	SCI4.TDR = c;	
	SCI4.SSR.BIT.TDRE = 0;	
}
/*F**************************************************************************
* NAME:     receive_rx4
* PURPOSE:	rx4_data[20],rx4_data_count,rx4_start
*****************************************************************************/
void receive_rx4()
{

	char c;			
	static BYTE rx4_start;
	static BYTE rx4_data_count;
	static char rx4_data[20];


	if(rx4_ring.head != rx4_ring.tail)
	{				
		c = rx4_ring.buf[rx4_ring.tail++];	
		if(rx4_ring.tail == RX_RING_BUF_SIZE) rx4_ring.tail=0;
		
		if( (c == '[') )
		{
			rx4_start = ON;
			rx4_data_count = 0;
			memset(rx4_data,0,20);
		}
		else if(rx4_start == ON && c == ']' )
		{					
			rx4_start = OFF;			
			com4_BufPutData(rx4_data);
		}			
		else if(rx4_start == ON)
		{
			rx4_data[rx4_data_count++] = c;
		}				
	}					
}
// vector 160 ERI4 SCI4
__interrupt(vect=160) void INT_ERI4_SCI4(void)
{
	SCI4.SSR.BIT.ORER = 0;
 	SCI4.SSR.BIT.FER  = 0;
  	SCI4.SSR.BIT.PER  = 0; 
  	SCI4.SCR.BIT.RE   = 1;      //rxd enable
    SCI4.SCR.BIT.RIE  = 1;    	//rxd interript enable		
	   
}

// vector 161 RXI4 SCI4
__interrupt(vect=161) void INT_RXI4_SCI4(void)
{
	char c;	                  
	SCI4.SSR.BIT.RDRF=0;         	// receive data register full bit clear; 
	c = SCI4.RDR;   	         	//receive data register read;			
	
	rx4_ring.buf[rx4_ring.head++] = c;
	 
	if(rx4_ring.head == RX_RING_BUF_SIZE) rx4_ring.head = 0;

	receive_rx4();			
}

/*F**************************************************************************
* NAME:     com5_putchar
* PURPOSE:	
*****************************************************************************/
void com5_putchar(char c)	
{	
	while(SCI5.SSR.BIT.TDRE == 0);	
	SCI5.TDR = c;	
	SCI5.SSR.BIT.TDRE = 0;	
}
/*F**************************************************************************
* NAME:     Generator : GenPuts
* PURPOSE:	
*****************************************************************************/
void GenPuts(char *s)	
{	
 	if(gen_option == OFF)		return;

	while(*s)
	{		
		com5_putchar(*s);
		s++;		
	}	
} 
/*F**************************************************************************
* NAME:     receive_rx5
* PURPOSE:	rx5_data[20],rx5_data_count,rx5_start
*****************************************************************************/
void receive_rx5()
{
	char c;		
	if(rx5_ring.head != rx5_ring.tail)
	{				
		c = rx5_ring.buf[rx5_ring.tail++];
		
		if(rx5_ring.tail == RX_RING_BUF_SIZE) rx5_ring.tail=0;
		if(c>64 && c<91) c += 32;
		
		if(c == '(')
		{
			rx5_start = ON;
			rx5_data_count = 0;
			memset(rx5_data,0,20);
		}
			else if(rx5_start == ON && c == ')')
		{					
			rx5_start = OFF;			
			com5_BufPutData(rx5_data);
		}			
		else if(rx5_start == ON)
		{
			rx5_data[rx5_data_count++] = c;
		}			
	}		
}
// vector 192 ERI5 SCI5
__interrupt(vect=192) void INT_ERI5_SCI5(void)
{
	SCI5.SSR.BIT.ORER = 0;
 	SCI5.SSR.BIT.FER  = 0;
  	SCI5.SSR.BIT.PER  = 0; 
  	SCI5.SCR.BIT.RE   = 1;      //rxd enable
    SCI5.SCR.BIT.RIE  = 1;    	//rxd interript enable		
	   
}

// vector 193 RXI5 SCI5
__interrupt(vect=193) void INT_RXI5_SCI5(void)
{
	char c;                  
	SCI5.SSR.BIT.RDRF=0;         	// receive data register full bit clear; 
	c = SCI5.RDR;   	         	// receive data register read;			
	
	rx5_ring.buf[rx5_ring.head++] = c;
	 
	if(rx5_ring.head == RX_RING_BUF_SIZE) rx5_ring.head = 0;
	
	receive_rx5();				
}

//////////////////////////////////////////////////////////////
//Ring Buffer
//////////////////////////////////////////////////////////////

//Buffer_0
//////////////////////////////////////////////////////////////
int com0_BufGetSize()
{
	return (m_nHead0 - m_nTail0 + RX_RING_BUF_SIZE) % RX_RING_BUF_SIZE;
}

void com0_BufClear()
{	
	m_nHead0=m_nTail0=0;
	memset(rx_buf0,0,sizeof(rx_buf0));

}
void com0_BufPutData(char *data)
{
	if(com0_BufGetSize() == (RX_RING_BUF_SIZE - 1))
		return;	

	memcpy(rx_buf0[m_nHead0++],data,20);
	m_nHead0 %= RX_RING_BUF_SIZE;
}

void com0_BufGetData(char *data)
{
	if(com0_BufGetSize() == 0)
		return;

	memcpy(data,rx_buf0[m_nTail0++],20);
	m_nTail0 %= RX_RING_BUF_SIZE;	
}
//////////////////////////////////////////////////////////////

//Buffer_2
//////////////////////////////////////////////////////////////
int com2_BufGetSize()
{
	return (m_nHead2 - m_nTail2 + RX_RING_BUF_SIZE) % RX_RING_BUF_SIZE;
}
void com2_BufClear()
{	
	m_nHead2=m_nTail2=0;
	memset(rx_buf2,0,sizeof(rx_buf2));
}
void com2_BufPutData(char *data)
{
	if(com2_BufGetSize() == (RX_RING_BUF_SIZE - 1))
		return;	

	memcpy(rx_buf2[m_nHead2++],data,20);
	m_nHead2 %= RX_RING_BUF_SIZE;
}
void com2_BufGetData(char *data)
{
	if(com2_BufGetSize() == 0)
		return;

	memcpy(data,rx_buf2[m_nTail2++],20);
	m_nTail2 %= RX_RING_BUF_SIZE;
}
//////////////////////////////////////////////////////////////

//Buffer_4
//////////////////////////////////////////////////////////////
int com4_BufGetSize()
{
	return (m_nHead4 - m_nTail4 + RX_RING_BUF_SIZE) % RX_RING_BUF_SIZE;
}
void com4_BufClear()
{	
	m_nHead4=m_nTail4=0;
	memset(rx_buf4,0,sizeof(rx_buf4));
}
void com4_BufPutData(char *data)
{
	if(com4_BufGetSize() == (RX_RING_BUF_SIZE - 1))
		return;	

	memcpy(rx_buf4[m_nHead4++],data,20);
	m_nHead4 %= RX_RING_BUF_SIZE;
}
void com4_BufGetData(char *data)
{
	if(com4_BufGetSize() == 0)
		return;

	memcpy(data,rx_buf4[m_nTail4++],20);
	m_nTail4 %= RX_RING_BUF_SIZE;
}
//////////////////////////////////////////////////////////////

//Buffer_5
//////////////////////////////////////////////////////////////
int com5_BufGetSize()
{
	return (m_nHead5 - m_nTail5 + RX_RING_BUF_SIZE) % RX_RING_BUF_SIZE;
}
void com5_BufClear()
{	
	m_nHead5=m_nTail5=0;
	memset(rx_buf5,0,sizeof(rx_buf5));
}
void com5_BufPutData(char *data)
{
	if(com5_BufGetSize() == (RX_RING_BUF_SIZE - 1))
		return;	

	memcpy(rx_buf5[m_nHead5++],data,20);
	m_nHead5 %= RX_RING_BUF_SIZE;
}
void com5_BufGetData(char *data)
{
	if(com5_BufGetSize() == 0)
		return;

	memcpy(data,rx_buf5[m_nTail5++],20);
	m_nTail5 %= RX_RING_BUF_SIZE;
}
//////////////////////////////////////////////////////////////
void MP3Printf(char *format, ...)
{
	va_list	ap;
	char str[256];

	va_start(ap, format);
	vsprintf(str, format, ap);
	MP3Puts(str);
	va_end(ap);
	
	return;
}


void LCDPrintf(char *format, ...)
{
	va_list	ap;
	char str[256];

	va_start(ap, format);
	vsprintf(str, format, ap);
	LCDPuts(str);
	va_end(ap);
	
	return;
}

void PCPrintf(char *format, ...)
{
	va_list	ap;
	char str[256];

	va_start(ap, format);
	vsprintf(str, format, ap);
	PCPuts(str);
	va_end(ap);
	
	return;
}
 
void GenPrintf(char *format, ...)
{
	va_list	ap;
	char str[256];

	if(gen_option == OFF)		return;

	va_start(ap, format);
	vsprintf(str, format, ap);
	GenPuts(str);
	va_end(ap);
	
	return;
} 

// used for Debugging
void UartPrintf(char *format, ...)
{
	va_list	ap;
	char str[256];

	va_start(ap, format);
	vsprintf(str, format, ap);
	PCPuts(str);
	va_end(ap);
	
	return;
}


void Debug(BYTE sw, char *format, ...)
{
	va_list	ap;
	char str[256];

	if(sw == ON)
	{
		va_start(ap, format);
		vsprintf(str, format, ap);
		PCPuts(str);
		va_end(ap);
	}
	
	return;
}


// WDG90 version 날짜 정보를 버전넘버로 변환하기 위한 함수
void ASCII2Version(char str[5])
{
	// Qface 코드
	Change_ASC(str[0]); Change_ASC(str[1]); Change_ASC(str[2]);
    if(str[3]) com4_putchar(str[3]); else PCPuts("0");
    PCPuts(" \r\n\r\n");

}

void Change_ASC (char val)
{
    switch(val)
    {
        case '1': PCPuts("01"); break;
        case '2': PCPuts("02"); break;
        case '3': PCPuts("03"); break;
        case '4': PCPuts("04"); break;
        case '5': PCPuts("05"); break;
        case '6': PCPuts("06"); break;
        case '7': PCPuts("07"); break;
        case '8': PCPuts("08"); break;
        case '9': PCPuts("09"); break;
        case 'a': PCPuts("10"); break;
        case 'b': PCPuts("11"); break;
        case 'c': PCPuts("12"); break;
        case 'd': PCPuts("13"); break;
        case 'e': PCPuts("14"); break;
        case 'f': PCPuts("15"); break;
        case 'g': PCPuts("16"); break;
        case 'h': PCPuts("17"); break;
        case 'i': PCPuts("18"); break;
        case 'j': PCPuts("19"); break;
        case 'k': PCPuts("20"); break;
        case 'l': PCPuts("21"); break;
        case 'm': PCPuts("22"); break;
        case 'n': PCPuts("23"); break;
        case 'o': PCPuts("24"); break;
        case 'p': PCPuts("25"); break;
        case 'q': PCPuts("26"); break;
        case 'r': PCPuts("27"); break;
        case 's': PCPuts("28"); break;
        case 't': PCPuts("29"); break;
        case 'u': PCPuts("30"); break;
        case 'v': PCPuts("31"); break;
        default: break;
    }
}