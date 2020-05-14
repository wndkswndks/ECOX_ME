/*C***************************************************************************
* $RCSfile: capture.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the exp_sw library
*----------------------------------------------------------------------------
* RELEASE:      $Name: capture.c   
* Edited  By    $Id: sslim
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include 	"includes.h"

#include	"pano_stan_table.h"
#include 	"pano_tmj_table.h"

#include 	"scan_ceph_la_table.h"
#include 	"scan_ceph_pa_table.h"
/*_____ D E C L A R A T I O N ______________________________________________*/
enum CaptureStates
{ ST_INIT, ST_IDLE, ST_START, ST_FINISH, ST_VIA, ST_ORIGIN, ST_WAIT_1S, 
  ST_CHK_ERR, ST_CHK_STOP, ST_ERR, ST_GO_END_POS, ST_CHK_FINISH, 
  ST_EXP_ON, ST_DET_ON, ST_RING_ON, ST_CAPTURE_END, ST_DEC_INDEX,
  ST_EXP_ON2, ST_DET_ON2, ST_RING_ON2, ST_CAPTURE_END2, ST_EXP_RDY,
};

enum CaptureEvent
{ EVENT_CLEAR = 0, EVENT_START = 1, };

typedef struct
{
	int 	event;
	int 	reqCnt;
	int 	numError;
	LONG 	offset_pos;
	BYTE	isEnd;
} c_TaskData_t;

typedef struct
{
	SWORD idx_expOn;			// expOn되는 table index
	SWORD idx_detOn;			// detOn되는 table index
	LONG timeCnt_sync;		// 디텍터 초반 불안정 이미지 2pulse에 대한 동기 Table time count
	SWORD idx_expOn2;		// 촬영 오른쪽 진행을 table 역방향 index를 위한 설정값(bitewing - right, both에서 사용)
	SWORD idx_detOn2;
	LONG timeCnt_sync2;
} Table_t;

/*_____ D E F I N E ________________________________________________________*/
#define S_CEPH_LA_TBL	240
#define S_CEPH_PA_TBL	240



/*_____ F U C T I O N ______________________________________________________*/
void check_exp_mode(void);
void check_exp_AEC_mode(void);

//----- CT 관련 function
void exp_capt(void);
void exp_capt_scout(void);
void Set_InitialCaptureValue(void);	// todo rename : SetCaptureValue_CT()
void SetSmoothTrajectoryTable(int, LONG);

void Close_CaptureCT(void);
void Halt_ExpCapture(void);

//------ pano 관련 function
void exp_table_pano(void);
void default_clk_cnt_pano(void);	// todo rename : SetCaptureValue_Pano()
void pano_stan_mode(void);
void pano_bitewing_left_mode(Table_t, BYTE*, WORD, WORD);
void pano_bitewing_right_mode(Table_t, BYTE*, WORD, WORD);
void pano_bitewing_both_mode(Table_t, BYTE*, WORD, WORD);
void pano_bitewing_mode(void);
void pano_tmj_mode(void);

//----- ceph 관련 function
void exp_table_ceph(void);
void default_clk_cnt_ceph(void);	// todo rename : SetCaptureValue_Ceph()
void can_seq_move(void);
void can_seq_move2(void);
void can_seq_stop(void);
void scan_ceph_la_mode(void);
void scan_ceph_pa_mode(void);

//------------- Close functions(Pano, Ceph)
void Close_Capture(BYTE);
void Close_CaptureHalt(BYTE);
void Close_TaskRO(c_TaskData_t*);
void Close_TaskPA(c_TaskData_t*);
void Close_TaskCO_SD(c_TaskData_t*);
int ST_Finish(c_TaskData_t*);
int ST_Idle(c_TaskData_t*);

/*_____ V A R I A B L E ____________________________________________________*/
extern motor_t		mRO;

BYTE	exp_ok_flag = NO;	//촬영준비 완료 flag
BYTE	exp_push;			//최초 초기화를 위한 플래그 조사 스위치 처리 플래그
BYTE	exp_sw_flag = OFF;
BYTE	software_exp = OFF;		// [pmc_swon] 명령으로 셋팅됨
BYTE    software_rdy;
BYTE	ct_capt_end_check; 


LONG	ct_capt_frame = 1200;
LONG	ct_capt_fps = 50;
LONG	ct_capt_speed;
LONG	real_ct_capt_speed;
BYTE	ct_capt_end_check;
LONG	pre_ct_capt_frame;
LONG	pre_ct_capt_fps;
//LONG	ct_capt_frame_capt = 720;
//LONG	ct_capt_fps_capt = 30;
// LONG	ct_capt_angle = 36000;


LONG	ct_capt_msg_cnt;
LONG	ct_capt_exp_start_cnt;
LONG	ct_capt_det_on_cnt;
LONG	ct_capt_ring_on_cnt;		// cnt -> pulseCnt를 의미, 이동거리 
LONG	ct_capt_end_cnt;
LONG	ct_capt_ring_off_cnt;
LONG	scout_gen_rdy_cnt;			// todo local var. : 함수내에서만 쓰임

// todo : 지역변수화
LONG	ct_capt_speed_chk1;
LONG	ct_capt_speed_chk2;
LONG	ct_capt_speed_chk3;
LONG	verify_1;
LONG	verify_2;
LONG	verify_3;
LONG	verify_4;
LONG	verify_5;
LONG	verify_6;

BYTE	scout_mode	= NO;
LONG	scout_pulse;

BYTE	capt_tm_flag = NO;
LONG 	capt_tm_cnt = 0;

WORD	det_trig_off_cnt = 0;


// LONG    angle_pulse_acc_dec = 19679;
// LONG    angle_pulse_500ms = -8072;
// LONG    angle_end_cw = 3800;

extern LONG	sync_cnt;	// from timer.c, todo : CT capture 함수에서는 제외할 것임.
extern LONG	genSyncCnt; // from timer.c , sync_cnt와 같은 기능을 함. 
extern BYTE A1_table_step_flag;

////////////// todo :be due to delete
BYTE	capt_mode_push_step = STEP1;
BYTE	capt_mode_release_step = STEP1;
LONG	ro_pos_cnt_tmp;
extern BYTE	gen_preheat_time_flag;


//----- ceph 작업시 사용하는 variables
BYTE 	tbl_end_chk = NO;
BYTE	capt_pano_sub_step = SUB_STEP1;
BYTE	can_seq_cnt = 0;  	// todo rocal var. can_seq_cnt() 함수내에서만 사용됨.
WORD 	ceph_trig_num = 1600;	// Qface 값 '1600'을 가져옴. dentri에서는 변수가 없음.

extern WORD	ceph_falling_edge_cnt;  // from time.c
extern WORD vw_exp_ok_cnt;			// from time.c
BYTE	sc_tbl_sel_1 = 0;			// sc scan을 의미?  scan collimator를 의미??
BYTE	sc_tbl_sel_2 = 0;
SWORD	tbl_no_tmp = 0;
extern LONG s_ceph_la_capt_start_tbl_num;
extern LONG s_ceph_pa_capt_start_tbl_num;	// from command.c
extern BYTE	table_loop_flag;				// from timer.c

BYTE	co_can_flag = NO;
BYTE	sd_can_flag = NO;
BYTE	co_move_const_flag = NO;
BYTE	sd_move_const_flag = NO;
extern BYTE can_seq_stop_time_flag;	// from cmd_ack.c
SWORD	can_tbl_no = 0;

//----- pano var.
 BYTE	pano_ro_end_angle = 2;	// default 2 : 180도
 BYTE	pano_ro_end_tm = 0;
 WORD 	cur_tbl_cnt = 0;
 WORD	pano_tbl_no = PANO_T47_TBL;
 BYTE	pano_tbl_sel = 0;

WORD	pano_trig_num = 2790;		// Qface값은 2760을 가져옴. 덴트리 값은 2790
LONG	pano_stan_capt_start_table_num = 39;
BYTE	pano_nor_tbl_optn = 1;		// default  : multilayer
BYTE	pano_tmj_tbl_optn;

LONG	pano_tmj_capt_end_table_num = 200;
LONG	pano_tmj_capt_start_table_num = 30;
BYTE	bitewing_flag = 2;

//---- 궤적 table 관련
WORD	xf_cnt = 0;			// todo rocal var. table 사용함수내에서만 사용됨.
SWORD 	tbl_no = 0;
SWORD	exp_tbl_no = 0;
SWORD 	detTableNo = 0;
SWORD 	exp_tbl_no2 = 0;
SWORD 	detTableNo2 = 0;
SWORD 	tbl_no_tmp2 = 0;
LONG 	sync_cnt2;

//------------- 촬영 종료 오류
BYTE	isCaptureClosing;

//---- AEC
int isOnAEC = 0;

//---- Tracking CO
t_TrackingCO 	g_tc;
ULONG	g_measureTime;
extern BYTE g_TcOption;		// from cammand_mode.c



double A1_curve_arr[140] = {
    0.000000, 0.000000, 0.000279, 0.000579, 0.000903, 0.001251, 0.001627, 0.002031, 0.002466, 0.002935, 
    0.003440, 0.003984, 0.004569, 0.005200, 0.005878, 0.006608, 0.007394, 0.008240, 0.009150, 0.010129, 
    0.011182, 0.012315, 0.013533, 0.014842, 0.016250, 0.017763, 0.019389, 0.021135, 0.023010, 0.025024, 
    0.027185, 0.029504, 0.031991, 0.034657, 0.037516, 0.040578, 0.043857, 0.047366, 0.051121, 0.055135, 
    0.059425, 0.064005, 0.068893, 0.074105, 0.079659, 0.085572, 0.091862, 0.098547, 0.105644, 0.113171, 
    0.121145, 0.129582, 0.138499, 0.147910, 0.157828, 0.168267, 0.179235, 0.190742, 0.202793, 0.215391, 
    0.228536, 0.242227, 0.256455, 0.271213, 0.286484, 0.302253, 0.318497, 0.335190, 0.352302, 0.369800, 
    0.387645, 0.405797, 0.424211, 0.442841, 0.461636, 0.480546, 0.499519, 0.518500, 0.537437, 0.556277, 
    0.574968, 0.593460, 0.611705, 0.629658, 0.647277, 0.664523, 0.681360, 0.697758, 0.713689, 0.729130, 
    0.744062, 0.758470, 0.772342, 0.785672, 0.798454, 0.810688, 0.822378, 0.833526, 0.844142, 0.854234, 
    0.863814, 0.872896, 0.881493, 0.889621, 0.897296, 0.904535, 0.911357, 0.917777, 0.923815, 0.929488, 
    0.934813, 0.939808, 0.944490, 0.948876, 0.952981, 0.956821, 0.960411, 0.963767, 0.966900, 0.969825, 
    0.972555, 0.975101, 0.977476, 0.979689, 0.981751, 0.983671, 0.985460, 0.987125, 0.988675, 0.990118, 
    0.991459, 0.992707, 0.993868, 0.994948, 0.995951, 0.996884, 0.997750, 0.998556, 0.999305, 1.000000
};

double A1_speed_arr[140] = {
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000
};

double A1_pulse_arr[140] = {
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000, 0.000000, 0.000000,  0.000000, 0.000000, 0.000000, 0.000000, 0.000000
};

/*F**************************************************************************
* NAME:     check_exp_mode
* PURPOSE:	조사스위치를 눌렀을 때 각 촬영 모드별로 다르게 반응한다.
*****************************************************************************/
void check_exp_mode(void)
{

	// AEC가 선택되면 이 루틴 대신에 timer에서 루틴이 작동
	if(isOnAEC == ON) 	
	{
		// mA Gen 통신과 Detector trig 동기화 
		SyncMaInAEC(); 
		return;
	}	


	switch(main_mode)
	{
		case CT_MODE:				exp_capt();			break;  // todo : naming : ExpCapt_CT()
		case CT_SCOUT_MODE:			exp_capt_scout();	break;

		case PANO_STAN_MODE:
		case PANO_BITEWING_MODE:
		case PANO_TMJ1_MODE:
		case PANO_TMJ2_MODE:		exp_table_pano();	break;

		case SCAN_CEPH_LA_MODE:
		case SCAN_CEPH_PA_MODE:		exp_table_ceph();	break;	
		default:	break;
	}
}

// input var. main_mode
void check_exp_AEC_mode(void)
{

	if( (isOnAEC == YES))
	{
		switch(main_mode)
		{
			case CT_MODE:				exp_capt();
										if(ct_capt_end_check)	isOnAEC = NO;		
										break;

			case PANO_STAN_MODE:
			case PANO_BITEWING_MODE:
			case PANO_TMJ1_MODE:
			case PANO_TMJ2_MODE:		exp_table_pano();
										if(tbl_end_chk)	isOnAEC = NO;
										break;

			case SCAN_CEPH_LA_MODE:
			case SCAN_CEPH_PA_MODE:		exp_table_ceph();
										if(tbl_end_chk)	isOnAEC = NO;
										break;

			default:	break;
		}
	}	
}


/*F**************************************************************************************
* NAME:     exp_table_pano
* PURPOSE:	Table을 이용한 촬영 모드에서 조사스위치를 눌렀을 때 이루어지는 동작들을 코딩
*****************************************************************************************/
// todo : exp_push 내부변수화 
void exp_table_pano(void)
{
	static Table_t panoTbl;
	static BYTE isInitialized;


	if( (exp_ok_flag) && (exp_sw_flag || software_exp) )	// prec로 촬영 준비가 완료되고 조사스위치가 눌려졌을 때
	{
		if(!exp_push)		// 이전 루프에서 조사스위치 안 눌러졌었음
		{
		// UartPrintf("pano_tblStep\r\n");
			if(!sound_play_time_end)	return; 	// prec에서 실행하던 음성안내가 완전히 종료된 뒤부터 실행되도록
            if(!software_rdy) 			return;				
		

			exp_push = YES;	// 조사스위치 눌러졌음 ==> 조사스위치 눌린 첫 시점만 써먹기 위해.
		
			SoundPlay(SOUND_CAPT_BGM);	// 촬영 음악
		
			ring_off();
		
			tbl_end_chk = NO;		// "tbl_no <= 0"인 경우에만 YES로 바뀜
		
			//---- table 관련 값 셋팅
			default_clk_cnt_pano();			// 촬영모드별 조건 설정
			panoTbl.idx_expOn = exp_tbl_no;
			panoTbl.idx_detOn = detTableNo;
			panoTbl.timeCnt_sync = sync_cnt;
			panoTbl.idx_expOn2 = exp_tbl_no2;
			panoTbl.idx_detOn2 = detTableNo2;
			panoTbl.timeCnt_sync2 = sync_cnt2;
		
			capt_mode_push_step	= STEP1;
			// capt_mode_release_step	= STEP1;
			capt_pano_sub_step = SUB_STEP1;
			isInitialized = NO;
		
			rdy_lamp_on();	gen_rdy();
		}
		else	//exp_push == YES
		{
				// UartPrintf("pano_tblStep0\r\n");
			switch(capt_mode_push_step)
			{
				case STEP1:
				// UartPrintf("pano_tblStep1\r\n");
					if(!gen_preheat_time_flag)	// 제네레이터 예열 완료되면
					{
					// UartPrintf("pano_tblStep1-1\r\n");
						s_ceph_det_off();
						capt_mode_push_step = STEP2;
					}
				break;
				
				case STEP2:
				// UartPrintf("pano_tblStep2\r\n");
					PCPuts("[rpm_grab] \r\n");	// 촬영단에 디텍터 grab 시작하라고 알림
					capt_mode_push_step = STEP3;
				break;

				case STEP3:
				// UartPrintf("pano_tblStep3\r\n");
					// todo : detector_ready Prec에서 on 됨. ==> prec를 해야 촬영이 되는데 필요없는 루틴인듯
					if(detector_ready) 
					{
						capt_mode_push_step = STEP4;		// 디텍터 준비가 완료되면 촬영 시작
					}
				break;

				case STEP4:
					if(table_loop_flag)	// 5000 Hz 주기로 테이블을 읽어서 속도 가변
					{
				// UartPrintf("pano_tblStep4\r\n");
						// 0.2 ms(5000Hz) 마다 수행되는 루틴이기에 Serial debug등을 사용하면 
						// time delay가 발생하여 가속 프로파일이 제대로 동작하지 않는다. 
						table_loop_flag	= NO;
						
						switch(main_mode)
						{
							case PANO_STAN_MODE:		pano_stan_mode();		break;

							// bitewing_flag = 0 : right, 1: left, 2: both 
							case PANO_BITEWING_MODE:    
	                            if(bitewing_flag == 0)		pano_bitewing_left_mode(panoTbl, &isInitialized, falling_edge_cnt, det_trig_off_cnt); 
								else if(bitewing_flag == 1) pano_bitewing_right_mode(panoTbl, &isInitialized, falling_edge_cnt, det_trig_off_cnt);
	                            else if(bitewing_flag == 2)	pano_bitewing_both_mode(panoTbl, &isInitialized, falling_edge_cnt, det_trig_off_cnt); 
	                        break;


							case PANO_TMJ1_MODE:
							case PANO_TMJ2_MODE:		pano_tmj_mode();		break;

							default:	PCPuts("Undefined main_mode! \r\n"); break;
						}
					}
				break;
			}
		}
	}
	else	// 촬영끝나고 정리 모드
	{
		// todo : 이 조건은 없어도 됨,==> 있어야 함.    exp_ok_flag가 '0'이면 이루틴에 들어오는 거라 exp_push 조건은 없어도 됨.
		if(exp_push)	// 조사스위치를 눌렀다가 떼면
		{
			
			if(tbl_end_chk)	//촬영이 정상적으로 종료(촬영 끝날 때까지 조사스위치를 떼지 않아서 영상을 다 얻었을 경우)되었을 경우
			{				//"tbl_no <= 0"인 경우(정상 종료)에만 table_end_check가 YES로 바뀜
				Close_Capture(PANO_MODE);		// 정상촬영 종료
			}
			else	//	촬영이 중단된 경우(조사가 완료되지 않았는데 조사스위치를 뗀 경우)
			{
				Close_CaptureHalt(PANO_MODE);		// 촬영중단 정리
			}
		}
	}
}


