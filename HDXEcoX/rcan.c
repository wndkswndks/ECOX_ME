/*C***************************************************************************
* $RCSfile: rcan.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the rcan library
*----------------------------------------------------------------------------
* RELEASE:      $Name: rcan.c     
* Edited  By    $Id: Joony  
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include "Includes.h"

/*_____ D E C L A R A T I O N ______________________________________________*/

/*_____ D E F I N E ________________________________________________________*/
#define RCAN_BUF_SIZE		64

/*_____ F U C T I O N ______________________________________________________*/
void rcan_init(void);
void set_RCAN0_init(void);
void set_RCAN0MB_init(void);
void set_RCAN0_start(void);
void set_1544_INTC_init(void);

void rcan_puts(BYTE, WORD, LONG, WORD);
void RCAN0_Tx(void);
void rcan_get_data(BYTE);
int CANBufGetSize(void);
void CANBufClear(void);
void CANBufPutData(CANData_t *);
void CANBufGetData(CANData_t *);

/*_____ V A R I A B L E ____________________________________________________*/
unsigned int n_rcanhead,n_rcantail;
CANData_t	rcanrx_buf[RCAN_BUF_SIZE];


void rcan_init(void)
{	
    CANBufClear();
	set_RCAN0_init();								/* RCAN initialization */
	set_RCAN0MB_init();
	set_RCAN0_start();
	set_1544_INTC_init();							/* H8SX/1544 interrupt settings */
	
	/*** IO ***/
	RCANET0.RCANMON.BYTE = 0x20;					/* Enable RCAN-ET transmit and receive pins */
	P6.ICR.BIT.B4 = 1;								/* Set P64 as CRx_0 (input pin) */
}
/****************************************************************************/
/* H8SX/1544 INTC Initialize routine */
/****************************************************************************/
void set_1544_INTC_init(void)
{
	INTC.INTCR.BIT.INTM = 2;						/* Interrupt control mode 2 */
	INTC.IPRQ.BIT._RCAN01 = 2;						/* Set interrupt priority level: RCAN */
	set_imask_exr(0);								/* Interrupt mask level 0 */
}

/****************************************************************************/
/* RCAN Initialize routine */
/****************************************************************************/
void set_RCAN0_init(void)
{
    unsigned int i,j;
	
    RCANET0.MCR.WORD |= 0x0001;						/* Reset request (set automatically by a hardware reset) */ 
    while((RCANET0.GSR.WORD & 0x0008) != 0x0008);	/* GSR3 = 1? (RCAN-ET reset status) */
    while((RCANET0.IRR.WORD & 0x0001) != 0x0001);	/* IRR0 = 1? (reset/halt/sleep interrupt) */
    RCANET0.IRR.WORD = 0x0001;						/* Clear IRR0 (clearing condition: write 1) */
    RCANET0.MCR.WORD |= 0x8000; 					/* Reorder ID: Set to MCR15 = 1 (initial setting) */
	
    for(i = 0;i < 16;i++)
	{ 												/* Initialize mailbox (RAM area) */
        RCANET0.MB[i].CTRL0.WORD.H = 0;
        RCANET0.MB[i].CTRL0.WORD.L = 0;
		RCANET0.MB[i].LAFM.WORD.H = 0;
		RCANET0.MB[i].LAFM.WORD.L = 0;
		for(j = 0;j < 8;j++)
		{
			RCANET0.MB[i].MSG_DATA[j] = 0;
		}
	}
}

