

//_____ I N C L U D E S ____________________________________________________
#include "Includes.h"



//_____ D E C L A R A T I O N ______________________________________________
enum CaptureStates
{ 	S_IDLE, S_INITIAL, S_START, S_LOOP, S_CHANGE_SPEED, S_MOTOR_STOP, S_PRINT, S_FINISH, 
	S_WAIT, S_MOVE_INITIAL_CO, S_STEP2, S_STEP3, S_STEP4, S_STEP5, S_STEP6, 
};



//_____ D E F I N E ________________________________________________________
#define PI_ 		3.1415926535
#define	SCD_H		65.0				// source - Horizon collimator distance
#define	SCD_V		71.5				// source - Vertical collimator distance
#define	SOD			377.0				// source - object center distance
#define	SDD			577.0				// source - detecter distance


double Radians(double);
void MoveTrackingCo(t_TrackingCO *, t_Position_um *);
void CalculateTrackingCo(t_ObjectInfo *, t_TrackingCO *);
void TrackingObj(t_TrackingCO *);


extern LONG g_OnStartEvent;		// from command.c
LONG g_selectTrackingCO;
LONG offset_SCD_h;		// four_axis.c
LONG offset_SCD_v;		// four_axis.c
LONG g_SpeedOffset_H;
LONG g_SpeedOffset_V;

// { radius, distance, alpha, z }
t_ObjectInfo g_Obj = { 70.0, 10.0, 90.0, 0 } ;


//_____ F U C T I O N ______________________________________________________


// focal spot을 원점으로 해서 CP(Center Postion), CW(Center Width)으로 
// 4축 HR, HL, VT, VB collimator 이동
void MoveTrackingCo(t_TrackingCO *tc, t_Position_um *position_um)
{
    const LONG hl_focalPosition_um = 12050;		// 설계값, unit [um]
    const LONG hr_focalPosition_um = 22050;		// 설계값, unit [um]
    const LONG vt_focalPosition_um = 15700;		// 설계값, unit [um]
    const LONG vb_focalPosition_um = 5550;		// 설계값, unit [um]

    // hl, hr
	position_um->hl = hl_focalPosition_um - tc->init_CP_um - tc->init_CW_um;
	position_um->hr = hr_focalPosition_um + tc->init_CP_um - tc->init_CW_um;

    // vt, vb
	position_um->vt = vt_focalPosition_um - tc->init_CPZ_um - tc->init_CWZ_um;
    position_um->vb = vb_focalPosition_um + tc->init_CPZ_um - tc->init_CWZ_um;


	UartPrintf("==> hl_focal : %ld [um], hr_focal : %ld [um], vt_focal : %ld [um], vb_focal : %ld [um]\r\n", 
			hl_focalPosition_um, hr_focalPosition_um, vt_focalPosition_um, vb_focalPosition_um);
	UartPrintf("==> calcul position : HL = %ld [um], HR = %ld [um]  VT = %ld [um], VB = %ld [um] \r\n",	
			position_um->hl, position_um->hr, position_um->vt, position_um->vb);
    
    // offset postition r
	position_um->hl = position_um->hl + g_HL.offset_um;
	position_um->hr = position_um->hr + g_HR.offset_um;
	position_um->vt = position_um->vt + g_VT.offset_um;
    position_um->vb = position_um->vb + g_VB.offset_um;

    UartPrintf("==> Offset position : HL : %ld[um], HR : %ld[um], VT : %ld[um], VB : %ld[um] \r\n", 
    		g_HL.offset_um, g_HR.offset_um, g_VT.offset_um, g_VB.offset_um);
	UartPrintf("==> adjust Position : HL = %ld [um], HR = %ld [um], VT = %ld [um], VB = %ld [um] \r\n", 
			position_um->hl, position_um->hr, position_um->vt, position_um->vb);
	UartPrintf("\r\n");


	Ctrl_CAN_MOVE_UM(&g_HL, position_um->hl);
	Ctrl_CAN_MOVE_UM(&g_HR, position_um->hr);
	Ctrl_CAN_MOVE_UM(&g_VT, position_um->vt);
	Ctrl_CAN_MOVE_UM(&g_VB, position_um->vb);
}


