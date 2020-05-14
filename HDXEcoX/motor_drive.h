/*H***************************************************************************
* $RCSfile: motor_drive.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the motor_drive header
*----------------------------------------------------------------------------
* RELEASE:      $Name: motor_drive.h      
* Edited  By    $Id: Joony 
*****************************************************************************/
#ifndef _MOTOR_DRIVE_H
#define _MOTOR_DRIVE_H

// todo : header에서는 제외해도 될듯
// motor_drive.c에서만 사용되는듯  ==> capture.c등에서 사용
typedef struct 
{
	BYTE 	dir;				// direction
	BYTE 	pos_dir;			// value of positive dir
	BYTE 	neg_dir;			// value of negative dir

	LONG 	cycle;				// period(1 cycle) ==> naming : A0_Hz, only is used in file
	LONG 	duty;
	LONG 	speedHz;			// speedHz : ex) 2000Hz
	LONG 	dest_speedHz;		// destinate

	BYTE 	stop_flag;		
	BYTE	setting_cycle;		// set change cycle(주기를 변경하여 속도를 변경함여러곳에서 값 변경됨
	BYTE 	accel_stage;		// 1: 가속상태, 0 : 등속상태

	LONG 	total_pulseCnt;		// 총 이동(할) 거리 (거리 = pulse * count)
	LONG 	moved_pulseCnt;		// 이동된 거리(pulseCnt)
	LONG 	decrease_pulseCnt;	// 감속을 시작할 위치

	LONG	pos_cnt;			// position of axis( = pulse * cnt)
	LONG 	p_maxPulseCnt;		// 양의 방향 최대거리
	LONG 	n_maxPulseCnt;		// 음의 방향 최대거리 
	LONG 	unit_0_1_pulse;		// 0.1mm 단위 pulseCnt, pulseCnt_0_1mm, 
	
	LONG 	min_speedHz;		// minimum speedHz of axis
	LONG 	max_speedHz;		// 
	LONG 	accel_step;

	BYTE	p_over_flag;		// 양의 거리 over flag -> isOver_pMaxPulseCnt,
	BYTE	n_over_flag;		// 음의 거리 over flag -> isOver_nMaxPulseCnt

	BYTE 	org_complete_flag;
	BYTE 	onOrgSensor; 			// org sensor detect on/off
} motor_t;


// /////////////////////////////////////////////////////////////////
extern void RO_SetInitialValue(void);
extern void PA_SetInitialValue(void);
extern void PA_MotorStart(BYTE, LONG);
extern void RO_MotorStart(BYTE, LONG);
extern void PA_MotorDrive(BYTE, LONG, LONG, LONG);
extern void PA_MotorDrive(BYTE, LONG, LONG, LONG);
extern void PA_MotorStop(void);
extern void RO_MotorStop(void);
extern LONG RO_GetPositionCnt(void);
extern LONG PA_GetPositionCnt(void);
extern LONG RO_GetSpeed(void);
extern LONG PA_GetSpeed(void);

extern void PA_MotorChangeSpeed(BYTE, LONG);
extern void RO_MotorChangeSpeed(BYTE, LONG);

extern void PA_SetOrigin(BYTE);
extern void RO_SetOrigin(BYTE);
extern void PA_UpdateState(void);

// extern void A0_slow_stop(void);
// extern void A1_slow_stop(void);

void init_A1_table_value(void);

//------------- pano 관련 func
extern void PA_increase_stage(BYTE);
extern void RO_increase_stage(BYTE);
extern void PA_stop_cnt(LONG);
extern void RO_stop_cnt(LONG);
extern void PA_tot_pulse(LONG);
extern void RO_tot_pulse(LONG);

extern BYTE RO_p_n_over_flag(void);
extern BYTE PA_p_n_over_flag(void);

BYTE IsMotorStop(varMotor_t*);

extern BYTE RO_CheckStall(void);
extern BYTE PA_CheckStall(void);

// /////////////////////////////////////////////////////////////////
// // Variable
extern motor_t		mPA;
extern motor_t		mRO;

/**************************************************************************
* origin position
***************************************************************************/
extern BYTE		A0_pos_step;		// todo : interface func
extern BYTE		A1_pos_step;

extern BYTE 	resetOrigin_PA;
extern BYTE 	resetOrigin_RO;


// /////////////////////////////////////////////////////////////////
// // MOTOR A1 Variable


// extern BYTE	A1_ufs_accel_flag;



#endif