//===================================================================================//
// 	Halt Capture
//===================================================================================//
void Close_CaptureHalt(BYTE capMode)
{
	static haltStep = STEP1;


	switch(haltStep)
	{
		case STEP1:
		// UartPrintf("===> haltStep1 \r\n");
			isCaptureClosing = YES;
			PCPuts("[rpm_swof] \r\n");	// 촬영단에 조사스위치 뗏음을 알림
			software_exp = OFF;	// [pmc_swon] 사용한 뒤에 [pmc_swof] 보내지 않을 경우를 대비하여 추가

			gen_off(); s_ceph_det_off(); lamp_off(); ring_off(); LaserOff();

			capt_tm_flag = NO;
			exp_ok_flag	= NO;
			table_loop_flag	= NO;

			SoundPlay(SOUND_CAPT_STOP);	//촬영이 중단 되었습니다.

			if(capMode == PANO_MODE)
			{
	        	PCPrintf("<det_trg_off = %d> \r\n", det_trig_off_cnt);
	            PCPrintf("<falling_edge = %d> \r\n", falling_edge_cnt);

				// PA_MotorSlowStop();			// 파노축 정지
				Ctrl(PA_SLOW_STOP, NULL);
				Ctrl(RO_SLOW_STOP, NULL);
			}
			else if(capMode == SCAN_CEPH_MODE)
			{
				PCPrintf("<ceph_falling_edge = %d> \r\n", ceph_falling_edge_cnt);

				co_move_const_flag = YES;
				sd_move_const_flag = YES;
			}

			PCPuts("[rpm_exst]	[rpm_expo_stop] \r\n");

			haltStep = STEP2;
		break;

		case STEP2:
            if(sound_play_time_end)
            {
		// UartPrintf("===> haltStep2 \r\n");
                SoundPlay(SOUND_OPEN_EYES);		//눈을 뜨시고 다음안내가 있을 때까지 기다려주세요.
                PCPrintf("Capture Time = %ld ms \r\n", capt_tm_cnt);
                haltStep = STEP3;
            }
		break;

		case STEP3:
		// UartPrintf("===> haltStep3 \r\n");
			Ctrl(TUBE_TEMP_REQ, NULL);	// 튜브(탱크) 온도 확인
            ct_pano_det_off();
            exp_push = NO;		// 이 셋팅으로 한번만 실행하고 이 루틴을 들어오지 않음!!
			isCaptureClosing = NO;
            haltStep = STEP1;
		break;

		default:	PCPuts("Undefined haltStep! \r\n");	break;
	}

}

//===================================================================================//
// 	Close Capture
//===================================================================================//
void Close_Capture(BYTE capMode)
{
	static c_TaskData_t taskRO, taskPA, taskCO_SD;
	static closeStep = STEP1;


	switch(closeStep)
	{
		case STEP1:			// 로테이터축을 환자가 빠져나오기 편한 위치로 이동
			// UartPrintf("===> close step1 \r\n");
			isCaptureClosing = YES;

			SoundPlay(SOUND_CAPT_FINISH);	//촬영이 완료 되었습니다. 
		
			gen_off();	s_ceph_det_off(); lamp_off(); LaserOff();
			PCPuts("[rpm_swof] \r\n");	// 촬영단에 조사스위치 뗏음을 알림

            software_exp = OFF;	// [pmc_swon] 사용한 뒤에 [pmc_swof] 보내지 않을 경우를 대비하여 추가
			exp_ok_flag	= NO;
			
			if(capMode == PANO_MODE)
			{
				// todo renaming  : det_trig_off --> capture tirg num
				// todo renameing : falling_edge --> detetor trig num
	            PCPrintf("Msg: ---- Pano Capture end\r\n");
	            PCPrintf("<capture trig num = %d> \r\n", det_trig_off_cnt);
	            PCPrintf("<detector trig num = %d> \r\n", falling_edge_cnt);
				PCPrintf("<Capture Time = %ld ms> \r\n", capt_tm_cnt);
				PCPrintf("\r\n");

				taskRO.event = EVENT_START;
				taskPA.event = EVENT_START;
			}
			else if(capMode == SCAN_CEPH_MODE)
			{
	            PCPrintf("Msg: ---- Ceph Capture end\r\n");
	            PCPrintf("<capture trig num = %d> \r\n", det_trig_off_cnt);
				PCPrintf("<detector trig num = %d> \r\n", ceph_falling_edge_cnt);
				PCPrintf("<Capture Time = %ld ms> \r\n", capt_tm_cnt);

				taskCO_SD.event = EVENT_START;
			}
		
			closeStep = STEP2;
		break;

		case STEP2:
			// UartPrintf("===> close step2-1 \r\n");
			
			// WDG90의 경우 온도 피드백 기능이 없다. 리미트 여부만 알려준다. 
			Ctrl(TUBE_TEMP_REQ, NULL);	// 튜브(탱크) 온도 확인
			closeStep = STEP4;
		break;

		case STEP4:
			if(sound_play_time_end)
			{
			// UartPrintf("===> close step4-1 \r\n");
				if(main_mode != PANO_TMJ1_MODE)
				{
					SoundPlay(SOUND_CAPT_THANKS);	//촬영에 협조해 주셔서 감사합니다. 수고하셨습니다.
				}
				// 펌웨어 test시 실험장비에서 실제로 Xray를 쏘지 않아서 루틴 수행 안됨
				if(gen_ack_flag == 7)		PCPuts("X-Ray Stable \r\n");
                else if(gen_ack_flag == 8)	PCPuts("X-Ray Unstable \r\n");

				closeStep = STEP5;
			}
		break;

		case STEP5:
			if(capMode == PANO_MODE)
			{
				if(taskRO.isEnd == YES && taskPA.isEnd == YES) closeStep = STEP7;
				else closeStep = STEP5;
			}
			else if(capMode == SCAN_CEPH_MODE)
			{
				if(taskCO_SD.isEnd == YES) closeStep = STEP7;
				else closeStep = STEP5;
			}
		break;

		case STEP7:
			if(sound_play_time_end)
			{
			// UartPrintf("===> close step7 \r\n");
				// todo : STEP1에서 처리해도 되지 않는가? 여기까지 사용하는가?
		        ct_pano_det_off();
	            exp_push = NO;
				PCPuts("[rpm_exed]	[rpm_capt_end] \r\n");
				PCPrintf("----------------------------------- Capture End! \r\n");
				isCaptureClosing = NO;
	            closeStep = STEP1;	
			}
		break;

		default:	PCPuts("Undefined closeStep! \r\n");	break;
	}


	if(capMode == PANO_MODE)
	{
		Close_TaskRO(&taskRO);
		Close_TaskPA(&taskPA);
	}
	else if(capMode == SCAN_CEPH_MODE)
	{
		Close_TaskCO_SD(&taskCO_SD);
	}
}


//===================================================================================//
// 	Close 동작시 PA Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void Close_TaskPA(c_TaskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = ST_IDLE;


	switch(currState)
	{
		case ST_IDLE:
			currState = ST_Idle(pTaskData);
		break;

		case ST_START:
		// UartPrintf("==> PA ST_START \r\n");

			// todo magic number : 
			// todo : ro_end_angle 옵션과 pa축 멈추는 것의 관계는?
			if(pano_ro_end_angle == 3) PA_MotorStop();
			else PA_MotorSlowStop();					// 파노축 정지
		
		  	currState = ST_CHK_STOP;
		break;

		case ST_CHK_STOP:
			if( (gvPA.stop_flag) )
			{
				lastTime = TimeNow();
				currState = ST_GO_END_POS;
			}
		break;

		case ST_GO_END_POS:
			if(TimePassed(lastTime) > ONE_SECOND)	// 반대방향으로 가기전에 1초 기다림
			{
				switch(main_mode)
				{
					case PANO_STAN_MODE:
						// 덴트리 축 충돌때문에?? 다른 이유??  ==> 덴트리 충돌때문인듯 
						if(pa_axis_end_position)	Ctrl(PA_MOVE, (LONG)150);
					break;
					
					case PANO_BITEWING_MODE:
					case PANO_TMJ1_MODE:
					break;
					
					case PANO_TMJ2_MODE:
						
						// 촬영 종료 후에 15mm 위치로 이동 ??? 가는 이유는?
						// 덴트리 축 충돌때문에?? 다른 이유??  ==> 덴트리 충돌때문인듯 
						if(pa_axis_end_position)	Ctrl(PA_MOVE, (LONG) 150);
					break;
					
					default:	PCPuts("Undefined main_mode! \r\n");	break;
				}

				currState = ST_CHK_FINISH;
			}
		break;

		case ST_CHK_FINISH:
			if(gvPA.stop_flag == YES)
			{
				// UartPrintf("==> PA ST_CHK_FINISH \r\n");
				currState = ST_FINISH;
			}
		break;

		case ST_FINISH:
			// UartPrintf("==> PA ST_FINISH \r\n");
			currState = ST_Finish(pTaskData);
		break;

		// default:	UartPrintf("Undefined PA State : %d !! \r\n", currState);	break;
	}
}

//===================================================================================//
// 	Close 동작시 RO 동작 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void Close_TaskRO(c_TaskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = ST_IDLE;


	switch(currState)
	{
		case ST_IDLE:
			currState = ST_Idle(pTaskData);
		break;

		case ST_START:
		// UartPrintf("==> RO ST_START \r\n");
			if(pano_ro_end_angle == 3)		// pa none???
			{
				// Ctrl(RO_MOVE_CONST, (LONG)0);	// ????
				Ctrl(RO_SLOW_STOP, NULL);			// 로테이터축 정지
			}
			else
			{
				Ctrl(RO_SLOW_STOP, NULL);			// 로테이터축 정지
			}
		  	currState = ST_CHK_STOP;
		break;

		case ST_CHK_STOP:
			if( (gvRO.stop_flag) )
			{
				lastTime = TimeNow();
				currState = ST_GO_END_POS;
			}
		break;

		case ST_GO_END_POS:
			if(TimePassed(lastTime) > ONE_SECOND)	// 반대방향으로 가기전에 1초 기다림
			{
				switch(main_mode)
				{
					case PANO_STAN_MODE:
						//--------------------------------------------- 로테이터축 0도 또는 180도 위치로 회전
						if(pano_ro_end_angle == 0)		Ctrl(RO_MOVE, (LONG) -2);
						else if(pano_ro_end_angle == 1)	Ctrl(RO_MOVE, (LONG) 900);
						else							Ctrl(RO_MOVE, (LONG) 1800);	// 180도 또는 none 옵션일 경우
					break;
					
					case PANO_BITEWING_MODE:
						Ctrl(RO_MOVE, (LONG) 0);
					break;

					case PANO_TMJ1_MODE:
					break;
					
					case PANO_TMJ2_MODE:
						//--------------------------------------------- 로테이터축 0도 위치로 회전
						Ctrl(RO_MOVE, (LONG) 0);
					break;
					
					default:	PCPuts("Undefined main_mode! \r\n");	break;
				}

				currState = ST_CHK_FINISH;
			}
		break;

		case ST_CHK_FINISH:
			if(gvRO.stop_flag == YES)
			{
				// UartPrintf("==> RO ST_CHK_FINISH \r\n");
				currState = ST_FINISH; ST_Finish(pTaskData);
			}
		break;

		case ST_FINISH:
			// UartPrintf("==> RO ST_FINISH \r\n");
			currState = ST_Finish(pTaskData);
		break;

		default:	UartPrintf("Undefined RO State : %d !! \r\n", currState);	break;
	}
}


//============================================================
//  State Functions
//============================================================

//============================================================
// 대기(Idle)상태에서 외부 이벤트 start가 들어오면 MT_START 상태로 전이
//============================================================
int ST_Idle(c_TaskData_t *pTaskData)
{
	if(pTaskData->event == EVENT_START) {
		// initailize
		pTaskData->isEnd = NO;
		pTaskData->numError = 0;
		pTaskData->reqCnt = 0;

		// clear event 
		pTaskData->event = EVENT_CLEAR;

		// UartPrintf("==> addr pTaskData %lx \r\n", pTaskData);
		return ST_START;
	}
	
	return ST_IDLE;
}

int ST_Finish(c_TaskData_t *pTaskData)
{
	pTaskData->isEnd = YES;
	return ST_IDLE;
}


/****************************************************************************
* NAME:     
*****************************************************************************/
void pano_bitewing_left_mode(Table_t tbl, BYTE *isInitialized, WORD detTrigCnt, WORD capTrigNum)
{
	static unsigned int	timeCnt;
	static unsigned int currState;
	static unsigned int tableIdx;				//table index


	// 촬영중단등으로 currState가 중간에 끝났을 때를 위해 
	// 처음 함수 시작할 때 static 변수 초기화 한다.
	if( *isInitialized == NO)
	{
		timeCnt = 0;
		tableIdx = 0;
		currState = ST_INIT;
		*isInitialized = YES;
	}

	//------ check of tableIdx 
	if(timeCnt >= pano_stan_cnt[pano_nor_tbl_optn][tableIdx])	//109
	{
		timeCnt = 0;	
		++cur_tbl_cnt; 
		++tableIdx;
	}
	++timeCnt;
	
	
	//-------- 궤적 table에 의한 PA, RO 궤적 동작
	if(timeCnt == 1)
	{
		PA_MotorChangeSpeed(PA_POS_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tableIdx]);
		RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_stan_rfreq[pano_nor_tbl_optn][tableIdx]);
	}


	//--------- capture 상태루틴
	switch(currState)
	{
		case ST_INIT:
			// UartPrintf("ST_INIT : tableIdx : %u \r\n", tableIdx);
			tableIdx = 0;
			currState = ST_EXP_ON;
		break;

		case ST_EXP_ON:	
			if( (tableIdx == tbl.idx_expOn) && (timeCnt == 1) ) 
			{
			// UartPrintf("ST_EXP_ON : tableIdx : %u \r\n", tableIdx);
				exp_lamp_on();
				gen_cont_exp();
				currState = ST_DET_ON;
			}	
		break;
		
		case ST_DET_ON:
			if( (tableIdx == tbl.idx_detOn) && (timeCnt == tbl.timeCnt_sync) )
			{
			// UartPrintf("ST_DET_ON : tableIdx : %u \r\n", tableIdx);
				// timeCnt_syn는 detector unstable pulse 2개를 ring on 신호보다 
				// 앞에서 나오게 하기위한 변수
				ct_pano_det_capt();
				currState = ST_RING_ON;
			}

		break;
		
		case ST_RING_ON:	
			// 유효 capture tableIdx는 detOn idx 다음 index다. ring on 신호와 동기화(측정 디버그용)
			if( (tableIdx > tbl.idx_detOn) && (timeCnt == 1) )
			{
			// UartPrintf("ST_RING_ON : tableIdx : %u \r\n", tableIdx);	
				ring_on();
				capt_tm_flag = YES;			// capture 시간을 재기 시작
				currState = ST_CAPTURE_END;
			}
		break;

		case ST_CAPTURE_END:
			// 촬영갯수를 획득하면 촬영정리 
			// capTrigNum(얻을 유효 이미지 갯수), detTrigCnt(ring on 구간동안 det trig count)
			// 실제 det trig count 수는 2개가 더 많다.(유효 이미지 갯수 + 2(불안정 이미지))
			if(detTrigCnt >= capTrigNum)
			{
			// UartPrintf("ST_CAPUTURE_END : tableIdx : %u \r\n", tableIdx);	
				ring_off();	gen_off();	lamp_off();	ct_pano_det_off();

				capt_tm_flag = NO;	
				exp_ok_flag = NO;	// 조사 스위치를 계속 누르고 있어도 반응하지 않기 위해
				tbl_end_chk = YES;
				tableIdx = 0;
				currState = ST_INIT;
			}
		break;

		default:	break;
	}
	
}