void CalculateTrackingCo(t_ObjectInfo *obj, t_TrackingCO *tc)
{
	double SCD_h = SCD_H + (double)offset_SCD_h/1000;
	double SCD_v = SCD_V + (double)offset_SCD_v/1000;
	LONG thetaPerSec_RO; 			// RO theta per 1 second.
	int CW_pixel = 0;
	const double pixel_pitch = 0.238;
	double z_SODratio = 0;


	
	// 기본 값 계산
	thetaPerSec_RO = 360 / tc->captureTime;
	tc->d_Time = 1 / (double)tc->fpsCT;
	tc->d_Theta = (double)thetaPerSec_RO * tc->d_Time;
	tc->index = (int)(tc->captureTime / tc->d_Time);
	UartPrintf("==> fpsCT : %ld, index : %d,  d_Theta : %.3lf, d_Time : %.3lf, thetaPerSec_RO : %ld  \r\n", 
			tc->fpsCT, tc->index, tc->d_Theta, tc->d_Time, thetaPerSec_RO); 


	// cp_co 초기값 계산
	tc->init_CP = SCD_h * obj->distance * sin(Radians(obj->alpha)) 
				/ ( SOD - obj->distance * cos(Radians(obj->alpha)) );
	tc->init_CP_um = (LONG) (tc->init_CP * 1000);

	// cw_co 초기값 계산
	tc->init_CW = obj->radius * SCD_h / sqrt( pow(SOD - obj->distance, 2) - pow(obj->radius, 2) );
	tc->init_CW_um = (LONG) (tc->init_CW * 1000);
	CW_pixel = (int)(tc->init_CW * ( SDD / SCD_h) / pixel_pitch);

	// CPZ, CWZ
	tc->init_CPZ = obj->z * SCD_v / ( SOD - obj->distance * cos(Radians(obj->alpha)) );
	tc->init_CPZ_um = (LONG) (tc->init_CPZ * 1000); 

	z_SODratio = (SOD - obj->distance + obj->radius) / pow( (SOD - obj->distance - obj->radius), 2);
	tc->init_CWZ = SCD_v * obj->zh / 2 * z_SODratio;
	tc->init_CWZ_um = (LONG) (tc->init_CWZ * 1000);



	// Object info : radius, distance, alpha, z
	UartPrintf("==> object : d = %.3lf [mm], r = %.3lf [mm], alpha : %.3lf [mdgree] \r\n", 
			obj->distance, obj->radius, obj->alpha);
	UartPrintf("==> object : z = %.3lf [mm], zh = %.3lf [mm] \r\n", obj->z, obj->zh);
	UartPrintf("\r\n");

	UartPrintf("==> speedOffset_H : %.1lf, speedOffset_V : %.1lf \r\n", (double)g_SpeedOffset_H/100, (double)g_SpeedOffset_V/100);
	UartPrintf("==> SCD_H : %.2lf [mm], offset SCD_H: %.2lf [mm],  adj_SCD_h : %.2lf [mm] \r\n", 
			(double)SCD_H, (double)offset_SCD_h/1000, SCD_h);
	UartPrintf("==> init_CP = %.3lf [mm],  init_CW = %.3lf [mm],  pixel_CW : %d [pixel] \r\n", 
				tc->init_CP, tc->init_CW, CW_pixel);
	UartPrintf("==> SCD_V : %.2lf [mm], offset SCD_V: %.2lf [mm],  adj_SCD_v : %.2lf [mm] \r\n", 
			(double)SCD_V, (double)offset_SCD_v/1000, SCD_v);
	UartPrintf("==> init_CPZ = %.3lf [mm],  init_CWZ = %.3lf [mm],\r\n", 
				tc->init_CPZ, tc->init_CWZ);
	UartPrintf("\r\n");
}

