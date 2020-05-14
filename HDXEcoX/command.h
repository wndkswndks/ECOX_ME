/*H***************************************************************************
* $RCSfile: command.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the command header
*----------------------------------------------------------------------------
* RELEASE:      $Name: command.h      
* Edited  By    $Id: Joony
*****************************************************************************/
#ifndef _COMMAND_H
#define _COMMAND_H 


/*===================================================================================*/	
// Mode Select
/*===================================================================================*/					
#define CT		        			0x63745F5F	// [ct__]
#define CT_CHILD        			0x63746368	// [ctch]

#define	PANO_STAN					0x70616E6F	// [pano]
#define PANO_BITEWING				0x62697769	// [biwi]
#define PANO_TMJ1					0x746D6A31	// [tmj1]
#define PANO_TMJ2					0x746D6A32	// [tmj2]
#define PANO_SINUS					0x73696E75	// [sinu]

#define SCAN_CEPH_LA				0x6365706C	// [cepl]
#define SCAN_CEPH_PA				0x63657070	// [cepp]

#define	PACKING_MODE				0x706B6D64	// [pkmd]

/*===================================================================================*/	
//// COMMON Command
/*===================================================================================*/
#define GENERATOR_CALIBRATION		0x6765636C	// [gecl]	
#define	SYSTEM_INIT_COMPLETE		0x756F6B3F	// [uok?]
#define	CAPT_SW_APPLY_CPLT			0x616F6B3F	// [aok?]

#define PREC_COMPLETE    			0x70726563	// [prec]	//준비 완료 (조사스위치 반응 시작)
#define PREC_CANCEL					0x7072636C	// [prcl]	//준비 취소 (조사스위치 반응 취소)

#define DETECTOR_READY				0x64726479	// [drdy]	// detector 영상 준비 완료

#define	SCFR						0x73636672	// [scfr]	//scan frame
#define	CT_CAPT_FPS					0x63667073	// [cfps]
// #define	CT_CAPT_ANGLE				0x63616E67	// [cang]

#define	SCOUT						0x73637574	// [scut]	//scout 촬영 기능 활성화
#define SCTM        				0x7363746D	// [sctm]	//scout 촬영할 때 각 지정 각도에서 조사시간 설정
#define	SCOUT_SET					0x73637374	// [scst]	//scout 촬영의 prec

#define	DAP_VALUE					0x6461705F	// [dap_]	// pc에서 전달 받은 값을 lcd로 전달
#define DAP_CAPTURE_TIME			0x63746D3F	// [ctm?]	// 촬영 시간, 조사 시간, dap 값 pc전달

/*===================================================================================*/
// TUBE_CTRL Message
/*===================================================================================*/
#define KV_SETTING					0x6B767374	// [kvst]	// 관전압 설정
#define MA_SETTING					0x6D617374	// [mast]	// 관전류 설정
#define KV_REQ     					0x6B763F5F	// [kv?_]	// 관전압 설정값 확인
#define MA_REQ     					0x6D613F5F	// [ma?_]	// 관전류 설정값 확인
#define TUBE_MODE_REQ				0x74756D3F	// [tum?]	// 조사 모드 확인 (노말 or 펄스)
#define X_RAY_EXP_STATE_REQ			0x6578703F	// [exp?]	// 조사 상태 확인 (조사 중 or 조사 안하는 중)
#define INVERTER_BUZZER_STATE_REQ	0x69623F5F	// [ib?_]	// 부저 상태 확인 (조사 중 울림 or 안 울림)
#define MA_DA_VALUE_REQ				0x64613F5F	// [da?_]	// 관전류 DA 설정값 확인
#define KV_DA_VALUE_REQ				0x64763F5F	// [dv?_]	// 관전압 DA 설정값 확인
#define	TUBE_NORMAL_MODE    	   	0x74756E72	// [tunr]	// TUBE NORMAL MODE
#define	TUBE_PULSE_MODE				0x7475706C	// [tupl]	// TUBE pulsed MODE
#define INVERTER_BUZZER_ON			0x69626F6E	// [ibon]
#define INVERTER_BUZZER_OFF			0x69626F66	// [ibof]
#define KV_DA_VALUE_SETTING			0x64767374	// [dvst]	// 관전압 DA값 설정
#define MA_DA_VALUE_SETTING			0x64617374	// [dast]	// 관전류 DA값 설정
#define TUBE_TEMP_REQ				0x74743F5F	// [tt?_]	// 튜브(탱크) 온도 확인
		
