/*H***************************************************************************
* $RCSfile: system.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the system header define
*----------------------------------------------------------------------------
* RELEASE:      $Name:system      
* Edited  By    $Id: Joony 
*****************************************************************************/
#ifndef _SYSTEM_H
#define _SYSTEM_H


//비트 클리어
#define clear_bit(data, loc)			((data) &= ~(0x1<<(loc)))
#define clear_bits(data, area, loc)		((data) &= ~((area)<<(loc)))

//비트 설정
#define set_bit(data, loc)				((data) |= (0x1<<(loc)))
#define set_bits(data, area, loc)		((data) |= ((area)<<(loc)))

//비트 체크
#define check_bit(data, loc)			((data) & (0x1<<(loc)))

#define	L_BYTE(w)						((BYTE)((WORD)(w) & 0xFF))
#define H_BYTE(w)						((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define	L_WORD(w)						((WORD)((LONG)(w) & 0xFFFF))
#define H_WORD(w)						((WORD)(((LONG)(w) >> 16) & 0xFFFF))	

#define	BYTE0(w)						((BYTE)((LONG)(w) & 0xFF))
#define BYTE1(w)						((BYTE)(((LONG)(w) >> 8) & 0xFF))	
#define BYTE2(w)						((BYTE)(((LONG)(w) >> 16) & 0xFF))
#define BYTE3(w)						((BYTE)(((LONG)(w) >> 24) & 0xFF))


//////////////////////	Main Board IO SETTING	////////////////////////
#define REVISION_NUM        "EX1_X1_20200509_myecoX"



#define SYNC				P1.DR.BIT.B2
#define CPU_RING			P1.DR.BIT.B3
#define EXPOSURE_SW			P1.PORT.BIT.B4

// #define MOTOR_CLK_B3		P2.DR.BIT.B0       // not used
// #define MOTOR_CLK_B4		P2.DR.BIT.B4       // not used
// #define MOTOR_CLK_A4		P2.DR.BIT.B5       // not used

#define MOTOR_CLK_A0		P3.DR.BIT.B0	   // PA axis 
// #define MOTOR_CLK_B0		P3.DR.BIT.B1	   // not used
// #define MOTOR_CLK_C0		P3.DR.BIT.B2	   // not used
#define MOTOR_CLK_D0		P3.DR.BIT.B3  	   // SPREAD_EXP_LAMP_ENABLE
#define MOTOR_CLK_A1		P3.DR.BIT.B4	   // RO axis
// #define MOTOR_CLK_A2		P3.DR.BIT.B6	   // not used
// #define MOTOR_CLK_B2		P3.DR.BIT.B7	   // not used

#define ORG_A0				P4.PORT.BIT.B0	   // PA ORG Sensor
#define ORG_B0				P4.PORT.BIT.B1	   // PA Limt Sensor
// #define ORG_C0			P4.PORT.BIT.B2	   // not used
// #define ORG_D0			P4.PORT.BIT.B3	   // not used
#define ORG_A1				P4.PORT.BIT.B4	   // RO Ceph ORG sensor
// #define ORG_B3			P4.PORT.BIT.B5	   // not used
// #define ORG_A2			P4.PORT.BIT.B6	   // not used
// #define ORG_B2			P4.PORT.BIT.B7	   // not used

// #define ORG_A4			P5.PORT.BIT.B0	   // not used
// #define ORG_B4			P5.PORT.BIT.B1	   // not used

//----------

// #define LED_RUN				P6.DR.BIT.B6	// J2-33
// #define LED_ERR				P6.DR.BIT.B7	// J2-34

#define IN09				PA.PORT.BIT.B3	// RO limit seonsor  // J3-06
#define IN10				PA.PORT.BIT.B4	// RO ORG sensor   // J3-07
// #define IN11				PA.PORT.BIT.B5	// J3-08
// #define IN12				PA.PORT.BIT.B6	// J3-09
// #define IN13				PA.PORT.BIT.B7	// J3-10

