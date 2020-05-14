/*H***************************************************************************
* $RCSfile: serial.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the includes serial header define
*----------------------------------------------------------------------------
* RELEASE:      $Name: serial.h     
* Edited  By    $Id: sslim
*****************************************************************************/

#ifndef	__SERIAL_HEADER__
#define __SERIAL_HEADER__

////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////
extern void MP3Puts(char *);
extern void LCDPuts(char *);
extern void PCPuts(char *);
extern void GenPuts(char *);

extern int  com0_BufGetSize();
extern int  com2_BufGetSize();
extern int  com4_BufGetSize();
extern int  com5_BufGetSize();

extern void com0_BufPutData(char *);
extern void com2_BufPutData(char *);
extern void com4_BufPutData(char *);
extern void com5_BufPutData(char *);

extern void com0_BufGetData(char *);
extern void com2_BufGetData(char *);
extern void com4_BufGetData(char *);
extern void com5_BufGetData(char *);

extern void serial_init(void);
extern void LCDPrintf(char *, ...);
extern void PCPrintf(char *, ...);
extern void GenPrintf(char *, ...);
extern void	UartPrintf(char *,...);
extern void MP3Printf(char *, ...);
extern void Debug(BYTE, char*, ...);
extern void ASCII2Version(char*);


#endif 	/*__SERIAL_HEADER__*/