#define	TUBE_TEMP_THRES				0x74747468	// [ttth]	// 튜브(탱크) 과열온도 기준값 설정
#define TUBE_TEMP_THRES_REQ 		0x7474743F	// [ttt?]	// 튜브(탱크) 과열온도 기준값 확인

#define	KV_REF						0x6B767266	// [kvrf]	// 관전압 ref 확인
#define	MA_REF						0x6D617266	// [marf]	// 관전류 ref 확인
#define	X_RAY_ON					0x785F6F6E	// [x_on]	// 예열한 뒤에 x-ray 조사
#define	X_RAY_OFF					0x785F6F66	// [x_of]	// 예열과 x-ray 둘 다 중지
#define	INV_CUR_READ				0x696E7663	// [invc]	// 인버터 전류값 확인
#define	X_RAY_ONLY_ON				0x786F6F6E	// [xoon]	// 예열이 이미 되었다는 가정하에 x-ray 조사
#define	X_RAY_ONLY_OFF				0x786F6F66	// [xoof]	// 예열은 유지하면서 x-ray만 끔
#define	INV_ERROR					0x696E7665	// [inve]	// 인버터 에러 상태 확인
#define	TUBE_RDY					0x74726479	// [trdy]	// 인버터 준비
#define	PRE_HEAT_ON					0x70726874	// [prht]	// 예열 시작
#define	STAT_CHK					0x7374636B	// [stck]	// 상태 정보 확인
#define	STAT_CHK_INIT				0x73746369	// [stci]	// 상태 정보 삭제
#define	ERR_CHK						0x6572636B	// [erck]	// 에러 정보 확인
#define	ERR_CHK_INIT				0x65726369	// [erci]	// 에러 정보 삭제
#define GEN_TRIG_TIME_START			0x67747473	// [gtts]	//  Gen trig 시작 time 10us단위
#define GEN_TRIG_TIME_END			0x67747465	// [gtte]	//  Gen trig 끝 time 10us단위

// #define	GEN_TRIG_TIME				0x6774746D	// [gttm]	// GEN trig 시작 및 끝 Time 설정
#define GEN_FW_REVIRSION			0x67667276	// [gfrv]

/*===================================================================================*/
// Alignment
/*===================================================================================*/	
#define	ALIGN_EXPOSE				0x616C6578	// [alex]	//Align Expose

/*===================================================================================*/
// Dark & Bright Frame Acquisition Message
/*===================================================================================*/
#define DARK_ACQ_TIME               0x6461746D	// [datm]
#define	DARK_FRAME_ACQ				0x6461726B	// [dark]
#define	BRIGHT_ACQ_TIME				0x6272746D	// [brtm]
#define	BRIGHT_FRAME_ACQ			0x62726974	// [brit]

/*===================================================================================*/
// Software Exposure Switch Message
/*===================================================================================*/
#define	SOFTWARE_EXP_ON				0x73776F6E	// [swon]	//sw exposure on
#define	SOFTWARE_EXP_OFF			0x73776F66	// [swof]	//sw exposure off

#define SOFTWARE_READY              0x73726479  // [srdy]	// 구형장비 -> not used