// #define IN01				PD.PORT.BIT.B0	// J1-07
// #define IN02				PD.PORT.BIT.B1	// J1-08
// #define IN03				PD.PORT.BIT.B2	// J1-09
// #define IN04				PD.PORT.BIT.B3	// J1-10
// #define IN05				PD.PORT.BIT.B4	// J1-11
#define IN06				PD.PORT.BIT.B5	// J1-12
// #define IN07				PD.PORT.BIT.B6	// J1-13
// #define IN08				PD.PORT.BIT.B7	// J1-14

// #define L_OUT01				PE.DR.BIT.B0	// J1-17
// #define L_OUT02				PE.DR.BIT.B1	// J1-18
// #define L_OUT03				PE.DR.BIT.B2	// J1-19
#define OP1					PE.DR.BIT.B3	// COLUMN_UP
#define OP2					PE.DR.BIT.B4	// COLUMN_DN
#define SP_OUT0				PE.DR.BIT.B5	// X-RAY_RDY
#define	SP_OUT1				PE.DR.BIT.B6	// X-RAY_EXP
#define SP_OUT2				PE.DR.BIT.B7	// X-RAY_PULSE

#define MOTOR_DIR_A0		PH.DR.BIT.B0	// Motor A0 dir
// #define MOTOR_DIR_B0		PH.DR.BIT.B1	// not used //J4-24
#define MOTOR_DIR_C0		PH.DR.BIT.B2	// SPREAD_EXP_LAMP_RED
#define MOTOR_DIR_D0		PH.DR.BIT.B3	// SPREAD_EXP_LAMP_GREEN
#define MOTOR_DIR_A1		PH.DR.BIT.B4	// Motor A1 dir
// #define MOTOR_DIR_B3		PH.DR.BIT.B5	// not used // J4-28
// #define MOTOR_DIR_A2		PH.DR.BIT.B6	// not used // J4-29
// #define MOTOR_DIR_B2		PH.DR.BIT.B7	// not used // J4-30
// #define MOTOR_DIR_A4		PI.DR.BIT.B0	// not used // J4-33
// #define MOTOR_DIR_B4		PI.DR.BIT.B1	// not used // J4-34

#define RELAY1				PI.DR.BIT.B2	// not used
#define RELAY2				PI.DR.BIT.B3	// not exist

#define LASER1				PJ.DR.BIT.B0	
#define LASER2				PJ.DR.BIT.B1	
#define LASER3				PJ.DR.BIT.B2 	// FRONT_LR		
#define LASER4				PJ.DR.BIT.B3	// CANINE_LR
#define LASER5				PJ.DR.BIT.B4	// not used

// #define	XON_L				PK.DR.BIT.B0	// not used
// #define	READY				PK.DR.BIT.B1	
#define	LAMP1				PK.DR.BIT.B2	// EXP_LAMP_RED
#define	LAMP2				PK.DR.BIT.B3 	// EXP_LAMP_GREEN	


//--------------------- detector control port
// #define CT_TRIG_PULSE       PJ.DR.BIT.B5	// H/W 포트로 사용하지 않음
#define S_CEPH_TRIG         PJ.DR.BIT.B6	// H/W으로 연결이 안되어 있음.
#define S_CEPH_EN           PF.DR.BIT.B1
#define VW_EXP_SYNC         PF.DR.BIT.B0 	// 포트와 회로가 역전되어 있음 port = 0 --> 5V

//---------------------- generator control port
#define X_RAY_RDY 			SP_OUT0			// Active Low
#define	X_RAY_EXP 			SP_OUT1			// Active Low
#define X_RAY_PULSE	 		SP_OUT2			// Active HIGH


////////////////////////////////////	IO Define	//////////////////////////////////////////
#define COLUMN_UP			OP2
#define COLUMN_DN			OP1


#define EXP_LAMP_RED		LAMP1			// 버티컬에 부착된 3색 LED 조사 표시등	// Active HIGH
#define EXP_LAMP_GREEN		LAMP2			// 버티컬에 부착된 3색 LED 조사 표시등	// Active HIGH

#define	SPREAD_EXP_LAMP_ENABLE		MOTOR_CLK_D0	// 로테이터에 부착된 Spread LED 조사 표시등	// Active Low
#define SPREAD_EXP_LAMP_RED			MOTOR_DIR_C0	// 로테이터에 부착된 Spread LED 조사 표시등	// Active HIGH
#define SPREAD_EXP_LAMP_GREEN		MOTOR_DIR_D0	// 로테이터에 부착된 Spread LED 조사 표시등	// Active HIGH