/****************************************************************************
* NAME:     
*****************************************************************************/
void pano_bitewing_right_mode(Table_t tbl, BYTE *isInitialized, WORD detTrigCnt, WORD capTrigNum)
{
	static unsigned int	timeCnt;
	static unsigned int currState;
	static unsigned int tableIdx;				//table index
	static BYTE isIndexDecrease;


	// 촬영중단등으로 currState가 중간에 끝났을 때를 위해 
	// 처음 함수 시작할 때 static 변수 초기화 한다.
	if( *isInitialized == NO)
	{
		timeCnt = 0;
		tableIdx = 0;
		currState = ST_INIT;
		isIndexDecrease = NO;
		*isInitialized = YES;
	}

	//------ check of tableIdx 
	if(timeCnt >= pano_stan_cnt[pano_nor_tbl_optn][tableIdx])	//109
	{
		timeCnt = 0;	

		if(isIndexDecrease == NO) 	++tableIdx;
		if(isIndexDecrease == YES) 	--tableIdx;
	}
	++timeCnt;
	
	
	//-------- 궤적 table에 의한 PA, RO 궤적 동작
	if(timeCnt == 1)
	{
		RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_stan_rfreq[pano_nor_tbl_optn][tableIdx]);

		if(isIndexDecrease == NO)	// index가 증가하는 상태이면 PA는 양의 방향으로 이동
			PA_MotorChangeSpeed(PA_POS_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tableIdx]);
		else						// index가 감소하는 상태이면 PA는 음의 방향으로 이동
			PA_MotorChangeSpeed(PA_NEG_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tableIdx]);
	}


	//--------- capture 상태루틴
	switch(currState)
	{
		case ST_INIT:
			// UartPrintf("ST_INIT : tableIdx : %u \r\n", tableIdx);
			tableIdx = 0;
			currState = ST_DEC_INDEX;
		break;

		case ST_DEC_INDEX:
			if(tableIdx == (pano_tbl_no-1))
			{
			// UartPrintf("ST_DEC_INDEX : tableIdx : %u \r\n", tableIdx);
				isIndexDecrease = YES;
				++tableIdx;// 마지막 tableIdx를 초기값으로 시작하기 위해, 없으면 (tableIdx-1)값으로 초기시작된다. 
				currState = ST_EXP_ON;
			}
		break;

		case ST_EXP_ON:	
			if( (tableIdx == tbl.idx_expOn2) && (timeCnt == 1) ) 
			{
			// UartPrintf("ST_EXP_ON : tableIdx : %u \r\n", tableIdx);
				exp_lamp_on();
				gen_cont_exp();
				currState = ST_DET_ON;
			}	
		break;
		
		case ST_DET_ON:
			if( (tableIdx == tbl.idx_detOn2) && (timeCnt == tbl.timeCnt_sync) )
			{
			// UartPrintf("ST_DET_ON : tableIdx : %u \r\n", tableIdx);
				ct_pano_det_capt();
				currState = ST_RING_ON;
			}
		break;
		
		case ST_RING_ON:	
			if( (tableIdx < tbl.idx_detOn2) && (timeCnt == 1) )
			{
			// UartPrintf("ST_RING_ON : tableIdx : %u \r\n", tableIdx);	
				ring_on();
				capt_tm_flag = YES;			// capture 시간을 재기 시작
				currState = ST_CAPTURE_END;
			}
		break;

		case ST_CAPTURE_END:
			if(detTrigCnt >= capTrigNum)
			{
			// UartPrintf("ST_CAPUTURE_END : tableIdx : %u \r\n", tableIdx);	
				ring_off();	gen_off();	lamp_off();	ct_pano_det_off();

				capt_tm_flag = NO;	
				exp_ok_flag = NO;	// 조사 스위치를 계속 누르고 있어도 반응하지 않기 위해
				tbl_end_chk = YES;
				tableIdx = 0;
				currState = ST_INIT;
			}
		break;

		default:	break;
	}
	
}

/****************************************************************************
* NAME:     
*****************************************************************************/
// todo : 입력매개변수들을 구조체 포인터로 받아서 정리
void pano_bitewing_both_mode(Table_t tbl, BYTE *isInitialized, WORD detTrigCnt, WORD capTrigNum)
{
	static unsigned int	timeCnt;
	static unsigned int currState;
	static unsigned int tableIdx;				//table index
	static unsigned int tableIdx_capEnd;
	static BYTE isIndexDecrease;


	// 촬영중단등으로 currState가 중간에 끝났을 때를 위해 
	// 처음 함수 시작할 때 static 변수 초기화 한다.
	if( *isInitialized == NO)
	{
		timeCnt = 0;
		tableIdx = 0;
		currState = ST_INIT;
		isIndexDecrease = NO;
		*isInitialized = YES;
	}

	//------ check of tableIdx 
	if(timeCnt >= pano_stan_cnt[pano_nor_tbl_optn][tableIdx])	//109
	{
		timeCnt = 0;	

		if(isIndexDecrease == NO) 	++tableIdx;
		if(isIndexDecrease == YES) 	--tableIdx;
	}
	++timeCnt;
	
	
	//-------- 궤적 table에 의한 PA, RO 궤적 동작
	if(timeCnt == 1)
	{
		RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_stan_rfreq[pano_nor_tbl_optn][tableIdx]);

		if(isIndexDecrease == NO)	// index가 증가하는 상태이면 PA는 양의 방향으로 이동
			PA_MotorChangeSpeed(PA_POS_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tableIdx]);
		else						// index가 감소하는 상태이면 PA는 음의 방향으로 이동
			PA_MotorChangeSpeed(PA_NEG_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tableIdx]);
	}


	//--------- capture 상태루틴
	switch(currState)
	{
		case ST_INIT:
			// UartPrintf("ST_INIT : tableIdx : %u \r\n", tableIdx);
			tableIdx = 0;
			currState = ST_EXP_ON;
		break;

		case ST_EXP_ON:
			if( (tableIdx == tbl.idx_expOn) && (timeCnt == 1) ) 
			{
			// UartPrintf("ST_EXP_ON : tableIdx : %u \r\n", tableIdx);
				exp_lamp_on();
				gen_cont_exp();
				currState = ST_DET_ON;
			}
		break;

		case ST_DET_ON:
			if( (tableIdx == tbl.idx_detOn) && (timeCnt == tbl.timeCnt_sync) )
			{
			// UartPrintf("ST_DET_ON : tableIdx : %u \r\n", tableIdx);
				ct_pano_det_capt();
				currState = ST_RING_ON;
			}
		break;

		case ST_RING_ON:
			if( (tableIdx > tbl.idx_detOn) && (timeCnt == 1) )
			{
			// UartPrintf("ST_RING_ON : tableIdx : %u \r\n", tableIdx);	
				ring_on();
				capt_tm_flag = YES;			// capture 시간을 재기 시작
				currState = ST_CAPTURE_END;
			}
		break;

		case ST_CAPTURE_END:
			if(detTrigCnt >= capTrigNum)
			{
			// UartPrintf("ST_CAPUTURE_END : tableIdx : %u \r\n", tableIdx);	
				ring_off();	
				gen_off();
				ct_pano_det_off();
				tableIdx_capEnd = tableIdx;

				// capt_tm_flag = NO;	
				// exp_ok_flag = NO;	// 조사 스위치를 계속 누르고 있어도 반응하지 않기 위해
				// tbl_end_chk = YES;
				// tableIdx = 0;
				currState = ST_EXP_RDY;
			}
		break;

		case ST_EXP_RDY:
			// gen_off()후 gen_rdy() 활성화에 시간이 필요, 어느정도 필요한지는 확인 필요, 
			// '40'은 TMJ 코드에서 table idx 40개 만큼 해서 따옴
			if(tableIdx >= tableIdx_capEnd + 40)	
			{
			// UartPrintf("ST_EXP_RDY : tableIdx : %u \r\n", tableIdx);	
				gen_rdy(); 
				rdy_lamp_on();
				currState = ST_DEC_INDEX;
			}
		break;

		case ST_DEC_INDEX:
			if(tableIdx == (pano_tbl_no-1))
			{
			// UartPrintf("ST_DEC_INDEX : tableIdx : %u \r\n", tableIdx);	
				isIndexDecrease = YES;
				++tableIdx;// 마지막 tableIdx를 초기값으로 시작하기 위해, 없으면 (tableIdx-1)값으로 초기시작된다. 
				currState = ST_EXP_ON2;
			}
		break;

		case ST_EXP_ON2:	
			if( (tableIdx == tbl.idx_expOn2) && (timeCnt == 1) ) 
			{
			// UartPrintf("ST_EXP_ON2 : tableIdx : %u \r\n", tableIdx);
				exp_lamp_on();
				gen_cont_exp();
				currState = ST_DET_ON2;
			}	
		break;
		
		case ST_DET_ON2:
			if( (tableIdx == tbl.idx_detOn2) && (timeCnt == tbl.timeCnt_sync) )
			{
			// UartPrintf("ST_DET_ON2 : tableIdx : %u \r\n", tableIdx);
				ct_pano_det_capt();
				currState = ST_RING_ON2;
			}

		break;
		
		case ST_RING_ON2:	
			if( (tableIdx < tbl.idx_detOn2) && (timeCnt == 1) )
			{
			// UartPrintf("ST_RING_ON2 : tableIdx : %u \r\n", tableIdx);	
				ring_on();
				capt_tm_flag = YES;			// capture 시간을 재기 시작
				currState = ST_CAPTURE_END2;
			}
		break;

		case ST_CAPTURE_END2:
			if( detTrigCnt >= capTrigNum*2 )
			{
			// UartPrintf("ST_CAPUTURE_END2 : tableIdx : %u \r\n", tableIdx);	
				ring_off();	gen_off();	lamp_off();	ct_pano_det_off();

				capt_tm_flag = NO;	
				exp_ok_flag = NO;	// 조사 스위치를 계속 누르고 있어도 반응하지 않기 위해
				tbl_end_chk = YES;
				tableIdx = 0;
				isIndexDecrease = NO;
				currState = ST_INIT;
			}
		break;

		default:	break;
	}
	
}

/****************************************************************************
* NAME:     
*****************************************************************************/
// todo function parameter : func(exp_start, exp_end, capt_start, capt_end) 또는 구조체 param : func(ctlTrajet_t pano)
// todo localize variables : xf_cnt, cur_tbl_cnt, capt_pano_sub_step, tbl_no, tbl_no_tmp, ...
void pano_bitewing_mode(void)
{
	++xf_cnt;

	if(cur_tbl_cnt < pano_tbl_no)	//파노촬영 처음부터 절반까지(파노 테이블 순서대로 첨부터 끝까지 수행)
	{
		if(xf_cnt < 2)	// xf_cnt == 1
		{
			switch(capt_pano_sub_step)
			{
				case SUB_STEP1:
					if(tbl_no > exp_tbl_no)	// ex) tbl_no == 10
					{
					// UartPrintf("bitewing tblStep 1-1 : tbl_no : %d \r\n", tbl_no);	
						
						if( (kv_value) && (ma_value) )	
						{
							exp_lamp_on();
							gen_cont_exp();
						}
						
						capt_pano_sub_step = SUB_STEP2;
					}
				break;
				
				// detector capture를 시작 단계. tbl_no가 det_tbl_no크고, 
				// 세부 카운트 xf_cnt가 sync_cnt와 맞을 때 detector 시작
				case SUB_STEP2:
					if(tbl_no > detTableNo)	// det_tbl_no
					{
					// UartPrintf("bitewing tblStep 2-1 : tbl_no : %d \r\n", tbl_no);	

        				// buzzer_flag = 2;    // pano mode 부저 동작 시작
						capt_tm_flag = YES;	ring_on();
						
						// 아래 detecter on을 시키기 위한 루틴을 위해 설정
						// todo naming : det_tbl_no, detStart_tbl_no, ...
						// ?? -1은 왜 했을까?? 배열 index 0, 
						// tbl_no_tmp 값은 38은 step3에서 사용됨
						tbl_no_tmp = pano_tbl_no - 50;

						capt_pano_sub_step = SUB_STEP3;
					}
				break;
				
				case SUB_STEP3:
					if(tbl_no > tbl_no_tmp)
					{
         				capt_pano_sub_step = SUB_STEP11;
					}
				break;
				
				default:	break;
			}

			PA_MotorChangeSpeed(PA_POS_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tbl_no]);
			RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_stan_rfreq[pano_nor_tbl_optn][tbl_no]);
		}
		else if(xf_cnt == sync_cnt)	//	59
		{
			if(capt_pano_sub_step == SUB_STEP2)	
			{	
				// tbl_no_tmp 값은 37로 default_clk_cnt_pano()에서 셋팅된 값임.
				if(tbl_no == tbl_no_tmp)	
				{
		// UartPrintf("bitewing tblStep (2) : tbl_no : %d, tbl_no_tmp : %d \r\n", tbl_no, tbl_no_tmp);
					ct_pano_det_capt();	
				}
			}
		}
		else if(xf_cnt >= pano_stan_cnt[pano_nor_tbl_optn][tbl_no])	//109
		{
		// UartPrintf("bitewing tblStep (3) : tbl_no : %d \r\n", tbl_no);	
			xf_cnt = 0;	++cur_tbl_cnt;
			
			++tbl_no;	// pano_stan_pfreq[pa_tbl_sel][0]을 실행한 뒤에 tbl_no 증가시켜야 함
			if(tbl_no >= pano_tbl_no)	--tbl_no;	// tbl_no PANO_STAN_TABLE과 같아지면 다음 번 비교에 문제 발생
		}

		// det_trig_off_cnt 값에 따라 촬영영역이 결정되는데, bitewing의 값은 어찌 정하면 되나?		
		if(falling_edge_cnt > det_trig_off_cnt)
		{
		// UartPrintf("bitewing tblStep (4) : tbl_no : %d, falling_edge_cnt : %d \r\n", tbl_no, falling_edge_cnt);	
			capt_tm_flag = NO;	ring_off();	gen_off(1);	lamp_off();
			falling_edge_cnt = 0;
		}
	}
	else		//파노촬영 절반 지난 지점부터 끝까지(파노 테이블 거꾸로 반복)
	{
		if(xf_cnt < 2)
		{

			switch(capt_pano_sub_step)
			{
				case SUB_STEP11:
					if(tbl_no < exp_tbl_no2)
					{
		// UartPrintf("bitewing tblStep 11-1 : tbl_no : %d, falling_edge_cnt : %d \r\n", tbl_no, falling_edge_cnt);	
                        if(bitewing_flag == 2) 	// bitewing 양쪽 촬영인 경우에만 두번째 촬영 조사
                        {        
    						if( (kv_value) && (ma_value) )
    						{
    							exp_lamp_on();
    							gen_cont_exp(1);
    						} 
    					}

						capt_pano_sub_step = SUB_STEP12;
					}
				break;
				
				case SUB_STEP12:
					if(tbl_no < tbl_no_tmp2)
					{
		// UartPrintf("bitewing tblStep 12-1 : tbl_no : %d \r\n", tbl_no);	
                        if(bitewing_flag == 2) {        // bitewing 양쪽 촬영인 경우에만 두번째 촬영 부저음 동작 및 조사시간 증가
        				    // buzzer_flag = 2;
						    capt_tm_flag = YES; }
                        ring_on();

						capt_pano_sub_step = SUB_STEP13;
					}
				break;
				
				default:	break;
			}

			PA_MotorChangeSpeed(PA_NEG_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tbl_no]);
			RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_stan_rfreq[pano_nor_tbl_optn][tbl_no]);
		}
		else if(xf_cnt >= pano_stan_cnt[pano_nor_tbl_optn][tbl_no])
		{
		// UartPrintf("bitewing tblStep (4) : tbl_no : %d \r\n", tbl_no);	
			xf_cnt = 0;	++cur_tbl_cnt;
			
			if(tbl_no > 0)	--tbl_no;
		}

		// 촬영갯수를 획득하면 촬영정리
		if(falling_edge_cnt > det_trig_off_cnt)
		{
			ring_off();	gen_off();	lamp_off();	ct_pano_det_off();

			capt_tm_flag = NO;	
			exp_ok_flag = NO;	// 조사 스위치를 계속 누르고 있어도 반응하지 않기 위해
			tbl_end_chk = YES;
		}
	}
}


