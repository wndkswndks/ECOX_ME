/*C***************************************************************************
* $RCSfile: command.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the command source
*----------------------------------------------------------------------------
* RELEASE:      $Name: command.c     
* Edited  By    $Id: sslim
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include	"includes.h"


/*_____ D E F I N E ________________________________________________________*/
// #define NOT_EXIST	99
// #define LEAD_2_PA	0

/*_____ D E C L A R A T I O N ______________________________________________*/
// todo refactory
typedef struct
{
	LONG 	move_value;
	BYTE	stop_flag;
	LONG	start_speed;
	LONG 	max_speed;
} controlMotor_t;


/*_____ F U C T I O N ______________________________________________________*/
void InitCmd(void);
void Ctrl(LONG, LONG);
void ProcessGenAck(void);
BYTE gen_ack(char*);
void gen_cmd(char*);
LONG ExtractValue(char*);
cmd_t ReceiveCmd_PC(void);
cmd_t ReceiveCmd_LCD(void);
cmd_t SplitDataToCmd(char []);
void Change_ASC (char);
void SyncMaInAEC(void);


void Ctrl_KV_SETTING(LONG);
void Ctrl_MA_SETTING(LONG);
void Ctrl_KV_REQ(LONG);
void Ctrl_MA_REQ(LONG);
void Ctrl_TUBE_MODE_REQ(LONG);
void Ctrl_X_RAY_EXP_STATE_REQ(LONG);
void Ctrl_INVERTER_BUZZER_STATE_REQ(LONG);
void Ctrl_MA_DA_VALUE_REQ(LONG);
void Ctrl_KV_DA_VALUE_REQ(LONG);
void Ctrl_TUBE_NORMAL_MODE(LONG);
void Ctrl_TUBE_PULSE_MODE(LONG);
void Ctrl_INVERTER_BUZZER_ON(LONG);
void Ctrl_INVERTER_BUZZER_OFF(LONG);
void Ctrl_TUBE_TEMP_REQ(LONG);
void Ctrl_KV_DA_VALUE_SETTING(LONG);
void Ctrl_MA_DA_VALUE_SETTING(LONG);
void Ctrl_TUBE_TEMP_THRES(LONG);
void Ctrl_TUBE_TEMP_THRES_REQ(LONG);
void Ctrl_GEN_FW_REVISION(LONG);

void Ctrl_PA_MOVE(LONG);
void Ctrl_RO_MOVE(LONG);
void Ctrl_CO_MOVE(LONG);
void Ctrl_SD_MOVE(LONG);
void Ctrl_CAN_MOVE(varMotor_t*, LONG);
void Ctrl_CAN_MOVE_ACCU(varMotor_t*, LONG);
void Ctrl_CAN_MOVE_UM(varMotor_t*, LONG);

void Ctrl_PA_SLOW_STOP(LONG);
void Ctrl_RO_SLOW_STOP(LONG);
void Ctrl_RO_MAX_SPEED(LONG);
void Ctrl_RO_START_SPEED(LONG);
void Ctrl_RO_ACCEL_STEP(LONG);
void Ctrl_CO_MOVE_ACCU(LONG);
void Ctrl_CO_MOVE_CONST(LONG);
void Ctrl_SD_MOVE_CONST(LONG);
void Ctrl_SD_MOVE_PULSE(LONG);
void Ctrl_SD_MAX_SPEED(LONG);
void Ctrl_SD_START_SPEED(LONG);

void Ctrl_SOUND_PLAY_OPTION(LONG);

void Ctrl_DARK_FRAME_ACQ(LONG);
void Ctrl_BRIGHT_FRAME_ACQ(LONG);
void Ctrl_DARK_ACQ_TIME(LONG);
void Ctrl_BRIGHT_ACQ_TIME(LONG);

void Ctrl_LASER_ON(LONG);
void Ctrl_LASER_OFF(LONG);
void Ctrl_LASER_ON_TIME(LONG);

void Ctrl_SOFTWARE_EXP_ON(LONG);	
void Ctrl_SOFTWARE_EXP_OFF(LONG);

void Ctrl_CT_RO_END_ANGLE(LONG);
void Ctrl_CT_RO_END_TIME(LONG);

void Ctrl_PREC_CANCEL(LONG);
void Ctrl_FIRMWARE_REVISION_NUM(LONG);
void Ctrl_SYSTEM_REVISION_NUM(LONG);
void Ctrl_SYSTEM_INIT_COMPLETE(LONG);

void Ctrl_SCFR(LONG);
void Ctrl_CT_CAPT_FPS(LONG);
void Ctrl_PA_CT_OFFSET_POS(LONG);
void Ctrl_RO_CT_OFFSET_POS(LONG);
void Ctrl_CO_CT_OFFSET_POS(LONG);
void Ctrl_CO_PANO_OFFSET_POS(LONG);
void Ctrl_DEBUGGING_MODE(LONG);
void Ctrl_ACTUATOR_UPUP(LONG);
void Ctrl_ACTUATOR_DOWN(LONG);
void Ctrl_ACTUATOR_STOP(LONG);
void Ctrl_GEN_TRIG_TIME_START(LONG);
void Ctrl_GEN_TRIG_TIME_END(LONG);
void Ctrl_CAPT_SW_APPLY_CPLT(LONG);
void Ctrl_MULT_VALUE(LONG);

void Ctrl_CEPH_LA_CAPT_START(LONG);
void Ctrl_CEPH_LA_CAPT_END(LONG);
void Ctrl_CEPH_PA_CAPT_START(LONG);
void Ctrl_CEPH_PA_CAPT_END(LONG);
void Ctrl_CO_SCLA_OFFSET_POS(LONG);
void Ctrl_CO_SCPA_OFFSET_POS(LONG);
void Ctrl_SD_SCLA_OFFSET_POS(LONG);
void Ctrl_SD_SCPA_OFFSET_POS(LONG);
void Ctrl_RO_CEPH_OFFSET_POSITION(LONG);
void Ctrl_PA_CEPH_OFFSET_POS(LONG);

void Ctrl_CANINE_BEAM_VALUE_ENABLE(LONG);
void Ctrl_CANINE_BEAM_VALUE_REQ(LONG);
void Ctrl_PANO_NOR_TBL_OPTION(LONG);
void Ctrl_PANO_TMJ_TBL_OPTION(LONG);
void Ctrl_PANO_STAN_PREC_ANGLE(LONG);
void Ctrl_PANO_TMJ_PREC_ANGLE(LONG);
void Ctrl_PANO_STAN_CAPT_START(LONG);
void Ctrl_PANO_TMJ_CAPT_START(LONG);
void Ctrl_PANO_TMJ_CAPT_END(LONG);
void Ctrl_PANO_TMJ2_PREC_ANGLE(LONG);
void Ctrl_PANO_TRIGGER_NUMBER(LONG);
void Ctrl_PA_PANO_OFFSET_POS(LONG);
void Ctrl_SELECT_BITEWING(LONG);
void Ctrl_PANO_RO_END_ANGLE(LONG);


void Ctrl_PULSE_PERIOD(LONG);
void Ctrl_PX_PANO_SET(LONG);

//---------- LCD command 
void LCtrl_KV_SETTING(LONG);
void LCtrl_MA_SETTING(LONG);
void LCtrl_ACTUATOR_UPUP(LONG);
void LCtrl_ACTUATOR_DOWN(LONG);
void LCtrl_ACTUATOR_STOP(LONG);
void LCtrl_LASER_ON(LONG);	
void LCtrl_LASER_OFF(LONG);
void LCtrl_CANINE_BEAM_VALUE_ENABLE(LONG);
void LCtrl_LASER_STATE_REQ(LONG);

//---------- alex
void Ctrl_ALIGN_EXPOSE(LONG);

//----- scout
void Ctrl_SCOUT(LONG value);
void Ctrl_SCTM(LONG value);

void Ctrl_AEC_SET(LONG);


void Ctrl_LAMP_OFF(LONG);
void Ctrl_RDY_LAMP_ON(LONG);
void Ctrl_EXP_LAMP_ON(LONG);
void Ctrl_ERR_LAMP_ON(LONG);

void Ctrl_RO_READ_ADC(LONG);
void Ctrl_PA_READ_ADC(LONG);
void Ctrl_RO_SET_ORIGIN(LONG);
void Ctrl_PA_SET_ORIGIN(LONG);

void Ctrl_SD_POSITION_CNT(LONG);

//------------ option 관련
void Ctrl_OPTION_PRINT(LONG);
void Ctrl_CEPH_OPTION(LONG);
void Ctrl_CO_OPTION(LONG);
void Ctrl_EXHIBITION_OPTION(LONG);
void Ctrl_AC_OPTION(LONG);
void Ctrl_GEN_OPTION(LONG);
void Ctrl_PRINT_COMMAND_LIST(void);
void Ctrl_TRACKING_CO_OPTION(LONG);

//---- 추가된 명령어  todo : 순서 정리 필요
void Ctrl_SOUND_VOLUME_SET(LONG);
void Ctrl_DAP_VALUE(LONG);
void Ctrl_DAP_CAPTURE_TIME(LONG);

void Ctrl_PA_START_SPEED(LONG);
void Ctrl_PA_MAX_SPEED(LONG);
void Ctrl_CEPH_TRIGGER_NUMBER(LONG);

void Ctrl_CAN_MAX_SPEED(varMotor_t*, LONG);
void Ctrl_CAN_START_SPEED(varMotor_t*, LONG);

void Ctrl_SET_TIMER_4AXIS_CO(LONG);
void Ctrl_CAN_AXIS_POSITION_CNT(varMotor_t *);
void Ctrl_CT_OFFSET_POS(varMotor_t *, LONG);

//----------- class 
// event call
// todo rename : g_IsStartedEvent
LONG g_OnStartEvent;

void Ctrl_START_EVENT(LONG);
LONG Get_IsStartedEvent(void);
void Set_IsStartedEvent(LONG);

void Ctrl_START_EVENT(LONG value) 	// [stev]
{
	g_OnStartEvent = value;

	if(g_OnStartEvent == ON) {
		// UartPrintf("==> Initialize Event \r\n");
	}
	else if(g_OnStartEvent == 2) {
		// UartPrintf("==> Start Event \r\n");
	}
	else {
		// UartPrintf("==> Start Event end\r\n");
	}
}

LONG Get_IsStartedEvent(void)
{
	return g_OnStartEvent;
}

void Set_IsStartedEvent(LONG value)
{
	g_OnStartEvent = value;
}

//----------- class 

void Ctrl_OBJECT_RADIUS(LONG);
void Ctrl_OBJECT_DISTANCE(LONG);
void Ctrl_OBJECT_ALPHA(LONG);
void Ctrl_OBJECT_Z(LONG);
void Ctrl_OBJECT_ZH(LONG);
void Ctrl_OFFSET_SCD_HORIZON(LONG);
void Ctrl_OFFSET_SCD_VERTICAL(LONG);
void Ctrl_AXIS_OFFSET_POS(varMotor_t *, LONG);
void Ctrl_CAN_MOVE_OFFSET(varMotor_t *, LONG);
void Ctrl_TRACKING_CO_SELECT(LONG);
void Ctrl_CAN_MOVE_CONST(varMotor_t *, LONG);
void Ctrl_SPEED_OFFSET_VCO(LONG);
void Ctrl_SPEED_OFFSET_HCO(LONG);

/*_____ V A R I A B L E ____________________________________________________*/
LONG	pa_0_1_unit_pulse = PA_0_1_MM_PULSE_2;
BYTE	pa_stop_flag;

extern motor_t		mPA;
extern motor_t		mRO;
extern BYTE	isModeCompleted;		// from EcoX_one.c  => todo 정리 : command.c에서 사용이 되어 extern함


//--------- variables of gen_ack()
BYTE	gen_ack_flag;
BYTE	gen_mode_ack_flag;		// todo naming  : genExpMode... pulse, continuous mode를 의미함, 
BYTE 	gen_err_flag;
WORD 	kv_value;
WORD 	ma_value;
WORD 	kv_da_value;			// DA kv설정을 위한 값
WORD 	ma_da_value;			// DA ma설정을 위한 값
BYTE 	kv_lcd_flag;
BYTE 	ma_lcd_flag;

BYTE	detector_ready = OFF;

LONG    dark_time = 5000;
LONG    brit_time = 5000;

BYTE	dark_ack_flag = NO;
BYTE	brit_ack_flag = NO;
BYTE	brit_end_flag = NO;

BYTE	align_exp_time_flag	= NO;
ULONG	align_exp_time_10u	= 0;
LONG	align_acq;

LONG	gen_trig_start_time = 700;  	// todo default : 30fps, 50fps ==> 700, 2000
LONG	gen_trig_end_time = 2000;


// todo relocate 
extern BYTE	aok_flag;

BYTE 	org_pos_90s_over_flag;


//----- ceph 관련 변수
BYTE	dark_frame_acq_flag;
LONG	s_ceph_la_capt_start_tbl_num	= 21;
LONG	s_ceph_pa_capt_start_tbl_num	= 21;
LONG	scan_ceph_la_det_trig_table_num	= S_CEPH_LA_DET_TRIG_4B_N;	// 덴트리 9차에서는 timer.c에서 사용되나, TD_x_2401경우는 timer.c에서 사용되지 않는다. 
LONG	scan_ceph_pa_det_trig_table_num	= S_CEPH_PA_DET_TRIG_4B_N;	// todo : 필요없는 변수 같다. 확인후 삭제
extern BYTE	sc_tbl_sel_1;				// from capture.c
extern BYTE	sc_tbl_sel_2;				// from capture.c
extern WORD	ceph_trig_num;				// from capture.c

//------- pano 관련 변수
LONG	pano_stan_prec_angle 	= 239;	// [0.1 degree]
LONG	pano_tmj_prec_angle		= 312;	// [0.1 degree]
LONG	pano_tmj2_prec_angle	= 2112;	// pano_tmj_prec_angle + 1800	TMJ1의 시작위치와 대칭	(1800 : 180 도)
LONG	pano_sinu_prec_angle	= 22;	// [0.1 degree]
extern 	BYTE pano_ro_end_angle;		// from capture.c

//------- canine beam  관련 변수
extern BYTE isOnCanineBeam;				// from command_mode.c
extern LONG Panomode_paxis_value;		// from command_mode.c


extern BYTE	pano_nor_tbl_optn;				// from capture.c
extern BYTE pano_tmj_tbl_optn;				// from capture.c
extern WORD	pano_tbl_no;					// from capture.c
extern LONG	pano_stan_capt_start_table_num;	// from capture.c
extern WORD	pano_trig_num;					// from capture.c
extern BYTE	bitewing_flag;					// from capture.c
extern LONG pano_tmj_capt_end_table_num;	// from capture.c
extern LONG pano_tmj_capt_start_table_num;	// from capture.c

//-------  LCD 관련 변수
BYTE	lr_lcd_flag = NO;

//------- alignment 
LONG	alex_val;

LONG	laser_on_time;
extern LONG laser_on_time_5m;			// from timer.c
extern BYTE	laser_lmt_cnt_flag;			// from timer.c

//----- scout 변수
extern LONG scout_pulse;			// from capture.c

//----- AEC 변수
extern int isOnAEC;					// from capture.c
BYTE 	isChanging_mAInAEC = 0;
LONG	maValue_AEC = 0;

//------ 전시회
extern BYTE 	isOnExhibition;		// from EcoX.c

// 촬영후 복귀 전에 [prcl] 실행시 버그 발생
extern BYTE isCaptureClosing;		// from capture.c

//----- Tracking CO
extern t_ObjectInfo g_Obj;				// from four_axis.c
extern BYTE g_TcOption;		// from cammand_mode.c
extern LONG	offset_SCD_h;		// four_axis.c
extern LONG	offset_SCD_v;		// four_axis.c
extern LONG g_selectTrackingCO; // four_axis.c
extern LONG g_SpeedOffset_H;
extern LONG g_SpeedOffset_V;


extern BYTE    g_IsInitializingGen;	// from system.c
///////////////////////////////

void PrintCtrl(LONG inst, LONG value)
{
	char strInst[5] = {0};
	strInst[0] = (char)((inst>>24) & 0x000000ff);
	strInst[1] = (char)((inst>>16) & 0x000000ff);
	strInst[2] = (char)((inst>>8) & 0x000000ff);
	strInst[3] = (char)((inst>>0) & 0x000000ff);
	strInst[4] = '\0';

	UartPrintf("==> Ctrl : [%s],  value : %ld \r\n", strInst, value);
}