/*===================================================================================*/
// System Option Message
/*===================================================================================*/
#define	FIRMWARE_REVISION_NUM			0x66726E3F	// [frn?]
#define	SYSTEM_REVISION_NUM             0x7265763F	// [rev?]
#define	OPTION_PRINT					0x6F703F5F	// [op?_]
#define	SYSTEM_OPTION					0x73796F70	// [syop]
#define	CEPH_OPTION						0x63656F70	// [ceop]	// cephalo option (0: non, 1: one-shot ceph, 2: scan ceph, 3: cep_ctrl scan ceph)
#define	PA_OPTION						0x70616F70	// [paop]
#define	RO_OPTION						0x726F6F70	// [roop]
#define	CO_OPTION						0x636F6F70	// [coop]
#define	AC_OPTION						0x61636F70	// [acop]	// 상승, 하강 방향을 바꾸는 옵션
#define	GEN_OPTION						0x67656F70	// [geop]
#define	CT_DET_OPTN						0x63646F70	// [cdop]
#define	SC_DET_OPTN						0x73646F70	// [sdop]

#define	CT_RO_END_ANGLE					0x63726561	// [crea]	// 0: 0 degrees, 1: 360 degrees
#define	CT_RO_STI_END_ANGLE				0x63736561	// [csea]	// 0: 0 degrees, 1: 360 degrees
#define	CT_RO_END_TIME					0x63726574	// [cret]	// 0 s ~ 120 s
#define	PANO_RO_END_ANGLE				0x70726561	// [prea]	// 0: 180 degrees, 1: 0 degrees
#define	PANO_RO_END_TIME				0x70726574	// [pret]	// 0 s ~ 120 s
#define	PANO_NOR_TBL_OPTION				0x706E746F	// [pnto]	// 궤적 선택
#define	PANO_TMJ_TBL_OPTION				0x7074746F	// [ptto]	// TMJ 궤적 선택(normal, fast)

#define	CEPH_AXIS_END_POSITION			0x63656570	// [ceep]	// ceph_axis_end_position (0: END, 1: START)
#define	DOOR_CHECK_OPTION				0x646F6F70	// [doop]	// 0: 확인 안 함, 1: door 열렸는지 확인 함
#define	CEPH_CAPT_END_COL_POSITION		0x63656370	// [cecp]	// ceph_capt_end_col_position (0: Not moving, 1: Moving)
#define PA_AXIS_END_POSITION			0x70616570	// [paep]	// pa_axis_end_position	(0: Not moving, 1: Moving)

#define	TOUCH_PANEL_MODE				0x74706D64	// [tpmd]
#define TOUCH_PANEL_CT_MAX_KV           0x63746B76  // [ctkv]
#define TOUCH_PANEL_CEPH_MAX_KV         0x63656B76  // [cekv]
#define TOUCH_PANEL_ALL_MIN_KV          0x616C6B76  // [alkv]
#define TOUCH_PANEL_COMPANY             0x636F6D70  // [comp]
#define TOUCH_PANEL_VERSION_FRONT       0x76657266  // [verf]
#define TOUCH_PANEL_VERSION_REAR        0x76657272  // [verr]
#define TOUCH_PANEL_MANUFACTURER		0x74706D3F	// [tpm?]
		
#define	PANO_EXP_OPTION					0x70656F70	// [peop]	// 0: cont. exp mode, 1: dose reduction mode

#define	CLEAR_OPTION					0x636C6F70	// [clop]
/*===================================================================================*/
// Axis Control
/*===================================================================================*/
#define	EMERGENCY_STOP		0x656D7370	// [emsp]	// Emergency Stop (All Axis Stop Immediately)

#define	PA_MOVE				0x70616D76	// [pamv]	// Pano Axis Move
#define	PA_SLOW_STOP		0x70617370	// [pasp]
// #define	PA_MOVE_CONST		0x70616D63	// [pamc]	// Pano Axis Move Const
// #define	PA_ACCEL_STEP		0x70616173	// [paas]
#define	PA_START_SPEED		0x70617373	// [pass]
#define	PA_MAX_SPEED		0x70616D73	// [pams]

#define	RO_MOVE				0x726F6D76	// [romv]	// Rotator Axis Move
#define	RO_SLOW_STOP		0x726F7370	// [rosp]
#define	RO_MAX_SPEED		0x726F6D73	// [roms]
#define	RO_START_SPEED		0x726F7373	// [ross]
#define	RO_ACCEL_STEP		0x726F6173	// [roas]
// #define	RO_MOVE_CW			0x726F6377	// [rocw]	// Rotator Axis Move CW
// #define	RO_MOVE_CONST		0x726F6D63	// [romc]	// Rotator Axis Move Const
// #define	RO_MOVE_PULSE		0x726F6D70	// [romp]	// Rotator Axis Move Pulse