#define	PA_ORG_SENSOR   	ORG_A0      	// naming pinPA_ORG_SENSOR

#define RO_LMT_SENSOR		IN09			//
#define RO_ORG_SENSOR		IN10			//

#define	LR_VERTICAL			LASER1
#define	LR_HORIZENTAL		LASER2
// #define	LR_Z 				LASER3
#define	LR_CANINE			LASER4


#define VW_0606C_EXP_OK         IN06

/////////////////////////////	DEFINE	//////////////////////////////////
//	System Clock                                              
//  Input Clock     :  5MHz
//	Iphi            : 40MHz
//  Pphi            : 20MHz
#define	CPU_CLK		40000000L			
#define	P_CLK		20000000L

/****************************************************************************/

#define	GEN_NORMAL_MODE	1           
#define	GEN_PULSE_MODE	2

#define	PAN_CTRL_FWD_AC2	2
#define	PAN_CTRL_REV_AC2	3


///////////////////////////////////	Axis DEFINE	/////////////////////////////////
// PAno Axis
//	- 포토센서 수: 1 개(원점)
//	- potentior meter : 1개(제한용)
//	- 리드: 5 mm, 분주비: 32	==> 파노축 1 mm 움직이는데 필요한 펄스 수: 6400/5 = 1280
//	- 축 전체의 최대 이동 거리: 280mm (p_max : 260mm + n_max: 20mm)
//	- 원점 센서부터 이동할 수 있는 정방향 최대 거리: 260 mm
//	- 원점 센서부터 이동할 수 있는 역방향 최대 거리: 20 mm
//	// ?? - 센서 브라켓 길이:  mm
//	// ?? - 센서를 벗어나서 움직일 수 있는 거리:  mm
#define PA_FRONT			0		// 파노축이 환자 기준 앞으로 움직이는 방향
#define PA_BACK				1		// 파노축이 환자 기준 뒤로 움직이는 방향
#define	PA_POS_DIR			PA_FRONT
#define	PA_NEG_DIR			PA_BACK

#define PA_0_1_MM_PULSE_2	128		// 0.1mm 움직이는데 필요한 펄스 수 : 1280 * 0.1 = 128
#define	PA_ACCEL_PULSE_2	2
#define	PA_START_PULSE_2	4000	// 이전값 1920 :  리드가  2mm -> 5mm 로 되어  덴트리 속도보다 2.5배 빠름 : 덴트리 초기값 4800
#define	PA_MAX_PULSE_2		23000	// 덴트리 초기값 137600  ->  137,600 /2.5 = 55040  -==> 소음이 발생해서 23000넣음.
#define	PA_P_MAX_PULSE_2	332800	// 파노축 정방향 최대 펄스 수: 260mm * 128 * 10 = 332,800 
#define	PA_N_MAX_PULSE_2	25600	// 파노축 역방향 최대 펄스 수: 20mm * 128 * 10 = 25,600

#define	PA_CT_OFFSET		409		// [0.1 mm]  40.9mm * 10 = 409
#define	PA_PANO_OFFSET		(-97)	// [0.1 mm]  409 - 492 : 492는 덴트리 CT offset과 pano offset 차이
#define	PA_CEPH_OFFSET		2550		// [0.1 mm]	 249mm * 10 = 2490


