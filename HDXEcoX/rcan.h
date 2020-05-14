/*H***************************************************************************
* $RCSfile: rcan.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the rcan header define
*----------------------------------------------------------------------------
* RELEASE:      $Name: rcan.h      
* Edited  By    $Id: Joony
*****************************************************************************/

#ifndef	_RCAN_HEADER_
#define _RCAN_HEADER_

/////////////////////////// CAN communication ///////////////////////////


/*====  CAN Motor ID  ====*/
#define	CO_MOTOR		13	// 1 axis collimator
#define	SD_MOTOR		22	// Scan ceph detector axis

#define	HL_MOTOR		51
#define	HR_MOTOR		52
#define	VT_MOTOR		53
#define	VB_MOTOR		54


//========== ADC CAN
#define ADC_CANINE		91
#define PA_ADC			95
#define RO_ADC			96



typedef struct {
	WORD	inst;
	LONG	value;
	WORD	msg;
} CANData_t;


/////////////////////////////////////////////////////////////////////////////////
extern void rcan_init(void);

// extern int CANBufGetSize();
// extern void CANBufClear();
// extern void CANBufPutData(CANData_t *rcan_data);
// extern void CANBufGetData(CANData_t *rcan_data);

// extern void process_canmsg(void);

#endif