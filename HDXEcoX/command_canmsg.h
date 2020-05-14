/*H***************************************************************************
* $RCSfile: canmsg.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the canmsg header define
*----------------------------------------------------------------------------
* RELEASE:      $Name: canmsg.h     
* Edited  By    $Id: Joony
*****************************************************************************/

#ifndef CANMSG_H
#define CANMSG_H

/**************************************************************************************

	CAN으로 보내는 명령어는 0x0000(16bit)
	상위 4bit	: 보드 ID
	하위 12bit	: command
	
****************************************************************************************/

/*===================================================================================*/
// Stepping Motor Driver Message
/*===================================================================================*/
#define	CAN_AXIS_POS_DIR			0x9101
#define	CAN_AXIS_0_1_UNIT_PULSE		0x9102
#define	CAN_AXIS_P_MAX_PULSE		0x9103
#define	CAN_AXIS_N_MAX_PULSE		0x9104
#define	CAN_AXIS_ACCEL_STEP			0x9105
#define	CAN_AXIS_START_SPEED		0x9106
#define	CAN_AXIS_MAX_SPEED			0x9107
#define	CAN_AXIS_MOVE_CONST			0x9108
#define	CAN_AXIS_MOVE				0x9109
#define	CAN_AXIS_SLOW_STOP			0x9110
#define	CAN_AXIS_CNT				0x9111	// 축의 카운트 값 회신
#define	CAN_AXIS_MOVE_ACK_REQ		0x9114

#define	CAN_FW_REV_NUM				0x9191

#define	CAN_AXIS_ERROR				0x9199

#define	CAN_TEST					0x9999

/*===================================================================================*/	
// CAN Common Command
/*===================================================================================*/					
#define	CAN_LASER_CTRL				0x1300

/*===================================================================================*/
// ADC board
/*===================================================================================*/
#define	CAN_ADC_CTRL				0x4700 // 견치, 직선축 거리, 회전축 각도 요청




////////////////////////////////
// todo : where is right file? system.h? command_canmag.h?, rcan.h?, motor_driver.h?
typedef struct 
{
	LONG 	pos_dir;
	LONG 	neg_dir;
	LONG 	unit_0_1_pulse;
	LONG 	unit_0_01_pulse;
	LONG 	p_max_pulse;
	LONG 	n_max_pulse;
	LONG 	accel_step;
	LONG 	start_speed;
	LONG 	max_speed;
	BYTE 	stop_flag;
} axisCANMotor_t;		//CAN Motor comunication protocal

//////////////////////////////////////
extern void ProcessCANAck(void);
extern int ReadCanAdc_RO(WORD *adcValue);


extern axisCANMotor_t gAxisCO;
extern axisCANMotor_t gAxisSD;
extern WORD	dmy_msg;
extern WORD	dly_t;
extern BYTE	axis_init_value_flag;
extern BYTE	duplicated_ID_check_cnt;
extern LONG	dmy_val;

#endif
                                                        