// #define EXHIBIT_MODE	// 전시회 모드 
// #define PULLEY12		// define 여부에 따라 폴리비 12:1이 적용됨 define안하면 63:1 적용
#ifdef PULLEY12
	// Rotator Axis  폴리비 12:1 적용시
	//	- 포토센서 수: 1 개(원점센서), 
	//  - 포텐션미터 1개 
	//	- 모터 분주비 64 ==> 모터 1바퀴(360도)에 필요한 펄스수 = 200pulse * 분주비 = 128400 pulse
	//	- 폴리비: 12:1, 분주비: 64   ==> 로테이터 1도 돌리는데 필요한 펄스 수: 12800*12/360 = 426.66
	//	- 위에서 장비를 내려다 봤을 때, 시계 방향으로 회전하며 촬영함.
	#define RO_CW					0		// 장비 위쪽에서 아래쪽으로 로테이터축을 내려다 봤을 때 시계 방향
	#define RO_CCW					1		// 장비 위쪽에서 아래쪽으로 로테이터축을 내려다 봤을 때 반시계 방향
	#define	RO_POS_DIR				RO_CW
	#define	RO_NEG_DIR				RO_CCW
	#define RO_0_1_DEGREE_PULSE		42		// 로테이터축이 0.1 도 회전하기 위해 필요한 펄스 수
	#define RO_1_DEGREE_PULSE		426		// 로테이터축이 1 도 회전하기 위해 필요한 펄스 수
	#define	RO_P_MAX_PULSE			187733	// 12800*12*440/360 = pulse
	#define	RO_N_MAX_PULSE			34133	// 12800*12*80/360
	#define	RO_ACCEL_PULSE			2
	#define	RO_START_PULSE			1600		// 최주임 넣은 값 400	,=> 이부장님 시작속도 2배로 
	#define	RO_MAX_PULSE			6000	
	#define RO_360_DEGREE_PULSE		153600	// 426.6666 *360 = 153,600 pulse
#else
	// Rotator Axis
	//	- 포토센서 수: 1 개(원점센서), 
	//  - 포텐션미터 1개 
	//  - 모터 분주비 32 ==> 모터 1바퀴(360도)에 필요한 펄스수 = 200pulse * 분주비 = 6400 pulse
	//	- 풀리비: 63:1, 	==> 로테이터 1도 돌리는데 필요한 펄스 수 =  6400*63/360 = 403,200/360 = 1,120
	//	- 위에서 장비를 내려다 봤을 때, 시계 방향으로 회전하며 촬영함.
	//  - RO 1도 pulse수가 정수로 정수로 떨어지지 않는 경우 정밀도를 위해 RO_360_DEGREE_PULSE를 별도로 define함
	#define RO_CW					0		// 장비 위쪽에서 아래쪽으로 로테이터축을 내려다 봤을 때 시계 방향
	#define RO_CCW					1		// 장비 위쪽에서 아래쪽으로 로테이터축을 내려다 봤을 때 반시계 방향
	#define	RO_POS_DIR				RO_CW
	#define	RO_NEG_DIR				RO_CCW
	#define RO_0_1_DEGREE_PULSE		112		// 로테이터축이 0.1 도 회전하기 위해 필요한 펄스 수
	#define RO_1_DEGREE_PULSE		1120	// 로테이터축이 1 도 회전하기 위해 필요한 펄스 수
	#define	RO_P_MAX_PULSE			492800	// 6400*63*440/360 = 492,800 pulse
	#define	RO_N_MAX_PULSE			89600	// 6400*63*80/360 = 89,600 pulse
	#define	RO_ACCEL_PULSE			2
	#define	RO_START_PULSE			4000	// 2187
	#define	RO_MAX_PULSE			16800	// 초당 15도 도는 속도(1바퀴 24sec)
	#define RO_360_DEGREE_PULSE		403200	// 1120 * 360 = 403,200 pulse  
#endif

	#define	RO_CT_OFFSET		0		// [0.1 degrees]
	#define	RO_PANO_OFFSET		0		// [0.1 degrees]	환자 정렬할 때 시각적인 오류가 생길 수 있어서 -10도에서 0도로 변경
	#define	RO_CEPH_OFFSET		140 	// [0.1 degrees]  