BYTE CheckObjValidity(t_ObjectInfo *obj)
{
	const double minValue = 0.0;
	const double maxValue = 80.0;
	double checkValue;


	checkValue = obj->radius + obj->distance;

	if(  minValue <= checkValue && checkValue <= maxValue) {
		return 1;
	} 
	else {
		UartPrintf("==> distance + radius = %.3lf[mm] : 0 ~ 80.000[mm] range is over :  \r\n");
		return 0;
	}	
}


// tracking CO 동작 디버깅용 함수
void TestDynamicSpeed()
{
	static UINT currState;
	static ULONG lastTime;

	static t_TrackingCO tc;
	static t_Position_um position_um;

	
	switch(currState) {
		case S_IDLE:
			currState = S_IDLE;
			if(Get_IsStartedEvent() == 1) {
				// UartPrintf("==> S_IDLE\r\n");
				currState = S_INITIAL;
			}
			break;

		case S_INITIAL:
			// UartPrintf("==> S_INITIAL\r\n");
			if( CheckObjValidity(&g_Obj) == NO ) {
				UartPrintf("Check Object info!! \r\n");
				currState = S_FINISH;
				break;
			}

			tc.fpsCT = 50;
			tc.captureTime = 24;
			CalculateTrackingCo(&g_Obj, &tc);

			currState = S_MOVE_INITIAL_CO;
			break;

		case S_MOVE_INITIAL_CO:
			UartPrintf("==> S_MOVE_INITIAL_CO \r\n");
			MoveTrackingCo(&tc, &position_um);
			currState = S_WAIT;
			break;	

		case S_WAIT:
			if( g_HL.stop_flag && g_HR.stop_flag) {
				rcan_puts(HL_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg); dly_10us(70);
				rcan_puts(HR_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg); dly_10us(70);

				currState = S_FINISH;
			}
			break;

		case S_FINISH:
			UartPrintf("==> S_FINISH \r\n");
			Set_IsStartedEvent(NO);
			currState = S_IDLE;
			break;
	}

	TrackingObj(&tc);
}