/****************************************************************************
* NAME:     pano_stan_mode
*****************************************************************************/
void pano_stan_mode(void)
{
	++xf_cnt;
	
	if(cur_tbl_cnt < pano_tbl_no)	//파노촬영 처음부터 절반까지(파노 테이블 순서대로 첨부터 끝까지 수행)
	{
		if(xf_cnt < 2)	// xf_cnt == 1
		{
			switch(capt_pano_sub_step)
			{
				case SUB_STEP1:
					if(tbl_no > exp_tbl_no)	// ex) tbl_no == 10
					{
					// UartPrintf("pano tblStep 1-1 : tbl_no : %d \r\n", tbl_no);	
						exp_lamp_on();
						
						if( (kv_value) && (ma_value) )	gen_cont_exp();
						
						capt_pano_sub_step = SUB_STEP2;
					}
				break;
				
				case SUB_STEP2:
					if(tbl_no > tbl_no_tmp)	// ex) tbl_no == 20
					{
					// UartPrintf("pano tblStep 2-1 : tbl_no : %d, PAcycle: %ld \r\n", tbl_no, PA_GetSpeed());	
        				// buzzer_flag = 2;    // pano mode 부저 동작 시작
						ring_on();	capt_tm_flag = YES;
						
						tbl_no_tmp = pano_stan_capt_start_table_num - 1;
						
						capt_pano_sub_step = SUB_STEP11;
					}
				break;
				
				default:	break;
			}

			PA_MotorChangeSpeed(PA_POS_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tbl_no]);
			RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_stan_rfreq[pano_nor_tbl_optn][tbl_no]);

		}
		else if(xf_cnt == sync_cnt)	//	59
		{
		// UartPrintf("pano tblStep (2) : tbl_no : %d \r\n", tbl_no);	
			if(capt_pano_sub_step == SUB_STEP2)	{	if(tbl_no == tbl_no_tmp)	ct_pano_det_capt();	}
		}
		else if(xf_cnt >= pano_stan_cnt[pano_nor_tbl_optn][tbl_no])	//109
		{
		// UartPrintf("pano tblStep (3) : tbl_no : %d \r\n", tbl_no);	
			xf_cnt = 0;	++cur_tbl_cnt;
			
			++tbl_no;	// pano_stan_pfreq[pa_tbl_sel][0]을 실행한 뒤에 tbl_no 증가시켜야 함
			if(tbl_no >= pano_tbl_no)	--tbl_no;	// tbl_no PANO_STAN_TABLE과 같아지면 다음 번 비교에 문제 발생
		}
	}
	else		//파노촬영 절반 지난 지점부터 끝까지(파노 테이블 거꾸로 반복)
	{
		if(xf_cnt < 2)
		{
			switch(capt_pano_sub_step)
			{
				case SUB_STEP11:
					if((falling_edge_cnt > det_trig_off_cnt) || (tbl_no < tbl_no_tmp)) 
					{
						// UartPrintf("pano tblStep 11-1 : tbl_no : %d, PAcycle: %ld \r\n", tbl_no, PA_GetSpeed());
						capt_tm_flag = NO;
						
						Ctrl(X_RAY_EXP_STATE_REQ, (LONG) 0);

						ring_off();	gen_off(1);	lamp_off();
                        ct_pano_det_off();

                        tbl_end_chk = YES;
						
						exp_ok_flag = NO;	// 조사 스위치를 계속 누르고 있어도 반응하지 않기 위해
						
						capt_pano_sub_step = SUB_STEP12;
					}
				break;
				
				default:	break;
			}
			
			//A0_setting_cycle = 1;	A0_drive_const(PA_NEG_DIR, (LONG)pano_stan_pfreq[pa_tbl_sel][tbl_no]);
			PA_MotorChangeSpeed(PA_NEG_DIR, (LONG)pano_stan_pfreq[pano_nor_tbl_optn][tbl_no]);
			RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_stan_rfreq[pano_nor_tbl_optn][tbl_no]);

			if(debug_mode)
			{
				// UartPrintf("tbl no : %u \r\n", tbl_no);
				UartPrintf("tbl no : %u,  PA speed : %ld, RO speed : %ld \r\n", tbl_no, PA_GetSpeed(), RO_GetSpeed() );
			}
		}
		else if(xf_cnt >= pano_stan_cnt[pano_nor_tbl_optn][tbl_no])
		{
		// UartPrintf("pano tblStep (4) : tbl_no : %d \r\n", tbl_no);	
			xf_cnt = 0;	++cur_tbl_cnt;
			
			if(tbl_no > 0)	--tbl_no;
		}
	}

}

/****************************************************************************
* NAME:     pano_tmj_mode
*****************************************************************************/
void pano_tmj_mode(void)
{
	++xf_cnt;

	if(cur_tbl_cnt < PANO_TMJ_TBL)
	{	
		if(xf_cnt < 2)
		{
						//--------------------------------------------------------------------------------------------
			switch(capt_pano_sub_step)
			{
				case SUB_STEP1:
					if(tbl_no > exp_tbl_no)					// 20 - 12 = 8
					{
						exp_lamp_on();
						
						if( (kv_value) && (ma_value) )	gen_cont_exp();
						
						capt_pano_sub_step = SUB_STEP2;
					}
				break;
				
				case SUB_STEP2:
					if(tbl_no > tbl_no_tmp) //
					{
        				// buzzer_flag = 2;    // pano mode 부저 동작 시작
						ring_on();	capt_tm_flag = YES;
						
						capt_pano_sub_step = SUB_STEP3;
					}
				break;
				
				case SUB_STEP3:
					if(tbl_no > pano_tmj_capt_end_table_num)	// 210 
					{
						gen_off();	lamp_off();	ring_off();	capt_tm_flag = NO;
						
						tbl_no_tmp = pano_tmj_capt_end_table_num + 29;
						
						capt_pano_sub_step = SUB_STEP4;
					}
				break;
				
				case SUB_STEP4:
					if(tbl_no > tbl_no_tmp)					// 239
					{
						tbl_no_tmp = pano_tmj_capt_end_table_num + 39;
						
						capt_pano_sub_step = SUB_STEP5;
					}
				break;
				
				case SUB_STEP5:
					if(tbl_no > tbl_no_tmp)					// 249
					{
						rdy_lamp_on();	gen_rdy();
						
						tbl_no_tmp = pano_tmj_capt_end_table_num + 16;
						
						capt_pano_sub_step = SUB_STEP11;
					}
				break;
				
				default:	break;
			}
			
			PA_MotorChangeSpeed(PA_POS_DIR, (LONG)pano_tmj_pfreq[pano_tmj_tbl_optn][tbl_no]);
			if(main_mode == PANO_TMJ1_MODE)	RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_tmj_rfreq[pano_tmj_tbl_optn][tbl_no]);
			else							RO_MotorChangeSpeed(RO_NEG_DIR, (LONG)pano_tmj_rfreq[pano_tmj_tbl_optn][tbl_no]);

		}
		else if(xf_cnt == sync_cnt)
		{
			if(capt_pano_sub_step == SUB_STEP2)	{	if(tbl_no == tbl_no_tmp)	ct_pano_det_capt();	}
		}
		else if(xf_cnt >= pano_tmj_cnt[pano_tmj_tbl_optn][tbl_no])
		{
			xf_cnt = 0;	++cur_tbl_cnt;
			
			++tbl_no;
			if(tbl_no >= PANO_TMJ_TBL)	--tbl_no;
		}
	}
	else
	{
		if(xf_cnt < 2)
		{
			//--------------------------------------------------------------------------------------------
			switch(capt_pano_sub_step)
			{
				case SUB_STEP11:
					if(tbl_no < tbl_no_tmp)		// 226
					{
						exp_lamp_on();
						
						if( (kv_value) && (ma_value) )	gen_cont_exp();
						
						tbl_no_tmp = pano_tmj_capt_end_table_num + 1;
						//				210 + 1 = 211
						
						capt_pano_sub_step = SUB_STEP12;
					}
				break;
				
				case SUB_STEP12:
					if(tbl_no < tbl_no_tmp)		// 211
					{
        				// buzzer_flag = 2;    // pano mode 부저 동작 시작
						ring_on();	capt_tm_flag = YES;
						
						tbl_no_tmp = pano_tmj_capt_end_table_num + 4; 	// 210 + 4 = 214
						
						capt_pano_sub_step = SUB_STEP13;
					}
				break;
				
				case SUB_STEP13:
					if(tbl_no < tbl_no_tmp)		// 214
					{
						Ctrl(X_RAY_EXP_STATE_REQ, NULL);
						
						capt_pano_sub_step = SUB_STEP14;
					}
				break;
				
				case SUB_STEP14:
					if(tbl_no < pano_tmj_capt_start_table_num)	// 20
					{
						gen_off();	lamp_off();	ring_off();	capt_tm_flag = NO;
						
						capt_pano_sub_step = SUB_STEP15;
					}
				break;
				
				case SUB_STEP15:
					if(tbl_no < 1)
					{
					// UartPrintf("==> tmjSetp 15-1 : tbl_no : %d \r\n", tbl_no);	
                        ct_pano_det_off();
						
						tbl_end_chk = YES;
						
						exp_ok_flag = NO;
						
						capt_pano_sub_step = SUB_STEP16;
					}
				break;
				
				default:	break;
			}
			
			PA_MotorChangeSpeed(PA_NEG_DIR, (LONG)pano_tmj_pfreq[pano_tmj_tbl_optn][tbl_no]);
			if(main_mode == PANO_TMJ1_MODE)	RO_MotorChangeSpeed(RO_POS_DIR, (LONG)pano_tmj_rfreq[pano_tmj_tbl_optn][tbl_no]);
			else							RO_MotorChangeSpeed(RO_NEG_DIR, (LONG)pano_tmj_rfreq[pano_tmj_tbl_optn][tbl_no]);
		}
		else if(xf_cnt == sync_cnt)
		{
			if(capt_pano_sub_step == SUB_STEP12)	{	if(tbl_no == tbl_no_tmp)	ct_pano_det_capt();	}
		}
		else if(xf_cnt >= pano_tmj_cnt[pano_tmj_tbl_optn][tbl_no])
		{
			xf_cnt = 0;	++cur_tbl_cnt;
			
			if(tbl_no > 0)	--tbl_no;
		}
	}	
}


/*F**************************************************************************
* NAME:     default_clk_cnt_pano
* PURPOSE:
*****************************************************************************/
// Qface 함수와 덴트리 값을 조정
void default_clk_cnt_pano(void)
{
	const LONG unstableImage = 2;
	LONG timePer_tbl_no;
	const LONG timeStableXray = 200;		// generator X-ray 조사 안정시간(ms)

	cur_tbl_cnt = 0;
	tbl_no = 0;
	xf_cnt = 0;
	
	can_seq_cnt = 0;
	
    falling_edge_cnt = 0; vw_exp_ok_cnt = 0;
	
	switch(main_mode)
	{
		case PANO_STAN_MODE:
		case PANO_BITEWING_MODE:
		case PANO_TMJ1_MODE:
		case PANO_TMJ2_MODE:
			// PA_tot_pulse는 파노라마 궤적 중에서 P축이 가장 많이 움직이는 궤적의 왕복 이동 거리를 기준으로 설정
			// 현재는 궤적을 만드는 중이므로 기구적인 최대 값으로 임시 적용
			// 기구적으로 P축의 왕복 이동 거리는 P축 최대 이동 거리의 2배 이하
			//
			// eco X는 PA 리드 5mm를 사용.
			// A0_tot_pulse <= (PA_P_MAX_PULSE_2 + PA_N_MAX_PULSE_2) * 2
			//				<= (345600 + 25600) * 2
			//				<= 742,400
			// 컬럼부에 부딪칠 수 있을 듯... 

			PA_increase_stage(2);	
			PA_stop_cnt(-1);	
			// 궤적등으로 인해 PA가 움직일 수 있는 제한 거리(PA축이 컬럼과 충돌이 안되는 거리값으로 정함
			// todo : 마법의 숫자를 제거하고 수식으로 만들기
			PA_tot_pulse(307200);	// 덴트리 값은 452000, eco x값은 742,400
			
			RO_increase_stage(2);	
			RO_stop_cnt(-1);	
			RO_tot_pulse(gvRO.p_max_pulse);


            trig_pulse_setting(FRAME_PANO);
		break;
		
		
		default: break;
	}

	switch(main_mode)
	{
		case PANO_STAN_MODE:
		case PANO_BITEWING_MODE:	//일단은 pano_stan모드와 같게 설정
			// pano_trig_num 은 2790으로 초기화 되어 있는데, 값의 원리는? 
			// bitewing의 값은??
            det_trig_off_cnt = pano_trig_num;     // Teeth4.7: 2790 frame, multilayer는??
		break;

		case PANO_TMJ1_MODE:
		case PANO_TMJ2_MODE:	break;

		default: PCPuts("Undefined main_mode!\r\n");	 break;
	}
	//-------------------------------------------------------------------------------------
	// ring 신호를 이용하는 방법은 영상 획득 시점이 정확하지 않을 수 있음을 검증하였으므로,
	// 원하는 시점부터 정확히 영상을 얻기 위해서는 영상 획득을 시작하고자 하는 시점부터
	// 디텍터 트리거 신호를 출력해야 함.
	// 다만, 처음 두 개 정도의 프레임은 다크 커런트의 영향을 받기 때문에
	// 영상 획득 시점 이전에 두 개의 트리거를 추가로 출력하고 처음 두 개의 영상은 버림
	// (dentri의 경우) x-ray 처음 조사후 안정시간이 200ms 필요하므로 영상획득 전 200ms 부터 조사시작
	// 각 원소의 cnt값이 100이라 하면 0.2ms * 100 = 20ms  ==> 10개 tbl_no가 필요
	// (Qface 의 경우) WDG-90은 50ms 전부터 조사시작 
	//
	// 
	// 파노라마 촬영은 200 fps
	//	==> 디텍터 트리거의 주기는 1/200 = 5 ms
	//	==> 트리거 두 개가 출력되는 시간은 5 x 2 = 10 ms
	// 촬영을 위한 함수는 0.2 ms 마다 반복 실행됨
	// 현재 사용 중인 테이블 중에서 카운트가 50 이하인 값은 없음
	//	==> 촬영 직전의 카운트 테이블 값을 활용
	switch(main_mode)
	{
		case PANO_STAN_MODE:
		case PANO_BITEWING_MODE:
			//------- 덴트리 코드
		/*	// '12'의 의미는? 10번째 전과 불안정 detector trig 2개를 의미?? 아닌듯.
			exp_tbl_no = pano_stan_capt_start_table_num - 12;	// 영상 획득 10번째 테이블 전에 해당하는 시점부터 조사 시작
			// tbl_no_tmp는 detector triger 시작 tbl_no
			// '2'의 의미는?
			tbl_no_tmp = pano_stan_capt_start_table_num - 2;
			// '50' 은 detector unstable 2개 trig 10ms에 대한 카운트
			sync_cnt = (LONG)pano_stan_cnt[pano_nor_tbl_optn][tbl_no_tmp] - 50;		//	
		*/	//------------------
			

			//---------- Qface 코드 
		  	// dentri 구조(숫자를 통해 계산)에서 ==> Qface구조(식을 이용해 계산) 형태로 바꿈 			
			// timePer_tbl_no : 테이블 번호당 소요 시간[ms](카운트 테이블의 원소가 모두 동일하다는 가정)
			// ex) 125cnt * (0.2 ms) = 25ms
			timePer_tbl_no = (LONG)pano_stan_cnt[pano_nor_tbl_optn][0] * 2 / 10;	
			// timeStableXray : X-ray 조사 안정시간 덴트리 200ms, 
			// WDG-90 출력이 안정된 상태에서 영상을 획득하려면 실제 영상 획득 200 ms 전부터 조사 시작해야 함
			// '-1'은 if( >= )를 쓰지 않고 if( > )를 쓰기위해 사용함. (일종의 속도 최적화)
			// todo : 속도에 문제 없다면 '-1'을 없애고 읽기 쉽게 바꾼다. ==> 여러 부분 수정이 필요
			// ex) 30 - ( 200ms/25ms ) -1 = 30 - 9 = 21  ==> tbl_no 21에서 x-ray 조사, 근데...
			exp_tbl_no = pano_stan_capt_start_table_num - (timeStableXray / timePer_tbl_no) - 1;
			// ex) if(50%25 == 0) exp_tbl_no = 21 - 1 = 20 ==> tbl_no 20에서 x-ray 조사
			if(timeStableXray % timePer_tbl_no)	exp_tbl_no = exp_tbl_no - 1;
			
			// tbl_no_tmp는 detector triger 시작 tbl_no
			// todo question: '2'의 의미는? detector 시작 table_no에서 왜 2를 빼는가? 
			// 불안정 detector tirg? ==> 아닌듯!!, 속도최적화로 인한 if(<)때문? 그래도 '1'만 빼야하는데...
			// '2'의 의미느 	1) capture start table_no 이전에 불완전 trig 2개를 시작해야 하고
			//				2) 속도최적화로 인한 if(<)때문 '-1'을 하여 
			//              ==> '-2'의 값을 설정됨.
			// todo : 
			tbl_no_tmp = pano_stan_capt_start_table_num - 2;
			//bitewing에서 사용할 변수 테스트후 리팩토링시 전체 적용!!
			detTableNo = tbl_no_tmp;

			// '50' 은 detector unstable 2개 trig 10ms에 대한 카운트
			// sync_cnt는 detector on table_no에서 불안정 2개 detector pulse 버린 후의 타이밍
			sync_cnt = (LONG)pano_stan_cnt[pano_nor_tbl_optn][tbl_no_tmp] - 50;
			
			// ?? 왜 5ms를 곱하나? ==> detector trig 주기(200fps)!!
			// todo delete magic number : 5 -> periodDetTrig, 50 -> unstableDetTrigCnt
			// ex) (800 * 5 ms) / 25 ms + 101 = 261
			tbl_no_tmp2 = ((det_trig_off_cnt * 5) / timePer_tbl_no) + pano_stan_capt_start_table_num;	
			detTableNo2 = tbl_no_tmp2;
			exp_tbl_no2 = tbl_no_tmp2 + (timeStableXray / timePer_tbl_no);		// 261 + (200 / 25) = 269
			sync_cnt2 = (LONG)pano_stan_cnt[pano_nor_tbl_optn][tbl_no_tmp2] - 50;
			if(timeStableXray % timePer_tbl_no)	exp_tbl_no2 = exp_tbl_no2 + 1;

		break;

		
		case PANO_TMJ1_MODE:
		case PANO_TMJ2_MODE:
			// todo delete magic number ==> 위 수식형태로
			exp_tbl_no = pano_tmj_capt_start_table_num - 12;	// start idx 20 - 12 = 8
			
			tbl_no_tmp = pano_tmj_capt_start_table_num - 2;	

			//'-50'은 0.2ms * 50 = 10ms 를 의미하고 2개 detetcor trigger를 계산하기위해
			// bug : table cnt의 요소가 50 미만일 경우 '-'값이 되어 버그발생(TMJ fast의 경우)	
			//  ==> 임시 방편으로 sync_cnt를 0으로 한다. 
			//  이 경우 2개 unstable detector trigger 시간을 확보못하고, 1개만 확보됨.
			sync_cnt = (LONG)pano_tmj_cnt[pano_tmj_tbl_optn][tbl_no_tmp] - 50;
			if(sync_cnt < 2) sync_cnt = 2;

		break;

		default: PCPuts("Undefined main_mode!\r\n");	 break;
	}

	if(debug_mode)
	{

		PCPrintf("Msg:----- Pano setting value\r\n");
		PCPrintf("exp_tbl_no = %d \r\n", exp_tbl_no);
		PCPrintf("det_tbl_no = %d \r\n", tbl_no_tmp);
		PCPrintf("sync_cnt = %ld \r\n", sync_cnt);

		if(main_mode == PANO_BITEWING_MODE)
		{
			PCPrintf("exp_tbl_no2 = %d \r\n", exp_tbl_no2);
			PCPrintf("det_tbl_no2 = %d \r\n", tbl_no_tmp2);
			PCPrintf("sync_cnt2 = %ld \r\n", sync_cnt2);
			PCPrintf("\r\n");
		}

		UartPrintf("\r\n");
	}

}