/****************************************************************************/
/* RCAN Mailbox Initialize routine */
/****************************************************************************/
void set_RCAN0MB_init(void)
{
	RCANET0.IMR.WORD &= 0xFFFD;						/* Enable mailbox empty interrupt */
//	RCANET0.IMR.WORD &= 0xFEFD;						/* Enable mailbox empty interrupt and receive interrupt */
	RCANET0.MBIMR0.WORD &= 0xFFFB;					/* Enable MB0 interrupt */
	
	RCANET0.MB[1].CTRL0.WORD.H = 0x8000;			/* STDID: H'000, extended format, data frame */
	RCANET0.MB[1].CTRL0.WORD.L = 0x0001;			/* EXTID: H'00000 */
	RCANET0.MB[1].CTRL1.BYTE.H = 0x00;				/* Set mailbox 1 to transmit */
	RCANET0.MB[1].CTRL1.BYTE.L = 0x08;				/* Data length: 8 bytes */
	RCANET0.MB[1].MSG_DATA[0] = 0x55;				/* Transmit data: 0x55 */
	RCANET0.MB[1].MSG_DATA[1] = 0x55;				/* Transmit data: 0x55 */
	RCANET0.MB[1].MSG_DATA[2] = 0x55;				/* Transmit data: 0x55 */
	RCANET0.MB[1].MSG_DATA[3] = 0x55;				/* Transmit data: 0x55 */
	RCANET0.MB[1].MSG_DATA[4] = 0x55;				/* Transmit data: 0x55 */
	RCANET0.MB[1].MSG_DATA[5] = 0x55;				/* Transmit data: 0x55 */
	RCANET0.MB[1].MSG_DATA[6] = 0x55;				/* Transmit data: 0x55 */
	RCANET0.MB[1].MSG_DATA[7] = 0x55;				/* Transmit data: 0x55 */
   
	RCANET0.MB[2].CTRL0.WORD.H	= 0x92FF;			/* STDID: H'555, extended format, data frame */
	RCANET0.MB[2].CTRL0.WORD.L	= 0x3401;			/* EXTID: H'2AAAA */
	RCANET0.MB[2].LAFM.WORD.H	= 0x0000;			/* STD_LAFM and IDE_LAFM settings */
	RCANET0.MB[2].LAFM.WORD.L	= 0x0000;			/* EXTID_LAFM settings */
	RCANET0.MB[2].CTRL1.BYTE.H	= 0x02;				/* Set mailbox 0 to receive */
    
	/* Bit rate = 500 kbps */
	RCANET0.BCR1.WORD = 0x5200;						/* TSEG1 = 5 (6tq), TSEG2 = 2 (3tq), SJW = 0, BSP = 0, (pφ = 20 MHz)*/
	RCANET0.BCR0.WORD = 0x0001;						/* BRP = 1 : Baud Rate Pre-scale : 4*peripheral bus clock */
	/* Bit rate = 1 Mbps */
//	RCANET0.BCR1.WORD = 0x4300;						/* TSEG1 = 4 (5tq), TSEG2 = 3 (4tq), SJW = 0, BSP = 0, (pφ = 20 MHz)*/
//	RCANET0.BCR0.WORD = 0x0000;						/* BRP = 0 : Baud Rate Pre-scale : 2*peripheral bus clock */	
	
}

/****************************************************************************/
/* RCAN start routine */
/****************************************************************************/
void set_RCAN0_start(void)
{
	RCANET0.MCR.WORD &= 0xFFFE;						/* Clear MCR0 */
	while ((RCANET0.GSR.WORD & 0x0008) != 0x0000);	/* GSR3 = 0? */
}
/****************************************************************************/
/* RCAN send message routine */
/****************************************************************************/
void RCAN0_Tx(void)
{    
    unsigned int time_out= 100000;
	RCANET0.TXACK0.WORD = 0x0002;					/* Clear transmit-end flag (clearing condition: write 1) */
	RCANET0.TXPR0.LONG = 0x00000002;				/* Set MB1 to transmit-wait status */
	
    while((RCANET0.TXACK0.WORD & 0x0002) && time_out) /* Check flag */
    {
        time_out--;
        if(time_out == 1) PCPuts("\r\n*** RCAN Tx Time out ***\r\n");
    }
}