#define	CO_MOVE				0x636F6D76	// [comv]	// Collimator Axis Move				[0.1 mm]
#define	CO_MOVE_ACCU		0x636F6D61	// [coma]	// Collimator Axis Move Accurately	[0.01 mm]
#define	CO_MOVE_CONST		0x636F6D63	// [comc]	// Collimator Axis Move Const
// #define	CO_SLOW_STOP		0x636F7370	// [cosp]
// #define	CO_ACCEL_STEP		0x636F6173	// [coas]
// #define	CO_START_SPEED		0x636F7373	// [coss]
// #define	CO_MAX_SPEED		0x636F6D73	// [coms]

#define	SD_MOVE				0x73646D76	// [sdmv]	// Scan type Cephalo Detector Axis Move
#define	SD_MOVE_CONST		0x73646D63	// [sdmc]	// Scan type Cephalo Detector Axis Move Const
// #define	SD_SLOW_STOP		0x73647370	// [sdsp]
// #define	SD_ACCEL_STEP		0x73646173	// [sdas]
#define	SD_START_SPEED		0x73647373	// [sdss]
#define	SD_MAX_SPEED		0x73646D73	// [sdms]
#define SD_MOVE_PULSE		0x73646D70	// [sdmp]

// 4 axis Collimator
#define	HL_MOVE				0x686C6D76	// [hlmv]	// 4 axis Collimator Horizontal Left axis move
#define	HL_MOVE_ACCU		0x686C6D61	// [hlma]
#define	HL_MOVE_UM			0x686C6D75	// [hlmu]
#define	HL_MOVE_CONST		0x686C6D63	// [hlmc]
#define	HL_SLOW_STOP		0x686C7370	// [hlsp]
#define	HL_ACCEL_STEP		0x686C6173	// [hlas]
#define	HL_START_SPEED		0x686C7373	// [hlss]
#define	HL_MAX_SPEED		0x686C6D73	// [hlms]

#define	HR_MOVE				0x68726D76	// [hrmv]	// 4 axis Collimator Horizontal Right axis move
#define	HR_MOVE_ACCU		0x68726D61	// [hrma]
#define	HR_MOVE_UM			0x68726D75	// [hrmu]
#define	HR_MOVE_CONST		0x68726D63	// [hrmc]
#define	HR_SLOW_STOP		0x68727370	// [hrsp]
#define	HR_ACCEL_STEP		0x68726173	// [hras]
#define	HR_START_SPEED		0x68727373	// [hrss]
#define	HR_MAX_SPEED		0x68726D73	// [hrms]

#define	VT_MOVE				0x76746D76	// [vtmv]	// 4 axis Collimator Vertical Top axis move
#define	VT_MOVE_ACCU		0x76746D61	// [vtma]
#define	VT_MOVE_UM			0x76746D75	// [vtmu]
#define	VT_MOVE_CONST		0x76746D63	// [vtmc]
#define	VT_SLOW_STOP		0x76747370	// [vtsp]
#define	VT_ACCEL_STEP		0x76746173	// [vtas]
#define	VT_START_SPEED		0x76747373	// [vtss]
#define	VT_MAX_SPEED		0x76746D73	// [vtms]

#define	VB_MOVE				0x76626D76	// [vbmv]	// 4 axis Collimator Vertical Bottom axis move
#define	VB_MOVE_ACCU		0x76626D61	// [vbma]
#define	VB_MOVE_UM			0x76626D75	// [vbmu]
#define	VB_MOVE_CONST		0x76626D63	// [vbmc]
#define	VB_SLOW_STOP		0x76627370	// [vbsp]
#define	VB_ACCEL_STEP		0x76626173	// [vbas]
#define	VB_START_SPEED		0x76627373	// [vbss]
#define	VB_MAX_SPEED		0x76626D73	// [vbms]