/*F**************************************************************************************
* NAME:     exp_table_ceph
* PURPOSE:	Table을 이용한 촬영 모드에서 조사스위치를 눌렀을 때 이루어지는 동작들을 코딩
*****************************************************************************************/
// todo : exp_push 내부변수화 
void exp_table_ceph(void)
{

	static ULONG lastTime, lastTime_dark;
	static Table_t cephTbl;


	if( (exp_ok_flag) && (exp_sw_flag || software_exp) )	// prec로 촬영 준비가 완료되고 조사스위치가 눌려졌을 때
	{
		if(!exp_push)		// 이전 루프에서 조사스위치 안 눌러졌었음
		{
			if(!sound_play_time_end)	return; 	// prec에서 실행하던 음성안내가 완전히 종료된 뒤부터 실행되도록
            if(!software_rdy) 			return;				
		

			exp_push = YES;	// 조사스위치 눌러졌음 ==> 조사스위치 눌린 첫 시점만 써먹기 위해.
		
			SoundPlay(SOUND_CAPT_BGM);	// 촬영 음악
		
			ring_off();
		
			tbl_end_chk = NO;		// "tbl_no <= 0"인 경우에만 YES로 바뀜
		
			// todo : 변수를 리턴 변수로 함수내로 넣기
			default_clk_cnt_ceph();			// 촬영모드별 조건 설정
		
			capt_mode_push_step	= STEP1;
			capt_mode_release_step	= STEP1;
			capt_pano_sub_step = SUB_STEP1;
		
			rdy_lamp_on();	gen_rdy();
		}
		else	//exp_push == YES
		{
			switch(capt_mode_push_step)
			{
				case STEP1:
					// UartPrintf("intblStep1\r\n");
					if(!gen_preheat_time_flag)	// 제네레이터 예열 완료되면
					{
					// UartPrintf("intblStep1-1\r\n");
						s_ceph_det_off();

						capt_mode_push_step = STEP2;
					}
				break;
				
				case STEP2:
					// UartPrintf("intblStep2\r\n");
					PCPuts("[rpm_grab] \r\n");	// 촬영단에 디텍터 grab 시작하라고 알림
					
					capt_mode_push_step = STEP3;
				break;

				case STEP3:
					// UartPrintf("intblStep3\r\n");
					// todo : detector_ready 는 사용되는 곳이 없음. 삭제해도 될듯
					if(detector_ready) 
					{
						// caputure 전에 CO의 position 값 요청
						// rcan_puts(CO_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg);  dly_10us(70);
						// rcan_puts(SD_MOTOR, CAN_AXIS_CNT, (LONG) 0, dmy_msg);  dly_10us(70);
						capt_mode_push_step = STEP4;		// 디텍터 준비가 완료되면 촬영 시작
					}
				break;

				case STEP4:
					if(table_loop_flag)	// 5000 Hz 주기로 테이블을 읽어서 속도 가변
					{
					// UartPrintf("intblStep4-1\r\n");
						table_loop_flag	= NO;
						
						switch(main_mode)
						{
							// case SCAN_CEPH_LA_MODE:	scan_ceph_la_mode();	break;
							case SCAN_CEPH_LA_MODE:	scan_ceph_la_mode();	break;
							case SCAN_CEPH_PA_MODE:	scan_ceph_pa_mode();	break;
							
							default:	PCPuts("Undefined main_mode! \r\n"); break;
						}
						
						can_seq_move2();
					}
				break;
				
				default: break;
			}
		}
	}	
	else	// 촬영끝나고 정리 모드
	{
		if(exp_push)	// 조사스위치를 눌렀다가 떼면
		{
			if(tbl_end_chk)	//촬영이 정상적으로 종료(촬영 끝날 때까지 조사스위치를 떼지 않아서 영상을 다 얻었을 경우)되었을 경우
			{				//"tbl_no <= 0"인 경우(정상 종료)에만 table_end_check가 YES로 바뀜
				Close_Capture(SCAN_CEPH_MODE);
			}
			else	//	촬영이 중단된 경우(조사가 완료되지 않았는데 조사스위치를 뗀 경우)
			{
				Close_CaptureHalt(SCAN_CEPH_MODE);
			}
		}
	}

	// ?? 무슨 기능???  -> scan CAN axis 멈추는 기능..??
	// todo : 기능 분석 필요,can_seq_move()와 함께 쓰인다. 
	can_seq_stop();
}

//===================================================================================//
// 	Close 동작시 CO Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void Close_TaskCO_SD(c_TaskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = ST_IDLE;


	switch(currState)
	{
		case ST_IDLE:
			currState = ST_Idle(pTaskData);
		break;

		case ST_START:
		// UartPrintf("==> CO_SD ST_START \r\n");
			// co_move_const_flag는 can_seq_stop()에서 속도를 '0'(stop)으로 하는 flag
			co_move_const_flag = YES;
			sd_move_const_flag = YES;
		  	currState = ST_CHK_STOP;
		break;

		case ST_CHK_STOP:
			if( (gvCO.stop_flag) && (gvSD.stop_flag) )
			{
				lastTime = TimeNow();
				currState = ST_WAIT_1S;
			}
			else
			{
                if(!gvCO.stop_flag)		{ 	if(!co_move_const_flag)	co_move_const_flag = YES;	}
                else if(!gvSD.stop_flag){	if(!sd_move_const_flag)	sd_move_const_flag = YES;	}
			}
		break;

		case ST_WAIT_1S:
			if(TimePassed(lastTime) > ONE_SECOND)	// 반대방향으로 가기전에 1초 기다림
			{
				// 원래 offset 위치로 
				Ctrl(SD_MOVE, (LONG) 0);
				if(main_mode == SCAN_CEPH_LA_MODE) 	Ctrl(CO_MOVE_ACCU, gvCO.scla_offset_pos);
				if(main_mode == SCAN_CEPH_PA_MODE) 	Ctrl(CO_MOVE_ACCU, gvCO.scpa_offset_pos);

				currState = ST_CHK_FINISH;
			}
		break;

		case ST_CHK_FINISH:
			if( gvCO.stop_flag && gvSD.stop_flag)
			{
				// UartPrintf("==> CO_SD ST_CHK_FINISH \r\n");
				currState = ST_FINISH;
			}
		break;

		case ST_FINISH:
			// UartPrintf("==> CO_SD ST_FINISH \r\n");
			currState = ST_Finish(pTaskData);
		break;

		default:	UartPrintf("Undefined CO, SD State : %d !! \r\n", currState);	break;
	}
}



/****************************************************************************
* NAME:     scan_ceph_la_mode
*****************************************************************************/
void scan_ceph_la_mode(void)
{

	BYTE sw = OFF;		// 함수내에서 debug 메세지 on/off 용

	++xf_cnt;

	if(tbl_no >= S_CEPH_LA_TBL)			// early return
	{
		co_move_const_flag = YES;
		sd_move_const_flag = YES;	
		return;
	}

	//------------------- (tbl_no < S_CEPH_LA_TBL)으면 아래 실행

	if(xf_cnt < 2)			// xf_cnt 가 1일때 한번 실행, xf_cnt 가 222일때마다 초기화
	{
				
		// todo naming : capt_pano_sub_step -> capt_ceph_sub_step
		switch(capt_pano_sub_step)
		{
			case SUB_STEP1:
				if(tbl_no > exp_tbl_no)		// exp_tbl_no = 5 : 조사하는 tbl_no
				{
				Debug(sw, "===> x-ray on : tbl_no : %d \r\n", tbl_no);
					exp_lamp_on();
					
					if( (kv_value) && (ma_value) )	gen_cont_exp();
					
					capt_pano_sub_step = SUB_STEP2;
				}
			break;
			
			case SUB_STEP2:
				if(tbl_no > detTableNo)	// 10 > 9	// tbl_no_tmp = 20 : capture하는 deetec on하는 넘버
				{
				Debug(sw, "===> ring on : tbl_no : %d \r\n", tbl_no);
					ring_on();	capt_tm_flag = YES;
					capt_pano_sub_step = SUB_STEP3;
				}
			break;
			
			case SUB_STEP3:
				// 설정된 이미지 갯수 20개(5ms * 20 = 100ms)이전이나 
				// 가속테이블 감속 시작구간부터. '20'은 (start Index -1) 현재 궤적에는 가속 20개, 감속 20개있음
				if(ceph_falling_edge_cnt > (det_trig_off_cnt - 20) || tbl_no > (S_CEPH_LA_TBL - 20)) 	
				{
				Debug(sw, "===> req x-ray state : tbl_no : %d \r\n", tbl_no);

					Ctrl(X_RAY_EXP_STATE_REQ, (LONG) 0);
					capt_pano_sub_step = SUB_STEP4;
				}
			break;
			
			case SUB_STEP4:
				// 설정된 capture 이미지 갯수보다 트리거된 수가 많거나, 가속테이블 감속구간에 들어가면
				if( (ceph_falling_edge_cnt > det_trig_off_cnt) || (tbl_no > (S_CEPH_LA_TBL - 20)) )	// '20'은 table의 감속구간 부분
				{
				Debug(sw, "===> capture end : tbl_no : %d \r\n", tbl_no);
					capt_tm_flag = NO;
					ring_off();	gen_off();	lamp_off();	s_ceph_det_off();
					capt_pano_sub_step = SUB_STEP5;
				}
			break;
			
			case SUB_STEP5:
				// 가속 table 끝 index가 되면 촬영 완료
				if( tbl_no >= (S_CEPH_LA_TBL - 1) )
				{
				Debug(sw, "===> move end : tbl_no : %d \r\n", tbl_no);
					tbl_end_chk	= YES;
					exp_ok_flag = NO;
					capt_pano_sub_step = SUB_STEP6;
				}
			break;
		}

		can_tbl_no = tbl_no;
		
		gvCO.stop_flag = NO;		// stop_flag를 clear 시켜 움직임 상태임을 
		gvSD.stop_flag = NO;	

		co_can_flag = YES;
		sd_can_flag = YES;	
		
	}
	else if(xf_cnt == sync_cnt)
	{
		if(capt_pano_sub_step == SUB_STEP2)
		{	
			if(tbl_no == tbl_no_tmp)	
			{	
			Debug(sw, "===> detector on : tbl_no : %d \r\n", tbl_no);
					s_ceph_det_exp_capt();	
			}	
		}
	}
	else if(xf_cnt >= scan_ceph_la_cnt[sc_tbl_sel_1][tbl_no])
	{
	// Debug(sw, "table initial...\r\n");
		xf_cnt = 0;
		++tbl_no;
	}

}

/****************************************************************************
* NAME:     scan_ceph_pa_mode
*****************************************************************************/
void scan_ceph_pa_mode(void)
{
	++xf_cnt;


	if(tbl_no >= S_CEPH_PA_TBL)			// early return
	{
		co_move_const_flag = YES;
		sd_move_const_flag = YES;	
		return;
	}
	


	if(xf_cnt < 2)
	{

		switch(capt_pano_sub_step)
		{
			case SUB_STEP1:
				if(tbl_no > exp_tbl_no)
				{
					exp_lamp_on();
					
					if( (kv_value) && (ma_value) )	gen_cont_exp();
					
					capt_pano_sub_step = SUB_STEP2;
				}
			break;
			
			case SUB_STEP2:
				// if(tbl_no > tbl_no_tmp)
				if(tbl_no > detTableNo)
				{
					ring_on();	capt_tm_flag = YES;
					
					// tbl_no_tmp = S_CEPH_PA_TBL - s_ceph_pa_capt_start_tbl_num;
					
					capt_pano_sub_step = SUB_STEP3;
				}
			break;
			
			case SUB_STEP3:
				// if(tbl_no > tbl_no_tmp)
				// 설정된 이미지 갯수 20개(5ms * 20 = 100ms)이전이나 
				// 가속테이블 감속 시작구간부터. '20'은 (start Index -1), 현재 궤적에는 가속 20개, 감속 20개있음
				if(ceph_falling_edge_cnt > (det_trig_off_cnt - 20) || tbl_no > (S_CEPH_PA_TBL - 20)) 	
				{
					Ctrl(X_RAY_EXP_STATE_REQ, (LONG) 0);
					capt_pano_sub_step = SUB_STEP4;
				}
			break;
			
			case SUB_STEP4:
				if(tbl_no > tbl_no_tmp)
				{
					capt_tm_flag = NO;
					ring_off();	gen_off();	lamp_off();	s_ceph_det_off();
					capt_pano_sub_step = SUB_STEP5;
				}
			break;
			
			case SUB_STEP5:
				// 가속 table 끝 index가 되면 촬영 완료
				if(tbl_no >= (S_CEPH_PA_TBL - 1) )
				{
					tbl_end_chk	= YES;
					exp_ok_flag = NO;
					capt_pano_sub_step = SUB_STEP6;
				}
			break;
		}

		can_tbl_no = tbl_no;
		
		gvCO.stop_flag = NO;		// stop_flag를 clear 시켜 움직임 상태임을 
		gvSD.stop_flag = NO;	

		co_can_flag = YES;
		sd_can_flag = YES;	
	}
	else if(xf_cnt == sync_cnt)
	{
		if(capt_pano_sub_step == SUB_STEP2)
		{	
			if(tbl_no == tbl_no_tmp)	
			{	
					s_ceph_det_exp_capt();	
			}	
		}
	}
	else if(xf_cnt >= scan_ceph_pa_cnt[sc_tbl_sel_1][tbl_no])
	{
		xf_cnt	= 0;
		tbl_no++;
	}
}

/*

void scan_ceph_pa_mode_old(void)
{
	++xf_cnt;


	if(tbl_no >= S_CEPH_PA_TBL)			// early return
	{
		co_move_const_flag = YES;
		sd_move_const_flag = YES;	
		return;
	}
	


	if(xf_cnt < 2)
	{

		switch(capt_pano_sub_step)
		{
			case SUB_STEP1:
				if(tbl_no > exp_tbl_no)
				{
					exp_lamp_on();
					
					if( (kv_value) && (ma_value) )	gen_cont_exp();
					
					capt_pano_sub_step = SUB_STEP2;
				}
			break;
			
			case SUB_STEP2:
				// if(tbl_no > tbl_no_tmp)
				if(tbl_no > detTableNo)
				{
					ring_on();	capt_tm_flag = YES;
					
					tbl_no_tmp = S_CEPH_PA_TBL - s_ceph_pa_capt_start_tbl_num;
					
					capt_pano_sub_step = SUB_STEP3;
				}
			break;
			
			case SUB_STEP3:
				if(tbl_no > tbl_no_tmp)
				{
					capt_tm_flag = NO;
					
					Ctrl(X_RAY_EXP_STATE_REQ, (LONG) 0);
					
					tbl_no_tmp = tbl_no_tmp + 1;
					
					capt_pano_sub_step = SUB_STEP4;
				}
			break;
			
			case SUB_STEP4:
				if(tbl_no > tbl_no_tmp)
				{
					ring_off();	gen_off();	lamp_off();	s_ceph_det_off();
					
					tbl_no_tmp = S_CEPH_PA_TBL - 2;
					
					capt_pano_sub_step = SUB_STEP5;
				}
			break;
			
			case SUB_STEP5:
				if(tbl_no > tbl_no_tmp)
				{
					tbl_end_chk	= YES;
					
					exp_ok_flag = NO;
					
					capt_pano_sub_step = SUB_STEP6;
				}
			break;
		}

		can_tbl_no = tbl_no;
		
		gvCO.stop_flag = NO;		// stop_flag를 clear 시켜 움직임 상태임을 
		gvSD.stop_flag = NO;	

		co_can_flag = YES;
		sd_can_flag = YES;	
	}
	else if(xf_cnt == sync_cnt)
	{
		if(capt_pano_sub_step == SUB_STEP2)
		{	
			if(tbl_no == tbl_no_tmp)	
			{	
					s_ceph_det_exp_capt();	
			}	
		}
	}
	else if(xf_cnt >= scan_ceph_pa_cnt[sc_tbl_sel_1][tbl_no])
	{
		xf_cnt	= 0;
		tbl_no++;
	}
}
*/