// Collimator Axis(EcoX)
//	- 포토센서 수: 2 개(원점센서(우측), 셉용 보조센서(좌측))
//	- 리드: 2 mm
//  - CAN motor drv 분주비: 32	==> 컬리메이터축 1 mm 움직이는데 필요한 펄스 수: 6400/2 = 3200
//	- 축 전체의 최대 이동 거리: 68 mm(가공 및 조립공차 감안)
//	- 신형
//		- 원점 센서부터 왼쪽으로 이동할 수 있는 최대 이동 거리: 94.25 mm
//		- 원점 센서부터 오른쪽으로 이동할 수 있는 최대 이동 거리: 2.55 mm
//		- 원점 기준으로 소아 CT(CTCHild) 위치: 0 mm
//		- 원점 기준으로 CT 모드 위치: 24 mm
//		- 원점 기준으로 파노모드 위치: 45 mm
//		- 원점 기준으로 B/W(Bite Wing)위치: 66 mm
//		- 원점 기준으로 CEPH 위치: 79.7 mm
//		//- 센서 기준으로 무빙 원샷 셒 LA 모드 위치: 왼쪽으로 45.75 mm
//		//- 센서 기준으로 무빙 원샷 셒 PA 모드 위치: 왼쪽으로 48.7 mm
//		//- 센서 기준으로 스캔 셒 LA 모드 위치: 왼쪽으로 45.005 mm
//		//- 센서 기준으로 스캔 셒 PA 모드 위치: 왼쪽으로 42.9 mm
#define	CO_POS_DIR				1		// 디텍터에서 컬리메이터를 바라봤을 때 컬리메이터축이 왼쪽으로 움직이는 방향
#define	CO_NEG_DIR				0		// 디텍터에서 컬리메이터를 바라봤을 때 컬리메이터축이 오른쪽으로 움직이는 방향

#define CO_0_1_MM_PULSE_C		320		// 컬리메이터축이 0.1 mm 이동하기 위해 필요한 펄스 수
#define CO_0_01_MM_PULSE_C		32		// 컬리메이터축이 0.01 mm 이동하기 위해 필요한 펄스 수
#define	CO_ACCEL_PULSE_C		2
#define	CO_START_PULSE_C		9000
#define	CO_MAX_PULSE_C			20000
#define	CO_P_MAX_PULSE_C		301600	// 320 * 10 * 94.25mm = 301,600
#define	CO_N_MAX_PULSE_C		8160	// 320 * 10 * 2.55mm = 8,160


#define	CO_CTCH_OFFSET			0		// [0.01 mm]	0mm 		// CT Child
#define	CO_CT_OFFSET			2400	// [0.01 mm]	24mm
#define	CO_PANO_OFFSET			4500	// [0.01 mm]	45mm
#define	CO_BW_OFFSET			6600	// [0.01 mm]	66mm
#define	CO_CEPH_OFFSET			6093	// [0.01 mm]	60.93mm


// #define	CO_CT_OFFSET_NEW		0		// [0.01 mm]	0 * 100 = 0
// #define	CO_PANO_OFFSET_NEW		-2605	// [0.01 mm]	-26.05 * 100 = -2 605
#define	CO_SCLA_OFFSET			7690	// [0.01 mm]	76.90 mm   	// SW팀에서 offset을 조정하여 사용
#define	CO_SCPA_OFFSET			7690	// [0.01 mm]	76.90 mm  	// SW팀에서 offset을 조정하여 사용

//Scan-type Cephalo Detector 축
//	- 포토센서 수: 2 개
//	- 모터 회전당 SD 리드 : 23.706 
//	- 분주비: 32	==> 1 mm 움직이는데 필요한 펄스 수: (200*32) / 23.706 = 270(269.9738.....)
//	- 센서부터 이동할 수 있는 정방향 최대 거리: 336 mm
//	- 센서부터 이동할 수 있는 역방향 최대 거리: 20 mm
#define SD_FRONT			0		// 스캔타입 세팔로 디텍터축이 환자 기준 앞으로 움직이는 방향
#define SD_BACK				1		// 스캔타입 세팔로 디텍터축이 환자 기준 뒤로 움직이는 방향
#define	SD_POS_DIR			SD_BACK
// #define	SD_P_MAX_PULSE		90711   // 269.9738 * 336 mm = 90711 (9071.1968)
// #define	SD_N_MAX_PULSE		5399    // 269.9738 *  20 mm =  5399 (5399.476)
#define	SD_P_MAX_PULSE		89362   // 269.9738 * 331 mm = 89361.3278
#define	SD_N_MAX_PULSE		55130   // 269.9738 *  19 mm =  5129.5022
#define	SD_0_1_MM_PULSE		27		// 269.9738 * 0.1 = 27 
#define	SD_ACCEL_PULSE		4
#define	SD_START_PULSE		3000	// 덴트리 값은9600인데, EcoX에서는 빨라 원점이 오차가 생겨 ceph 이미지 틀어지는 문제 야기하는 듯
#define	SD_MAX_PULSE		10491   // LA 궤적 테이블 등속값으로 설정