/*===================================================================================*/
// 4 Axis Collimator Position [0.01 mm] - HL, HR, VT, VB 
/*===================================================================================*/
#define HL_CT_OFFSET_POS		0x686C6374	// [hlct]
#define HL_PANO_OFFSET_POS		0x686C7061	// [hlpa]
#define HL_SCLA_OFFSET_POS		0x686C736C	// [hlsl]
#define HL_SCPA_OFFSET_POS		0x686C7370	// [hlsp]

#define HR_CT_OFFSET_POS		0x68726374	// [hrct]
#define HR_PANO_OFFSET_POS		0x68727061	// [hrpa]
#define HR_SCLA_OFFSET_POS		0x6872736C	// [hrsl]
#define HR_SCPA_OFFSET_POS		0x68727370	// [hrsp]

#define VT_CT_OFFSET_POS		0x76746374	// [vtct]
#define VT_PANO_OFFSET_POS		0x76747061	// [vtpa]
#define VT_SCLA_OFFSET_POS		0x7674736C	// [vtsl]
#define VT_SCPA_OFFSET_POS		0x76747370	// [vtsp]

#define VB_CT_OFFSET_POS		0x76626374	// [vbct]
#define VB_PANO_OFFSET_POS		0x76627061	// [vbpa]
#define VB_SCLA_OFFSET_POS		0x7662736C	// [vbsl]
#define VB_SCPA_OFFSET_POS		0x76627370	// [vbsp]

#define HL_OFFSET_POS 			0x686C6F66	// [hlof]
#define HR_OFFSET_POS 			0x68726F66	// [hrof]
#define VT_OFFSET_POS 			0x76746F66	// [vtof]
#define VB_OFFSET_POS 			0x76626F66	// [vbof]

#define HL_MOVE_OFFSET			0x686C6D6F	// [hlmo]
#define HR_MOVE_OFFSET			0x68726D6F	// [hrmo]
#define VT_MOVE_OFFSET			0x76746D6F	// [vtmo]
#define VB_MOVE_OFFSET			0x76626D6F	// [vbmo]

#define HL_MOVE_CONST			0x686C6D63	// [hlmc]
#define HR_MOVE_CONST			0x68726D63	// [hrmc]
#define VT_MOVE_CONST			0x76746D63	// [vtmc]
#define VB_MOVE_CONST			0x76626D63	// [vbmc]


// Actuator
#define	ACTUATOR_UPUP		0x61637570	// [acup]	// Column(Actuator) Move Up
#define ACTUATOR_DOWN		0x6163646E	// [acdn]	// Column(Actuator) Move Down
#define	ACTUATOR_STOP		0x61637370	// [acsp]	// Column(Actuator) Move Stop

#define PULSE_PERIOD		0x7072696F	// [prio]	// detector : pulse2 period

/*===================================================================================*/
// Canine Beam Control
/*===================================================================================*/
#define CANINE_BEAM_VALUE_ENABLE	0x63627665	// [cbve]	// 1: Elable,	0: Disable
#define CANINE_BEAM_VALUE_REQ		0x6362763F	// [cbv?]
#define PX_PANO_SET					0x70707374	// [ppst]	// Paxis Pano Setting Position (Canine Beam Offset Position)

/*===================================================================================*/
// Rotator Prepare Angle
/*===================================================================================*/
#define PANO_STAN_PREC_ANGLE  		0x70707073	// [ppps]	// 덴트리1과 명령어 통일
#define PANO_TMJ_PREC_ANGLE  		0x74707073	// [tpps]	// 덴트리1과 명령어 통일
#define	PANO_TMJ2_PREC_ANGLE		0x74327073	// [t2ps]
// #define	CEPH_PREC_ANGLE				0x63657061	// [cepa]	// Cephalo Mode Rotator Prepare Angle

#define	RO_CEPH_OFFSET_POSITION		0x726F636F	// [roco]

/*===================================================================================*/
// Pano Axis Offset Position - PA	[0.1 mm]
/*===================================================================================*/
#define	PA_CT_OFFSET_POS		0x70616374	// [pact]
#define	PA_PANO_OFFSET_POS		0x70617061	// [papa]
#define	PA_CEPH_OFFSET_POS		0x70616370	// [pacp]