/*F**************************************************************************
* NAME:     default_clk_cnt_ceph
* PURPOSE:
*****************************************************************************/
void default_clk_cnt_ceph(void)
{
	WORD 	cur_tbl_cnt = 0;	// 사용이 안되는 듯

	cur_tbl_cnt = 0;
	tbl_no = 0;
	xf_cnt = 0;			// todo rocal var. table 사용함수내에서만 사용됨.
	
	can_seq_cnt = 0;  	// todo rocal var. can_seq_cnt() 함수내에서만 사용됨.
    
    ceph_falling_edge_cnt = 0;	falling_edge_cnt = 0; vw_exp_ok_cnt = 0;

	switch(main_mode)
	{
		
		case SCAN_CEPH_LA_MODE:
		case SCAN_CEPH_PA_MODE:
			trig_pulse_setting(20);  // sc_det_optn == TD_X_2301
		break;
		
		default: break;
	}

	// detector trig 이미지 갯수
	det_trig_off_cnt = ceph_trig_num;	

	//-------------------------------------------------------------------------------------
	// ring 신호를 이용하는 방법은 영상 획득 시점이 정확하지 않을 수 있음을 검증하였으므로,
	// 원하는 시점부터 정확히 영상을 얻기 위해서는 영상 획득을 시작하고자 하는 시점부터
	// 디텍터 트리거 신호를 출력해야 함.
	// 다만, 처음 두 개 정도의 프레임은 다크 커런트의 영향을 받기 때문에
	// 영상 획득 시점 이전에 두 개의 트리거를 추가로 출력하고 처음 두 개의 영상은 버림
	// 
	// 파노라마 촬영은 200 fps
	//	==> 디텍터 트리거의 주기는 1/200 = 5 ms
	//	==> 트리거 두 개가 출력되는 시간은 5 x 2 = 10 ms
	// 촬영을 위한 함수는 0.2 ms 마다 반복 실행됨
	// 현재 사용 중인 테이블 중에서 카운트가 50 이하인 값은 없음
	//	==> 촬영 직전의 카운트 테이블 값을 활용
	switch(main_mode)
	{
		
		case SCAN_CEPH_LA_MODE:
			// scan_ceph_la_cnt의 모든 원소값은 동일함
			// 테이블 촬영용 함수는 1/(5000 Hz) = 0.2 ms 마다 한 번씩 반복(xf_cnt 증가)
			// 속도프로파일 scan_ceph_la_cnt의 값이 200이면 tbl_no가 하나 증가하는 시간은 0.2ms * 200 = 40ms
			//
			// 영상 획득 200 ms 이전부터 x-ray 조사를 시작해야 영상 획득 시점에서 안정된 x-ray가 출력됨을 보장
			// 200 ms / 40ms = 5,  따라서 tbl_no_tmp보다 5이전에서 조사를 시작해야 200ms이 보장됨
			//
			// detector trig 초반 2개 이미지가 안정적이지 않아 2개를 획득이미지 타이밍 전에 2개의 detecter trig 발생 필요
			// trig 주기는 200fps,  1/200 = 5ms, 2개의 trig 시간은 10ms!!  
			// ==> table_no는 40ms 주기이니 tbl_no로 제어할 수 없고, 원소 내부 값에서 계산해서 정해야 함.(syn_cnt)
			// 
			// 원소 내부값에서 detector 2  trig에 해당하는 값는 (10ms / 40ms) * 카운트값(200) = 50
			// xf_cnt의 경우 0.2ms 주기로 되기때문에 sync_cnt 의 경우 UartPrintf()함수등에 의해 시간 딜레이가 발생함. 
			// 디버그시 오실로 스코프를 사용할 것!!
			exp_tbl_no = (1000 / scan_ceph_la_cnt[sc_tbl_sel_1][0]) + 1;			// nor = 222, fast = 111   1000/222 + 1 = 5, 1000/111 + 1 = 10
			exp_tbl_no = s_ceph_la_capt_start_tbl_num - exp_tbl_no - 1;			// 조건이 초과이어서 '-1'을 함
			
			// todo naming : tbl_no_tmp -> det_trig_tbl_no  : detector on 시작 tbl_no
			// tbl_no_tmp = s_ceph_la_capt_start_tbl_num - 2;	// '-2'는  detector trig 초반 2개 이미지가 안정적이지 않아서 획득이미지 전에 2개를 의미			
			tbl_no_tmp = s_ceph_la_capt_start_tbl_num - 1;	// '-1'는  detector on을 하는 table_no
			// todo : 변수 변경 이상 없으면 tbl_no_tmp 대신 사용
			detTableNo = tbl_no_tmp;			
			sync_cnt = (LONG)scan_ceph_la_cnt[sc_tbl_sel_1][tbl_no_tmp] - 50;  // '50'은 2개의 trig 시간인 10ms를 의미 ( = 0.2ms * 50)
		break;
		
		case SCAN_CEPH_PA_MODE:
			exp_tbl_no = (1000 / scan_ceph_pa_cnt[sc_tbl_sel_1][0]) + 1;
			exp_tbl_no = s_ceph_pa_capt_start_tbl_num - exp_tbl_no -1;
			
			tbl_no_tmp = s_ceph_pa_capt_start_tbl_num - 1;
			detTableNo = tbl_no_tmp;
			sync_cnt = (LONG)scan_ceph_pa_cnt[sc_tbl_sel_1][tbl_no_tmp] - 50;
		break;
	}

	if(debug_mode)
	{
		PCPrintf("Msg:----- Ceph setting value\r\n");
		PCPrintf("exp_tbl_no = %d \r\n", exp_tbl_no);
		PCPrintf("det_tbl_no = %d \r\n", tbl_no_tmp);
		PCPrintf("sync_cnt = %ld \r\n", sync_cnt);
		PCPrintf("\r\n");
	}
}


/*F**************************************************************************
* NAME:     exp_capt
* PURPOSE:	CT 촬영 모드에서 조사스위치를 눌렀을 때 이루어지는 동작들을 코딩
*****************************************************************************/
void exp_capt_scout(void)
{
	LONG temp = 0;

	// exp_ok_flag가 Yes가 되는 것은 command_mode.c에서 3개의 경우(CTmode, Panomode, prec)
	// NO가 되는 곳은 굉장히 많다. (조사를 멈추기 위한 상황들이 많다. )
	if( (exp_ok_flag) && (exp_sw_flag || software_exp) )	
	{
		// 이전 루프에서 조사스위치 안 눌러졌었음, 처음으로 조사스위치가 눌려졌다면, 셋팅작업을 한다. 
		// exp_push - 조사스위치 처리 플래그
		// todo (!exp_push) 초기화 단계로 switch() 상태함수 화 
		if(!exp_push)	
		{

			if(!sound_play_time_end)	return; 	// todo early return
		    if(!software_rdy) return;		// todo : 구형장비에서 필요한 것인데, 여기서 필요한가?

			exp_push = YES;	//촬영 첫 시작시 사용됨. 조사스위치와 관련없음
			ct_capt_end_check = NO;		// 촬영정상 종료 flag  -> naming  : isNoramlTermination_CT_Capt, ...

			SoundPlay(SOUND_CAPT_BGM);	// 촬영 음악

			ct_pano_det_off(); ring_off(); gen_off();
			rdy_lamp_on();	gen_rdy();
			
			capt_mode_push_step = STEP1;	
			capt_mode_release_step = STEP1;

			Set_InitialCaptureValue();
		}
		else	//exp_push == YES
		{
			switch(capt_mode_push_step)
			{
				// RO 시계방향으로 회전시작,(380도를 돌고 360도로 되돌아오도록 되어있다. 
				case STEP1:
					if(!gen_preheat_time_flag)	// 제네레이터 예열 완료되면
					{
				// UartPrintf("captStep1-1 \r\n");
					
						// scout_mode의 angle_end_cw의 각도 230도
                        temp = 2300;
						Ctrl(RO_MOVE, temp);
						capt_mode_push_step = STEP2;
					}
				break;

				case STEP2:	// ring on 신호 보내기 약 500 ms 전부터 촬영단에 영상 획득하라고 메세지 전송
					if(  (temp = RO_GetPositionCnt()) >= ct_capt_msg_cnt)		
					{
				// UartPrintf("captStep2 : ct_capt_msg_cnt : %05ld , RO_pos_cnt: %5ld\r\n", ct_capt_msg_cnt, temp);
						PCPuts("[rpm_grab] \r\n");	// 촬영단에 디텍터 grab 시작하라고 알림
						capt_mode_push_step = STEP4;
					}
				break;


				case STEP4:	// x-ray 조사 초기에는 출력이 불안정하므로 ring on 신호 보내기 약 200 ms 전부터 조사 시작
					if( (temp = RO_GetPositionCnt()) >= ct_capt_exp_start_cnt)
					{
				// UartPrintf("captStep4 : Start X-ary : ct_capt_exp_start_cnt : %05ld, RO_pos_cnt: %5ld, ct_capt_speed_chk2 : %5ld \r\n", ct_capt_exp_start_cnt, temp, ct_capt_speed_chk2);
						verify_2 = temp;	ct_capt_speed_chk2 = RO_GetSpeed();
						exp_lamp_on();
						
						if( (kv_value) && (ma_value) )	gen_pulsed_exp();	// 제네레이터와 디텍터 펄스의 싱크 포함
						
						capt_mode_push_step = STEP6;
					}
				break;

				// RO가 sync_cnt 위치 이상이면 detector trig 시작
				// ring on 시작보다 det trig 2 period + gen mid timing 만큼 빠른 시간에 해당하는 카운트(디텍터 처음 영상은 불안정해서 버림)
				// todo renaming : sync_cnt -> roCnt_genSync
				case STEP6:	// detector trig 시작
					if( (temp = RO_GetPositionCnt()) >= sync_cnt)			// sync_cnt 보다 같거나 높으면... => 유효영상 획득이 시작되면!!
					{
				// UartPrintf("=> captStep6 Start Detector trig : sync_cnt : %05ld, RO_pos_cnt : %5ld \r\n", sync_cnt, temp);
					  	
                        ct_pano_det_capt();   // detector 트리거 시작
                        SetGenPulseCnt(0);   // 

						capt_mode_push_step = STEP7;
					}
				break;

				// RO가 가 ring_on_cnt 위치이상이면 ring_on()을 한다. ct_capt_ring_on_cnt = 0
				case STEP7:	// ring on 각도
					if( (temp = RO_GetPositionCnt()) >= ct_capt_ring_on_cnt)		// ring on 시작위치에 있으면
					{
						verify_3 = temp;
						capt_tm_flag = YES;	ring_on();
						
						ct_capt_speed_chk1 = RO_GetSpeed();	// for Debug
						
						ro_pos_cnt_tmp = ct_capt_end_cnt - 1;
				// UartPrintf("=> captStep7 ring_on : ct_capt_ring_on_cnt : %05ld, ct_capt_end_cnt : %ld, RO_pos_cnt : %5ld \r\n", ct_capt_ring_on_cnt, ct_capt_end_cnt, temp);
						
						// todo : if 부정조건 바꾸기, naming : isScoutMode
						capt_mode_push_step = STEP8;
					}
				
				break;


				case STEP8:	// scout_mode에서는 ct_capt_end_cnt = scout_pulse(스카웃 촬영시간) 임!!
					if(RO_GetPositionCnt() >= ct_capt_end_cnt)
					{
			  // UartPrintf("=> captStep8-1 \r\n");
						//변수 scout_gen_rdy_cnt는 이곳에서만 쓰임 -> local var.

						capt_tm_flag = NO;	ring_off();	gen_off();	lamp_off();
						
						// 180도에 해당하는 카운트 값 (스카웃 모드는 0도와 180도에서 촬영한 영상을 획득함)
						ct_capt_ring_on_cnt = RO_360_DEGREE_PULSE / 2;
						ct_capt_ring_off_cnt = ct_capt_ring_on_cnt + scout_pulse;	// 스카웃 펄스만큼 촬영

						scout_gen_rdy_cnt = ct_capt_speed * 2.5;	// 제네레이터 예열하는데 필요한 시간 2.5 s에 해당하는 펄스 수
						scout_gen_rdy_cnt = ct_capt_ring_on_cnt - scout_gen_rdy_cnt;
						
						ct_capt_exp_start_cnt = ct_capt_speed * 0.2;	// 제네레이터 출력 안정화에 필요한 시간 0.2 s에 해당하는 펄스 수
						ct_capt_exp_start_cnt = ct_capt_ring_on_cnt - ct_capt_exp_start_cnt;
						
					UartPrintf("ring on : %ld, ring off : %ld, scout_pulse : %ld \r\n", 
							ct_capt_ring_on_cnt, ct_capt_ring_off_cnt, scout_pulse);	
						capt_mode_push_step = STEP10;
					}
				break;


				case STEP10:
					if(RO_GetPositionCnt() >= scout_gen_rdy_cnt)
					{
			  // UartPrintf("=> captStep10-1 \r\n");
						rdy_lamp_on();	gen_rdy();
						capt_mode_push_step = STEP12;
					}
				break;


				case STEP12:
					if(RO_GetPositionCnt() >= ct_capt_exp_start_cnt)
					{
			  // UartPrintf("=> captStep12-1 \r\n");
						if( (kv_value) && (ma_value) )	
						{
							exp_lamp_on();
							gen_pulsed_exp();
						}
						
						capt_mode_push_step = STEP14;
					}
				break;


				case STEP14:
					if(RO_GetPositionCnt() >= ct_capt_ring_on_cnt)
					{
			  // UartPrintf("=> captStep14-1 \r\n");
						capt_tm_flag = YES;	ring_on();
						capt_mode_push_step = STEP16;
					}
				break;


				case STEP16:	// scout_mode
					if( (temp = RO_GetPositionCnt()) >= ct_capt_ring_off_cnt)	
					{
			  // UartPrintf("=> captStep16-1 \r\n");
						ring_off();
					UartPrintf("ring off rocnt : %ld \r\n", temp);
						capt_mode_push_step = STEP20;
					}
				break;
				

				// ct_capt_speed_chk3 : ring off시 RO speed
				// 촬영 정상종료 후  ring, gen, lamp off 한후, detector_off() 한후 완료!!
				case STEP20:		
				  // UartPrintf("=> captStep20 Detector off: RO_speed :  %05ld \r\n", RO_GetSpeed());
					verify_6 = RO_GetPositionCnt();	ct_capt_speed_chk3 = RO_GetSpeed();
					
					ring_off();	gen_off();	lamp_off();
					
					// CT_MODE에서 설정된 frame, fps로 되돌린다. 
					ct_capt_frame = pre_ct_capt_frame;	ct_capt_fps = pre_ct_capt_fps;
					main_mode = CT_MODE;
					ct_capt_end_check = YES; // 촬영 정상 종료
					exp_ok_flag	= NO;	// 조사 스위치를 계속 누르고 있어도 반응하지 않기 위해
					ct_pano_det_off();
					capt_mode_push_step = STEP1;
				break;

				default:	PCPuts("Invalid CT-Scout Capture Mode Push Step! \r\n"); 	break;
			}
		}

	}
	else //else : (exp_ok_flag) && (exp_sw_flag || software_exp)
	{
		// exp_push : 조사스위치 처리 플래그
		// 스위티가 눌러져 있을 때, 1) 촬영을 정상 종료 된경우 루틴, 2) 촬영이 중단(captuer가 된 경우 
		if(!exp_push)	return;

		if(ct_capt_end_check)	//	촬영이 정상적으로 종료되었을 경우, capture를 정리한다. 
			Close_CaptureCT();
		else					//	촬영이 중단된 경우 capture를 해제하는 스텝을 밟는다.
			Halt_ExpCapture();
	}


}