#define	SD_SCLA_OFFSET		0	// [0.1 mm]
#define	SD_SCPA_OFFSET		0	// [0.1 mm]


//4axis Collimator Left Axis
//	- 포토센서 수: 1 개
//	- 리드: 1.0 mm, 분주비: 8
//  - 기구 데이터 : -5mm ~ 38mm
//	- 센서 기준 정방향 최대 거리: 37 mm (기구공차 고려)
//	- 센서 기준 역방향 최대 거리: 4 mm (기구공차 고려)
#define	HL_POS_DIR			1
#define	HL_NEG_DIR			0
#define	HL_0_1_MM_PULSE		160		// ( (360/1.8) * 8) / 1 / 10 = 160
#define	HL_0_01_MM_PULSE	16		// ( (360/1.8) * 8) / 1 / 100 = 16
#define HL_P_MAX_PULSE	    59200	// 160 * 10 * 37 = 59,200
#define HL_N_MAX_PULSE	    6400	// 160 * 10 * 4 = 6,400
#define	HL_ACCEL_PULSE	    4
#define	HL_START_PULSE	    400
#define	HL_MAX_PULSE		8000

#define	HL_CT_OFFSET	    (-1318)		// [um]
#define	HL_PANO_OFFSET	    11375		// [um]
// #define	HL_OCLA_OFFSET	    0		// [0.01 mm]
// #define	HL_OCPA_OFFSET	    0		// [0.01 mm]
#define	HL_SCLA_OFFSET	    11400		// [um]
#define	HL_SCPA_OFFSET	    11400		// [um]


//4axis Collimator Right Axis
//	- 포토센서 수: 1 개
//	- 리드: 1.0 mm, 분주비: 8
//  - 기구 데이터 : -5mm ~ 38mm
//	- 센서 기준 정방향 최대 거리: 37 mm (기구공차 고려)
//	- 센서 기준 역방향 최대 거리: 4 mm (기구공차 고려)
#define	HR_POS_DIR			1
#define	HR_NEG_DIR			0
#define	HR_0_1_MM_PULSE		160		// ( (360/1.8) * 8) / 1 / 10 = 1600
#define	HR_0_01_MM_PULSE	16		// ( (360/1.8) * 8) / 1 / 100 = 16
#define HR_P_MAX_PULSE      59200	// 160 * 10 * 37 = 59,200
#define HR_N_MAX_PULSE      6400	// 160 * 10 * 4 = 6,400
#define	HR_ACCEL_PULSE      4
#define	HR_START_PULSE      400
#define	HR_MAX_PULSE        8000

#define	HR_CT_OFFSET		20958		// [um]
#define	HR_PANO_OFFSET	    21375	// [um]
// #define	HR_OCLA_OFFSET	    0		// [0.01 mm]
// #define	HR_OCPA_OFFSET	    0		// [0.01 mm]
#define	HR_SCLA_OFFSET	    21500		// [um]
#define	HR_SCPA_OFFSET	    21500		// [um]


//4axis Collimator Top Axis
//	- 포토센서 수: 1 개
//	- 리드: 0.5 mm, 분주비: 8
//  - 기구 데이터 : -5mm ~ 20.2mm
//	- 센서 기준 정방향 최대 거리: 20 mm
//	- 센서 기준 역방향 최대 거리: 2 mm
#define	VT_POS_DIR			0
#define	VT_0_1_MM_PULSE		160		
#define	VT_0_01_MM_PULSE	16		
#define VT_P_MAX_PULSE	    32000	// 160 * 10 * 20 = 64,000
#define VT_N_MAX_PULSE	    3200	// 160 * 10 * 2 = 6,400
#define	VT_ACCEL_PULSE	    4
#define	VT_START_PULSE	    400
#define	VT_MAX_PULSE		8000