/*===================================================================================*/
// Rotator Axis Offset Position - RO	[0.1 degree]
/*===================================================================================*/
#define	RO_CT_OFFSET_POS		0x726F6374	// [roct]
#define	RO_PANO_OFFSET_POS		0x726F7061	// [ropa]
#define	RO_CEPH_OFFSET_POS		0x726F6370	// [rocp]

/*===================================================================================*/
// 1 Axis Collimator Offset Position - CO	[0.01 mm]
/*===================================================================================*/
#define CO_CT_OFFSET_POS		0x636F6374	// [coct]
#define CO_PANO_OFFSET_POS		0x636F7061	// [copa]
// #define CO_OCFX_OFFSET_POS		0x636F6670	// [cofp]
// #define CO_OCLA_OFFSET_POS		0x636F6C70	// [colp]
// #define CO_OCPA_OFFSET_POS		0x636F7070	// [copp]
#define CO_SCLA_OFFSET_POS		0x636F6C6F	// [colo]	// Collimator축의 Lateral 모드 촬영 시작 위치 (거리값)
#define CO_SCPA_OFFSET_POS		0x636F706F	// [copo]	// Collimator축의 PA 모드 촬영 시작 위치 (거리값)

/*===================================================================================*/
// Scan Ceph Detector Offset Position - SD	[0.1 mm]
/*===================================================================================*/
#define SD_SCLA_OFFSET_POS		0x73646C6F	// [sdlo]	// Scan Ceph Detector축의 Lateral 모드 촬영 시작 위치 (거리값)
#define SD_SCPA_OFFSET_POS		0x7364706F	// [sdpo]	// Scan Ceph Detector축의 PA 모드 촬영 시작 위치 (거리값)

/*===================================================================================*/
// Capture Start(Ring ON) & Stop(Ring OFF) Table Number
/*===================================================================================*/
#define PANO_STAN_CAPT_START	0x70637370	// [pcsp]	// 덴트리1과 명령어 통일
#define PANO_TMJ_CAPT_START		0x7465736E	// [tesn]	// 덴트리1과 명령어 통일
#define PANO_TMJ_CAPT_END		0x7465656E	// [teen]	// 덴트리1과 명령어 통일
#define	PANO_TRIGGER_NUMBER		0x7061746E	// [patn]	// 파노라마 트리거 개수 설정
#define SELECT_BITEWING     	0x62776667  // [bwfg]	// bitewing mode 선택(both, right, left)

#define CEPH_LA_CAPT_START		0x636C6373	// [clcs]	//영상취득 시작 테이블값 설정
#define CEPH_LA_CAPT_END		0x636C6365	// [clce]	//영상취득 끝 테이블값 설정
#define CEPH_PA_CAPT_START		0x63706373	// [cpcs]	//영상취득 시작 테이블값 설정
#define CEPH_PA_CAPT_END		0x63706365	// [cpce]	//영상취득 끝 테이블값 설정
#define	CEPH_TRIGGER_NUMBER		0x6365746E	// [cetn]	//Ceph 트리거 개수 설정

/*===================================================================================*/
// Guide Laser
/*===================================================================================*/
#define LASER_ON       	0x6C726F6E	// [lron]
#define LASER_OFF      	0x6C726F66	// [lrof]
#define	LASER_ON_TIME	0x6C6F746D	// [lotm]	//limit setting laser on time
#define	LASER_STATE_REQ	0x6C72733F	// [lrs?]	// laser on/off request

/*===================================================================================*/
// Exposure Lamp
/*===================================================================================*/
#define LAMP_OFF		0x6C6D6F66	// [lmof]
#define RDY_LAMP_ON		0x72656C6F	// [relo]
#define	EXP_LAMP_ON		0x65786C6F	// [exlo]
#define	ERR_LAMP_ON		0x65726C6F	// [erlo]

