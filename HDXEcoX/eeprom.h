/*H***************************************************************************
* $RCSfile: eeprom.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the eeprom header 
*----------------------------------------------------------------------------
* RELEASE:      $Name: eeprom.h
* Edited  By    $Id: sslim
*****************************************************************************/
#ifndef _EEROM_H
#define _EEROM_H

/*_____ D E F I N E ________________________________________________________*/
#define	DELY		50
#define	SWP			P1.DR.BIT.B5
#define	SDA			P1.DR.BIT.B6		//PB.DR.BIT.B2
#define	SDA_CH		P1.PORT.BIT.B6
#define	SCL			P1.DR.BIT.B7
			
#define	SDA_IN   	(P1.DDR = 0xAD)		//1010	1101 	
#define	SDA_OUT  	(P1.DDR = 0xED)		//1110	1101

extern void write_eeprom_BYTE(BYTE address, BYTE data);
extern void write_eeprom_WORD(BYTE address, WORD data);
extern void write_eeprom_LONG(BYTE address, LONG data);

extern BYTE read_eeprom_BYTE(BYTE address);
extern WORD read_eeprom_WORD(BYTE address);
extern LONG read_eeprom_LONG(BYTE address);

#endif