/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
// todo naming : Ctrl(), 
// PC commnand에서만 쓰이는 명령이 아니라 자체제어, mode등에서 쓰인다. 보다 일반적인 이름 필요
// todo : change file name : control.c, control.h.  command_mode.c -> command.c or mode.c
// ProcessCmd -> EcoX.c로 옮기는 것??
void Ctrl(LONG inst, LONG value)
{
	// PrintCtrl(inst, value);

	switch(inst)
	{
		//------------------- Generator 관련 기능
		case INVERTER_BUZZER_STATE_REQ:	Ctrl_INVERTER_BUZZER_STATE_REQ(value);	break;	// [ib?_]
		case KV_SETTING:				Ctrl_KV_SETTING(value);				break;	// [kvst]
		case MA_SETTING:				Ctrl_MA_SETTING(value);				break;	// [mast]
		case KV_REQ:					Ctrl_KV_REQ(value);					break;	// [kv?_]
		case MA_REQ:					Ctrl_MA_REQ(value);					break;	// [ma?_]
		case TUBE_MODE_REQ:				Ctrl_TUBE_MODE_REQ(value);			break;	// [tum?]
		case X_RAY_EXP_STATE_REQ:		Ctrl_X_RAY_EXP_STATE_REQ(value);	break;	// [exp?]
		case MA_DA_VALUE_REQ:			Ctrl_MA_DA_VALUE_REQ(value);		break;	// [da?_]
		case KV_DA_VALUE_REQ:			Ctrl_KV_DA_VALUE_REQ(value);		break;	// [dv?_]
		case TUBE_NORMAL_MODE:			Ctrl_TUBE_NORMAL_MODE(value);		break;	// [tunr]
		case TUBE_PULSE_MODE:			Ctrl_TUBE_PULSE_MODE(value);		break;	// [tupl]
		case INVERTER_BUZZER_ON:		Ctrl_INVERTER_BUZZER_ON(value);		break;	// [ibon]
		case INVERTER_BUZZER_OFF:		Ctrl_INVERTER_BUZZER_OFF(value);	break;	// [ibof]
		case TUBE_TEMP_REQ:				Ctrl_TUBE_TEMP_REQ(value);			break;	// [tt?_]
		case KV_DA_VALUE_SETTING:		Ctrl_KV_DA_VALUE_SETTING(value);	break;	// [dvst]
		case MA_DA_VALUE_SETTING:		Ctrl_MA_DA_VALUE_SETTING(value);	break;	// [dast]
		case TUBE_TEMP_THRES:			Ctrl_TUBE_TEMP_THRES(value);		break;	// [ttth]
		case TUBE_TEMP_THRES_REQ:		Ctrl_TUBE_TEMP_THRES_REQ(value);	break;	// [ttt?]
		case GEN_FW_REVIRSION:			Ctrl_GEN_FW_REVISION(value);		break;	// [gfrv]
	
		//------------------- Axis 제어 기능
		case PA_MOVE:					Ctrl_PA_MOVE(value);				break;	// [pamv] // 0.1 mm 단위
		case PA_SLOW_STOP:				Ctrl_PA_SLOW_STOP(value);			break;	// [pasp]
		case RO_MOVE:					Ctrl_RO_MOVE(value);				break;	// [romv]	// 0.1 도 단위
		case RO_SLOW_STOP:				Ctrl_RO_SLOW_STOP(value);			break;  // [rosp]
		case RO_MAX_SPEED:				Ctrl_RO_MAX_SPEED(value);			break;  // [roms]
		case RO_START_SPEED:			Ctrl_RO_START_SPEED(value);			break;  // [ross]
		case RO_ACCEL_STEP:				Ctrl_RO_ACCEL_STEP(value);			break;  // [roas]
		case CO_MOVE:					Ctrl_CO_MOVE(value); 				break;	// [comv]	[0.1 mm]
		case CO_MOVE_ACCU:				Ctrl_CO_MOVE_ACCU(value); 			break;	// [coma]	[0.01 mm]
		case CO_MOVE_CONST: 			Ctrl_CO_MOVE_CONST(value);			break;	// [comc]
		case SD_MOVE:					Ctrl_SD_MOVE(value); 				break;	// [sdmv]	[0.1 mm]
		case SD_MOVE_CONST:				Ctrl_SD_MOVE_CONST(value);			break;	// [sdmc]
		case SD_MOVE_PULSE:				Ctrl_SD_MOVE_PULSE(value);			break;	// [sdmp]
		case SD_START_SPEED:			Ctrl_SD_START_SPEED(value);			break;	// [sdss]
		case SD_MAX_SPEED:				Ctrl_SD_MAX_SPEED(value);			break;	// [sdms]

		//------------------- 4-axis collimator
		case HR_MOVE:					Ctrl_CAN_MOVE(&g_HR, value); 		break;	// [hrmv]
		case HL_MOVE:					Ctrl_CAN_MOVE(&g_HL, value); 		break;	// [hlmv]
		case VT_MOVE:					Ctrl_CAN_MOVE(&g_VT, value); 		break;	// [vtmv]
		case VB_MOVE:					Ctrl_CAN_MOVE(&g_VB, value); 		break;	// [vbmv]

		case HR_MOVE_ACCU:				Ctrl_CAN_MOVE_ACCU(&g_HR, value);	break;	// [hrma]
		case HL_MOVE_ACCU:				Ctrl_CAN_MOVE_ACCU(&g_HL, value);	break;	// [hlma]
		case VT_MOVE_ACCU:				Ctrl_CAN_MOVE_ACCU(&g_VT, value);	break;	// [vtma]
		case VB_MOVE_ACCU:				Ctrl_CAN_MOVE_ACCU(&g_VB, value);	break;	// [vbma]

		case HR_MOVE_UM:				Ctrl_CAN_MOVE_UM(&g_HR, value);		break;	// [hrmu]
		case HL_MOVE_UM:				Ctrl_CAN_MOVE_UM(&g_HL, value);		break;	// [hlmu]
		case VT_MOVE_UM:				Ctrl_CAN_MOVE_UM(&g_VT, value);		break;	// [vtmu]
		case VB_MOVE_UM:				Ctrl_CAN_MOVE_UM(&g_VB, value);		break;	// [vbmu]

		case HL_MOVE_CONST:				Ctrl_CAN_MOVE_CONST(&g_HL, value);	break;	// [hlmc]
		case HR_MOVE_CONST:				Ctrl_CAN_MOVE_CONST(&g_HR, value);	break;	// [hrmc]
		case VT_MOVE_CONST:				Ctrl_CAN_MOVE_CONST(&g_VT, value);	break;	// [vtmc]
		case VB_MOVE_CONST:				Ctrl_CAN_MOVE_CONST(&g_VB, value);	break;	// [vbmc]

		//------------------- CT 관련 기능
		case PA_CT_OFFSET_POS:			Ctrl_PA_CT_OFFSET_POS(value);		break;	// [pact]
		case RO_CT_OFFSET_POS:			Ctrl_RO_CT_OFFSET_POS(value);		break;	// [roct]
		case CO_CT_OFFSET_POS:			Ctrl_CO_CT_OFFSET_POS(value);		break;	// [coct]
		case SCFR:						Ctrl_SCFR(value);					break;	// [scfr]
		case CT_CAPT_FPS:				Ctrl_CT_CAPT_FPS(value);			break;	// [cfps]
		case CT_RO_END_ANGLE:			Ctrl_CT_RO_END_ANGLE(value);		break;	// [crea]
		case CT_RO_END_TIME:			Ctrl_CT_RO_END_TIME(value);			break;	// [cret]
		case SCOUT:						Ctrl_SCOUT(value);					break;	// [scut]
		case SCTM:						Ctrl_SCTM(value);					break;	// [sctm]

		//------------------- Pano 관련 기능
		case PA_PANO_OFFSET_POS:		Ctrl_PA_PANO_OFFSET_POS(value);		break;	// [papa]
		case CO_PANO_OFFSET_POS:		Ctrl_CO_PANO_OFFSET_POS(value);		break;	// [copa]
		case PANO_NOR_TBL_OPTION:		Ctrl_PANO_NOR_TBL_OPTION(value);	break;	// [pnto]
		case PANO_TMJ_TBL_OPTION:		Ctrl_PANO_TMJ_TBL_OPTION(value);	break;	// [ptto]
		case PANO_STAN_CAPT_START:		Ctrl_PANO_STAN_CAPT_START(value);	break;	// [pcsp]
        case PANO_TRIGGER_NUMBER:		Ctrl_PANO_TRIGGER_NUMBER(value);	break;	// [patn]
		case PANO_STAN_PREC_ANGLE:		Ctrl_PANO_STAN_PREC_ANGLE(value);	break;	// [ppps] 0.1 도 단위
		case PANO_TMJ_CAPT_START:		Ctrl_PANO_TMJ_CAPT_START(value);	break;	// [tesn]	//[tesn]
		case PANO_TMJ_CAPT_END:			Ctrl_PANO_TMJ_CAPT_END(value);		break;	// [teen]
		case PANO_TMJ_PREC_ANGLE:		Ctrl_PANO_TMJ_PREC_ANGLE(value);	break;	// [tpps]
		case PANO_TMJ2_PREC_ANGLE:		Ctrl_PANO_TMJ2_PREC_ANGLE(value);	break;	// [t2ps]
		case SELECT_BITEWING:			Ctrl_SELECT_BITEWING(value);		break;	// [bwfg]
		case CANINE_BEAM_VALUE_ENABLE:	Ctrl_CANINE_BEAM_VALUE_ENABLE(value);	break;	// [cbve]
		case CANINE_BEAM_VALUE_REQ: 	Ctrl_CANINE_BEAM_VALUE_REQ(value);	break;	// [cbv?]
		case PX_PANO_SET:				Ctrl_PX_PANO_SET(value);			break;	// [ppst] // 0.1 mm 단위
		case PANO_RO_END_ANGLE:			Ctrl_PANO_RO_END_ANGLE(value);		break;	// [prea]

		//------------------- Ceph 관련 기능
		case PA_CEPH_OFFSET_POS:		Ctrl_PA_CEPH_OFFSET_POS(value);		break;	// [pacp]
		case RO_CEPH_OFFSET_POSITION:	Ctrl_RO_CEPH_OFFSET_POSITION(value);break;	// [roco] //비슷한 명령라인인 있음.
		case CO_SCLA_OFFSET_POS:		Ctrl_CO_SCLA_OFFSET_POS(value); 	break;	// [colo]	// 0.01 mm 단위
		case CO_SCPA_OFFSET_POS:		Ctrl_CO_SCPA_OFFSET_POS(value); 	break;	// [copo]	// 0.01 mm 단위
		case SD_SCLA_OFFSET_POS:		Ctrl_SD_SCLA_OFFSET_POS(value);		break;	// [sdlo]
		case SD_SCPA_OFFSET_POS:		Ctrl_SD_SCPA_OFFSET_POS(value);		break;	// [sdpo]
		case CEPH_LA_CAPT_START:		Ctrl_CEPH_LA_CAPT_START(value);		break;	// [clcs]
		case CEPH_LA_CAPT_END:			Ctrl_CEPH_LA_CAPT_END(value);		break;	// [clce]
		case CEPH_PA_CAPT_START:		Ctrl_CEPH_PA_CAPT_START(value);		break;	// [cpcs]
		case CEPH_PA_CAPT_END:			Ctrl_CEPH_PA_CAPT_END(value);		break;	// [cpce]
		case MULT_VALUE:				Ctrl_MULT_VALUE(value);				break;	// [mult]
		case CEPH_TRIGGER_NUMBER:		Ctrl_CEPH_TRIGGER_NUMBER(value);	break;	// [cetn]

		//------------------- Generator, Detector, capture 제어
		case SOFTWARE_EXP_ON:			Ctrl_SOFTWARE_EXP_ON(value);		break;	// [swon]
		case SOFTWARE_EXP_OFF:			Ctrl_SOFTWARE_EXP_OFF(value);		break;	// [swof]
		case SYSTEM_INIT_COMPLETE:		Ctrl_SYSTEM_INIT_COMPLETE(value); 	break;	// [uok?]
		case CAPT_SW_APPLY_CPLT:		Ctrl_CAPT_SW_APPLY_CPLT(value);		break;	// [aok?]
		case PREC_CANCEL:				Ctrl_PREC_CANCEL(value);			break;	// [prcl]
		case AEC_SET:					Ctrl_AEC_SET(value); 				break;	// [aest]

		case DARK_FRAME_ACQ:			Ctrl_DARK_FRAME_ACQ(value);			break;	// [dark]
        case DARK_ACQ_TIME:				Ctrl_DARK_ACQ_TIME(value);			break;	// [datm]
		case BRIGHT_FRAME_ACQ:			Ctrl_BRIGHT_FRAME_ACQ(value);		break;	// [brit]
        case BRIGHT_ACQ_TIME:			Ctrl_BRIGHT_ACQ_TIME(value);		break;	// [brtm]
		case GEN_TRIG_TIME_START:		Ctrl_GEN_TRIG_TIME_START(value);	break;	// [gtts]
		case GEN_TRIG_TIME_END:			Ctrl_GEN_TRIG_TIME_END(value);		break;	// [gtte]
		case PULSE_PERIOD:				Ctrl_PULSE_PERIOD(value);			break;	// [prio] pano pulse period	[10 us 단위]
		case ALIGN_EXPOSE:				Ctrl_ALIGN_EXPOSE(value);			break;	// [alex]

		//------------------- Actuator, Laser, Lamp 제어
		case ACTUATOR_UPUP:				Ctrl_ACTUATOR_UPUP(value); 			break;	// [acup]
		case ACTUATOR_DOWN:				Ctrl_ACTUATOR_DOWN(value); 			break;	// [acdn]
		case ACTUATOR_STOP:				Ctrl_ACTUATOR_STOP(value); 			break;	// [acsp]
		case LASER_ON:					Ctrl_LASER_ON(value);				break;	// [lron]
		case LASER_OFF:					Ctrl_LASER_OFF(value);				break;	// [lrof]
		case LASER_ON_TIME:				Ctrl_LASER_ON_TIME(value);			break;	// [lotm]
		case LAMP_OFF:					Ctrl_LAMP_OFF(value);				break;	// [lmof]
		case RDY_LAMP_ON:				Ctrl_RDY_LAMP_ON(value);			break;	// [relo]
		case EXP_LAMP_ON:				Ctrl_EXP_LAMP_ON(value);			break;	// [exlo]
		case ERR_LAMP_ON:				Ctrl_ERR_LAMP_ON(value);			break;	// [erlo]

		//------------------- option 관련 명령어
		case OPTION_PRINT:				Ctrl_OPTION_PRINT(value);			break;	// [op?_] 
		case CEPH_OPTION:				Ctrl_CEPH_OPTION(value);			break;	// [ceop]
		case CO_OPTION:		 			Ctrl_CO_OPTION(value); 				break;	// [coop]


		//------------------- 기타 명령어
		case FIRMWARE_REVISION_NUM:		Ctrl_FIRMWARE_REVISION_NUM(value);	break;	// [frn?]
		case SYSTEM_REVISION_NUM:   	Ctrl_SYSTEM_REVISION_NUM(value);	break;	// [rev?]
		case SOUND_PLAY_OPTION:			Ctrl_SOUND_PLAY_OPTION(value);		break;	// [spop]
		case DEBUGGING_MODE:			Ctrl_DEBUGGING_MODE(value);			break;	// [dbmd]
		case RO_READ_ADC:				Ctrl_RO_READ_ADC(value);			break;	// [rad?]
		case PA_READ_ADC:				Ctrl_PA_READ_ADC(value);			break;	// [pad?]
		case EXHIBITION_OPTION:			Ctrl_EXHIBITION_OPTION(value);		break; 	// [exst]
		case AC_OPTION:					Ctrl_AC_OPTION(value);				break;	// [acop]					
		case GEN_OPTION:				Ctrl_GEN_OPTION(value);				break;	// [geop]
		case PRINT_COMMAND_LIST:		Ctrl_PRINT_COMMAND_LIST();			break;	// [help]					


		//-------------------- 추가 명령어
		case RO_SET_ORIGIN:				Ctrl_RO_SET_ORIGIN(value);			break;	// [roog]
		case PA_SET_ORIGIN:				Ctrl_PA_SET_ORIGIN(value);			break;	// [paog]
		case PA_START_SPEED:			Ctrl_PA_START_SPEED(value);			break;	// [pass]
		case PA_MAX_SPEED:				Ctrl_PA_MAX_SPEED(value);			break;	// [pams]

		case SOUND_VOLUME_SET: 			Ctrl_SOUND_VOLUME_SET(value);		break;	// [svst]
		case DAP_VALUE:					Ctrl_DAP_VALUE(value);				break;	// [dap_]
		case DAP_CAPTURE_TIME:			Ctrl_DAP_CAPTURE_TIME(value);		break;  // [ctm?]
		case START_EVENT: 				Ctrl_START_EVENT(value); 			break; 	// [stev]

		case HL_CT_OFFSET_POS:			Ctrl_CT_OFFSET_POS(&g_HL, value);	break;	// [hlct]
		case HR_CT_OFFSET_POS:			Ctrl_CT_OFFSET_POS(&g_HR, value);	break;	// [hrct]
		case VT_CT_OFFSET_POS:			Ctrl_CT_OFFSET_POS(&g_VT, value);	break;	// [vtct]
		case VB_CT_OFFSET_POS:			Ctrl_CT_OFFSET_POS(&g_VB, value);	break;	// [vbct]

		// 4 Axis Control
		case SD_POSITION_CNT:			Ctrl_SD_POSITION_CNT(value);		break;	// [sdpc]
		case HL_POSITION_CNT: 			Ctrl_CAN_AXIS_POSITION_CNT(&g_HL);	break;	// [hlpc]
		case HR_POSITION_CNT: 			Ctrl_CAN_AXIS_POSITION_CNT(&g_HR);	break;	// [hrpc]
		case VT_POSITION_CNT: 			Ctrl_CAN_AXIS_POSITION_CNT(&g_VT);	break;	// [vtpc]
		case VB_POSITION_CNT: 			Ctrl_CAN_AXIS_POSITION_CNT(&g_VB);	break;	// [vbpc]
		case HR_MAX_SPEED: 				Ctrl_CAN_MAX_SPEED(&g_HR, value);	break;	// [hrms]
		case HL_MAX_SPEED: 				Ctrl_CAN_MAX_SPEED(&g_HL, value);	break;	// [hlms]
		case VT_MAX_SPEED: 				Ctrl_CAN_MAX_SPEED(&g_VT, value);	break;	// [vtms]
		case VB_MAX_SPEED: 				Ctrl_CAN_MAX_SPEED(&g_VB, value);	break;	// [vbms]
		case HR_START_SPEED: 			Ctrl_CAN_START_SPEED(&g_HR, value);	break;	// [hrss]
		case HL_START_SPEED: 			Ctrl_CAN_START_SPEED(&g_HL, value);	break;	// [hlss]
		case VT_START_SPEED: 			Ctrl_CAN_START_SPEED(&g_VT, value);	break;	// [vtss]
		case VB_START_SPEED: 			Ctrl_CAN_START_SPEED(&g_VB, value);	break;	// [vbss]
		case SET_TIMER_4AXIS_CO:		Ctrl_SET_TIMER_4AXIS_CO(value);		break;	// [sttm]

		case OBJECT_RADIUS:				Ctrl_OBJECT_RADIUS(value);			break;	// [objr]
		case OBJECT_DISTANCE:			Ctrl_OBJECT_DISTANCE(value);		break;	// [objd]
		case OBJECT_ALPHA:				Ctrl_OBJECT_ALPHA(value);			break;	// [obja]
		case OBJECT_Z:					Ctrl_OBJECT_Z(value);				break;	// [objz]
		case OBJECT_ZH:					Ctrl_OBJECT_ZH(value);				break; 	// [objh]
		case TRACKING_CO_OPTION:		Ctrl_TRACKING_CO_OPTION(value);		break;	// [tcop]
		case OFFSET_SCD_VERTICAL:		Ctrl_OFFSET_SCD_VERTICAL(value);	break;	// [scdv]
		case OFFSET_SCD_HORIZON:		Ctrl_OFFSET_SCD_HORIZON(value);		break;	// [scdh]

		case HL_OFFSET_POS:				Ctrl_AXIS_OFFSET_POS(&g_HL, value);	break;	// [hlof]
		case HR_OFFSET_POS:				Ctrl_AXIS_OFFSET_POS(&g_HR, value);	break;	// [hrof]
		case VT_OFFSET_POS:				Ctrl_AXIS_OFFSET_POS(&g_VT, value);	break;	// [vtof]
		case VB_OFFSET_POS:				Ctrl_AXIS_OFFSET_POS(&g_VB, value);	break;	// [vbof]

		case HL_MOVE_OFFSET:			Ctrl_CAN_MOVE_OFFSET(&g_HL, value);	break;	// [hlmo]
		case HR_MOVE_OFFSET:			Ctrl_CAN_MOVE_OFFSET(&g_HR, value);	break;	// [hrmo]
		case VT_MOVE_OFFSET:			Ctrl_CAN_MOVE_OFFSET(&g_VT, value);	break;	// [vtmo]
		case VB_MOVE_OFFSET:			Ctrl_CAN_MOVE_OFFSET(&g_VB, value);	break;	// [vbmo]

		case TRACKING_CO_SELECT:		Ctrl_TRACKING_CO_SELECT(value);		break;	// [tcsl]

		case SPEED_OFFSET_HCO:			Ctrl_SPEED_OFFSET_HCO(value);		break;  // [spoh]
		case SPEED_OFFSET_VCO:			Ctrl_SPEED_OFFSET_VCO(value);		break;  // [spov]
		default: 	PCPuts("Undefined Ctrl-command!\r\n");					break;
	}
}




void Ctrl_SPEED_OFFSET_HCO(LONG value)
{
	// 단위 펄스 수
	g_SpeedOffset_H = value;
	PCPrintf("[rpm_spoh_%ld] \r\n", g_SpeedOffset_H);
}