/*===================================================================================*/
// MP3 SOUND
/*===================================================================================*/
#define SOUND_PLAY_OPTION	0x73706F70	// [spop]	// 음성 안내 멘트 출력 선택 (0: 음성안내 없음, 1: 한국어, 2: 영어, 3: 스페인어)
#define	SOUND_VOLUME_SET	0x73767374	// [svst]	// mp3 volume setting (0 ~ 130)
#define SOUND_TEST			0x73647474	// [sdtt]	// 테스트용 음원 출력
#define	SOUND_BCM_SET		0x73627374	// [sbst]	// mp3 volume setting (0 ~ 130)

/*===================================================================================*/
// HELP
/*===================================================================================*/
#define PRINT_COMMAND_LIST	0x68656C70	// [help]


/*===================================================================================*/
// TEST & Debugging
/*===================================================================================*/
#define	TEST				0x74657374	// [test]
#define TEST_MODE_OPTION	0x746D6F70	// [tmop]
#define MULT_VALUE			0x6D756C74	// [mult]
#define DIVI_VALUE			0x64697669	// [divi]
#define	PARAMETER_CHECK		0x78787878	// [xxxx]
#define	DEBUGGING_MODE		0x64626D64	// [dbmd]
#define	FIRST_RUN_SET		0x66727374	// [frst]
#define	POS_CNT_FRAME		0x70636672	// [pcfr]
#define	ORG_CPLT_FLG_SET	0x6F636673	// [ocfs]

#define RO_READ_ADC			0x7261643F	// [rad?]
#define PA_READ_ADC			0x7061643F	// [pad?]

#define RO_SET_ORIGIN		0x726f6f67 	// [roog]
#define PA_SET_ORIGIN		0x70616f67 	// [paog]
#define EXHIBITION_OPTION	0x65786F70	// [exop]
#define SET_TIMER_4AXIS_CO	0x7374746D	// [sttm]


#define OBJECT_RADIUS		0x6F626A72	// [objr]
#define OBJECT_DISTANCE 	0x6F626A64	// [objd]
#define OBJECT_ALPHA		0x6F626A61	// [obja]
#define OBJECT_Z			0x6F626A7A	// [objz]
#define OBJECT_ZH			0x6F626A68	// [objh]
#define TRACKING_CO_OPTION	0x74636F70	// [tcop]
#define OFFSET_SCD_VERTICAL 0x73636476	// [scdv]
#define OFFSET_SCD_HORIZON	0x73636468	// [scdh]
#define TRACKING_CO_SELECT 	0x7463736C	// [tcsl]
#define SPEED_OFFSET_HCO  	0x73706F68	// [spoh]
#define SPEED_OFFSET_VCO  	0x73706F76	// [spov]


/*===================================================================================*/
// AEC(Auto Exposure Control)
/*===================================================================================*/
#define	AEC_SET				0x61657374	// [aest]

#define SD_POSITION_CNT		0x73647063	// [sdpc]
#define HL_POSITION_CNT		0x686C7063	// [hlpc]
#define HR_POSITION_CNT		0x68727063	// [hrpc]
#define VT_POSITION_CNT		0x76747063	// [vtpc]
#define VB_POSITION_CNT		0x76627063	// [vbpc]

/*===================================================================================*/

//

/*===================================================================================*/
// WED90 command ack(respond) protocol
/*===================================================================================*/
#define R_SET_KV			0x72736B76 	// [rskv] kv 설정에 대한 응답
#define R_SET_MA    		0x72736D61 	// [rsma]
#define R_DV_KV    			0x72647676 	// [rdvv]
#define R_DV_MA    			0x72647661 	// [rdva]
#define R_SET_BUZZER		0x7273627A 	// [rsbz]
#define R_XRAY_MODE			0x72786D6F 	// [rxmo]
#define R_Q_STATUS_KV    	0x72717376 	// [rqsv] kV 설정(상태)값 확인에 대한 응답
#define R_Q_STATUS_MA 	   	0x72717361 	// [rqsa]
#define R_Q_DV_KV    		0x72716476 	// [rqdv]
#define R_Q_DV_MA    		0x72716461 	// [rqda]
#define R_Q_STATUS_BUZZER   0x72717362 	// [rqsb]
#define R_Q_XRAY_MODE    	0x7271786D 	// [rqxm] Xray 조사 방법 확인에 대한 응답
#define R_STATUS_XRAY    	0x72787279 	// [rxry] Xray on/off 상태에 대한 응답
#define R_TUBE_TEMP    		0x72746274 	// [rtbt] Tube temperature
#define POWER_ON    		0x70776F6E 	// [pwon]
#define R_FW_VERSION  		0x72667276 	// [rfrv]