#define	VT_CT_OFFSET		1979		// [um]
#define	VT_PANO_OFFSET	    1100	    // [um]
// #define	VT_OCLA_OFFSET	    0		// [0.01 mm]
// #define	VT_OCPA_OFFSET	    0		// [0.01 mm]
#define	VT_SCLA_OFFSET	    10500		// [um]
#define	VT_SCPA_OFFSET	    10500		// [um]


//4axis Collimator Bottom Axis
//	- 포토센서 수: 1 개
//	- 리드: 0.5 mm, 분주비: 8
//  - 기구 데이터 : -5mm ~ 20.2mm
//	- 센서 기준 정방향 최대 거리: 20 mm
//	- 센서 기준 역방향 최대 거리: 2 mm
#define	VB_POS_DIR			1
#define	VB_0_1_MM_PULSE		160		
#define	VB_0_01_MM_PULSE	16		
#define VB_P_MAX_PULSE		32000	// 160 * 10 * 20 = 64,000
#define VB_N_MAX_PULSE		3200	// 160 * 10 * 2 = 6,400
#define	VB_ACCEL_PULSE		4
#define	VB_START_PULSE		400
#define	VB_MAX_PULSE		8000

#define	VB_CT_OFFSET		1280		// [um]
#define	VB_PANO_OFFSET      1550		// [um]
// #define	VB_OCLA_OFFSET      0		// [0.01 mm]
// #define	VB_OCPA_OFFSET      0		// [0.01 mm]
#define	VB_SCLA_OFFSET      (-450)		// [um]
#define	VB_SCPA_OFFSET      (-450)		// [um]


//----------- option 관련 
#define NONE_CEPH			0
#define SCAN_CEPH			1

#define ONE_AXIS_CO 		0
#define FOUR_AXIS_CO		1
#define NONE_AXIS_CO 		99
#define AXIS_ALL 		2


/*_____ D E C L A R A T I O N ______________________________________________*/
typedef struct
{
	BYTE 	pos_dir;
	BYTE 	neg_dir;

	LONG 	unit_0_1_pulse;
	LONG	unit_0_01_pulse;
	LONG 	accel_step;
	LONG 	start_speed;		// todo naming : start_speed
	LONG 	max_speed;
	LONG 	p_max_pulse;		// todo naming : p_max_pulseCnt
	LONG 	n_max_pulse;		// todo naming : n_max_pulseCnt
	LONG 	ct_offset_pos;
	LONG 	pano_offset_pos;
	LONG 	ceph_offset_pos;
	LONG 	scla_offset_pos;
	LONG	scpa_offset_pos;
	BYTE 	org_complete_flag;
	BYTE	stop_flag;
	LONG	move_distance;
	LONG	move_cw_value;
	LONG	move_pulse_val;
	LONG	pos_cnt;
	LONG 	speedHz;
	LONG	move_value;
	BYTE 	move_ack_flag;
	BYTE 	p_over_ack_flag;
	BYTE 	n_over_ack_flag;
	BYTE 	axisId;
	char	axisName[4];
	LONG 	offset_um;		
} varMotor_t;

/*_____ F U C T I O N ______________________________________________________*/
extern void InitializeBoard(void);
extern void InitializeSystem(void);
extern void InitializeMachine(void);
extern void Init_FlashValue(void);
extern void LaserOff(void);
extern void LaserOn(void);


/*_____ V A R I B L E S ______________________________________________________*/
extern BYTE	sound_option;
extern BYTE ceph_option;
extern BYTE	pa_option;
extern BYTE	co_option;
extern BYTE pa_axis_end_position;
extern BYTE	gen_overheat_threshold;
extern BYTE	debug_mode;
extern BYTE ct_ro_end_tm;
extern BYTE ct_ro_end_angle;
extern BYTE pano_ro_end_angle;
extern BYTE	ac_option;
extern BYTE	gen_option;


extern varMotor_t 	gvPA;
extern varMotor_t 	gvRO;
extern varMotor_t 	gvCO;
extern varMotor_t  	gvSD;
extern varMotor_t  	g_HL;
extern varMotor_t  	g_HR;
extern varMotor_t  	g_VT;
extern varMotor_t  	g_VB;



extern LONG imsi_l1;
extern LONG imsi_l2;
extern LONG imsi_l3;
extern BYTE	first_run;
extern BYTE isInitializedMachine;



#endif
