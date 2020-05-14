/*H***************************************************************************
* $RCSfile: capture.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the capture header define
*----------------------------------------------------------------------------
* RELEASE:      $Name: capture.h      
* Edited  By    $Id: Joony 
*****************************************************************************/

#ifndef _CAPT_H
#define _CAPT_H

///////// (main_mode)	/////////////
#define	CT_MODE				11
#define	CT_SCOUT_MODE		12
#define	CT_CHILD_MODE		13

#define PANO_MODE			20
#define PANO_STAN_MODE		21
#define PANO_TMJ1_MODE		22
#define PANO_TMJ2_MODE		23
#define PANO_BITEWING_MODE	24

#define SCAN_CEPH_MODE		40
#define SCAN_CEPH_LA_MODE	41
#define SCAN_CEPH_PA_MODE	42	// PA: posteroanterior

#define S_CEPH_LA_DET_TRIG_4B_N		3148	// 4 bin. normal
#define S_CEPH_LA_DET_TRIG_4B_F		3148	// 4 bin. fast
#define S_CEPH_PA_DET_TRIG_4B_N		3220	// 4 bin. normal
#define S_CEPH_PA_DET_TRIG_4B_F		3227	// 4 bin. fast

//--------------
#define PANO_STAN_TBL		380
#define PANO_T47_TBL		359	// Teeth4.7		//multi
#define PANO_TMJ_TBL		335		// todo : 외부에서 사용되는게 없어 capture.c로 이동 가능

extern BYTE	exp_ok_flag;
extern BYTE	exp_sw_flag;
extern BYTE	exp_push;
extern BYTE software_exp;
extern BYTE software_rdy;


extern LONG	ct_capt_frame;
// extern BYTE ct_capt_end_check;
extern LONG	ct_capt_fps;
extern BYTE	scout_mode;
// extern double A1_curve_arr[140];
extern double A1_speed_arr[140];
extern double A1_pulse_arr[140]; 


#endif