//------ not yet used in ecoX
// #define R_PDD    0x72706464 	// [rpdd] CT 조사시간 일
// #define R_PHH    0x72706868 	// [rphh] CT 조사시간 시간
// #define R_PMM    0x72706D6D 	// [rpmm] CT 조사시간 분
// #define R_PSS    0x72707373 	// [rpss] CT 조사시간 초
// #define R_CDD    0x72636464 	// [rcdd] 파노 조사시간 일
// #define R_CHH    0x72636868 	// [rchh] 파노 조사시간 시간
// #define R_CMM    0x72636D6D 	// [rcmm] 파노 조사시간 분
// #define R_CSS    0x72637373 	// [rcss] 파노 조사시간 초
// #define R_EMD    0x72656D64 	// [remd] 엔지니어 모드
// #define R_EST    0x72657374 	// [rest] [GEN_ERR_31]	Exposure Time Over Error!

//------ Generator 와 error 통신 test를 진행한 후에 코드 적용할 것
// #define ERR_SKV				0x65736B76	// [eskv]
// #define ERR_SMA				0x65736D61	// [esma]
// #define ERR_SOV				0x65736F76	// [esov]
// #define ERR_SOA				0x65736F61	// [esoa]
// #define ERR_SOF				0x65736F66	// [esof]
// #define ERR_RUF				0x65727566	// [eruf]
// #define ERR_XOV				0x65786F76	// [exov]
// #define ERR_XUV				0x65787576	// [exuv]
// #define ERR_XOA				0x65786F61	// [exoa]
// #define ERR_XUA				0x65787561	// [exua]
// #define ERR_XOT				0x65786F74	// [exot]
// #define ERR_ROT				0x65726F74	// [erot]
// #define ERR_OTT				0x656F7474	// [eott]
// #define ERR_DKV				0x65646B76	// [edkv]
// #define ERR_DMA				0x65646D61	// [edma]
// #define ERR_COM1			0x65636F6D	// [ecom]
// #define ERR_ITL				0x6569746C	// [eitl]



//------------------------------------ capture frame mode
#define		FRAME_OFF		0
#define		FRAME_CT		1
#define		FRAME_UFS		2
#define		FRAME_PANO		10
#define		FRAME_CEPH		20

/*===================================================================================*/
typedef struct
{
	char	type[4];
	LONG	inst;
	LONG 	value;
	LONG	value2; 	// for [pmc_gttm_0000_0000] 2nd variable
} cmd_t;



//////// funtion
extern void ProcessCmd(void);
extern void ProcessGenAck(void);
extern cmd_t ReceiveCmd_PC(void);
extern cmd_t ReceiveCmd_LCD(void);
extern BYTE CmdMode(LONG, LONG);
extern void LCDCtrl(LONG, LONG);

extern BYTE	gen_ack_flag;
extern BYTE	gen_mode_ack_flag;
extern BYTE gen_err_flag;
extern WORD kv_value;
extern WORD ma_value;
extern BYTE kv_lcd_flag;
extern BYTE ma_lcd_flag;
extern BYTE	detector_ready;
extern BYTE	main_mode;

// todo : 
extern BYTE align_exp_time_flag;
extern ULONG	align_exp_time_10u;
extern LONG	align_acq;
extern BYTE	dark_ack_flag;
extern LONG    dark_time;
extern LONG    brit_time;
extern BYTE	brit_end_flag;
extern BYTE	brit_ack_flag;

extern LONG	gen_trig_start_time;		// todo relocate : only used in timer.c
extern LONG	gen_trig_end_time;


#endif	/*_COMMAND_H*/