/*F**************************************************************************
* NAME:    rcan_puts
* PURPOSE: 해당하는 CANdriver(id)에 명령, valu, msg를 보낸다.
*****************************************************************************/
void rcan_puts(BYTE id, WORD inst, LONG value, WORD msg)
{
  	LONG	canId;


  	// id(십진수 13)를 canId 형태(0x0013)형태로 변경 (id/10 * 0x10 + id%10)
  	// ex) CO_MOTOR : 13 -> 0x0013
	canId = (id / 10) * 0x10 + (id % 10);
	RCANET0.MB[1].CTRL0.WORD.H = 0x8000;
    RCANET0.MB[1].CTRL0.WORD.L = canId;

	RCANET0.MB[1].MSG_DATA[0] = H_BYTE(inst);
	RCANET0.MB[1].MSG_DATA[1] = L_BYTE(inst);
	RCANET0.MB[1].MSG_DATA[2] = BYTE3(value);
	RCANET0.MB[1].MSG_DATA[3] = BYTE2(value);
	RCANET0.MB[1].MSG_DATA[4] = BYTE1(value);
	RCANET0.MB[1].MSG_DATA[5] = BYTE0(value);
	RCANET0.MB[1].MSG_DATA[6] = H_BYTE(msg);
	RCANET0.MB[1].MSG_DATA[7] = L_BYTE(msg);	
	
	// UartPrintf("==> BYTE : %d, Msg_data : %d \r\n", BYTE0(value), RCANET0.MB[1].MSG_DATA[5]);
    RCAN0_Tx();
}


/*F**************************************************************************
* NAME:     rcan_get_data
* PURPOSE:
*****************************************************************************/
void rcan_get_data(BYTE i)
{
	CANData_t	rcan_data;
	
	rcan_data.inst  = (((WORD)RCANET0.MB[i].MSG_DATA[0] << 8) & 0xff00);
	rcan_data.inst |= (((WORD)RCANET0.MB[i].MSG_DATA[1]) & 0x00ff);
	
	rcan_data.value  = (((LONG)RCANET0.MB[i].MSG_DATA[2] << 24) & 0xff000000);
	rcan_data.value |= (((LONG)RCANET0.MB[i].MSG_DATA[3] << 16) & 0x00ff0000);
	rcan_data.value |= (((LONG)RCANET0.MB[i].MSG_DATA[4] << 8)	& 0x0000ff00);
	rcan_data.value |= (((LONG)RCANET0.MB[i].MSG_DATA[5]) & 0x000000ff);
	
    rcan_data.msg  = (((WORD)RCANET0.MB[i].MSG_DATA[6] << 8) & 0xff00);
	rcan_data.msg |= (((WORD)RCANET0.MB[i].MSG_DATA[7]) & 0x00ff);
	
	CANBufPutData(&rcan_data);
}

/****************************************************************************/
/* Data Frame Received Interrupt routine(MailBox0) */
/****************************************************************************/
__interrupt(vect=222) void INT_ERSOVR0_HCAN(void)
{
	RCANET0.RXPR0.WORD = 0x0004;					/* Clear receive-end flag (clearing condition: write 1) */
	while(RCANET0.RXPR0.WORD & 0x0004);				/* Check flag */
	rcan_get_data(2); 								//Mail Box 0번 사용

   RCANET0.IRR.WORD |= 0x0002; 						//receive interrupt clear
}


/////////////////////////////////////////////////////////////
//CAN Ring Buffer
//////////////////////////////////////////////////////////////

int CANBufGetSize()
{
	return (n_rcanhead - n_rcantail + RCAN_BUF_SIZE) % RCAN_BUF_SIZE;
}

void CANBufClear()
{	
	n_rcanhead=n_rcantail=0;
	memset(rcanrx_buf,0,(8*RCAN_BUF_SIZE));

}

void CANBufPutData(CANData_t *rcan_data)
{    
	if(CANBufGetSize() == (RCAN_BUF_SIZE - 1))
		return;	

	memcpy(rcanrx_buf+n_rcanhead,rcan_data,8);
	n_rcanhead++;
	n_rcanhead %= RCAN_BUF_SIZE;	
}

void CANBufGetData(CANData_t *rcan_data)
{
	if(CANBufGetSize() == 0)
		return;

	memcpy(rcan_data,rcanrx_buf+n_rcantail,8);
	n_rcantail++;
	n_rcantail %= RCAN_BUF_SIZE;	
}