void Ctrl_SPEED_OFFSET_VCO(LONG value)
{
	// 단위 펄스 수
	g_SpeedOffset_V = value;
	PCPrintf("[rpm_spov_%ld] \r\n", g_SpeedOffset_V);
}


void Ctrl_CAN_MOVE_CONST(varMotor_t *axis, LONG value)
{
	axis->stop_flag = NO;
	rcan_puts(axis->axisId, CAN_AXIS_MOVE_CONST, value, dmy_msg);	dly_10us(dly_t);
}


void Ctrl_TRACKING_CO_SELECT(LONG value)
{
	// 0 : both,  1 : Horizon CO, 2: Vertical CO
	g_selectTrackingCO = value;
	PCPrintf("[rpm_tcsl_%ld] \r\n", g_selectTrackingCO);
}


void Ctrl_AXIS_OFFSET_POS(varMotor_t *axis, LONG value)
{
	axis->offset_um = value;
	PCPrintf("[rpm_%sof_%ld] \r\n", axis->axisName, axis->offset_um);
}


void Ctrl_OFFSET_SCD_HORIZON(LONG value)
{
	// 단위 um
	offset_SCD_h = value;
	UartPrintf("[rpm_scdh_%ld]\r\n", offset_SCD_h);
}

void Ctrl_OFFSET_SCD_VERTICAL(LONG value)
{
	// 단위 um
	offset_SCD_v = value;
	UartPrintf("[rpm_scdv_%ld]\r\n", offset_SCD_v);
}

void Ctrl_CT_OFFSET_POS(varMotor_t *axis, LONG value)   
{
	// 단위 : um
	LONG p_maxLength_um;
	LONG n_maxLength_um;

	p_maxLength_um = (axis->p_max_pulse * 10) / axis->unit_0_01_pulse;
	n_maxLength_um = (axis->n_max_pulse * 10) / axis->unit_0_01_pulse;

    // 수신 값이 P max 값보다 크면 P max로 설정
    if(value > p_maxLength_um)
    {
        PCPrintf("%s ct offset high \r\n", axis->axisName);
        axis->ct_offset_pos = p_maxLength_um;
    }
    // N max 값보다 작으면 N max로 설정
    else if(value < (-n_maxLength_um))
    {
        PCPrintf("%s ct offset low \r\n", axis->axisName);
        axis->ct_offset_pos = -n_maxLength_um;
    }
    else
        axis->ct_offset_pos = value;

    PCPrintf("[rpm_%sct_%ld]\r\n", axis->axisName, axis->ct_offset_pos);
}


void Ctrl_TRACKING_CO_OPTION(LONG value)
{
	g_TcOption = (BYTE)value;
	PCPrintf("[rpm_tcop_%d]\r\n", g_TcOption);
}


void Ctrl_CAN_AXIS_POSITION_CNT(varMotor_t *axis)
{
	rcan_puts(axis->axisId, CAN_AXIS_CNT, (LONG) 0, dmy_msg); dly_10us(70);
}


void Ctrl_OBJECT_RADIUS(LONG value)
{
	LONG obj_radius_um;

	if( 1 <= value && value <= 80000 ) {
		obj_radius_um = value;
		g_Obj.radius = (double)obj_radius_um / 1000; 
		UartPrintf("[rpm_objr_%ld]\r\n", obj_radius_um);
	}
	else {
		UartPrintf("%ld[um] is not radius range 1 ~ 80000[um] \r\n", value);
	}
}

void Ctrl_OBJECT_DISTANCE(LONG value)
{
	LONG obj_distance_um;


	if( 0 <= value && value <= 80000 ) {
		obj_distance_um = value;
		g_Obj.distance = (double)obj_distance_um / 1000;
		UartPrintf("[rpm_objd_%ld] \r\n", obj_distance_um);
	}
	else {
		UartPrintf("%ld[um] is not distance range 0 ~ 80000[um] \r\n", value);
	}
}
void Ctrl_OBJECT_ALPHA(LONG value)
{
	LONG obj_alpha_mdegree;

	if( 0 <= value && value <= 360000 ) {
		obj_alpha_mdegree = value;
		g_Obj.alpha = (double)obj_alpha_mdegree / 1000;
		UartPrintf("[rpm_obja_%ld] \r\n", obj_alpha_mdegree);
	}
	else {
		UartPrintf("%ld[m_degree] is not distance range 0 ~ 360000[m_degree] \r\n", value);
	}
}
void Ctrl_OBJECT_Z(LONG value)
{
	LONG obj_z_um;

	obj_z_um = value;
	g_Obj.z = (double)obj_z_um / 1000;
	UartPrintf("[rpm_objz_%ld] \r\n", obj_z_um);
}

void Ctrl_OBJECT_ZH(LONG value)
{
	LONG obj_zh_um;

	obj_zh_um = value;
	g_Obj.zh = (double)obj_zh_um / 1000;
	UartPrintf("[rpm_objh_%ld] \r\n", obj_zh_um);
}

//-------------------------------------------------
// command functions
//-------------------------------------------------
void Ctrl_SET_TIMER_4AXIS_CO(LONG value)	// [sttm]
{
	ULONG timerCnt_ms;

	timerCnt_ms = value;
	SetTimer_Dynamic4Axis(timerCnt_ms);
	UartPrintf("[rpm_sttm_%lu] \r\n", timerCnt_ms);
}




//-----------------------------------
// option
//----------------------------------
void Ctrl_GEN_OPTION(LONG value)	// [geop]
{

	if(value < 0 || 1 < value)	value = ON;
	gen_option = (BYTE)value;

	write_eeprom_BYTE(40, gen_option);

	if(gen_option == OFF)		PCPrintf("[rpm_geop_0000]	Generator OFF \r\n");
	else if(gen_option == ON)		PCPrintf("[rpm_geop_0001]	Generator ON, \r\n");
}



void Ctrl_AC_OPTION(LONG value)		// [acop]					
{
	if(value < 2 || 3 < value)	value = 3;
	ac_option = (BYTE)value;
	
	write_eeprom_BYTE(38, ac_option);
	
	if(ac_option == PAN_CTRL_FWD_AC2)	PCPuts("[rpm_acop_0002]	PAN_CTRL Forward 2 SMPS \r\n");
	else if(ac_option == PAN_CTRL_REV_AC2)	PCPuts("[rpm_acop_0003]	PAN_CTRL Reverse 2 SMPS \r\n");
}	