/*F**************************************************************************
* NAME:     exp_capt
* PURPOSE:	CT 촬영 모드에서 조사스위치를 눌렀을 때 이루어지는 동작들을 코딩
*****************************************************************************/
void exp_capt(void)
{
	LONG roPosCnt = 0;
	static LONG ctEndAngle = 3800;
	unsigned int captureTime;

	// exp_ok_flag가 Yes가 되는 것은 command_mode.c에서 3개의 경우(CTmode, Panomode, prec)
	// exp_ok_flag : PREC가 되면 'YES'
	// exp_sw_flag : 조사 switch,  software_exp : [pmc_swon] command
	if( (exp_ok_flag) && (exp_sw_flag || software_exp) )	
	{
		
		// 이전 루프에서 조사스위치 안 눌러졌었음, 처음으로 조사스위치가 눌려졌다면, 셋팅작업을 한다. 
		// exp_push - 조사스위치 처리 플래그
		// todo : if(!exp_push) 초기화 단계로 switch() 상태함수 변환 가능
		if(!exp_push)	
		{
			if(!sound_play_time_end)	return; 	// early return
		    if(!software_rdy) 			return;		// todo : 구형장비에서 필요한 것인데, 여기서 필요한가?

			exp_push = YES;				//촬영 첫 시작시 사용됨. 조사스위치와 관련없음
			ct_capt_end_check = NO;		// 촬영정상 종료 flag  -> naming  : isNoramlTermination_CT_Capt, ...

			SoundPlay(SOUND_CAPT_BGM);	// 촬영 음악
			
			ct_pano_det_off(); ring_off(); gen_off();
			rdy_lamp_on();	gen_rdy();
			
			Set_InitialCaptureValue();

			capt_mode_push_step = STEP1;	
			capt_mode_release_step = STEP1;
		}
		else	//exp_push == YES
		{
			switch(capt_mode_push_step)
			{
				case STEP1:
					if(!gen_preheat_time_flag)	// 제네레이터 예열 완료되면
					{
				// UartPrintf("captStep1-1 \r\n");
						captureTime = ct_capt_frame/ct_capt_fps;
						if(captureTime <= 12)	ctEndAngle = 4000;		// 빠른 회전에서는 360도 + 40도
						else 	ctEndAngle = 3800;						// 느린 회전에서는 360도 + 20도

						// Ctrl(RO_MOVE, angle_end_cw);
						Ctrl(RO_MOVE, ctEndAngle);
						capt_mode_push_step = STEP2;
					}
				break;


				case STEP2:	// ring on 신호 보내기 약 500 ms 전부터 촬영단에 영상 획득하라고 메세지 전송
					if(  (roPosCnt = RO_GetPositionCnt()) >= ct_capt_msg_cnt)		
					{
				// UartPrintf("captStep2 : ct_capt_msg_cnt : %05ld , roPosCnt: %5ld\r\n", ct_capt_msg_cnt, roPosCnt);
						PCPuts("[rpm_grab] \r\n");	// 촬영단에 디텍터 grab 시작하라고 알림
						capt_mode_push_step = STEP4;
					}
				break;


				case STEP4:	// x-ray 조사 초기에는 불안정하므로 ring on 신호 약 200 ms 전부터 조사 시작
					if( (roPosCnt = RO_GetPositionCnt()) >= ct_capt_exp_start_cnt)
					{
						verify_2 = roPosCnt;	ct_capt_speed_chk2 = RO_GetSpeed();
				// UartPrintf("captStep4 : Start X-ary : ct_capt_exp_start_cnt : %05ld, RO_pos_cnt: %5ld, ct_capt_speed_chk2 : %5ld \r\n", ct_capt_exp_start_cnt, roPosCnt, ct_capt_speed_chk2);
						exp_lamp_on();
						if( (kv_value) && (ma_value) )	gen_pulsed_exp();	// 제네레이터와 디텍터 펄스의 싱크 포함

						capt_mode_push_step = STEP6;
					}
				break;

				// RO가 sync_cnt 위치 이상이면 detector trig 시작
				// ring on 시작보다 det trig 2 period + gen mid timing 만큼 빠른 시간에 해당하는 카운트(디텍터 처음 영상은 불안정해서 버림)
				case STEP6:	// detector trig 시작
					// if( (roPosCnt = RO_GetPositionCnt()) >= sync_cnt)			// sync_cnt 보다 같거나 높으면... => 유효영상 획득이 시작되면!!
					if( (roPosCnt = RO_GetPositionCnt()) >= ct_capt_det_on_cnt)			// sync_cnt 보다 같거나 높으면... => 유효영상 획득이 시작되면!!
					{
				// UartPrintf("=> captStep6 Start Detector trig : sync_cnt : %05ld, RO_pos_cnt : %5ld \r\n", sync_cnt, roPosCnt);
                        ct_pano_det_capt();   // detector 트리거 시작
                        SetGenPulseCnt(0);   // 
						capt_mode_push_step = STEP7;
					}
				break;

				// RO가 가 ring_on_cnt 위치이상이면 ring_on()을 한다. ct_capt_ring_on_cnt = 0
				case STEP7:	// ring on 각도
					if( (roPosCnt = RO_GetPositionCnt()) >= ct_capt_ring_on_cnt)		// ring on 시작위치에 있으면
					{
				// UartPrintf("=> captStep7 ring_on : ct_capt_ring_on_cnt : %05ld, ct_capt_end_cnt : %ld, RO_pos_cnt : %5ld \r\n", ct_capt_ring_on_cnt, ct_capt_end_cnt, roPosCnt);

						verify_3 = roPosCnt; ct_capt_speed_chk1 = RO_GetSpeed();	// for Debug
						capt_tm_flag = YES;	
						g_measureTime = TimeNow();
						ring_on();
						Ctrl_START_EVENT(2);

						capt_mode_push_step = STEP17;
					}
				break;

				
				case STEP17:				
					if( (roPosCnt = RO_GetPositionCnt()) >= ct_capt_end_cnt)	
					{
				// UartPrintf("=> captStep17 HI~  REQ x-ray : ct_capt_end_cnt : %05ld \r\n", ct_capt_end_cnt);
						capt_tm_flag = NO;	verify_4 = roPosCnt;
						
						// X-ray 상태확인 [pmc_exp?] 
						// ==> 상태확인 메세지를 받는가? 확인해서 무엇을 하는가?? 무엇때문에 확인을하는가?
						Ctrl(X_RAY_EXP_STATE_REQ, (LONG) 0);	// x-ray가 출력되는지 제네레이터에 확인 (UFS에서는 시리얼 메시지 보내고 받을 시간이 부족)
						capt_mode_push_step = STEP19;
					}
				break;

				// falling_edge_cnt 는 timer.c 10us 인터럽트에서 CPU_RING이 셋될때 카운트증가 
				// det_trig_off_cnt = (WORD)ct_capt_frame+3; 720 + 3
				case STEP19:	// 디텍터 트리거 출력 완료
					if(falling_edge_cnt > det_trig_off_cnt)
					{
				// UartPrintf("=> captStep19 falling_edge_cnt :  %05ld \r\n", falling_edge_cnt);
						verify_5 = RO_GetPositionCnt();	capt_mode_push_step = STEP20;
					}
				break;

				// verify_5, verify_6은 A1_pos_cnt 값을 스텝을 달리해 출력하는 데, 디버그용으로 사용되는 듯
				// ct_capt_speed_chk3 : ring off시 RO speed
				// 촬영 정상종료 후  ring, gen, lamp off 한후, detector_off() 한후 완료!!
				case STEP20:		
				  // UartPrintf("=> captStep20 Detector off: RO_speed :  %05ld \r\n", RO_GetSpeed());
					verify_6 = RO_GetPositionCnt();	ct_capt_speed_chk3 = RO_GetSpeed();
					
					ring_off();	
					g_measureTime = TimePassed(g_measureTime);
					gen_off();	lamp_off();
					
					// if(scout_mode)
					// {
					// 	ct_capt_frame = pre_ct_capt_frame;	ct_capt_fps = pre_ct_capt_fps;
					// }
					
					ct_capt_end_check = YES; // 촬영 정상 종료
					exp_ok_flag	= NO;	// 조사 스위치를 계속 누르고 있어도 반응하지 않기 위해
					ct_pano_det_off();
					capt_mode_push_step = STEP1;
				break;

				default:	PCPuts("Invalid CT Capture Mode Push Step! \r\n"); 	break;
			}
		}


	}
	else //else : (exp_ok_flag) && (exp_sw_flag || software_exp)
	{
		// exp_push : 조사스위치 처리 플래그 보다는 촬영루틴 초기화가 되었다. 
		// rename : exp_push --> isInitialCapture
		// 스위티가 눌러져 있을 때, 1) 촬영을 정상 종료 된경우 루틴, 2) 촬영이 중단(captuer가 된 경우 
		if(!exp_push)	return;

		if(ct_capt_end_check)	//	촬영이 정상적으로 종료되었을 경우, capture를 정리한다. 
			Close_CaptureCT();
		else					//	촬영이 중단된 경우 capture를 해제하는 스텝을 밟는다.
			Halt_ExpCapture();
	}

	// 이 루틴 조건은? ==> (co_option == 4axis)  && (TC모드) 
	// 시작 조건은?   ==> (isInitialCaputre(exp_push) == YES) && (RO가 0도를 지날때)
	// 끝나는 조건은? ==> (RO가 360도를 지날때) && ( 촬영이 중단될때)
	// 촬영시간이 끝나면, 촬영 중단되면  마무리 

	if( (co_option == FOUR_AXIS_CO) && (g_TcOption == YES) ) {
		TrackingObj(&g_tc);
	}
}

/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
void Close_CaptureCT(void)
{
	static ULONG lastTime;
	LONG temp = 0;


	switch(capt_mode_release_step)
	{
		// capture 상태현황 print, '촬영이 완료되었습니다.' 셋팅
		// todo : else 문 step1과 중복, todo : 함수화
		case STEP1:			// 로테이터축을 환자가 빠져나오기 편한 위치로 이동
		  // UartPrintf("=> Step1 : print capture status \r\n");
			isCaptureClosing = YES;
            PCPrintf("\r\n Msg: --- CT capture end\r\n");
            PCPrintf("<ct_capt_frame = %ld> \r\n", ct_capt_frame);
            PCPrintf("<det_trg_off = %d> \r\n", det_trig_off_cnt);
            PCPrintf("<falling_edge = %d> \r\n", falling_edge_cnt);
            PCPrintf("\r\n");
		
			PCPuts("[rpm_swof] \r\n");	// 촬영단에 조사스위치 뗏음을 알림
			software_exp = OFF;	// [pmc_swon] 사용한 뒤에 [pmc_swof] 보내지 않을 경우를 대비하여 추가

			
			//----------------------------- X-Ray 조사 중지, 영상 획득 중지, 조사표시등 끔, 촬영음악 끔
			gen_off(); ring_off();	lamp_off();	LaserOff();

			
			// todo : 함수화, DebugPrintCaptureState(), PrintCaptureState()
			if(debug_mode)
			{
				PCPrintf("Rotator Speed when exp on = %ld Hz \r\n", ct_capt_speed_chk2);
				PCPrintf("Rotator Speed when ring on = %ld Hz \r\n", ct_capt_speed_chk1);
				PCPrintf("Rotator Speed when ring off = %ld Hz \r\n", ct_capt_speed_chk3);
				PCPrintf("Check ct_capt_exp_start_cnt = %ld \r\n", verify_2);
				PCPrintf("Check ct_capt_ring_on_cnt = %ld \r\n", verify_3);
				PCPrintf("Check ct_capt_end_cnt = %ld \r\n", verify_4);
				PCPrintf("Check det_trig_off_cnt = %ld \r\n", verify_5);
				PCPrintf("Check det_trig_off_cnt = %ld \r\n", verify_6);
				PCPrintf("Falling edge cnt = %d \r\n", falling_edge_cnt);
			}	
			// PCPrintf("Capture Time = %ld ms \r\n", capt_tm_cnt);
			PCPrintf("Capture Time = %ld ms \r\n", g_measureTime*5);

			//-----------------------------------------------------------------------------
			//	디텍터 트리거 펄스가 10 us 타이머를 사용하므로
			//	30 fps, 720 프레임의 실제 촬영 시간은 0.03333 x 720 = 23.9976 s
			//	30 fps, 540 프레임의 실제 촬영 시간은 0.03333 x 540 = 17.9982 s
			//	50 fps, 400 프레임의 실제 촬영 시간은 0.02000 x 400 = 8.0000 s

			
			//   CT촬영 360도 돌고 나서 원복을 하기전에 
			// 환자가 나올 수 있도록 하는 옵션 tm, 시간에 대한 , end_angle은 최종 각도
			if( (ct_ro_end_tm > 0) || (ct_ro_end_angle >= 4) )	// 360 degrees or None
			{
                if(sound_option)    SoundPlay(SOUND_CAPT_FINISH);	//촬영이 완료 되었습니다.
                else                SoundPlay(SOUND_STOP);
			}
			
			capt_mode_release_step	= STEP2;
		break;

		// 원래속도로 복귀pmc_roms_17000, 1초 기다림를 기다리고, 튜브온도 확인, 
		case STEP2:
			if(gvRO.stop_flag)
			{
			  // UartPrintf("=> Step2 origin speed? : pmc_roms_%ld \r\n", (LONG)RO_MAX_PULSE );
				Ctrl(RO_MAX_SPEED, (LONG)RO_MAX_PULSE);	// 원래 속도로 복귀
                A1_table_step_flag = NO; 	init_A1_table_value();	

				lastTime = TimeNow();							
				
				Ctrl(TUBE_TEMP_REQ, NULL);	// 튜브(탱크) 온도 확인
				
				capt_mode_release_step	= STEP3;
			}
		break;


		// 1초쉬었다가, 
		case STEP3:
			// todo : early return
			if(TimePassed(lastTime) > ONE_SECOND ) 	// 반대 방향으로 움직이기 전에 1 초 쉬었다가
			{
				//--------------------------------------------- 로테이터축 종료 위치로 회전
				if(!scout_mode)
				{
												
					// ?? ct_ro_end_tm의 기능은 무엇? ct_ro_end_tm RO동작이 끝나는 시간(s)
					if(ct_ro_end_tm == 0)
					{
					  // UartPrintf("=> step3-1 : ct_ro_end_tm = %d, ct_ro_end_angle = %d \r\n", ct_ro_end_tm, ct_ro_end_angle);
						switch(ct_ro_end_angle)	// 시간 지연 없는 경우
						{
							case 0:	temp = -20;		break;
							case 1:	temp = 900;		break;
							case 2:	temp = 1800;	break;
							case 3:	temp = 2700;	break;
							case 4:	temp = 3600;	break;
							case 5:	temp = 3999;	break;
						}
					}
					else	ro_end_tm_flag = ON;	// 시간 지연 있는 경우
						

					// 무슨 이유로 15mm쪽으로 이동하는가??
					if(pa_axis_end_position)	Ctrl(PA_MOVE, (LONG)150);
					
				}
				else if(scout_mode)	temp = -20;
				

				// 다음 스텝 제어를 위한 셋팅
				ro_pos_cnt_tmp = (temp + 100) * RO_0_1_DEGREE_PULSE;	// 멈추기 10도 전
				
				// temp 초기값은 0000, 
				// UartPrintf("=> Step3-2 : temp : %05ld\r\n", temp);
				if( (temp != 0) && (temp != 3999) )	
				{	
				// UartPrintf("=> Step3-2 : ro_move : %05ld\r\n", temp);
					Ctrl(RO_MOVE, temp);	
				}
				
				 // UartPrintf("=> step3-3 : ct_ro_end_tm = %d, ct_ro_end_angle = %d \r\n", ct_ro_end_tm, ct_ro_end_angle);
				if( (ct_ro_end_tm > 0) || (ct_ro_end_angle >= 4)/* || (main_mode == CT_STI2_MODE)*/ )	// 360 degrees or None
				{
				  // UartPrintf("=> step3-3 : sound_option : %d \r\n", sound_option);
					SoundPlay(SOUND_CAPT_THANKS);		// 촬영에 협조해 주셔서 감사합니다. 수고하셨습니다.
					
					lastTime = TimeNow();	
					capt_mode_release_step = STEP10;
				}
				else	capt_mode_release_step = STEP4;
				
				//-------------------------	x-ray가 출력되었는지 제네레이터에 확인한 결과 출력
				if(gen_ack_flag == 7)		PCPuts("X-Ray Stable \r\n");
				else if(gen_ack_flag == 8)	PCPuts("X-Ray Unstable \r\n");
			}
		break;
		
		// 반대방향으로 ro_pos_cnt_tmp(멈추기 10도전) 만큼 움직였다면, 다음 스텝으로
		// ?? 어떤 처리를 안하나?
		case STEP4:
				// // UartPrintf("step4 : RO_positionCnt : %ld, RO_temp : %ld \r\n", gvRO.pos_cnt,ro_pos_cnt_tmp );
			if(RO_GetPositionCnt() <= ro_pos_cnt_tmp)	
			{
				// UartPrintf("step4-1 \r\n");
				capt_mode_release_step = STEP5;
			}
		break;
		
		// 멈추기 10도 전
		case STEP5:
				// // UartPrintf("step5 :  \r\n");
            if(sound_play_time_end)
            {
            // UartPrintf("=> Step5 sound cpat finish : option : %d \r\n", sound_option);
            	SoundPlay(SOUND_CAPT_FINISH);
              
                // (멈추기 10도전) - 9도  ==> 멈추기 1도전
                ro_pos_cnt_tmp = ro_pos_cnt_tmp - (90 * RO_0_1_DEGREE_PULSE); 

                capt_mode_release_step = STEP6;
            }
		break;

		// 멈추기 1도 전.. 그냥 다음 패스
		case STEP6:
				// // UartPrintf("step6 \r\n");
			if(RO_GetPositionCnt() <= ro_pos_cnt_tmp)	
			{
			// UartPrintf("=> step6 : ro_pos_cnt_tmp = %ld \r\n", ro_pos_cnt_tmp);	
				capt_mode_release_step = STEP7;
			}
			
		break;					

		case STEP7:
			if( sound_play_time_end && (gvPA.stop_flag))
			{
			// UartPrintf("=> step7 PA stop and sound Thanks \r\n");	
				SoundPlay(SOUND_CAPT_THANKS);	// 촬영에 협조해 주셔서 감사합니다. 수고하셨습니다.
				
				capt_mode_release_step = STEP10;
			}
		break;

		case STEP10:
			if( (gvRO.stop_flag) && (sound_play_time_end))
			{
				SoundPlay(SOUND_STOP);	// 안 보내면 MP3 보드 릴레이가 안 끊어져서 화이트 노이즈 계속 출력

			// UartPrintf("=> step7 RO stop and sound Stop \r\n");	
				PCPuts("[rpm_exed]	[rpm_capt_end] \r\n");

                exp_push = NO;
                PCPuts("----------------------------------- CT Capture End! \r\n");
                isCaptureClosing = NO;
				capt_mode_release_step = STEP1;	
			}
		break;

		default:	break;					
	}
}