void TrackingObj(t_TrackingCO *tc)
{
	static UINT currState = S_START;
	static ULONG lastTime;

	static int indexCnt;
	static double theta = 0;				// RO rotation degree
	LONG speedHz_CP;						// Center Position speed pulse
	LONG speedHz_CPZ;
	LONG HL_speedHz;
	LONG HR_speedHz;
	LONG VT_speedHz;
	LONG VB_speedHz;

	double post_theta;
	static double now_CP;
	static double post_CP;
	static double now_CPZ;
	static double post_CPZ;
	static double d_CP;
	static double d_CPZ;

	static t_TrackingCO *p_tc;
	const UINT one_ms = 100;
	static ULONG lastTime_tc;
	const LONG CP_1mm_PULSE = HL_0_1_MM_PULSE * 10;	// 1mm 움직이는데 필요한 pulse 수 

	const double SCD_h = SCD_H + (double)offset_SCD_h/1000;
	const double SCD_v = SCD_V + (double)offset_SCD_v/1000;
	const double speedOffset_H = (double)g_SpeedOffset_H/100;
	const double speedOffset_V = (double)g_SpeedOffset_V/100;


	switch(currState) {
		case S_START:
			if( Get_IsStartedEvent() == 2) {
				// UartPrintf("==> S_START\r\n");

				// initialize
				p_tc = tc;		// 포인터 초기화
				now_CP = p_tc->init_CP;
				now_CPZ = p_tc->init_CPZ;
				
				post_theta = 0;
				indexCnt = 0;
				lastTime_tc = TimeNow_10us();
				currState = S_LOOP;
			}
			break;

		case S_LOOP:
			// 조기리턴
			if( TimePassed_10us(lastTime_tc) < one_ms * 20) break;

			lastTime_tc = TimeNow_10us();

			// Tracking 멈추는 조건, 끝났을 때 CO 위치 확인
			if( indexCnt >= p_tc->index ) {
				rcan_puts(HL_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg); dly_10us(70);
				rcan_puts(HR_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg); dly_10us(70);
				rcan_puts(VT_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg); dly_10us(70);
				rcan_puts(VB_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg); dly_10us(70);
				currState = S_MOTOR_STOP;
				break;	
			} 


			post_theta = theta + p_tc->d_Theta;

			if(g_selectTrackingCO == 0 || g_selectTrackingCO == 1) {

				// Center Postion 속도 계산 
				// '0.5'는 형변환시 정수버림을 반올림 형식으로 하기 위해 더해줌(전체 오차를 줄이기위해)
				post_CP = SCD_h * g_Obj.distance * sin( Radians(post_theta + g_Obj.alpha) ) 
						/ ( SOD - g_Obj.distance *cos( Radians(post_theta + g_Obj.alpha) ));
				d_CP = (post_CP - now_CP);
				now_CP = post_CP;
				speedHz_CP = (LONG) ( (d_CP * CP_1mm_PULSE / p_tc->d_Time) + speedOffset_H);
				
				HL_speedHz = - speedHz_CP;
				HR_speedHz = speedHz_CP;
				rcan_puts(HL_MOTOR, CAN_AXIS_MOVE_CONST, HL_speedHz, dmy_msg); dly_10us(50);
				rcan_puts(HR_MOTOR, CAN_AXIS_MOVE_CONST, HR_speedHz, dmy_msg); dly_10us(50);
				// UartPrintf("==> [%d] post_theta : %.3lf, post_CP : %.3lf, speedHz_CP : %ld \r\n", 
				// 		indexCnt, post_theta, post_CP, speedHz_CP);
			}


			if(g_selectTrackingCO == 0 || g_selectTrackingCO == 2) {
				// CPZ 속도 계산
				post_CPZ = g_Obj.z * SCD_v / (SOD - g_Obj.distance * cos( Radians(post_theta + g_Obj.alpha)) );
				d_CPZ = (post_CPZ  - now_CPZ);
				now_CPZ = post_CPZ;
				speedHz_CPZ = (LONG)( (d_CPZ * CP_1mm_PULSE / p_tc->d_Time) + speedOffset_V);
				
				VT_speedHz = -speedHz_CPZ;
				VB_speedHz = speedHz_CPZ;
				rcan_puts(VT_MOTOR, CAN_AXIS_MOVE_CONST, VT_speedHz, dmy_msg); dly_10us(50);
				rcan_puts(VB_MOTOR, CAN_AXIS_MOVE_CONST, VB_speedHz, dmy_msg); dly_10us(50);
				
				// UartPrintf("==> idx[%d] theta : %.3lf, speedHz_CP : %ld, speedHz_CPZ : %ld, d_CPZ : %lf \r\n", 
				// 		indexCnt, theta, speedHz_CP, speedHz_CPZ, d_CPZ);
			}

            theta = theta + p_tc->d_Theta;
			indexCnt++;
			currState = S_LOOP;
			break;

		case S_MOTOR_STOP:
			rcan_puts(HL_MOTOR, CAN_AXIS_MOVE_CONST, (LONG)0, dmy_msg); dly_10us(50);
			rcan_puts(HR_MOTOR, CAN_AXIS_MOVE_CONST, (LONG)0, dmy_msg); dly_10us(50);
			rcan_puts(VT_MOTOR, CAN_AXIS_MOVE_CONST, (LONG)0, dmy_msg); dly_10us(50);
			rcan_puts(VB_MOTOR, CAN_AXIS_MOVE_CONST, (LONG)0, dmy_msg); dly_10us(50);
			UartPrintf("==> S_MOTOR_STOP\r\n");
			lastTime = TimeNow();
			currState = S_PRINT;
			break;

		case S_PRINT:
			if(g_HL.stop_flag && g_HR.stop_flag) {
				UartPrintf("==> S_PRINT\r\n");
				currState = S_FINISH;
			}
			break;

		case S_FINISH:
			UartPrintf("==> S_FINISH\r\n");
			theta = 0;
			indexCnt = 0;
			Set_IsStartedEvent(NO);

			currState = S_START;
			break;
	}
}



double Radians(double degree)
{
	return (PI_ * degree)/180.0;
}