void Ctrl_PRINT_COMMAND_LIST(void)	// [help]
{
	PCPuts("[rpm_help]\r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Generator Control \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_ib?_]	Buzzer State Request \r\n");
	PCPuts("[pmc_kvst_####]	Tube Voltage [1 kV] \r\n");
	PCPuts("[pmc_mast_####]	Tube Current [0.1 mA] \r\n");
	PCPuts("[pmc_kv?_]	Tube Voltage Request \r\n");
	PCPuts("[pmc_ma?_]	Tube Current Request \r\n");
	PCPuts("[pmc_tum?]	Tube Mode Request \r\n");
	PCPuts("[pmc_exp?]	Exposure State Request \r\n");
	PCPuts("[pmc_da?_####]	Tube Voltage DA Value Request \r\n");
	PCPuts("[pmc_dv?_####]	Tube Current DA Value Request \r\n");
	PCPuts("[pmc_tunr]	Tube Normal Mode \r\n");
	PCPuts("[pmc_tupl]	Tube Pulse Mode \r\n");
	PCPuts("[pmc_ibon]	Exposure Buzzer On \r\n");
	PCPuts("[pmc_ibof]	Exposure Buzzer Off \r\n");
	PCPuts("[pmc_dvst_####]	Tube Voltage DA Value \r\n");
	PCPuts("[pmc_dast_####]	Tube Current DA Value \r\n");
	PCPuts("[pmc_ttth_####]	Generator Overheat Threshold \r\n");
	PCPuts("[pmc_tt?_]	Tube Temperature Request \r\n");
	PCPuts("[pmc_ttt?]	Tube Temperature Threshold Request \r\n");
	PCPuts("[pmc_gfrv]	Generator FW Revision \r\n");
	PCPuts(" \r\n");
	
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Axis Control \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_pamv_####]	Pano axis move [0.1 mm] (-200 ~ 2600) \r\n");
	PCPuts("[pmc_pasp_####]	Pano axis slow Stop \r\n");
	PCPuts("[pmc_romv_#####]	Rotator axis move [0.1 degree] (-800 ~ 4400) \r\n");
	PCPuts("[pmc_rosp_####]	rotator axis slow stop \r\n");
	PCPuts("[pmc_roms_####]	rotator axis max speed \r\n");
	PCPuts("[pmc_comv_####]	1x Collimator axis move [0.1 mm] (-25 ~ 942) \r\n");
	PCPuts("[pmc_coma_####]	1x Collimator axis move [0.01 mm] (-25 ~ 942) \r\n");
	PCPuts("[pmc_comc_####]	collimator axis move const \r\n");
	PCPuts("[pmc_sdmv_####]	Ceph Detector axis move [0.1 mm] (-190 ~ 3310) \r\n");
	PCPuts("[pmc_sdmc_####]	Ceph Detctor axis move const \r\n");
	PCPuts("[pmc_sdmp_####]	Ceph Detctor axis move pulse \r\n");
	PCPuts("[pmc_sdss_####]	Ceph Detctor axis move start speed \r\n");
	PCPuts("[pmc_sdms_####]	Ceph Detctor axis move max speed \r\n");
	PCPuts("[pmc_roog]	Set Rotator origin \r\n");
	PCPuts("[pmc_paog]	Set Pano origin \r\n");
	PCPuts("[pmc_pass_####]	Pano axis start speed \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	CT Capture Parameters \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_pact_####]	CT Pano Axis Offset Position [0.1 mm] \r\n");
	PCPuts("[pmc_roct_####]	Rotator Axis Offset Position [0.1 mm] \r\n");
	PCPuts("[pmc_coct_####]	Collimator Axis Offset Position [0.1 mm] \r\n");
	PCPuts("[pmc_scfr_####]	# of Scanning Frame (1200, 400) \r\n");
	PCPuts("[pmc_cfps_####]	CT FPS (default 50) \r\n");
	PCPuts("[pmc_crea_####]	CT Rotator End Angle (0: 0, 1: 90, 2: 180, 3: 270, 4: 360, 5: None [degrees]) \r\n");
	PCPuts("[pmc_cret_####]	CT Rotator End delay Time (0 s ~ 120 s) \r\n");
	PCPuts("[pmc_scut]	Scout Capture Apply \r\n");
	PCPuts("[pmc_sctm_####]	Scout Capture Time [1 ms] \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Panorama Capture Parameters \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_papa_####]	Panorama Pano Axis Offset Position [0.1 mm] \r\n");
	PCPuts("[pmc_copa_####]	Collimator Pano Axis Offset Position [0.1 mm] \r\n");
	PCPuts("[pmc_pnto_####]	Panorama Normal Mode Trajectory Option \r\n (0: Teeth 11, 1: Fast Teeth 11, 2: Multi Layer, 3: Fast Multi Layer, 4: Bitewing, 5: Fast Bitewing) \r\n");
	PCPuts("[pmc_ptto_####]	pano TMJ Table select(0:noraml, 1:fast) \r\n");
	PCPuts("[pmc_pcsp_####]	Pano Standard Capture Start Table Number \r\n");
	PCPuts("[pmc_patn_####]	Pano Trigger Number \r\n");
	PCPuts("[pmc_ppps_####]	Pano Standard Prepare Angle [0.1 degree] \r\n");
	PCPuts("[pmm_tesn_####]	Pano TMJ Capture Start Table Number \r\n");
	PCPuts("[pmm_teen_####]	Pano TMJ Capture End Table Number \r\n");
	PCPuts("[pmc_tpps_####]	Pano TMJ Prepare Angle [0.1 degree] \r\n");
	PCPuts("[pmc_t2ps_####]	Pano TMJ2 Prepare Angle [0.1 degree] \r\n");
	PCPuts("[pmc_bwfg_####]	Panorama Bitewing Select (0: left, 1 : right, 2: both) \r\n");
	PCPuts("[pmc_cbve_####]	Canine Beam Apply (0: Disable, 1: Enable) \r\n");
	PCPuts("[pmc_cbv?]	Canine Beam Apply Request (0: Disable, 1: Enable) \r\n");
	PCPuts("[pmc_ppst_####]	Canine Beam Offset [0.1 mm] \r\n");
	PCPuts("[pmc_prea_####]	Panorama Rotator End Angle (0: 0, 1: 90, 2: 180, 3: None [degrees]) \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Cephalo Capture Parameters \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_pacp_####]	Ceph Pano Axis Offset Position [0.1 mm] \r\n");
	PCPuts("[pmc_roco_####]	Ceph Rotator Axis Offset Position (0 ~ 200)[degree] \r\n");
	PCPuts("[pmc_colo_#####]	Collimator Axis Lateral Offset Position [0.01 mm] \r\n");
	PCPuts("[pmc_copo_#####]	Collimator Axis PA Offset Position [0.01 mm] \r\n");
	PCPuts("[pmc_sdlo_####]	Ceph Detector Axis Lateral Offset Position [0.1 mm] \r\n");
	PCPuts("[pmc_sdpo_####]	Ceph Detector Axis PA Offset Position [0.1 mm] \r\n");
	PCPuts("[pmc_clcs_####]	Ceph Lateral Capture Start Table Number \r\n");
	PCPuts("[pmc_clce_####]	Ceph Lateral Capture End Table Number \r\n");
	PCPuts("[pmc_cpcs_####]	Ceph PA Capture Start Table Number \r\n");
	PCPuts("[pmc_cpce_####]	Ceph PA Capture End Table Number \r\n");
	PCPuts("[pmc_mult_####]	Ceph Table Select \r\n");
	PCPuts("[pmc_cetn_####]	Ceph Trigger Number\r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Generator, Detector, capture control   \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_swon]	Software Exposure Switch On \r\n");
	PCPuts("[pmc_swof]	Software Exposure Switch Off \r\n");
	PCPuts("[pmc_uok?]	System Status Request \r\n");
	PCPuts("[pmc_aok?]	Cephalo Detector Dark Image Acquire Complete (PC to Machine) \r\n");
	PCPuts("[pmc_prcl]	Ready Cancel \r\n");
	PCPuts("[pmc_aest]	(AEC)Auto Exposure Control \r\n");
	PCPuts("[pmc_dark_####]	Dark Frame Acquisition for (1: CT, 2: CT UFS, 10: Pano, 20: Ceph) \r\n");
	PCPuts("[pmc_datm_#####]	Dark Frame Acquisition Time \r\n");
	PCPuts("[pmc_brit_####]	Bright Frame Acquisition for (1: CT, 2: CT UFS, 10: Pano, 20: Ceph) \r\n");
	PCPuts("[pmc_brtm_#####]	Bright Frame Acquisition Time \r\n");
	PCPuts("[pmc_gtts]	Generator Time start \r\n");
	PCPuts("[pmc_gtte]	Generator Time end \r\n");
	PCPuts("[pmc_prio_####]	Detector Trigger Pulse Period [10 us] \r\n");
	PCPuts("[pmc_alex_#####]	Generator Expose ##### ms (Normal:[pmc_tunr][pmc_alex_#####], Pulse:[pmc_tupl][pmc_alex_#####]) \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Actuator, Laser, Lamp \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_acup]	Column Actuator Axis Up \r\n");
	PCPuts("[pmc_acdn]	Column Actuator Axis Down \r\n");
	PCPuts("[pmc_acsp]	Column Actuator Axis Stop \r\n");
	PCPuts("[pmc_lron]	Laser On \r\n");
	PCPuts("[pmc_lrof]	Laser Off \r\n");
	PCPuts("[pmc_lotm_####]	Laser On Time [sec] (default: 300) \r\n");
	PCPuts("[pmc_lmof]	Lamp Off \r\n");
	PCPuts("[pmc_relo]	Ready Lamp On \r\n");
	PCPuts("[pmc_exlo]	Exposure Lamp On \r\n");
	PCPuts("[pmc_erlo]	Error Lamp On \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Option \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_op?_]	Print current options \r\n");
	PCPuts("[pmc_ceop_####]	Cephalo Option (0: non ceph, 1: scan ceph) \r\n");
	PCPuts("[pmc_spop_####]	Voice Guidance Message Option (0: OFF, 1: KO = Korean, 2: EN = English, 3: ZH = Chinese) \r\n");
	PCPuts("[pmc_acop_####]	Actuator Option (2: PAN_CTRL Forward 2 SMPS, 3: PAN_CTRL Reverse 2 SMPS) \r\n");
	PCPuts("[pmc_geop_####]	Generator Option (0: Generator OFF, 1: Generator ON \r\n");
	PCPuts("[pmc_dbmd_####]	Debugging Mode Option (0: Debugging NO, 1: Debugging YES) \r\n");
	PCPuts("[pmc_svst_####]	Sound Volume Setting (30 ~ 130) \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Etc \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_frn?]	Firmware Revision Number Request \r\n");
	PCPuts("[pmc_rev?]	System Revision Number \r\n");
	PCPuts("[pmc_rad?]	Rotator Axis Read ADC \r\n");
	PCPuts("[pmc_pad?]	Pano Axis Read ADC \r\n");
	PCPuts("[pmc_help]	Print Commnad List \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Add \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmc_dap_]	Dap Value \r\n");
	PCPuts("[pmc_ctm?]	Dap Capture Time \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("	Mode Select Command \r\n");
	PCPuts("//------------------------------------------------------------- \r\n");
	PCPuts("[pmm_ct__]	CT Mode Apply \r\n");
	PCPuts("[pmm_pano]	Panorama Standard Mode Apply [0.1 mm] \r\n");
	PCPuts("[pmm_biwi]	Panorama Bitewing Mode Apply [0.1 mm] \r\n");
	PCPuts("[pmm_tmj1]	Panorama TMJ1 Mode Apply [0.1 mm] \r\n");
	PCPuts("[pmm_tmj2]	Panorama TMJ2 Mode Apply \r\n");
	PCPuts("[pmm_cepl]	Scan type Cephalo Lateral Mode Apply \r\n");
	PCPuts("[pmm_cepp]	Scan type Cephalo PA Mode Apply \r\n");
	PCPuts("[pmm_prec]	Ready \r\n");
	PCPuts(" \r\n");

	PCPuts("//------------------------------------------------------------- \r\n");
	return;
}


void Ctrl_DAP_VALUE(LONG value)		// [dap_]	// pc에서 전달 받은 값을 lcd로 전달
{
	// sw팀 테스트용 코드
	LCDPrintf("[tml_dap__%05ld] \r\n", value);
}
void Ctrl_DAP_CAPTURE_TIME(LONG value)	// [ctm?]	// 촬영 시간, 조사 시간, dap 값 pc전달
{
	// sw팀 테스트용 코드
	LCDPrintf("[tml_ctm?_%05ld] \r\n", value);
}

void Ctrl_SOUND_VOLUME_SET(LONG value)		// [svst]  // 초기값은 112
{
	LONG soundVolume;

	if(0 <= value && value <= 130)	soundVolume = value;
	else if(value > 130)	soundVolume = 130;
	else if(value < 0)		soundVolume = 0;
		
	MP3Printf("@V%02lx\r", soundVolume);	// 16진수 2자리로 입력
	MP3Puts("@P3WHELLO1\r");
}

/*===================================================================================*/ 
// System Option Message
/*===================================================================================*/
void Ctrl_EXHIBITION_OPTION(LONG value) 	// [exst]
{
	if(value < 0 || 1 < value)	value = 0;

	isOnExhibition = value;
	UartPrintf("isOnExhibition = %d \r\n", isOnExhibition);
}

void Ctrl_OPTION_PRINT(LONG value)		// [op?_] 
{
	// ro_end_tm_flag = 0;	// 기능적 연관성이 무엇인가?  ==> 없는 듯
	PrintOption();
}

// cephalo option (0: non-ceph, 1: scan-ceph)
// todo : non-ceph 선택시 ceph관련 루틴을 수행하지 않게끔 하면 좋겠다. 
//        현재는 SD축 org_completed flag만 셋되게 해놓은 상태 
void Ctrl_CEPH_OPTION(LONG value)		// [ceop]
{
	if(value < 0 || 2 < value)	value = 0;

	ceph_option = value;
	write_eeprom_BYTE(2, ceph_option);
	
	if(ceph_option == NONE_CEPH)			PCPuts("[rpm_ceop_0000]	non-cephalo \r\n");
	else if(ceph_option == SCAN_CEPH)	PCPuts("[rpm_ceop_0001]	scan type cephalo \r\n");
}


void Ctrl_CO_OPTION(LONG value) 	// [coop]
{
	if( (value < 0 || 1 < value) && (value != NONE_AXIS_CO) )	value = (LONG)ONE_AXIS_CO;
	co_option = (BYTE)value;
	
	write_eeprom_BYTE(28, co_option);

	if(co_option == ONE_AXIS_CO)			PCPuts("[rpm_coop_0000]	1-axis CO \r\n");
	else if(co_option == FOUR_AXIS_CO)		PCPuts("[rpm_coop_0001]	4-axis CO\r\n");
	else if(co_option == NONE_AXIS_CO)		PCPuts("[rpm_coop_0099]	none axis CO\r\n");
	else if(co_option == AXIS_ALL)			PCPuts("[rpm_coop_0002]	all axis CO\r\n");
}


	/*
		case CLEAR_OPTION:	// [clop]
			ro_end_tm_flag = 0;
			
			cmd_mode(CLEAR_OPTION,"pmm_clop");
		break;

		// case SYSTEM_OPTION:	// [syop]

		// 	if(value4 < 0 || 1 < value4)	value4 = DTR_rev5;
		// 	sys_optn = value4;
			
		// 	write_eeprom_BYTE(50, sys_optn);
			
		// 	if(sys_optn == DTR_rev5)			com4_puts("[rpm_syop_0000]	DENTRI rev5 \r\n");
		// 	else if(sys_optn == DTR_rev8)		com4_puts("[rpm_syop_0001]	DENTRI rev8 \r\n");	
		// break;



		case PA_OPTION:	// [paop]
			
			if( (value4 < 0 || 0 < value4) && (value4 != 99) )	value4 = (LONG)LEAD_2_PA;
			pa_option = (BYTE)value4;
			
			write_eeprom_BYTE(42, pa_option);
			
			if(pa_option == LEAD_2_PA)				com4_puts("[rpm_paop_0000]	Lead 2 mm \r\n");
		break;

		case RO_OPTION:	// [roop]
			
			if(value4 < 0 || 0 < value4)	value4 = (LONG)PAN_CTRL_RO_ADC;
			ro_option = (BYTE)value4;
			
			write_eeprom_BYTE(36, ro_option);
			
			if(ro_option == PAN_CTRL_RO_ADC)	com4_puts("[rpm_roop_0001]	PAN_CTRL Rotator ADC \r\n");
		break;


	*/

/*===================================================================================*/	
// Test and Debugging Protocol
/*===================================================================================*/	
void Ctrl_DEBUGGING_MODE(LONG value)
{
	if(value < 0 || 1 < value)	value = 0;

	debug_mode = (BYTE)value;
	write_eeprom_BYTE(4, debug_mode);
	
	if(debug_mode == 0)			PCPuts("[rpm_dbmd_0000]	NO \r\n");
	else if(debug_mode == 1)	PCPuts("[rpm_dbmd_0001]	YES \r\n");
}




void Ctrl_SD_POSITION_CNT(LONG value)
{
	rcan_puts(SD_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg); dly_10us(70);
}

void Ctrl_RO_SET_ORIGIN(LONG value)
{
	gvRO.stop_flag = NO;
	resetOrigin_RO = ON;	
	gvRO.org_complete_flag = NO;
	mRO.org_complete_flag = NO;
}


void Ctrl_PA_SET_ORIGIN(LONG value)
{
	gvPA.stop_flag = NO;
	resetOrigin_PA = ON;	
	gvPA.org_complete_flag = NO;
	mPA.org_complete_flag = NO;
}

void Ctrl_RO_READ_ADC(LONG value)	// [rad?]
{
	WORD canAdcValue;

	ReadCanAdc_RO(&canAdcValue);
	PCPrintf("[rpm_rad?_%4u]\r\n", canAdcValue);

}

void Ctrl_PA_READ_ADC(LONG value)	// [pad?]
{
	WORD canAdcValue;

	ReadCanAdc_PA(&canAdcValue);
	PCPrintf("[rpm_pad?_%4u]\r\n", canAdcValue);

}

/*===================================================================================*/
// Exposure Lamp Control
/*===================================================================================*/	
void Ctrl_LAMP_OFF(LONG value)	//[lmof]
{
	lamp_off();
}

void Ctrl_RDY_LAMP_ON(LONG value)	//[relo]
{
	rdy_lamp_on();
}

void Ctrl_EXP_LAMP_ON(LONG value)	//[exlo]
{
	exp_lamp_on();
}

void Ctrl_ERR_LAMP_ON(LONG value)	//[erlo]
{
	err_lamp_on();
}


/*===================================================================================*/
// AEC(Auto Exposure Control)
/*===================================================================================*/
void Ctrl_AEC_SET(LONG value) 	//[aest]
{
	isOnAEC = (int)value;
	PCPrintf("[rpm_aest_%d]\r\n", isOnAEC);
}

/*===================================================================================*/	
//	CT SCOUT
/*===================================================================================*/	
void Ctrl_SCOUT(LONG value)		// [scut]
{
	
	// bug : 장비 초기화에서 CT_MODE apply안된 상태에서 SCOUT_mode로 갈수 있음
	// ==> CT_MODE apply 후 CT_SCOUT_MODE 셋팅 될 수 있게할 것!!
	if(main_mode == CT_MODE)
	{
		Msg_kVmA();

		// if( (kv_value) && (ma_value) )
		{
			scout_mode = YES;

			main_mode = CT_SCOUT_MODE;
			PCPuts("[rpm_scut] \r\n");
		}
		// else PCPuts("kV,mA first! \r\n");
	}
	else PCPuts("Caution! CT mode first!\r\n");
}

void Ctrl_SCTM(LONG value)	// [sctm] //지정 각도에서 스카웃 촬영하는 시간을 ms 단위로 지정
{
	LONG scout_time;	// 덴트리 코드에서 전역으로 안쓰임

	if(main_mode == CT_MODE || CT_SCOUT_MODE)
	{
		scout_time = value;
		
		// scout_pulse는 스카웃 촬영 시간에 해당하는 펄스 수 
		// RO 한바퀴 도는데 필요한 펄스수(=CT촬영 펄스수) RO_360_DEGREE_PULSE, 한 바퀴 도는데 필요한 시간 8초
		// 1000은 어떤 의미? ==> scout_time [단위 ms] ms를 하기위해 나누기 1000
		scout_pulse = (LONG)( RO_360_DEGREE_PULSE/8 * scout_time / 1000);
		
		PCPrintf("[rpm_sctm_%04ld] \r\n", scout_time);
	}
	else
		PCPuts("Caution! Select CT-SCOUT option first!\r\n");
}


/*===================================================================================*/
// Capture Start(Ring ON) & Stop(Ring OFF) Table Number
/*===================================================================================*/
// Mode 명령어 보다는 Cmd 명령어인데... 어째서..??  
// ==> Qface에서는 [pmc_tesn] 형태로 있음. pmm없음
void Ctrl_PANO_TMJ_CAPT_START(LONG value)		//[tesn]
{
	if(value >= 20)	pano_tmj_capt_start_table_num = value;
	else			pano_tmj_capt_start_table_num = 20;
	
	PCPrintf("[rpm_tesn_%04ld] \r\n",pano_tmj_capt_start_table_num);
}
		
void Ctrl_PANO_TMJ_CAPT_END(LONG value)			//[teen]
{
	if(value >= 270)	pano_tmj_capt_end_table_num = 270;
	else				pano_tmj_capt_end_table_num = value;
	
	PCPrintf("[rpm_teen_%04ld] \r\n",pano_tmj_capt_end_table_num);
}




/*===================================================================================*/
// Alignment
/*===================================================================================*/			
void Ctrl_ALIGN_EXPOSE(LONG value)	// [alex]	// 디텍터는 인터널모드로 설정한다고 가정. 오로지 조사만 되는 명령
{
	// ecoX사용법 [pmc_tupl][pmc_alex_1200]  => pulse, normal 선택 명령을 먼저하고 사용.

    align_acq = ALIGN_EXPOSE;
	if( (kv_value == 0) || (ma_value == 0) )	PCPuts("kV,mA first! \r\n");
	else
	{
		if(value > 25000)	PCPuts("Exposure time too long! (1 ms ~ 25000 ms) \r\n");	// POSKOM 제네레이터의 최대 조사 가능 시간은 25 초
		else if(value <= 0)	align_exp_time_10u = 0;	// 조사하다가 멈출 필요가 있을 때는 0 이하의 값을 입력해서 조사 종료
		else
		{
			alex_val = value;
			
			ro_end_tm_flag = 0;
			
			// cmd_mode_step = STEP1;

			isModeCompleted = CmdMode(ALIGN_EXPOSE, alex_val);
			// sprintf(string,"pmm_alex_%05ld%c", alex_val, rd[14]);	cmd_mode(ALIGN_EXPOSE, string);
		}
	}
}



/*===================================================================================*/ 
// System Option Message
/*===================================================================================*/


/*===================================================================================*/
// Canine Beam Offset	(견치빔을 적용할 경우에만 반영)
/*===================================================================================*/
void Ctrl_CANINE_BEAM_VALUE_ENABLE(LONG value)	// [cbve]
{
	isOnCanineBeam = value;
	
	PCPrintf("[rpm_cbve_%04d] \r\n",isOnCanineBeam);

	if(isOnCanineBeam)                            // canine beam이 ON이고
    {
    	Ctrl(LASER_ON, (LONG) 0);
    	LaserCanine(ON);
        // 덴트리에서 FRONT_LR 기능이 이해가 안감. ==> 일단 eco x에서는 사용 안함
        // if(FRONT_LR == ON) Ctrl(LASER_ON, (LONG) 0);   // LASER가 ON이면 LASER 한번 더 ON
    }
}

void Ctrl_CANINE_BEAM_VALUE_REQ(LONG value)	// [cbv?]
{
	PCPrintf("[rpm_cbv?_%04d] \r\n",isOnCanineBeam);
}



void Ctrl_PX_PANO_SET(LONG value)		// 0.1 mm 단위
{
	LONG temp;
	const LONG crashDistance = 1083;

	//-------------------------------------------------------------------------------------
	// ppst 값의 개념 : 견치빔 위치 + ppst  =  pamv 값!!
	// todo : 위 개념으로 코드 정리 필요, Set_Canine_Values() 함수도 함께 고려
	
	
	// ppst 양의 최대값 = P축 정방향 최대 이동거리 - 견치빔 최대 이동거리 - 파노노말 P축 이동거리 - 기구공차
	// ppst 음의 최소값 = - P축 역방향 최대 이동거리 - 견치빔 최소 이동거리 - 기구공차
	//
	// P축 정방향 최대 이동거리 = pa_p_max_pulse / pa_0_1_unit_pulse (= 384000 / 320 = 1200)
	// 견치빔 최대 이동거리 = (60000 - 4950) / 165 = 333.63 ==> 33.3 mm
	// 파노노말 P축 이동거리 = 600 (60 mm 이하)
	// 기구공차 = 50 (5 mm 이하)
	// P축 역방향 최대 이동거리 = pa_n_max_pulse / pa_0_1_unit_pulse (= 32000 / 320 = 100)
	// 견치빔 최소 이동거리 = (4950 - 4950) / 165 = 0 mm
	// 
	// ppst 양의 최대값 <= (pa_p_max_pulse / pa_0_1_unit_pulse) - 333 - 600 - 50
	// 					<= (pa_p_max_pulse / pa_0_1_unit_pulse) - 983
	// 					<= 1200 - 983 = 217
	// 
	// ppst 음의 최소값 >= - P축 역방향 최대 이동거리 - 0 - 50
	// 					>= - (pa_n_max_pulse / pa_0_1_unit_pulse) - 50
	// 					>= - 100 - 50 = -150


	// eco X의 경우 gvPA.p_max_pulse 의 PA축 최대이동거리 대신에 
	// 컬럼부 충돌되는 거리값을 적용해야 함. crashDistance 
	// 일단 어림값으로 Panomode_paxis_value 제한 값이 100이 되도록 정함. 
	if(value >= 0)
	{
		// temp = (gvPA.p_max_pulse / gvPA.unit_0_1_pulse) - 983;
		temp = crashDistance - 983;
		if(value <= temp)		Panomode_paxis_value = value;
		else if(value > temp)	Panomode_paxis_value = temp;
	}
	else if (value < 0)
	{
		Panomode_paxis_value = value;

	/*
		// 아래 덴트리코드를 할 경우 ppst의 값이 '-250'을 넘어갈 경우
		// Panomode_paxis_value 값이 바뀌어 터미널 숫자들이 맞지 않는 현상 

		// 숫자 '50'은 기구공차 ==> 기구공차를 의미하는게 아닌드...
		// temp = - (gvPA.n_max_pulse / gvPA.unit_0_1_pulse) - 50;
		if(value >= temp)
		{
		UartPrintf("==> pAX - 3\r\n");
			Panomode_paxis_value = value;
		}	
		else if(value < temp)
		{
		UartPrintf("==> pAX - 4\r\n");
			Panomode_paxis_value = temp;
		}	
	*/

	}
	
	PCPrintf("[rpm_ppst_finish]	[rpm_ppst_%04ld] \r\n",Panomode_paxis_value);
}

/*===================================================================================*/
// Rotator Prepare Angle
/*===================================================================================*/
void Ctrl_PANO_STAN_PREC_ANGLE(LONG value)	// [ppps] 0.1 도 단위
{
	pano_stan_prec_angle = value;

	PCPrintf("[rpm_ppps]	[rpm_ppps_%04ld] \r\n", pano_stan_prec_angle);
}
		
void Ctrl_PANO_TMJ_PREC_ANGLE(LONG value)	// [tpps]
{
	pano_tmj_prec_angle = value;
	
	PCPrintf("[rpm_tpps]	[rpm_tpps_%04ld] \r\n", pano_tmj_prec_angle);
}

void Ctrl_PANO_TMJ2_PREC_ANGLE(LONG value)
{
	pano_tmj2_prec_angle = value;
	
	PCPrintf("[rpm_tpps]	[rpm_t2ps_%04ld] \r\n", pano_tmj2_prec_angle);
}

// 어떤 기능을 하는가? RO_CEPH_OFFSET_POS()함수와는 뭐가 다른가?
void Ctrl_RO_CEPH_OFFSET_POSITION(LONG value)
{
	if( ( 0 <= value) && (value <= 200) )	gvRO.ceph_offset_pos = value;
	else									gvRO.ceph_offset_pos = 0;
	
	PCPrintf("[rpm_roco_%04ld] \r\n",gvRO.ceph_offset_pos);
}


/*===================================================================================*/
// Scan Ceph Detector Offset Position - SD	[0.1 mm]
/*===================================================================================*/
void Ctrl_SD_SCLA_OFFSET_POS(LONG value)
{
	gvSD.scla_offset_pos = value;
	PCPrintf("[rpm_sdlo_%04ld] \r\n",gvSD.scla_offset_pos);
}

void Ctrl_SD_SCPA_OFFSET_POS(LONG value)
{
	gvSD.scpa_offset_pos = value;
	PCPrintf("[rpm_sdpo_%04ld] \r\n",gvSD.scpa_offset_pos);
}
/*===================================================================================*/
// 1 Axis Collimator Offset Position - CO	[0.01 mm]
/*===================================================================================*/
void Ctrl_CO_SCLA_OFFSET_POS(LONG value)
{
	gvCO.scla_offset_pos = value;
	PCPrintf("[rpm_colo_%05ld] \r\n",gvCO.scla_offset_pos);
}

void Ctrl_CO_SCPA_OFFSET_POS(LONG value)
{
	gvCO.scpa_offset_pos = value;
	PCPrintf("[rpm_copo_%05ld] \r\n",gvCO.scpa_offset_pos);
}

/*===================================================================================*/
// Capture Start(Ring ON) & Stop(Ring OFF) Table Number
/*===================================================================================*/
// todo renaming ; normal, standard 용어 통일이 필요. pano standard, pano normal.. ==> standard로 하는게 좋을듯
void Ctrl_PANO_STAN_CAPT_START(LONG value)		//[pcsp]
{
	if(value >= 11)	pano_stan_capt_start_table_num = value;
	else				pano_stan_capt_start_table_num = 11;
	
	PCPrintf("[rpm_pcsp_%04ld]\r\n", pano_stan_capt_start_table_num);
}


void Ctrl_PANO_TMJ_TBL_OPTION(LONG value)	// [ptto] : pano TMJ Table select(0:noraml, 1:fast)
{
    if(value < 0 || 1 < value)	value = 0;
    pano_tmj_tbl_optn = (BYTE)value;


    switch(pano_tmj_tbl_optn)
    {
        case 0: PCPuts("[rpm_ptto_0000]	Trajectory 0 : TMJ Normal\r\n");	break;
        case 1: PCPuts("[rpm_ptto_0001]	Trajectory 1 : TMJ Fast \r\n");		break;
        default:PCPuts("Undefined Trajectory!\r\n");	break;
    }

}
	

void Ctrl_PANO_NOR_TBL_OPTION(LONG value)	// [pnto] : pano normal Table select
{
    if(value < 0 || 5 < value)	value = 1;
    pano_nor_tbl_optn = (BYTE)value;

    write_eeprom_BYTE(16, pano_nor_tbl_optn);

    switch(pano_nor_tbl_optn)
    {
        case 0: PCPuts("[rpm_pnto_0000]	Trajectory 0 : Teeth 11\r\n");          pano_tbl_no = PANO_T47_TBL;    	break;
        case 1: PCPuts("[rpm_pnto_0001]	Trajectory 1 : Fast Teeth 11\r\n");     pano_tbl_no = PANO_T47_TBL;  	break;
        case 2: PCPuts("[rpm_pnto_0002]	Trajectory 2 : Multi Layer \r\n");		pano_tbl_no = PANO_T47_TBL;     break;
        case 3: PCPuts("[rpm_pnto_0003]	Trajectory 3 : Fast Multi Layer \r\n");	pano_tbl_no = PANO_T47_TBL;     break;
        case 4: PCPuts("[rpm_pnto_0004]	Trajectory 4 : Bitewing \r\n");     	pano_tbl_no = PANO_STAN_TBL;    break;
        case 5: PCPuts("[rpm_pnto_0005]	Trajectory 5 : Fast Bitewing \r\n");    pano_tbl_no = PANO_STAN_TBL;    break;
        default:PCPuts("Undefined Trajectory!\r\n");	break;
    }

}
	

/*		
		case PANO_TMJ_CAPT_START:	// [tesn]
			pano_tmj_capt_start_table_num = value4;
			sprintf(str, "tesn_%04ld]\r\n", pano_tmj_capt_start_table_num); select_LCDorPC(lcd_flag, str);
		break;
*/

void Ctrl_CEPH_TRIGGER_NUMBER(LONG value)	// [cetn]
{
	ceph_trig_num = (WORD)value;
	PCPrintf("[rpm_cetn_%04d] \r\n", ceph_trig_num);
}

void Ctrl_CEPH_LA_CAPT_START(LONG value)	// [clcs]
{
	s_ceph_la_capt_start_tbl_num = value;
	PCPrintf("[rpm_clcs_%04ld] \r\n",s_ceph_la_capt_start_tbl_num);
}
		
void Ctrl_CEPH_LA_CAPT_END(LONG value)
{
	PCPuts("Not yet coding\r\n");
	// s_ceph_la_capt_end_tbl_num = value;
	// PCPrintf("[rpm_clce_%04ld] \r\n",s_ceph_la_capt_end_tbl_num);
}
		
void Ctrl_CEPH_PA_CAPT_START(LONG value)	// [cpcs]
{
	s_ceph_pa_capt_start_tbl_num = value;
	PCPrintf("[rpm_cpcs_%04ld] \r\n",s_ceph_pa_capt_start_tbl_num);
}
		
void Ctrl_CEPH_PA_CAPT_END(LONG value)
{
	PCPuts("Not yet coding\r\n");
	// s_ceph_pa_capt_end_tbl_num = value;
	// PCPrintf("[rpm_cpce_%04ld] \r\n",s_ceph_pa_capt_end_tbl_num);
}


/*===================================================================================*/
// pmc_mult_0003 : scan normal speed, 0004 : fast speed
// pmc_aok? --> dark frame off
/*===================================================================================*/
void Ctrl_MULT_VALUE(LONG value)
{
	static LONG	mult_val = 3;

	if( (3 <= value) && (value <= 4) )	mult_val = value;
	else 									mult_val = 3;
	
	switch(mult_val)
	{
		case 3:	// normal
			scan_ceph_la_det_trig_table_num	= S_CEPH_LA_DET_TRIG_4B_N;
			scan_ceph_pa_det_trig_table_num	= S_CEPH_PA_DET_TRIG_4B_N;
			
			// CO axis, 32분주, lead 2 mm
			sc_tbl_sel_1 = 0;	sc_tbl_sel_2 = 0;	break;
		break;
		
		case 4:	// fast
			scan_ceph_la_det_trig_table_num	= S_CEPH_LA_DET_TRIG_4B_F;
			scan_ceph_pa_det_trig_table_num	= S_CEPH_PA_DET_TRIG_4B_F;

			// CO axis, 32분주, lead 2 mm
			sc_tbl_sel_1 = 1;	sc_tbl_sel_2 = 1;	break;
	}
	
	PCPrintf("[rpm_mult_%04ld] \r\n", mult_val);
}

void Ctrl_CAPT_SW_APPLY_CPLT(LONG value)
{
	switch(main_mode)
	{
		case SCAN_CEPH_LA_MODE:
		case SCAN_CEPH_PA_MODE:
			aok_flag = (BYTE)value;
			
			Ctrl(DARK_FRAME_ACQ, (LONG) FRAME_OFF);  // pmc_dark_off
			
			if(isModeCompleted == YES)	{	PCPrintf("[rpm_aok?_%04d] \r\n",aok_flag);	}
		break;
		
		default:	PCPuts("Undefined main mode! \r\n");	break;
	}
	
}

/*===================================================================================*/	
// Generator Trig Control (for Viewworks detector)
/*===================================================================================*/	
void Ctrl_GEN_TRIG_TIME_START(LONG value)
{
	gen_trig_start_time = value;			
	PCPrintf("[rpm_gtts_%04ld]\r\n",gen_trig_start_time);
}


void Ctrl_GEN_TRIG_TIME_END(LONG value)
{
	LONG detTrigPeriod;


	detTrigPeriod = 100000 / ct_capt_fps;

	if(value > detTrigPeriod)
	{
		PCPrintf("warning: gtte(%04ld) must be less than or equal to detTrigPeriod(%04ld)!\r\n", 
					value, detTrigPeriod);	
		gen_trig_end_time = detTrigPeriod;		// 2000->0150
	}
	else gen_trig_end_time = value;

	PCPrintf("[rpm_gtte_%04ld]\r\n",gen_trig_end_time);	
	PCPrintf("gen_period : %04ld,  gen_on_time : %04ld ~ %04ld\r\n", 
				detTrigPeriod, gen_trig_start_time ,gen_trig_end_time);	
}

/*===================================================================================*/	
// Control Column(Acturator)
/*===================================================================================*/	
void Ctrl_ACTUATOR_UPUP(LONG value)	// [acup]
{
	LONG temp;

	switch(ac_option)
	{
		case PAN_CTRL_FWD_AC2:	COLUMN_UP = HIGH;	COLUMN_DN = LOW;	temp = 5; break;
		case PAN_CTRL_REV_AC2:	COLUMN_UP = LOW;	COLUMN_DN = HIGH;	temp = 6; break;
		default:	temp = 8; break;
	}
	PCPrintf("[rpm_acup],%ld \r\n", temp);
}

void Ctrl_ACTUATOR_DOWN(LONG value)
{
	LONG temp;
	
	switch(ac_option)
	{
		case PAN_CTRL_FWD_AC2:	COLUMN_UP = LOW;	COLUMN_DN = HIGH;	temp = 5; break;
		case PAN_CTRL_REV_AC2:	COLUMN_UP = HIGH;	COLUMN_DN = LOW;	temp = 6; break;
		default:	temp = 8;	break;
	}
	PCPrintf("[rpm_acdn],%ld \r\n", temp);
}

void Ctrl_ACTUATOR_STOP(LONG value)
{
	LONG temp;

	switch(ac_option)
	{
		case PAN_CTRL_FWD_AC2:
		case PAN_CTRL_REV_AC2:	COLUMN_UP = HIGH;	COLUMN_DN = HIGH; temp = 1; break;
		default:	temp = 3;	break;
	}
	PCPrintf("[rpm_acsp],%ld \r\n", temp);
}

// todo naming : detector period
void Ctrl_PULSE_PERIOD(LONG value)		// [prio] pano pulse period	[10 us 단위]
{
	pulse2_period = value;
	
	PCPrintf("[rpm_prio2_set]	[rpm_prio_%04ld]\r\n", value);
}

/*===================================================================================*/
// Pano Axis Offset Position - PA	[0.1 mm]
/*===================================================================================*/
void Ctrl_PA_CT_OFFSET_POS(LONG value)
{
	gvPA.ct_offset_pos = value;
	
	PCPrintf("[rpm_pact_%04ld] \r\n", gvPA.ct_offset_pos);
}
	
	
void Ctrl_PA_PANO_OFFSET_POS(LONG value)	// [papa]
{
		gvPA.pano_offset_pos = value;
		
		PCPrintf("[rpm_papa_%04ld] \r\n", gvPA.pano_offset_pos);
}
	
	
void Ctrl_PA_CEPH_OFFSET_POS(LONG value)	// [pacp]
{
		gvPA.ceph_offset_pos = value;
		
		PCPrintf("[rpm_pacp_%04ld] \r\n", gvPA.ceph_offset_pos);
}

/*===================================================================================*/
// Rotator Axis Offset Position - RO	[0.1 degree]
/*===================================================================================*/
void Ctrl_RO_CT_OFFSET_POS(LONG value)	// [roct]
{
	gvRO.ct_offset_pos = value;
	
	PCPrintf("[rpm_roct_%04ld] \r\n", gvRO.ct_offset_pos);
}
	
/*	
	case RO_PANO_OFFSET_POS:	// [ropa]
		gvRO.pano_offset_pos = value;
		
		PCPrintf("[rpm_ropa_%04ld] \r\n", gvRO.pano_offset_pos);
	break;
	
	case RO_CEPH_OFFSET_POS:	// [rocp]
		gvRO.ceph_offset_pos = value;
		
		PCPrintf("[rpm_rocp_%04ld] \r\n", gvRO.ceph_offset_pos);
	break;
*/
/*===================================================================================*/
// 1 Axis Collimator Offset Position - CO	[0.01 mm]
/*===================================================================================*/
void Ctrl_CO_CT_OFFSET_POS(LONG value)
{
	gvCO.ct_offset_pos = value;
	
	PCPrintf("[rpm_coct_%05ld] \r\n",gvCO.ct_offset_pos);
}
		
void Ctrl_CO_PANO_OFFSET_POS(LONG value)
{
	gvCO.pano_offset_pos = value;
	
	PCPrintf("[rpm_copa_%05ld] \r\n",gvCO.pano_offset_pos);
}


/*===================================================================================*/
// Acquire Images
/*===================================================================================*/
// todo renaming : SCAN_FRAME, SCAN_TOTAL_FRAME...
void Ctrl_SCFR(LONG value)			// [scfr]
{
    ct_capt_frame = value;
    PCPrintf("[rpm_scfr_%04ld] \r\n", ct_capt_frame);
}

void Ctrl_CT_CAPT_FPS(LONG value)	// [cfps]
{
    ct_capt_fps = value;

    if(value > 50)		ct_capt_fps = 50;
    else if(value < 0)	ct_capt_fps = 0;

    PCPrintf("[rpm_cfps_%04ld] \r\n", ct_capt_fps);
}

void Ctrl_PANO_TRIGGER_NUMBER(LONG value)   // [patn]
{
    pano_trig_num = (WORD)value;
    PCPrintf("[rpm_patn_%04d] \r\n", pano_trig_num);
}

void Ctrl_SELECT_BITEWING(LONG value)	// [bwfg]
{
	bitewing_flag = (BYTE)value;		// 0: left, 1 : right, 2: both
	PCPrintf("[rpm_bwfg_%04d]\r\n", bitewing_flag);
}
	
//===================================================================================	
// 
//===================================================================================			
void Ctrl_SYSTEM_INIT_COMPLETE(LONG value)	// [uok?]  의미 : you ok??
{
	// 시스템을 초기화화기 위한 루틴으로 작성되었다가, 
	// 그 기능은 없어지고 상태를 확인하는 기능으로 변경된 듯함. 
	// todo rename : system status

	ro_end_tm_flag = 0;
	
	if( first_run || (!isInitializedMachine) )	// 전원 켜고 초기화 진행 중인 상태
	{
		if(!gvPA.org_complete_flag)	PCPuts("[rpm_uok?_0000] setting... PA origin\r\n");
		if(!gvRO.org_complete_flag)	PCPuts("[rpm_uok?_0000] setting... RO origin\r\n");
		if(!gvCO.org_complete_flag)	PCPuts("[rpm_uok?_0000] setting... CO origin\r\n");
		if(!gvSD.org_complete_flag)	PCPuts("[rpm_uok?_0000] setting... SD origin\r\n");
	}
	else if(isInitializedMachine)	// 초기화 정상 완료된 경우
	{
		if(exp_push == YES)				PCPuts("[rpm_uok?_0002] capturing... \r\n");	// 촬영 진행 중
		else if(isModeCompleted == YES)	PCPuts("[rpm_uok?_0001] standing by...\r\n");	// 대기 상태
		else if(isModeCompleted == NO)	
		{
			// todo : isModeComplete가 NO인 상태에서는 [pmc_uok?] 명령이 안먹힌다. 
			// pmm 진행중에 다른 명령어 안 먹힘
			switch(main_mode)
			{
				case CT_MODE:				PCPuts("[rpm_uok?_0011]	preparing for CT \r\n");	break;
				case PANO_STAN_MODE:		PCPuts("[rpm_uok?_0021]	preparing for Pano \r\n");	break;
				case PANO_TMJ1_MODE:		PCPuts("[rpm_uok?_0022]	preparing for TMJ1 \r\n");	break;
				case PANO_TMJ2_MODE:		PCPuts("[rpm_uok?_0023]	preparing for TMJ2 \r\n");	break;
				case SCAN_CEPH_LA_MODE:		PCPuts("[rpm_uok?_0041]	preparing for Scan Ceph LA \r\n");	break;
				case SCAN_CEPH_PA_MODE:		PCPuts("[rpm_uok?_0042]	preparing for Scan Ceph PA \r\n");	break;
				
				default:	PCPuts("Undefined main mode! \r\n");
			}
		}
	}

		
}


/*===================================================================================*/	
// PREC_CANCLE
/*===================================================================================*/	
void Ctrl_PREC_CANCEL(LONG value)
{
	// closing 중에 prcl이 들어오면 closing 루틴을 빠져나가서 오루빌생
	// closing 중에는 이 명령 안먹게 함
	if(isCaptureClosing == YES)
	{
		PCPuts("Warning : Capture is Closing. Not use [prcl]!!\r\n");
		 return;
	}	

	exp_ok_flag = NO;	scout_mode = NO;	exp_push = NO;
				
	gen_off();	ct_pano_det_off();	s_ceph_det_off();
	ring_off();	LaserOff();
	if(!gen_err_flag)	lamp_off();

	// 사용용도는?? 필요한게 판명되면 main.c 에서 isModeCompleted를 전역변수로 해서 대체야하나??
	// cmd_mode_step = STEP1; cmd_mode_flag = NO;  
	
	PCPuts("[rpm_prcl] \r\n\r\n");
}

void Ctrl_FIRMWARE_REVISION_NUM(LONG value)
{
	ro_end_tm_flag = 0;		// 왜 이곳에서 사용되나? capture.c에서 이용되는 변수
	
	isModeCompleted = CmdMode(FIRMWARE_REVISION_NUM, (LONG)0);

	// PCPrintf("\r\nMaster	= %s \r\n",REVISION_NUM);
	// rcan_puts(CO_MOTOR, CAN_FW_REV_NUM, dmy_val, dmy_msg);	
	// rcan_puts(SD_MOTOR, CAN_FW_REV_NUM, dmy_val, dmy_msg);
	// PCPuts("Generator	= WDG90 \r\n");
	// PCPuts("[rpm_frn?] \r\n\r\n");	
}

void Ctrl_SYSTEM_REVISION_NUM(LONG value)
{
	PCPuts("[rpm_rev1] \r\n");
}

/*===================================================================================*/
// Software Exposure Switch Message
/*===================================================================================*/
void Ctrl_SOFTWARE_EXP_ON(LONG value)
{
	software_exp = ON;
	if(exp_ok_flag == NO)	PCPuts("[rpm_swon]\r\n");
}

void Ctrl_SOFTWARE_EXP_OFF(LONG value)
{
	software_exp = OFF;	exp_ok_flag = NO;
	if(exp_ok_flag == NO)	PCPuts("[rpm_swof]\r\n");
}	




/*===================================================================================*/ 
// System Option Message
/*===================================================================================*/

void Ctrl_CT_RO_END_ANGLE(LONG value)	// [crea]	// 0: 0 degrees, 1: 360 degrees
{
	if(value < 0 || 5 < value)	value = 0;	// default 360 degrees
	ct_ro_end_angle = (BYTE)value;
	
	write_eeprom_BYTE(22, ct_ro_end_angle);
	
	if(ct_ro_end_angle == 0)		PCPuts("[rpm_crea_0000]	0 degrees \r\n");
	else if(ct_ro_end_angle == 1)	PCPuts("[rpm_crea_0001]	90 degrees \r\n");
	else if(ct_ro_end_angle == 2)	PCPuts("[rpm_crea_0002]	180 degrees \r\n");
	else if(ct_ro_end_angle == 3)	PCPuts("[rpm_crea_0003]	270 degrees \r\n");
	else if(ct_ro_end_angle == 4)	PCPuts("[rpm_crea_0004]	360 degrees \r\n");
	else if(ct_ro_end_angle == 5)	PCPuts("[rpm_crea_0005]	None \r\n");
}
	

void Ctrl_CT_RO_END_TIME(LONG value)	// [cret]	// 0 s ~ 120 s
{
	if(value < 0 || 120 < value)	value = 0;	// default 0 s
	ct_ro_end_tm = (BYTE)value;		ct_ro_end_tm_5ms = ct_ro_end_tm * 200;

	write_eeprom_BYTE(46, ct_ro_end_tm);

	PCPrintf("[rpm_cret_%04d] \r\n", ct_ro_end_tm);
}


void Ctrl_PANO_RO_END_ANGLE(LONG value)	// [prea]
{
	if(value < 0 || 3 < value)	value = 2;	// default 180 degrees
	pano_ro_end_angle = (BYTE)value;

	write_eeprom_BYTE(14, pano_ro_end_angle);

	if(pano_ro_end_angle == 0)		PCPuts("[rpm_prea_0000]	0 degrees \r\n");
	else if(pano_ro_end_angle == 1)	PCPuts("[rpm_prea_0001]	90 degrees \r\n");
	else if(pano_ro_end_angle == 2)	PCPuts("[rpm_prea_0002]	180 degrees \r\n");
	else if(pano_ro_end_angle == 3)	PCPuts("[rpm_prea_0003]	None \r\n");
}

/*===================================================================================*/
// Laser Control
/*===================================================================================*/	
void Ctrl_LASER_ON(LONG value)	// [lron]
{
	LaserOn();
}

void Ctrl_LASER_OFF(LONG value)	// [lrof]
{
	LaserOff();
}

void Ctrl_LASER_ON_TIME(LONG value)		// [lotm]
{
	if(value < 0 || 90 < value)	value = 90;	// default 90 s
	laser_on_time = (BYTE)value;	laser_on_time_5m = laser_on_time * 200;
	
	write_eeprom_BYTE(60, laser_on_time);
	
	PCPrintf("[rpm_lotm_%04ld] \r\n", value);
}



/*===================================================================================*/
// Dark & Bright Frame Acquisition Message
/*===================================================================================*/
// todo rename : frameMode -> value 함수 입력매개변수 통일을 위해 
void Ctrl_DARK_FRAME_ACQ(LONG frameMode)
{
	LONG detPeriodCnt = 0;

		
	align_acq = DARK_FRAME_ACQ;
	if( frameMode == FRAME_CT )  // '1' : ct mode
	{
        detPeriodCnt = 100000 / ct_capt_fps;  // fps에 따라 정해질 수 있도록 
        SetTriggerPulse_CT(detPeriodCnt);

		trig_pulse_time	= dark_time*100;	// 4 s
		align_exp_time_10u = trig_pulse_time - 200; // 2ms 이전
		
		ct_pano_det_capt();	align_exp_time_flag	= YES;
	}
	else if(frameMode == FRAME_UFS)
	{
        detPeriodCnt = 100000 / ct_capt_fps;  // fps에 따라 정해질 수 있도록 
        SetTriggerPulse_CT(detPeriodCnt);

		trig_pulse_time	= dark_time*100;	// 4 s
		align_exp_time_10u = trig_pulse_time - 200; // 2ms 이전
		
		ct_pano_det_capt();	align_exp_time_flag	= YES;
	}
	else if(frameMode == FRAME_PANO)
	{
        pulse2_period = 500;    // 100000 / 500 = 200

		trig_pulse_setting(FRAME_PANO);	trig_pulse_time	= dark_time*100;	// 4 s
		align_exp_time_10u = trig_pulse_time - 200; // 2ms 이전
		
		ct_pano_det_capt();	align_exp_time_flag	= YES;
	}
	else if(frameMode == FRAME_CEPH)
	{
		pulse2_period = 500;	trig_pulse_setting(FRAME_CEPH);
		s_ceph_det_exp_capt();
		
		trig_pulse_time	= dark_time*100;	// 4 s
		align_exp_time_10u = trig_pulse_time - 200; // 2ms 이전
		align_exp_time_flag	= YES;
	}
	else if(frameMode == FRAME_OFF)	// trigger test용
	{
		ct_pano_det_off();
		s_ceph_det_off();
	}
}


void Ctrl_DARK_ACQ_TIME(LONG value)
{
    dark_time = value;

    PCPrintf("[rpm_datm_%05ld] \r\n", dark_time);
}


void Ctrl_BRIGHT_ACQ_TIME(LONG value)
{
    brit_time = value;

    PCPrintf("[rpm_brtm_%05ld] \r\n", brit_time);
}




void Ctrl_BRIGHT_FRAME_ACQ(LONG frameMode)
{

    align_acq = BRIGHT_FRAME_ACQ;
	if( (kv_value == 0) || (ma_value == 0) )	PCPuts("kV,mA first! \r\n");
	else
	{
		if(frameMode == FRAME_CT)  // '1' : ct mode
		{
			ro_end_tm_flag = 0;	
			// cmd_mode_step = STEP1;	
			isModeCompleted = CmdMode(BRIGHT_FRAME_ACQ, (LONG) FRAME_CT);
		}
	// todo : 사용될 코드 같은데 덴트리 9차 소스에 주석으로 처리되어 있음. 분석필요.
	/*	else if(strcmp(rd, "pmc_brit_ufs") == 0)
		{
			ro_end_tm_flag = 0;	cmd_mode_step = STEP1;	cmd_mode(BRIGHT_FRAME_ACQ, "pmm_brit_ufs");
		}*/
		else if(frameMode == FRAME_PANO)
		{
			ro_end_tm_flag = 0;	
			// cmd_mode_step = STEP1;	
			isModeCompleted = CmdMode(BRIGHT_FRAME_ACQ, (LONG) FRAME_PANO);
		}
		else if(frameMode == FRAME_CEPH)
		{
			ro_end_tm_flag = 0;	
			// cmd_mode_step = STEP1;	
			isModeCompleted = CmdMode(BRIGHT_FRAME_ACQ, (LONG) FRAME_CEPH);
		}

		else	PCPuts("Unknown brit mode! \r\n");
	}
		
}

/*===================================================================================*/
// MP3 Control
/*===================================================================================*/
void Ctrl_SOUND_PLAY_OPTION(LONG value)		// [spop]
{
	if(value < 0 || 3 < value)	value = 0;
	sound_option = (BYTE)value;
	
	write_eeprom_BYTE(0, sound_option);
	
	sound_play_flag = NO;	// 이게 없으면 sound_play_korean이 0에서 1로 바뀔 경우에 음성이 출력될 수 있음

	switch(sound_option)
	{
		case 1:		PCPuts("[rpm_spop_0001]	KO = Korean  \r\n");	break;
		case 2:		PCPuts("[rpm_spop_0002]	EN = English \r\n");	break;
		case 3:		PCPuts("[rpm_spop_0003]	ZH = Chinese \r\n");	break;
		default:	PCPuts("[rpm_spop_0000]	OFF \r\n");	break;
	}

}
	
/*===================================================================================*/
// Axis Move Message - CO_AXIS
/*===================================================================================*/
void Ctrl_CO_MOVE(LONG value)
{
	LONG	moveCnt;
	const int 	SET_ORIGIN = 0;


	gvCO.stop_flag = NO;
	moveCnt = value * gvCO.unit_0_1_pulse;

    if(value == SET_ORIGIN) gvCO.org_complete_flag = NO;
	
	rcan_puts(CO_MOTOR, CAN_AXIS_MOVE, moveCnt, dmy_msg);	dly_10us(dly_t);
}


void Ctrl_CO_MOVE_ACCU(LONG value)
{
	LONG	moveCnt;

	gvCO.stop_flag = NO;
			
	if(value == 9999)	moveCnt = 9;	else	moveCnt = value * gvCO.unit_0_01_pulse;
	
	rcan_puts(CO_MOTOR, CAN_AXIS_MOVE, moveCnt, dmy_msg);	dly_10us(dly_t);
}

void Ctrl_CO_MOVE_CONST(LONG value)
{
	gvCO.stop_flag = NO;
	rcan_puts(CO_MOTOR, CAN_AXIS_MOVE_CONST, value, dmy_msg);	dly_10us(dly_t);
	
	// if(debug_mode)	{	PCPrintf("rcan_puts(CO_MOTOR, CAN_AXIS_MOVE_CONST, %ld, dmy_msg) \r\n", value);}
	// if(debug_mode)	{	PCPrintf("[rpm_comc_%ld] \r\n", value);	}

}

/*===================================================================================*/
// Axis Move Message - SD_AXIS
/*===================================================================================*/
void Ctrl_CAN_MOVE_OFFSET(varMotor_t *axis, LONG value)
{
	LONG 	moveCnt;

	//  입력단위가 [um]

	if( co_option == FOUR_AXIS_CO) {
		axis->stop_flag = NO;
		axis->move_value = value;
		moveCnt = (axis->move_value + axis->offset_um) * axis->unit_0_01_pulse / 10;

		rcan_puts(axis->axisId, CAN_AXIS_MOVE, moveCnt, dmy_msg);
		dly_10us(dly_t);

		// can 명령 프로토콜에 move_accu 명령이 없어 로그용으로 표시
		PCPrintf("[rpm_%smo_%ld] (offset : %ld [um]) \r\n", 
			axis->axisName, axis->move_value, axis->offset_um);
	}
	else {
		PCPrintf("[rpm_%smu_9999]	4axis Only! \r\n", axis->axisName);
	}

}

void Ctrl_SD_MOVE(LONG value)
{
	LONG		moveCnt;
	const int 	SET_ORIGIN = 0;


    gvSD.stop_flag = NO;
	moveCnt = value * SD_0_1_MM_PULSE;
    if(value == SET_ORIGIN) gvSD.org_complete_flag = NO;
	
	rcan_puts(SD_MOTOR, CAN_AXIS_MOVE, moveCnt, dmy_msg);	dly_10us(dly_t);
}

void Ctrl_CAN_MOVE(varMotor_t *axis, LONG value)
{
	LONG 	moveCnt;


	axis->stop_flag = NO;
	axis->move_value = value;
	moveCnt = axis->move_value * axis->unit_0_1_pulse;

	rcan_puts(axis->axisId, CAN_AXIS_MOVE, moveCnt, dmy_msg);
	dly_10us(dly_t);
}

void Ctrl_CAN_MOVE_UM(varMotor_t *axis, LONG value)
{
	LONG 	moveCnt;

	//  입력단위가 [um]

	if( co_option == FOUR_AXIS_CO) {
		axis->stop_flag = NO;
		axis->move_value = value;
		moveCnt = axis->move_value * axis->unit_0_01_pulse / 10;

		rcan_puts(axis->axisId, CAN_AXIS_MOVE, moveCnt, dmy_msg);
		dly_10us(dly_t);

		// can 명령 프로토콜에 move_accu 명령이 없어 로그용으로 표시
		PCPrintf("[rpm_%smu_%ld] \r\n", axis->axisName, axis->move_value);
	}
	else {
		PCPrintf("[rpm_%smu_9999]	4axis Only! \r\n", axis->axisName);
	}

}

void Ctrl_CAN_MOVE_ACCU(varMotor_t *axis, LONG value)
{
	LONG 	moveCnt;

	if( co_option == FOUR_AXIS_CO) {
		axis->stop_flag = NO;
		axis->move_value = value;
		moveCnt = axis->move_value * axis->unit_0_01_pulse;

		rcan_puts(axis->axisId, CAN_AXIS_MOVE, moveCnt, dmy_msg);
		dly_10us(dly_t);

		// can 명령 프로토콜에 move_accu 명령이 없어 로그용으로 표시
		PCPrintf("[rpm_%sma_%ld] \r\n", axis->axisName, axis->move_value);
	}
	else {
		PCPrintf("[rpm_%sma_9999]	4axis Only! \r\n", axis->axisName);
	}
}


void Ctrl_SD_MOVE_CONST(LONG value)
{
	gvSD.stop_flag = NO;
					
	rcan_puts(SD_MOTOR, CAN_AXIS_MOVE_CONST, value, dmy_msg);	dly_10us(dly_t);
	
	// if(debug_mode)	{	PCPrintf("rcan_puts(SD_MOTOR, CAN_AXIS_MOVE_CONST, %ld, dmy_msg) \r\n", value);	}
	// if(debug_mode)	{	PCPrintf("[rpm_sdmc_%ld] \r\n", value);	}
}

// case SD_MOVE_PULSE:				Ctrl_SD_MOVE_PULSE(value);			break;	// [sdmp]
void Ctrl_SD_MOVE_PULSE(LONG value)
{
	LONG	moveCnt;
    gvSD.stop_flag = NO;

    moveCnt = value;

    // if(value == 0) gvSD.org_complete_flag = NO;

	rcan_puts(SD_MOTOR, CAN_AXIS_MOVE, moveCnt, dmy_msg);	dly_10us(dly_t);

	PCPrintf("[rpm_sdmp_%04ld] \r\n", moveCnt);

}

void Ctrl_SD_START_SPEED(LONG value)	// [sdss]
{
	gAxisSD.start_speed = value;
	
	rcan_puts(SD_MOTOR, CAN_AXIS_START_SPEED, value, dmy_msg);	dly_10us(dly_t);
	if(debug_mode)	PCPrintf("rcan_puts(SD_MOTOR, CAN_AXIS_START_SPEED, %ld, dmy_msg) \r\n", value);
	
	PCPrintf("[rpm_sdss_%ld] \r\n", value); 
}

void Ctrl_SD_MAX_SPEED(LONG value)		// [sdms]
{
	gAxisSD.max_speed = value;
	
	rcan_puts(SD_MOTOR, CAN_AXIS_MAX_SPEED, value, dmy_msg);	dly_10us(dly_t);
	if(debug_mode)	PCPrintf("rcan_puts(SD_MOTOR, CAN_AXIS_MAX_SPEED, %ld, dmy_msg) \r\n", value);
	
	PCPrintf("[rpm_sdms_%ld] \r\n", value); 
}


void Ctrl_CAN_MAX_SPEED(varMotor_t *axis, LONG value)
{
	axis->max_speed = value;
	rcan_puts(axis->axisId, CAN_AXIS_MAX_SPEED, value, dmy_msg);	dly_10us(dly_t);
	PCPrintf("[rpm_%sms_%ld] \r\n", axis->axisName, axis->max_speed); 
}


void Ctrl_CAN_START_SPEED(varMotor_t *axis, LONG value)
{
	axis->start_speed = value;
	rcan_puts(axis->axisId, CAN_AXIS_START_SPEED, value, dmy_msg);	dly_10us(dly_t);
	PCPrintf("[rpm_%sss_%ld] \r\n", axis->axisName, axis->start_speed); 
}


/*===================================================================================*/	
// Poskom PXD-140CT X-Ray Generator Control			ACK는 gen_ack()에서
/*===================================================================================*/	
void Ctrl_KV_SETTING(LONG value)	// [kvst]
{
	kv_da_value = value; // for DA gen calibration 

	if(value != 0)
	{
		GenPrintf("(SETV%04ld)",value);		// WDG90 
	}
	else	{	kv_value = 0;	LCDPuts("[tml_kvst_0000] \r\n"); PCPuts("[rpm_kvst_0000] \r\n");	}
}

void Ctrl_MA_SETTING(LONG value)  // [mast]
{
	ma_da_value = value;	// for DA gen calibration 


	// AEC 상황에서 detector trig와 ma 변경을 동기화하기 위한 
	if(isOnAEC && exp_ok_flag && (exp_sw_flag || software_exp))
	{
		maValue_AEC = value;
		isChanging_mAInAEC = YES;
		return;
	}

	if(value != 0)
	{
		GenPrintf("(SETA%04ld)",value);
	}
	else	{	ma_value = 0;	LCDPuts("[tml_mast_0000] \r\n");	PCPuts("[rpm_mast_0000] \r\n");	}

}
void Ctrl_KV_REQ(LONG value)	// [kv?_]
{
	GenPuts("(QSKV)");
}
void Ctrl_MA_REQ(LONG value)	// [ma?_]
{
	GenPuts("(QSMA)");
}
void Ctrl_TUBE_MODE_REQ(LONG value)	// [tum?]
{
	GenPuts("(QXMO)");
}
void Ctrl_X_RAY_EXP_STATE_REQ(LONG value)	// [exp?]
{
	GenPuts("(QXRY)");
}
void Ctrl_INVERTER_BUZZER_STATE_REQ(LONG value)	// [ib?_]
{
	GenPuts("(QSBZ)");
}
void Ctrl_MA_DA_VALUE_REQ(LONG value)	// [da?_]
{
	GenPrintf("(SETA%04ld)(QDVA)",value);		// 입력한 mA 값의 DA 값을 출력
	PCPrintf("(SETA%04ld)(QDVA)\r\n",value);
}
void Ctrl_KV_DA_VALUE_REQ(LONG value) 	// [dv?_]
{
	GenPrintf("(SETV%04ld)(QDVV)",value);		// 입력한 kV 값의 DA 값을 출력
	PCPrintf("(SETV%04ld)(QDVV)\r\n",value);
}

void Ctrl_TUBE_NORMAL_MODE(LONG value)	// [tunr]
{
	gen_mode_ack_flag = 0;
	GenPuts("(SXMO0000)");
}
void Ctrl_TUBE_PULSE_MODE(LONG value)	// [tupl]
{
	gen_mode_ack_flag = 0;
	GenPuts("(SXMO0001)");
}
void Ctrl_INVERTER_BUZZER_ON(LONG value)	// [ibon]
{
	GenPuts("(SETB0001)");
}
void Ctrl_INVERTER_BUZZER_OFF(LONG value)	// [ibof]
{
	GenPuts("(SETB0000)");
}
void Ctrl_TUBE_TEMP_REQ(LONG value)	// [tt?_]
{
	GenPuts("(QTBT)");
}
void Ctrl_KV_DA_VALUE_SETTING(LONG value)	// [pmc_kvst_####][pmc_dvst_####]: kV 4자리, DA값 4자리
{
	static WORD address;

	switch(kv_da_value)
	{
		case 50:
			address = 100;		if(1177 > value) value = 1177;	if(1593 < value) value = 1593;	break;
		case 55:
			address = 105;		if(1295 > value) value = 1295;	if(1753 < value) value = 1753;	break;
		case 60:
			address = 110;		if(1413 > value) value = 1413;	if(1911 < value) value = 1911;	break;
		case 65:
			address = 115;		if(1531 > value) value = 1531;	if(2071 < value) value = 2071;	break;
		case 70:
			address = 120;		if(1648 > value) value = 1648;	if(2230 < value) value = 2230;	break;
		case 75:
			address = 125;		if(1766 > value) value = 1766;	if(2390 < value) value = 2390;	break;
		case 80:
			address = 130;		if(1884 > value) value = 1884;	if(2548 < value) value = 2548;	break;
		case 85:
			address = 135;		if(2002 > value) value = 2002;	if(2708 < value) value = 2708;	break;
		case 90:
			address = 140;		if(2119 > value) value = 2119;	if(2867 < value) value = 2867;	break;
		case 95:
			address = 145;		if(2237 > value) value = 2237;	if(3027 < value) value = 3027;	break;
		case 100:
			address = 150;		if(2355 > value) value = 2355;	if(3186 < value) value = 3186;	break;
		// case 105:
		// 	address = 155;		if(2473 > value) value = 2473;	if(3345 < value) value = 3345;	break;
		// case 110:
		// 	address = 160;		if(2590 > value) value = 2590;	if(3504 < value) value = 3504;	break;
		default:	
			address = 0;	PCPuts("Out of eeprom address range! \r\n");	break;	// 잘못된 값이 입력됐을 때, 잘못된 번지의 메모리에 저장하지 않기 위한 조건 생성
	}						
	GenPrintf("(SDVV%04d)",(WORD)value);		// 입력한 kV에 대응하는 DA 값을 적용
	
	if( (address != 0) && (g_IsInitializingGen == NO) )	write_eeprom_WORD(address, (WORD) value);
}

void Ctrl_MA_DA_VALUE_SETTING(LONG value)	// [pmc_mast_####][pmc_dast_####]: mA 4자리, DA값 4자리
{
	static WORD address;

	switch(ma_da_value)
	{
		case 40:
			address = 165;		if(439 > value)	value = 439; 	if(593 < value) value = 593;		break;
		case 45:
			address = 170;		if(494 > value)	value = 494; 	if(668 < value) value = 668;		break;
		case 50:
			address = 175;		if(548 > value) value = 548;	if(742 < value) value = 742;		break;
		case 55:
			address = 180;		if(604 > value) value = 604;	if(817 < value) value = 817;		break;
		case 60:
			address = 185;		if(658 > value) value = 658;	if(890 < value) value = 890;		break;
		case 65:
			address = 190;		if(713 > value) value = 713;	if(965 < value) value = 965;		break;
		case 70:
			address = 195;		if(768 > value) value = 768;	if(1038 < value) value = 1038;		break;
		case 75:
			address = 200;		if(823 > value)	value = 823;	if(1113 < value) value = 1113;		break;
		case 80:
			address = 205;		if(877 > value) value = 877;	if(1187 < value) value = 1187;		break;
		case 85:
			address = 210;		if(932 > value) value = 932;	if(1262 < value) value = 1262;		break;
		case 90:
			address = 215;		if(987 > value) value = 987;	if(1335 < value) value = 1335;		break;
		case 95:
			address = 220;		if(1042 > value) value = 1042;	if(1410 < value) value = 1410;	break;
		case 100:
			address = 225;		if(1097 > value) value = 1097;	if(1484 < value) value = 1484;	break;
		default:	
			address = 0;	PCPuts("Out of eeprom address range! \r\n");	break;	// 잘못된 값이 입력됐을 때, 잘못된 번지의 메모리에 저장하지 않기 위한 조건 생성
	}
	GenPrintf("(SDVA%04d)",(WORD)value);	// 입력한 mA DA 값을 적용
	
	if( (address != 0) && (g_IsInitializingGen == NO) )	write_eeprom_WORD(address, (WORD)value);
}

void Ctrl_TUBE_TEMP_THRES(LONG value)	// [ttth]
{
	// WDG90에는 없는 기능, PX-140CT에는 기능이 있다.
	PCPuts("- can not use ttth - \r\n");
}

void Ctrl_TUBE_TEMP_THRES_REQ(LONG value)	// [ttt?]
{
	// PCPuts("- can not use ttt? - \r\n");
	// ???? WDG90에는 없는 기능
	PCPrintf("[rpm_ttt?_%04d] \r\n", gen_overheat_threshold);
}

void Ctrl_GEN_FW_REVISION(LONG value)	// [gfrv]
{
	GenPuts("(QFRV)");
}


/*===================================================================================*/
// Axis Move Message - PANO_AXIS
/*===================================================================================*/
void Ctrl_PA_MOVE(LONG value)
{
	LONG 	moveCnt;


	gvPA.stop_flag = NO;
	gvPA.move_value = value;
	
	moveCnt = value * pa_0_1_unit_pulse;
	moveCnt = moveCnt - PA_GetPositionCnt();
	
	if(moveCnt > 0)		PA_MotorDrive(PA_POS_DIR, gvPA.start_speed, gvPA.max_speed, moveCnt);
	else if(moveCnt < 0)	PA_MotorDrive(PA_NEG_DIR, gvPA.start_speed,gvPA.max_speed, -1*moveCnt);

	// 원점 초기화가 된 후에 pamv에 대한 ack를 진행 
	// todo : 다른 방식으로 할 수 있을 듯.
	if(gvPA.org_complete_flag)	gvPA.move_ack_flag = YES;    //무슨 기능?? pa_move_ack_flag

}

void Ctrl_PA_SLOW_STOP(LONG value)
{
	gvPA.stop_flag = NO;
	PA_MotorSlowStop();
}

/*===================================================================================*/
// Axis Move Message - RO_AXIS
/*===================================================================================*/
void Ctrl_RO_MOVE(LONG value)
{
	LONG	moveCnt;


	gvRO.stop_flag = NO;
	gvRO.move_value = value;


	// pulley 비가 9의 배수가 아니면 RO를 1도 돌리는데 필요한 펄스수가 정수로 떨어지지 않아 
	// 해당 소수점 만큼 오차가 발생함. 
	// 오차를 줄이기 위해 360도 pulseCnt를 대입하여 계산을 함(큰 수로 나눌때 오차율은 적어진다)

	// 1) 덴트리 알고리즘 경우(pulley 12:1) 
	// 360도의 moveCnt = 3600 * 42 = 151,200
	// 180도의 moveCnt = 1800 * 42 = 75600
	// moveCnt = value * RO_0_1_DEGREE_PULSE;

	// 2) ecox 알고리즘 경우(pulley 12:1)
	// 360도의 moveCnt = 3600 * 153600 / 3600 = 153600
	// 180도의 moveCnt = 1800 * 153600 / 3600 = 76800
	moveCnt = value * RO_360_DEGREE_PULSE / 3600;
	moveCnt = moveCnt - RO_GetPositionCnt();

	if(moveCnt > 0)		RO_MotorDrive(RO_POS_DIR, gvRO.start_speed, gvRO.max_speed, moveCnt);
	else if(moveCnt < 0)	RO_MotorDrive(RO_NEG_DIR, gvRO.start_speed, gvRO.max_speed, -1*moveCnt);

	// 원점 초기화가 된 후에 romv에 대한 ack를 진행 
	// todo : 다른 방식으로 할 수 있을 듯.
	if(gvRO.org_complete_flag)	gvRO.move_ack_flag = YES;	//무슨 기능?? ro_move_ack_flag
}

void Ctrl_RO_SLOW_STOP(LONG value)
{
	gvRO.stop_flag = NO;
	RO_MotorSlowStop();
}
void Ctrl_RO_MAX_SPEED(LONG value)
{
	// mRO.max_speedHz	= value;  // 이 루틴을 넣을 경우 CT 촬영에서 속도가 바뀐다. 
	gvRO.max_speed	= value;   
	PCPrintf("[rpm_roms_%ld] \r\n", gvRO.max_speed);
}
void Ctrl_RO_START_SPEED(LONG value)
{
	gvRO.start_speed	= value;
	PCPrintf("[rpm_ross_%ld] \r\n", mRO.min_speedHz);
}
void Ctrl_RO_ACCEL_STEP(LONG value)
{
	gvRO.accel_step	= value;
	PCPrintf("[rpm_roas_%ld] \r\n", mRO.accel_step);	
}

void Ctrl_PA_MAX_SPEED(LONG value)		// [pams]
{
	gvPA.max_speed	= value;   
	PCPrintf("[rpm_pams_%ld] \r\n", gvPA.max_speed);
}
void Ctrl_PA_START_SPEED(LONG value)	// [pass]
{
	gvPA.start_speed	= value;
	PCPrintf("[rpm_pass_%ld] \r\n", mPA.min_speedHz);
}

/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
void LCDCtrl(LONG inst, LONG value)
{
	// PrintCtrl(inst, value);

	#ifdef EXHIBIT_MODE
	// 전시회 동작중에는 LCD에서 Acturator 동작은 기능하지 않게끔.
		if(isOnExhibition == YES)
		{
			if( (inst == ACTUATOR_UPUP) || (inst == ACTUATOR_DOWN) || (inst == ACTUATOR_STOP) )
				return;
		}
	#endif

	switch(inst)
	{
		case KV_SETTING:				LCtrl_KV_SETTING(value);				break;	// [kvst]
		case MA_SETTING:				LCtrl_MA_SETTING(value);				break;	// [mast]
		case ACTUATOR_UPUP:				LCtrl_ACTUATOR_UPUP(value); 				break;	// [acup]
		case ACTUATOR_DOWN:				LCtrl_ACTUATOR_DOWN(value); 				break;	// [acdn]
		case ACTUATOR_STOP:				LCtrl_ACTUATOR_STOP(value); 				break;	// [acsp]
		case LASER_ON:					LCtrl_LASER_ON(value);					break;	// [lron]
		case LASER_OFF:					LCtrl_LASER_OFF(value);					break;	// [lrof]

		case CANINE_BEAM_VALUE_ENABLE:	LCtrl_CANINE_BEAM_VALUE_ENABLE(value);	break;	// [cbve]

		case LASER_STATE_REQ:			LCtrl_LASER_STATE_REQ(value);			break;	// [lrs?]

		default: 	PCPrintf("Undefined LCD Control : 0x%lx\r\n", inst);			break;
	}
}

void LCtrl_LASER_STATE_REQ(LONG value)	// [lrs?]
{
	if(laser_lmt_cnt_flag == ON)	//	laser_lmt_cnt_flag : laser 제한시간 플래그
	{
		LCDPuts("[tml_lrst_0001] \r\n");
	}
	else
	{
		LCDPuts("[tml_lrst_0000] \r\n");
	}
}


/*===================================================================================*/
// Canine Beam Offset	(견치빔을 적용할 경우에만 반영)
/*===================================================================================*/
// todo : LCD touch pannel에서 사용되는 기능인가? => 매뉴얼엔 기능 설명 없음
void LCtrl_CANINE_BEAM_VALUE_ENABLE(LONG value)	// [cbve]
{
	isOnCanineBeam = value;
	
	LCDPrintf("[tml_cbve_%04d] \r\n",isOnCanineBeam);

	if(isOnCanineBeam == YES)                            // canine beam이 ON이고
    {
    	LCDCtrl(LASER_ON, (LONG) 0);
    	LaserCanine(ON);
		// 덴트리에서 FRONT_LR 기능이 이해가 안감. ==> 일단 eco x에서는 사용 안함
        // if(FRONT_LR == ON) LCDCtrl(LASER_ON, (LONG) 0);   // LASER가 ON이면 LASER 한번 더 ON
    }
    else	LaserCanine(OFF);
}


/*===================================================================================*/	
// X-Ray Generator Control			ACK는 gen_ack()에서
/*===================================================================================*/	
// todo LCtrl(), Ctrl() 함수 중복 최소화 
void LCtrl_KV_SETTING(LONG value)
{
	kv_lcd_flag = YES;

	if(value != 0)
	{
		GenPrintf("(SETV%04ld)",value);
	}
	else	{	kv_value = 0;	LCDPuts("[tml_kvst_0000] \r\n");	PCPuts("[rpm_kvst_0000] \r\n");	}

}

void LCtrl_MA_SETTING(LONG value)
{
	ma_lcd_flag = YES;

	if(value != 0)
	{
		GenPrintf("(SETA%04ld)",value);
	}
	else	{	ma_value = 0;	LCDPuts("[tml_mast_0000] \r\n");	PCPuts("[rpm_mast_0000] \r\n");	}

}

/*===================================================================================*/	
// Control Column(Acturator)
/*===================================================================================*/	
void LCtrl_ACTUATOR_UPUP(LONG value)	// [acup]
{
	LONG temp;

	switch(ac_option)
	{
		case PAN_CTRL_FWD_AC2:	COLUMN_UP = HIGH;	COLUMN_DN = LOW;	temp = 5; break;
		case PAN_CTRL_REV_AC2:	COLUMN_UP = LOW;	COLUMN_DN = HIGH;	temp = 6; break;
		default:	temp = 8; break;
	}
}

void LCtrl_ACTUATOR_DOWN(LONG value)
{
	LONG temp;
	
	switch(ac_option)
	{
		case PAN_CTRL_FWD_AC2:	COLUMN_UP = LOW;	COLUMN_DN = HIGH;	temp = 5; break;
		case PAN_CTRL_REV_AC2:	COLUMN_UP = HIGH;	COLUMN_DN = LOW;	temp = 6; break;
		default:	temp = 8;	break;
	}
}

void LCtrl_ACTUATOR_STOP(LONG value)
{
	LONG temp;

	switch(ac_option)
	{
		case PAN_CTRL_FWD_AC2:
		case PAN_CTRL_REV_AC2:	COLUMN_UP = HIGH;	COLUMN_DN = HIGH; temp = 1; break;
		default:	temp = 3;	break;
	}
}

/*===================================================================================*/
// Laser Control
/*===================================================================================*/	
void LCtrl_LASER_ON(LONG value)	// [lron]
{
	lr_lcd_flag = YES;
	LaserOn();
}
void LCtrl_LASER_OFF(LONG value)	// [lrof]
{
	lr_lcd_flag = YES;
	LaserOff();
}

/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
void InitCmd(void)
{
	if(com4_BufGetSize())				// com4 : PC
	{
		cmd_t 	cmd;
		cmd = ReceiveCmd_PC();

		// register initial command
		switch(cmd.inst)
		{
			case EMERGENCY_STOP:				Ctrl(cmd.inst, (LONG)0);		break;
			case SYSTEM_INIT_COMPLETE:			Ctrl(cmd.inst, cmd.value);	break;
			case TUBE_TEMP_REQ:					Ctrl(cmd.inst, cmd.value);	break;
			case FIRMWARE_REVISION_NUM:			Ctrl(cmd.inst, cmd.value);	break;

			case CEPH_OPTION:					Ctrl(cmd.inst, cmd.value);	break;
			case PA_OPTION:						Ctrl(cmd.inst, cmd.value);	break;
			case RO_OPTION:						Ctrl(cmd.inst, cmd.value);	break;
			case CO_OPTION:						Ctrl(cmd.inst, cmd.value);	break;
			case AC_OPTION:						Ctrl(cmd.inst, cmd.value);	break;
			case GEN_OPTION:					Ctrl(cmd.inst, cmd.value);	break;

			case SOUND_PLAY_OPTION:				Ctrl(cmd.inst, cmd.value);	break;
			case TUBE_TEMP_THRES:				Ctrl(cmd.inst, cmd.value);	break;
			case PANO_NOR_TBL_OPTION:			Ctrl(cmd.inst, cmd.value);	break;
			case CT_RO_END_ANGLE:				Ctrl(cmd.inst, cmd.value);	break;
			case CT_RO_STI_END_ANGLE:			Ctrl(cmd.inst, cmd.value);	break;
			case CT_RO_END_TIME:				Ctrl(cmd.inst, cmd.value);	break;
			case PANO_RO_END_ANGLE:				Ctrl(cmd.inst, cmd.value);	break;
			case PANO_RO_END_TIME:				Ctrl(cmd.inst, cmd.value);	break;
			case CEPH_AXIS_END_POSITION:		Ctrl(cmd.inst, cmd.value);	break;
			case DOOR_CHECK_OPTION:				Ctrl(cmd.inst, cmd.value);	break;
			case PRINT_COMMAND_LIST:			Ctrl(cmd.inst, cmd.value);	break;
			case DEBUGGING_MODE:				Ctrl(cmd.inst, cmd.value);	break;
			case PARAMETER_CHECK:				Ctrl(cmd.inst, cmd.value);	break;
			case TEST_MODE_OPTION:				Ctrl(cmd.inst, cmd.value);	break;
			case CEPH_CAPT_END_COL_POSITION:	Ctrl(cmd.inst, cmd.value);	break;			

			default: 	PCPrintf("[ %ld ] is not InitCmd!!\r\n", cmd.inst);		break;
		}
	}
}


/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
cmd_t ReceiveCmd_PC(void)			// PC
{	
	char 		serial_buf[20];
	char 		strValue[8] = {0};
	cmd_t 	cmd = { {0}, 0, 0 };
	int 		i; 

	if(com4_BufGetSize())
	{
		com4_BufGetData(serial_buf);
		cmd = SplitDataToCmd(serial_buf);
		return	cmd;
	}

	return cmd;
}

cmd_t ReceiveCmd_LCD(void)			
{
	char 		serial_buf[20];
	char 		strValue[8] = {0};
	cmd_t 	cmd = { {0}, 0, 0 };
	int 		i; 

	if(com2_BufGetSize())
	{
		com2_BufGetData(serial_buf);
		//터치패널에서 메인보드로 보내는 lmm명령을 log로 출력하기 위함
		PCPrintf("[%s]\r\n",serial_buf);
		cmd = SplitDataToCmd(serial_buf);
		return	cmd;
	}

	return cmd;
}

cmd_t SplitDataToCmd(char data[20])
{
	char 		strValue[8] = {0};
	char 		strValue2[8] = {0};
	cmd_t 		cmd = { {0}, 0, 0, 0 };
	int i;

	// ex) pmc_romv_2345 -> split 'pmc', 'romv', '2345'
	// 'pmc'
	for( i = 0 ; i < 3 ; i++)		cmd.type[i] = data[i];

	// 'romv'
	cmd.inst =  (((LONG)data[4] << 24) & 0xff000000);
	cmd.inst |= (((LONG)data[5] << 16) & 0x00ff0000);
	cmd.inst |= (((LONG)data[6] << 8 ) & 0x0000ff00);
	cmd.inst |= (((LONG)data[7]      ) & 0x000000ff);

	// '2345'
	for( i = 9 ; i < 16 ; i++)		strValue[i-9] = data[i];
	cmd.value = atol(strValue);

	return	cmd;
}


/*F**************************************************************************
*	Generator Ack
*****************************************************************************/
void ProcessGenAck(void)
{
	int bufsize;
	if(bufsize = com5_BufGetSize())				// GENERATOR 시리얼 버퍼에 뭔가 입력되었으면
	{
		char serial_buf[20];


	// UartPrintf("Gen_Bufsize : %d\r\n", bufsize);
		com5_BufGetData(serial_buf);
		gen_ack((char *)serial_buf);
	}
}

/****************************************************************************
*	Synchronize mA in AEC
*****************************************************************************/
void SyncMaInAEC(void)
{
	LONG temp = 0;


	if(isOnAEC != ON)	return;		// early return
	

	// WDG90 mA 변경시  serial 통신 동기화를 위해
	// ma가 임의적으로 변경될 때, 디텍터 trig 주기안에 임의적으로 X-ray가 변경되어 
	// 디텍터가 영상을 정상적으로 읽어들일 수 없는 경우가 있어 영상 껌뻑임이 발생 함
	//------- 해결책
	// detector 주기를 만들어 내는 detPulsCnt가 시작할 때 ma 변경 통신을 날린다. 
	// 하지만 detPulsCnt가 10us timer interrupt에서 동작하고, 
	// AEC 동작은 100us interrupt에서 동작하여 100us이상의 시간 확보가 필요.
	// '20'은 10us * 20 = 200us 를 의미함
	temp = GetDetPulseCnt();
	if( isChanging_mAInAEC && (temp < 20) )
	{
		// UartPrintf("==> DetPulseCnt : %ld \r\n ", temp);
		GenPrintf("(SETA%04ld)",maValue_AEC);
		isChanging_mAInAEC = NO; 	// 통신이 되었으면 clear
	}

}

/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
// generator에서 시리얼로 오는 메세지를 받아서 응답을 처리하는 함수
// generator에대한 응답처리뿐만 아니라 타 루틴에서 generator를 제어하기위해서도 사용됨.
// ?? generator에서 나오는 신호가  소문자인가? ==> 소문자!!
// todo : gen에서 입력받는 문자열을 cmd와 value를 구분하여 switch-case  패턴으로 ==> switch는 문자열은 안됨!!

// Gen에서 받은 문자열중에 뒤에 나오는 숫자만을  (LONG) value로 리턴
// 숫자가 없을 때는 0으로 반환
LONG ExtractValue(char *str)
{
	while(*str)
	{
		if( '0' <= *str && *str <= '9')	 
			break;
		++str;
	}
	return (LONG)atoi(str);
}


BYTE gen_ack(char *rd)
{
	char	buf[5] = {0};
	LONG 	inst = 0;
	LONG 	tempValue = 0;
	WORD	value = 0;


  	// UartPrintf("==> genAck : [ %s ]\r\n", rd);	// debug용

    inst =  (((LONG)rd[0] << 24) & 0xff000000);
    inst |= (((LONG)rd[1] << 16) & 0x00ff0000);
    inst |= (((LONG)rd[2] << 8 ) & 0x0000ff00);
    inst |= (((LONG)rd[3]      ) & 0x000000ff);

	buf[0]=rd[4]; buf[1]=rd[5]; buf[2]=rd[6]; buf[3]=rd[7];
    tempValue = (LONG)(atol((char *)buf));
    value = (WORD)tempValue;


	if(gen_option == OFF)		return;


    switch(inst)
    {
		case R_SET_KV:			// [rskv] kV 설정에 대한 응답 
			gen_ack_flag = 1;
		
			if(g_IsInitializingGen)	PCPrintf("(RSKV%04u)", value);
			else
			{
				kv_value = value;

                LCDPrintf("[tml_kvst_%04u] \r\n",kv_value);
				
				if(kv_lcd_flag)	{	kv_lcd_flag = NO; }
				else			{	PCPrintf("[rpm_kvst_%04u]\r\n",kv_value);	}
			}
			break;
			
		case R_SET_MA:			// [rsma] mAv 설정에 대한 응답 
			gen_ack_flag = 2;
			
			if(g_IsInitializingGen)	{	PCPrintf("(RSMA%04u)", value);	}
			else
			{
				// gen에서 받은 값으로 ma_value가 셋팅된다.
				ma_value = value;

				// AEC이 선택되고, 촬영중이면 mA 설정에 대한 응답을 하지 않는다. 
				if(isOnAEC && (exp_ok_flag) && (exp_sw_flag || software_exp) )	return;
				
				LCDPrintf("[tml_mast_%04u] \r\n",ma_value);

				if(ma_lcd_flag)	
				{ 
					ma_lcd_flag = NO; 
				}
				else
				{ 
					PCPrintf("[rpm_mast_%04u]\r\n", ma_value);	
				}
			}
			break;
			
		case R_Q_STATUS_KV:		// [rqsv] kV 설정(상태)값 확인에 대한 응답 
			gen_ack_flag = 3;
			PCPrintf("[tml_kvst_%04u]",value);
			PCPrintf("[rpm_kv?__%04u] \r\n", value);
			break;
			
		case R_Q_STATUS_MA:		// [rqsa] mA 설정(상태)값 확인에 대한 응답 
			gen_ack_flag = 4;
			PCPrintf("[tml_mast_%04u]",value);
			PCPrintf("[rpm_ma?__%04u] \r\n", value);
			break;

		case R_Q_XRAY_MODE:		// [rqxm] Xray 조사 방법 확인에 대한 응답 
			if(value)
			{
				gen_ack_flag = 6;	
				gen_mode_ack_flag = GEN_PULSE_MODE;
				PCPuts("[rpm_tupl] \r\n");
			}
			else
			{
				gen_ack_flag = 5;	
				gen_mode_ack_flag = GEN_NORMAL_MODE;
				PCPuts("[rpm_tunr] \r\n");
			}
			break;
			
		case R_STATUS_XRAY:		// [rxry] Xray on/off 상태에 대한 응답
			if(value)
			{
				gen_ack_flag = 7;
				if(!exp_ok_flag)	PCPuts("[rpm_exon] \r\n"); 
			}
			else
			{
				gen_ack_flag = 8;
				if(!exp_ok_flag)	PCPuts("[rpm_exof] \r\n");
			}
			break;
			
		case R_Q_STATUS_BUZZER:	// [rqsb] ?? Buzzer 상태에 대한 응답
			if(value)
			{
				gen_ack_flag = 9;
				PCPuts("[rpm_ibon] \r\n");
			}
			else
			{
				gen_ack_flag = 10;
				PCPuts("[rpm_ibof] \r\n");
			}
			break;


		case R_DV_MA:			// [rdva] DV mA 설정에 대한 응답 
			gen_ack_flag = 11;
			
			if(g_IsInitializingGen)
			{
				PCPrintf("(RDVA%04u) \r\n", value);
			}
			else
			{
				PCPrintf("[rpm_dast_%04u] \r\n", value);
			}
			break;
			
		case R_DV_KV:			// [rdvv] DV kV 설정에 대한 응답
			gen_ack_flag = 12;
		
			if(g_IsInitializingGen)
			{
				PCPrintf("(RDDV%04u) \r\n", value);
			}
			else
			{
				PCPrintf("[rpm_dvst_%04u] \r\n", value);
			} 
			break;
			
		// 덴트리에 없는 코드, Qface에서 가져옴	
		case R_Q_DV_MA:			// [rqda] DV mA 설정(상태)값 확인에 대한 응답
		    gen_ack_flag = 11;

            if(g_IsInitializingGen)
            {
                PCPrintf("(RQDA%04u) \r\n", value);
            }
            else
            {
                PCPrintf("[rpm_dast_%04u] \r\n", value);
            } 
			break;
		
		// 덴트리에 없는 코드, Qface에서 가져옴	
		case R_Q_DV_KV:			// [rqdv] DV kV 설정(상태)값 확인에 대한 응답
			gen_ack_flag = 12;

            if(g_IsInitializingGen)
            {
                PCPrintf("(RQDV%04u) \r\n", value);
            }
            else
            {
                PCPrintf("[rpm_dvst_%04u] \r\n", value);
            }
			break;

		case R_XRAY_MODE:		// [rxmo] Xray 모드 설정에 대한 응답 
			if(value)
			{
				gen_ack_flag = 14;	
				gen_mode_ack_flag = GEN_PULSE_MODE;
				PCPuts("[rpm_tupl] \r\n");
			}
			else
			{
				gen_ack_flag = 13;	
				gen_mode_ack_flag = GEN_NORMAL_MODE;
				PCPuts("[rpm_tunr] \r\n");
			}
			break;
			
		case R_SET_BUZZER:		// [rsbz] Buzzer 설정에 대한 응답 
			if(value)
			{
				gen_ack_flag = 15;
				PCPuts("[rpm_ibon] \r\n");
			}
			else
			{
				gen_ack_flag = 16;
				PCPuts("[rpm_ibof] \r\n");
			}
			break;
			
		case R_TUBE_TEMP:		// [rtbt] Tube temperature 
			gen_ack_flag = 17;
			PCPuts("[rpm_tt?__????] \r\n");
			break;
			
		case POWER_ON:			// [pwon] Generator Power on 
			gen_ack_flag = 18;
            PCPuts("Generator_Power_On \r\n");
			break;
			
		case R_FW_VERSION:		// [rfrv] Firmware version
            gen_ack_flag = 19;
            PCPuts("Generator = W90_V1_");
            ASCII2Version(buf);
			break;

		//--------------------------- Error Ack
		/*
	 	//------ Generator 와 error 통신 test를 진행한 후에 코드 적용할 것
		case ERR_SKV:		// [eskv]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_21]	kV value over! \r\n");	
			break;

		case ERR_SMA:		// [esma]	
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_22]	mA value over! \r\n");
			break;
		
		case ERR_SOV:		// [esov]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_23]	Overvoltage Error in Standby State! \r\n");	
			break;
		
		case ERR_SOA:		// [esoa]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_24]	Overcurrent Error in Standby State! \r\n");	
			break;
		
		case ERR_SOF:		// [esof]	
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_25]	Preheat Error in Standby State! \r\n");
			break;
		
		case ERR_RUF:		// [eruf]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_26]	Preheat Error in Ready State! \r\n");	
			break;
		
		case ERR_XOV:		// [exov]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_27]	Overvoltage Error in Exposure State! \r\n");	
			break;
		
		case ERR_XUV:		// [exuv]	
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_28]	Undervoltage Error in Exposure State! \r\n");
			break;
		
		case ERR_XOA:		// [exoa]	
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_29]	Overcurrent Error in Exposure State! \r\n");
			break;
		
		case ERR_XUA:		// [exua]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_30]	Undercurrent Error in Exposure State! \r\n");	
			break;
		
		case ERR_XOT:		// [exot]	// Qface코드에서는 주석처리되어 사용 안하는 루틴
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_31]	Exposure Time Over Error! \r\n");
			break;
		
		case ERR_ROT:		// [erot]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_32]	Preheat Time Over Error! \r\n");
			gen_off(1);
			ct_pano_det_off();
			exp_ok_flag = NO;	exp_push = NO;	
			break;
		
		case ERR_OTT:		// [eott]	// gen_err_flag 1은 별도 조건 
			gen_err_flag = 1;	err_lamp_on();
			PCPuts("[GEN_ERR_33]	Temperature Over Error! \r\n");
			break;

		case ERR_DKV:		// [edkv]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_34]	DV Setting Error! \r\n");	
			break;
		
		case ERR_DMA:		// [edma]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_35]	DA Setting Error! \r\n");	
			break;
		
		case ERR_COM1:		// [ecom]
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_36]	Generator Communication Error! \r\n");	
			break;
		
		case ERR_ITL:		// [eitl]	
			gen_err_flag = 2;	err_lamp_on();
			PCPuts("[GEN_ERR_37]	Interlock Error! \r\n");
			break;
		*/

		default: 	
			PCPuts("undefined gen_ack protocol! \r\n");  
			break;

    }
	
	//----------------------------------------------------------------------------------------	Error Ack
	
	// gen_err_flag의 값은 1, 2 두가지며, main()-while(1)에서 gen 에러체크하는 것으로만 쓰인다.
	// todo : 중복코드 삭제  -> case문으로 변경을 하였으나 error ack 테스트를 하지 못하여 if문을 사용함.
	if(strcmp(rd, "eskv") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_21]	kV value over! \r\n");
	}
	else if(strcmp(rd, "esma") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_22]	mA value over! \r\n");
	}
	else if(strcmp(rd, "esov") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_23]	Overvoltage Error in Standby State! \r\n");
	}
	else if(strcmp(rd, "esoa") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_24]	Overcurrent Error in Standby State! \r\n");
	}
	else if(strcmp(rd, "esof") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_25]	Preheat Error in Standby State! \r\n");
	}
	else if(strcmp(rd, "eruf") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_26]	Preheat Error in Ready State! \r\n");
	}
	else if(strcmp(rd, "exov") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_27]	Overvoltage Error in Exposure State! \r\n");
	}
	else if(strcmp(rd, "exuv") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_28]	Undervoltage Error in Exposure State! \r\n");
	}
	else if(strcmp(rd, "exoa") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_29]	Overcurrent Error in Exposure State! \r\n");
	}
	else if(strcmp(rd, "exua") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_30]	Undercurrent Error in Exposure State! \r\n");
	}
	else if(strcmp(rd, "exot") == 0)	// Qface코드에서는 주석처리과 된 루틴
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_31]	Exposure Time Over Error! \r\n");
	}
	else if(strcmp(rd, "erot") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_32]	Preheat Time Over Error! \r\n");
		gen_off(1);
		ct_pano_det_off();
		exp_ok_flag = NO;	exp_push = NO;
	}
	// gen_err_flag 1은 별도 조건  - 
	else if(strcmp(rd, "eott") == 0)
	{
		gen_err_flag = 1;	err_lamp_on();
		PCPuts("[GEN_ERR_33]	Temperature Over Error! \r\n");
	}
	else if(strcmp(rd, "edkv") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_34]	DV Setting Error! \r\n");
	}
	else if(strcmp(rd, "edma") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_35]	DA Setting Error! \r\n");
	}
	else if(strcmp(rd, "ecom") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_36]	Generator Communication Error! \r\n");
	}
	else if(strcmp(rd, "eitl") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_37]	Interlock Error! \r\n");
	}
 
}