/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
void Halt_ExpCapture(void)
{
	static	BYTE	haltStep = STEP1;

	switch(haltStep)
	{
		case STEP1:
		// UartPrintf("haltStep1\r\n");
			isCaptureClosing = YES;
            PCPrintf("<ct_capt_frame = %ld> \r\n", ct_capt_frame);
            PCPrintf("<det_trg_off = %d> \r\n", det_trig_off_cnt);
            PCPrintf("<falling_edge = %d> \r\n", falling_edge_cnt);
            // if(ct_det_optn == VW_V_0606)    {       // VIEWORKS사의 0606C인 경우에는 트리거 출력과 EXP OK 입력을 출력
            //     PCPrintf("<vw_exp_ok = %d> \r\n", vw_exp_ok_cnt);	}

			PCPuts("[rpm_swof] \r\n");	// 촬영단에 조사스위치 뗏음을 알림
			software_exp = OFF;	// [pmc_swon] 사용한 뒤에 [pmc_swof] 보내지 않을 경우를 대비하여 추가

			gen_off(); ring_off();	lamp_off();	LaserOff();
            ct_pano_det_off();

			capt_tm_flag = NO;
			exp_ok_flag = NO;

            A1_table_step_flag = NO; init_A1_table_value();
			
			Ctrl(RO_SLOW_STOP, NULL);	

			//----------------------------- X-Ray 조사 중지, 영상 획득 중지, 조사표시등 끔, 촬영음악 끔
            SoundPlay(SOUND_CAPT_STOP);	//촬영이 중단 되었습니다.
            
			if(debug_mode)
			{
				PCPrintf("Rotator Speed when exp on = %ld Hz \r\n", ct_capt_speed_chk2);
				PCPrintf("Rotator Speed when ring on = %ld Hz \r\n", ct_capt_speed_chk1);
				PCPrintf("Rotator Speed when ring off = %ld Hz \r\n", ct_capt_speed_chk3);
				PCPrintf("Check ct_capt_exp_start_cnt verify cnt = %ld \r\n", verify_2);
				PCPrintf("Check ct_capt_ring_on_cnt = %ld \r\n", verify_3);
				PCPrintf("Check ct_capt_end_cnt = %ld \r\n", verify_4);
				PCPrintf("Check det_trig_off_cnt = %ld \r\n", verify_5);
				PCPrintf("Check det_trig_off_cnt = %ld \r\n", verify_6);
				PCPrintf("Check Falling edge cnt = %d \r\n", falling_edge_cnt);
			}	PCPrintf("Check Capture Time = %ld ms \r\n", capt_tm_cnt);
			
			PCPuts("[rpm_exst]	[rpm_expo_stop] \r\n");

			haltStep	= STEP2;
		break;

		case STEP2:
			if(gvRO.stop_flag)
			{
		// UartPrintf("haltStep2-1\r\n");
				PCPrintf( "[rpm_romv_%05ld] \r\n", RO_GetPositionCnt() / RO_0_1_DEGREE_PULSE);
				Ctrl(RO_MAX_SPEED, (LONG)RO_MAX_PULSE);	// 원래 속도로 복귀
				Ctrl(TUBE_TEMP_REQ, NULL);	// 튜브(탱크) 온도 확인
				haltStep = STEP3;
			}
		break;

		case STEP3:
            if(sound_play_time_end)
            {
		// UartPrintf("haltStep3-1\r\n");
                SoundPlay(SOUND_OPEN_EYES);		//눈을 뜨시고 다음안내가 있을 때까지 기다려주세요.
                haltStep = STEP4;
            }
		break;

		case STEP4:
			if(sound_play_time_end)
			{
		// UartPrintf("haltStep4-1\r\n");
				SoundPlay(SOUND_STOP);	// 안 보내면 MP3 보드 릴레이가 안 끊어져서 화이트 노이즈 계속 출력
                exp_push = NO;
                isCaptureClosing = NO;
				haltStep = STEP1;		
			}
		break;

		default:	break;
	}
}
/*F**************************************************************************
* NAME:     exp_capt
* PURPOSE:	CT 촬영 모드에서 조사스위치를 눌렀을 때 이루어지는 동작들을 코딩
*****************************************************************************/
void Set_InitialCaptureValue(void)
{
	// LONG temp = 0;
	double timming_temp = 0;
	int captureTime;
	LONG	roCnt_detPeriod;
	LONG	roCnt_genSync;
	const LONG pulseCntPerSec = 100000;	// 펄스수 in timer 10us interrupt 
	LONG detPeriodCnt = 0;
	LONG genPeriodCnt = 0;


	// 촬영시간(8초, 18초, 24초)에 따른 RO Capture value 셋팅
	captureTime = ct_capt_frame/ct_capt_fps;

	//-----------------------------------------------------------------------------
	//	CT 촬영이 끝나는 RO position PulseCnt(360도 1바퀴)
	//	CT 촬영 시간동안 촬영구간에서의 로테이터 속도, 단위 : pulse/s(=Hz)
	ct_capt_end_cnt = (LONG)RO_360_DEGREE_PULSE;
	ct_capt_speed = ct_capt_end_cnt/captureTime;	// 403200/24 = 403200/24 = 16800
													// 403200/8  = 403200/8  = 50400

	//-----------------------------------------------------------------------------
	// A#_drive_const 함수로 실제로 만들어지는 주파수(ecox에서는 RO_MotorStart함수)
	// 이에 따른 detector trig period 보정, '100000'는 10us timer interrupt 에서 초당 pulse 수
	real_ct_capt_speed = P_CLK/(P_CLK/ct_capt_speed);	// todo : 알고리즘 분석필요(원리 이해 안됨)
	detPeriodCnt = 100000 / ct_capt_fps * ct_capt_speed / real_ct_capt_speed;
    SetTriggerPulse_CT(detPeriodCnt);


    //-----------------------------------------------------------------------------
    //  CT 촬영시 장비 흔들림을 줄이기 위해서 smoth trajctory table을 사용하여 가감속 동작
    //  MOVE와 같은 방법으로는 등속 구간 시작점에서 흔들림이 발생하여 table 동작 추가
	SetSmoothTrajectoryTable(captureTime, ct_capt_speed);
	

	//------------------------------------------------------------------------------
	//	촬영단으로 [rpm_grab] 보내서 디텍터가 영상을 안정적으로 읽을 수 있도록 
	//  Ring On 전에 통신 여유시간 500ms 확보
	ct_capt_msg_cnt = -(RO_360_DEGREE_PULSE / captureTime) * 0.5;


	//------------------------------------------------------------------------------
	//	X-Ray 초기 조사시 안정적으로 출력될 때까지 안정화 시간 필요 : 200 ms 이상 확보
	//  
	// timming_temp =  조사안정화 시간 + syn_cnt중앙값, ct fps는 30, 50 두가지만 있음.
	// 30fps : 33.3ms 주기  => 200ms / 33.3ms = 6.006 개  => pulse주기  6개 필요
	// 50fps : 20ms 주기    => 200ms / 20ms  = 10  ==> pulse주기 10개 필요
	// 정수로 계산시 31bit 이상 이 될 수가 있어(overflow) 실수로 변환하여 계산함
	genPeriodCnt = GetGenPeriodCnt();
	if(ct_capt_fps == 30)	timming_temp =  6 + (double)genSyncCnt / (double)genPeriodCnt;	// ring on 시점까지 제네레이터 트리거 개수
	if(ct_capt_fps == 50)	timming_temp = 10 + (double)genSyncCnt / (double)genPeriodCnt;	// ring on 시점까지 제네레이터 트리거 개수

	ct_capt_exp_start_cnt = -(long)(real_ct_capt_speed * ((double)genPeriodCnt * timming_temp / 100000));
	

	//------------------------------------------------------------------------------
	// 디텍터에서 영상을 처음 얻을 때는 dark current 때문에 이미지가 불안정함.
	// 1) 실험적으로 최소한 처음에 얻은 두 개 이상의 이미지는 dark current에 의해 밝기값이 달라서 버려야 함
	// 2) 첫 번째 프레임은 로테이터가 0도일 때 x-ray를 조사해서 얻은 영상이므로,
	//    디텍터의 extended exposure 영역(디텍터 매뉴얼의 진녹색 영역)이 0도에 위치하도록 맞춰야 함
	//    ring On 신호를 gen pulse high의 중앙에 오도록 동기화 함.
	// 3) detector on 시기는 2개 detector period와 genSync 시간이 필요
	roCnt_detPeriod = -real_ct_capt_speed * detPeriodCnt / 100000;	// detector 주기에 해당하는 ro position cnt
	roCnt_genSync = -real_ct_capt_speed * genSyncCnt / 100000;
	ct_capt_det_on_cnt = 2 * roCnt_detPeriod + roCnt_genSync;

	//------------------------------------------------------------------------------
	//	0도에서 첫 번째 프레임을 받기 위해서 촬영단으로 ring 신호 보내기 시작하는 카운트
	ct_capt_ring_on_cnt = 0;


	// 스카웃 촬영 시간에 해당하는 펄스 수
	// todo : scout_mode  ==> main_mode == CT_SCOUT_MODE
	if(scout_mode)	ct_capt_end_cnt = scout_pulse;	

	if(debug_mode)
	{
		PCPrintf("\r\nMsg: -------- CT setting value\r\n");
		PCPrintf("ro_pulse_freq = %ld Hz \r\n", ct_capt_speed);
		PCPrintf("ro_real_freq = %ld Hz \r\n", real_ct_capt_speed);
		PCPrintf("det_trig_real_cnt = %ld \r\n", detPeriodCnt);
		PCPrintf("roCnt_detPeriod = %ld \r\n", roCnt_detPeriod);
		PCPrintf("genSyncCnt = %ld \r\n", genSyncCnt);
		PCPrintf("ct_capt_msg_cnt = %ld \r\n", ct_capt_msg_cnt);
		PCPrintf("ct_capt_exp_start_cnt = %ld \r\n", ct_capt_exp_start_cnt);
		PCPrintf("ct_capt_det_on_cnt = %ld \r\n", ct_capt_det_on_cnt);
		PCPrintf("ct_capt_ring_on_cnt = %ld \r\n", ct_capt_ring_on_cnt);
		PCPrintf("ct_capt_end_cnt = %ld \r\n", ct_capt_end_cnt);
		PCPrintf("\r\n");
	}

	Ctrl(RO_MAX_SPEED, ct_capt_speed);	// 촬영을 위한 속도로 변경
	
	det_trig_off_cnt = (WORD)ct_capt_frame + 3;		
	falling_edge_cnt = 0; 
}



//-----------------------------------------------------------------------------
//  CT 촬영시 장비 흔들림을 줄이기 위해서 table을 사용하여 가속, 감속 동작
//  MOVE와 같은 방법으로는 등속 구간 시작점에서 흔들림이 발생하여 table 동작 추가
//-----------------------------------------------------------------------------
void SetSmoothTrajectoryTable(int captureTime_sec, LONG captureSpeed)
{
	LONG    angle_pulse_acc_dec = 0;	// todo rename 
	unsigned int idx = 0;
	double sum_freq = 0, sum_pulse = 0;


    A1_table_step_flag = YES;       // table 사용 플래그 설정

	switch(captureTime_sec)
	{
	// angle_pulse_acc_dec 값 셋팅.  이 변수값이 낮을 수록 가감속이 크게 됨, 
	// 부드럽게 하려면 값을 높이지만 가감속 속도는 낮아져서 촬영시간이 길어진다.  
		case 8:		// 8초 촬영시 사용하는 값들
		case 10:
		case 12:
			#ifdef PULLEY12
				// angle_pulse_acc_dec = 13770;	// 19.12.09 현재 ecox pulley12 에 적용된 값
				angle_pulse_acc_dec = 16000;	// 동작 확인 필요
			#else
				angle_pulse_acc_dec = 40453;
			#endif
		break;
		
		case 18:
		case 24:	// 24초 촬영시 사용하는 값들	
			#ifdef PULLEY12
				// angle_pulse_acc_dec = 6816;		// 19.12.09 현재 ecox pulley12 에 적용된 값
				angle_pulse_acc_dec = 8000;			// 동작 확인 필요
			#else
				angle_pulse_acc_dec = 19679;	// todo : 변수의 기능에 대해 완전한 이해가 안됨.
			#endif
		break;
		
		default:  PCPuts("Undefined capture sec!!\r\n");	break;
	}


	// 부드러운 가감속 궤적 테이블(배열) 셋팅
    for(idx=0; idx < 140; ++idx)		// 가감속 주파수 계산
    {
        A1_speed_arr[idx] = A1_curve_arr[idx] * captureSpeed;
        if(A1_speed_arr[idx] < mRO.min_speedHz) A1_speed_arr[idx] = mRO.min_speedHz;
        sum_freq += A1_speed_arr[idx];
    }

    // todo : (angle_pulse_acc_dec / sum_freq)이 부분이 어떤 기능적 의미인가?
    //  	==> mathlab 궤적 코드를 분석하면 이해가 될 듯
    for(idx=0; idx < 140; ++idx)		// 속도에 따른 출력 펄스 개수 계산
    {
        A1_pulse_arr[idx] = A1_speed_arr[idx] * angle_pulse_acc_dec / sum_freq;
        A1_pulse_arr[idx] -= 2;                // 테이블마다 2개씩 빼면 280
        sum_pulse += A1_pulse_arr[idx];
    }
	A1_pulse_arr[0] = A1_pulse_arr[0] + 280;    // 280 = table 개수 * 2, 제일 낮은 속도에서 머무는 시간 늘리기
}


/*F****************************************************************************************
* NAME:     can_seq_move
* PURPOSE:	5 kHz 주기로 반복되는 함수 내에 delay가 추가되면 함수의 주기가 틀어지므로,
			CAN 메시지는 별도의 함수에서 delay를 사용하지 않고 순차적으로 출력한다.
*******************************************************************************************/
void can_seq_move(void)
{
	++can_seq_cnt;
	
	if(can_seq_cnt > 1)
	{
		if(co_can_flag)
		{
		// PCPuts("CO/");	
			co_can_flag = NO;	can_seq_cnt = 0;
			
			if(main_mode == SCAN_CEPH_LA_MODE)
				rcan_puts(CO_MOTOR, CAN_AXIS_MOVE_CONST, -(LONG)scan_ceph_la_1cfreq[sc_tbl_sel_2][tbl_no], dmy_msg);
			else if(main_mode == SCAN_CEPH_PA_MODE)
				rcan_puts(CO_MOTOR, CAN_AXIS_MOVE_CONST, -(LONG)scan_ceph_pa_1cfreq[sc_tbl_sel_2][tbl_no], dmy_msg);
		}
		else if(sd_can_flag)
		{
		// PCPuts("SD/");	
			sd_can_flag = NO;	can_seq_cnt = 0;
			
			if(main_mode == SCAN_CEPH_LA_MODE)
				rcan_puts(SD_MOTOR, CAN_AXIS_MOVE_CONST, (LONG)scan_ceph_la_sdfreq[sc_tbl_sel_1][tbl_no], dmy_msg);
			else if(main_mode == SCAN_CEPH_PA_MODE)
				rcan_puts(SD_MOTOR, CAN_AXIS_MOVE_CONST, (LONG)scan_ceph_pa_sdfreq[sc_tbl_sel_1][tbl_no], dmy_msg);
		}

	}

}

void can_seq_move2(void)
{
	++can_seq_cnt;
	
	if(can_seq_cnt > 1)
	{
		if(co_can_flag)
		{
		// PCPuts("CO/");	
			co_can_flag = NO;	can_seq_cnt = 0;

			if(main_mode == SCAN_CEPH_LA_MODE)
			{
				Ctrl_CAN_MOVE_CONST(&g_HL, (LONG)scan_ceph_la_1cfreq[sc_tbl_sel_2][tbl_no]); 
				Ctrl_CAN_MOVE_CONST(&g_HR, -(LONG)scan_ceph_la_1cfreq[sc_tbl_sel_2][tbl_no]); 
			}
			else if(main_mode == SCAN_CEPH_PA_MODE)
			{
				Ctrl_CAN_MOVE_CONST(&g_HL, (LONG)scan_ceph_pa_1cfreq[sc_tbl_sel_2][tbl_no]); 
				Ctrl_CAN_MOVE_CONST(&g_HR, -(LONG)scan_ceph_pa_1cfreq[sc_tbl_sel_2][tbl_no]); 
			}			
			
			if(main_mode == SCAN_CEPH_LA_MODE)
				rcan_puts(CO_MOTOR, CAN_AXIS_MOVE_CONST, -(LONG)scan_ceph_la_1cfreq[sc_tbl_sel_2][tbl_no], dmy_msg);
			else if(main_mode == SCAN_CEPH_PA_MODE)
				rcan_puts(CO_MOTOR, CAN_AXIS_MOVE_CONST, -(LONG)scan_ceph_pa_1cfreq[sc_tbl_sel_2][tbl_no], dmy_msg);
		}
		else if(sd_can_flag)
		{
		// PCPuts("SD/");	
			sd_can_flag = NO;	can_seq_cnt = 0;
			
			if(main_mode == SCAN_CEPH_LA_MODE)
				rcan_puts(SD_MOTOR, CAN_AXIS_MOVE_CONST, (LONG)scan_ceph_la_sdfreq[sc_tbl_sel_1][tbl_no], dmy_msg);
			else if(main_mode == SCAN_CEPH_PA_MODE)
				rcan_puts(SD_MOTOR, CAN_AXIS_MOVE_CONST, (LONG)scan_ceph_pa_sdfreq[sc_tbl_sel_1][tbl_no], dmy_msg);
		}

	}

}

// can_seq_move()와 함께 쓰인다. 
// todo : 기능 분석 필요
void can_seq_stop(void)
{
    if(co_move_const_flag && (!can_seq_stop_time_flag))
    {
    // PCPuts("===>COC/");
        co_move_const_flag = NO;
        Ctrl(CO_MOVE_CONST, (LONG) 0);
		can_seq_stop_time_flag = YES;
    }
    else if(sd_move_const_flag && (!can_seq_stop_time_flag))
    {
    // PCPuts("===>SDC/");
        sd_move_const_flag = NO;
        Ctrl(SD_MOVE_CONST, (LONG) 0);
		can_seq_stop_time_flag = YES;
    }
}