// 시리얼 메시지 출력하지 않으면 gen_ack 함수 실행하는데 60 us 이하 걸림
// todo : WDG90 dip sw 옵션 선택중 PX-140CT을 사용하지 않을 경우가 확정되면 삭제
BYTE gen_ack_PX140CT(char *rd)
{
	char	buf[5] = {0};
	LONG	value = 0;
	
  // UartPrintf("==> genAck : [ %s ]\r\n", rd);
	value = ExtractValue(rd);
	
	// "xgv345" : 관전압 설정
	if(rd[0] == 'x' && rd[1] == 'g' && rd[2] == 'v' && ('0' <= rd[3] && rd[3] <= '9') && rd[6] == 0x00)
	{
		gen_ack_flag = 1;
		
		if(g_IsInitializingGen)	PCPrintf("(XGV%03ld)", value);
		else
		{
			kv_value = value;
			
			LCDPrintf("[tml_kvst_%04d]\r\n",kv_value);

			if(kv_lcd_flag)	{	kv_lcd_flag = NO; }
			else			{ 	PCPrintf("[rpm_kvst_%04d]\r\n",kv_value);	}
		}
	}
	// "xga345" : 관전류 설정
	else if(rd[0] == 'x' && rd[1] == 'g' && rd[2] == 'a' && ('0' <= rd[3] && rd[3] <= '9') && rd[6] == 0x00)
	{
		gen_ack_flag = 2;
			
		if(g_IsInitializingGen)	{	PCPrintf("(XGA%03ld)", value);	}
		else
		{
			ma_value = value;
	
			// AEC이 선택되고, 촬영중이면 mA에 대한 응답을 하지 않는다. 
			if(isOnAEC && (exp_ok_flag) && (exp_sw_flag || software_exp) )	return;

			LCDPrintf("[tml_mast_%04d]\r\n",ma_value);
			
			if(ma_lcd_flag)	
			{ 
				ma_lcd_flag = NO; 
			}
			else
			{ 
				PCPrintf("[rpm_mast_%04d]\r\n", ma_value);	
			}
		}	
	}
	// "iv34" : 관전압 설정값 확인
	else if(rd[0] == 'i' && rd[1] == 'v' && ('0' <= rd[2] && rd[2] <= '9') && rd[5] == 0x00)
	{
		gen_ack_flag = 3;
		
		PCPrintf("[tml_kvst_%04ld]",value);
		PCPrintf("[rpm_kv?__%04ld] \r\n", value);
	}
	// "ia34" : 관전류 설정값 확인
	else if(rd[0] == 'i' && rd[1] == 'a' && ('0' <= rd[2] && rd[2] <= '9') && rd[5] == 0x00)
	{
		gen_ack_flag = 4;
		
		PCPrintf("[tml_mast_%04ld]",value);
		PCPrintf("[rpm_ma?__%04ld] \r\n", value);
	}
	// "icon" : Continuous mode
	// !!! 추가적인 기능
	else if(strcmp(rd, "icon") == 0)
	{
		gen_ack_flag = 5;	gen_mode_ack_flag = GEN_NORMAL_MODE;
		
		PCPuts("[rpm_tunr] \r\n");
	}
	// "ipon" : Pulsed mode
	else if(strcmp(rd, "ipon") == 0)
	{
		gen_ack_flag = 6;	gen_mode_ack_flag = GEN_PULSE_MODE;
		
		PCPuts("[rpm_tupl] \r\n");
	}
	// "ixon" : X-ray on
	else if(strcmp(rd, "ixon") == 0)
	{
		gen_ack_flag = 7;
		
		if(!exp_ok_flag)	PCPuts("[rpm_exon] \r\n");
	}
	// "ixof" : X-ray off
	else if(strcmp(rd, "ixof") == 0)
	{
		gen_ack_flag = 8;
		
		if(!exp_ok_flag)	PCPuts("[rpm_exof] \r\n");
	}
	// "ibon" : Buzzer On
	else if(strcmp(rd, "ibon") == 0)
	{
		gen_ack_flag = 9;
		
		PCPuts("[rpm_ibon] \r\n");
	}
	// "ibof" : Buzzer Off
	else if(strcmp(rd, "ibof") == 0)
	{
		gen_ack_flag = 10;
		
		PCPuts("[rpm_ibof] \r\n");
	}
	// "iad3456" : mA DA설정
	// ???? DA
	else if(rd[0] == 'i' && rd[1] == 'a' && rd[2] == 'd' && ('0' <= rd[3] && rd[3] <= '9') && rd[7] == 0x00)
	{
		gen_ack_flag = 11;
		
		if( (g_IsInitializingGen) && (!org_pos_90s_over_flag) )
		{
			PCPrintf("(IAD%04ld) \r\n", value);
		}
		else
		{
			PCPrintf("[rpm_dast_%04ld] \r\n", value);
		}
	}
	//"ivd3456" : kV DA설정
	else if(rd[0] == 'i' && rd[1] == 'v' && rd[2] == 'd' && ('0' <= rd[3] && rd[3] <= '9') && rd[7] == 0x00)
	{
		gen_ack_flag = 12;
		
		if( (g_IsInitializingGen) && (!org_pos_90s_over_flag) )
		{
			PCPrintf("(IVD%04ld) \r\n", value);
		}
		else
		{
			PCPrintf("[rpm_dvst_%04ld] \r\n", value);
		}
	}
	//"icoxon" : set Continouse mode
	else if(strcmp(rd, "icxon") == 0)
	{
		gen_ack_flag = 13;	gen_mode_ack_flag = GEN_NORMAL_MODE;
		
		PCPuts("[rpm_tunr] \r\n");
	}
	// "ipxon" : set Pulsed mode
	else if(strcmp(rd, "ipxon") == 0)
	{
		gen_ack_flag = 14;	gen_mode_ack_flag = GEN_PULSE_MODE;
		
		PCPuts("[rpm_tupl] \r\n");
	}
	// "bon" :
	else if(strcmp(rd, "bon") == 0)
	{
		gen_ack_flag = 15;
		
		PCPuts("[rpm_ibon] \r\n");
	}
	// "bof"
	else if(strcmp(rd, "bof") == 0)
	{
		gen_ack_flag = 16;
		
		PCPuts("[rpm_ibof] \r\n");
	}
	// "s_t345"
	else if(rd[0] == 's' && rd[1] == '_' && rd[2] == 't' && ('0' <= rd[3] && rd[3] <= '9') && rd[6] == 0x00)
	{
		gen_ack_flag = 17;
		
		PCPrintf("[rpm_tt?__%04ld] \r\n", value);	// 변경된거... 
		
		if(value < gen_overheat_threshold)
		{
			gen_err_flag = 0;
		}
		else	// 제네레이터 탱크 내부 온도가 기준 온도 이상이면
		{
			gen_err_flag = 1;	err_lamp_on();
			PCPuts("[GEN_ERR_33]	Temperature Over Error! \r\n");
		}
	}
	// "start"
	else if(strcmp(rd, "start") == 0)
	{
		gen_ack_flag = 18;
		
		PCPuts("Generator_Power_On \r\n");
	}
	// "rev_34"
	else if(rd[0] == 'r' && rd[1] == 'e' && rd[2] == 'v' && rd[3] == '_' && ('0' <= rd[4] && rd[4] <= '9') && rd[6] == 0x00)
	{
		gen_ack_flag = 19;
		
		PCPrintf("Generator	= 1.%02ld \r\n\r\n", value);
	}
	
	//----------------------------------------------------------------------------------------	Error Ack
	
	// gen_err_flag의 값은 1, 2 두가지며, main()-while(1)에서 gen 에러체크하는 것으로만 쓰인다.
	// todo : extract fucn GenError2("esv"), genErrorAck2(), GenAckError2()...
	// 중복코드 삭제  -> case문으로 
	else if(strcmp(rd, "esv") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_21]	kV value over! \r\n");
	}
	else if(strcmp(rd, "esa") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_22]	mA value over! \r\n");
	}
	else if(strcmp(rd, "exv") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_23]	Overvoltage Error in Standby State! \r\n");
	}
	else if(strcmp(rd, "exa") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_24]	Overcurrent Error in Standby State! \r\n");
	}
	else if(strcmp(rd, "exf") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_25]	Preheat Error in Standby State! \r\n");
	}
	else if(strcmp(rd, "exfil") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_26]	Preheat Error in Ready State! \r\n");
	}
	else if(strcmp(rd, "exvu") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_27]	Overvoltage Error in Exposure State! \r\n");
	}
	else if(strcmp(rd, "exvd") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_28]	Undervoltage Error in Exposure State! \r\n");
	}
	else if(strcmp(rd, "exau") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_29]	Overcurrent Error in Exposure State! \r\n");
	}
	else if(strcmp(rd, "exad") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_30]	Undercurrent Error in Exposure State! \r\n");
	}
	else if(strcmp(rd, "ext") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_31]	Exposure Time Over Error! \r\n");
	}
	else if(strcmp(rd, "exrt") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_32]	Preheat Time Over Error! \r\n");
		gen_off(1);
		ct_pano_det_off();
		exp_ok_flag = NO;	exp_push = NO;
	}
	// gen_err_flag 1은 별도 조건  - 
	else if(strcmp(rd, "etemp") == 0)
	{
		gen_err_flag = 1;	err_lamp_on();
		PCPuts("[GEN_ERR_33]	Temperature Over Error! \r\n");
	}
	else if(strcmp(rd, "exsv") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_34]	DV Setting Error! \r\n");
	}
	else if(strcmp(rd, "exsa") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_35]	DA Setting Error! \r\n");
	}
	else if(strcmp(rd, "escom") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_36]	Generator Communication Error! \r\n");
	}
	else if(strcmp(rd, "lock") == 0)
	{
		gen_err_flag = 2;	err_lamp_on();
		PCPuts("[GEN_ERR_37]	Interlock Error! \r\n");
	}
}