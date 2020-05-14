/*C***************************************************************************
* $RCSfile: command.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the command source
*----------------------------------------------------------------------------
* RELEASE:      $Name: command_mode.c     
* Edited  By    $Id: sslim
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include "includes.h"

/*_____ D E C L A R A T I O N ______________________________________________*/

enum MotorStates
{ MT_INIT, MT_IDLE, MT_START, MT_FINISH, MT_GUARD, MT_ORIGIN, MT_WAIT_1S, 
  MT_CHK_ERR, MT_CHK_STOP, MT_ERR, MT_GO_OFFSET, 
};

enum Event
{ EVENT_CLEAR = 0, EVENT_START = 1, };

typedef struct
{
	int 	event;
	int 	reqCnt;
	int 	numError;
	LONG 	offset_pos;
	BYTE 	genMode;
	BYTE	isEnd;
} taskData_t;



/*_____ D E F I N E ________________________________________________________*/
// 개구부 guard를 이용해 근접 개구부의 X선 간섭을 막는용도
#define GUARD_POS_CEPH		912		// 91.2 mm
#define GUARD_POS_CT		0		// 0 mm (원점)
#define GUARD_POS_PANO		0		// 0 mm (원점)

//------- number of error 
#define ERR_COM 			2		// communication error
#define ERR_OFFSET			3		// offset position mismatch error


/*_____ F U C T I O N ______________________________________________________*/
BYTE CmdMode(LONG, LONG);
BYTE Mode_CT(LONG, LONG);
BYTE Mode_PANO(LONG, LONG);
BYTE Mode_PANO_TMJ2(LONG, LONG);
BYTE Mode_CEPH(LONG, LONG);

//--------------------- Task func.
void Mode_TaskRO(taskData_t*);
void Mode_TaskPA(taskData_t*);
void Mode_TaskCO(taskData_t*);

//--------------------- 4 axis control
void Mode_TaskCO_4Axis(taskData_t *, t_Info4Axis *);
void Mode_TaskCO_TrckingCO(taskData_t *, t_TrackingCO *);
BYTE CheckOffset_4Axis(t_Info4Axis *);
BYTE CheckStop_4Axis(void);
BYTE CheckPosition_TC(t_Position_um*);


int MT_Idle(taskData_t*);
int MT_Err(taskData_t*);
int MT_Finish(taskData_t*);
int MT_CHK_Stop(BYTE);
int MT_CHEK_Err(ULONG, int*, int);


void Mode_CEPH_TaskRO(taskData_t*);
void Mode_CEPH_TaskPA(taskData_t*);
void Mode_CEPH_TaskCO(taskData_t*);
void Mode_CEPH_TaskSD(taskData_t*);

BYTE CmdMode_LCD(LONG, LONG);
BYTE ModeLCD_CT(LONG);
BYTE ModeLCD_PANO(LONG, LONG);

//---------------------- Prec func.
BYTE Mode_PREC_CT(LONG);
BYTE Mode_PREC_PANO(LONG);
BYTE Mode_PREC_CEPH(LONG);
BYTE Mode_SCOUT_SET(LONG);	// Scout Prec

void Set_Canine_Values(WORD);
void Prec_TaskGen(taskData_t*);
void Prec_TaskRO(taskData_t*);

//--------------------------
BYTE Mode_FIRMWARE_REVISION_NUM(LONG);
BYTE Mode_BRIGHT_FRAME_ACQ(LONG);
BYTE Mode_ALIGN_EXPOSE(LONG);
void Msg_kVmA(void);


/*_____ V A R I A B L E ____________________________________________________*/
BYTE	main_mode = CT_MODE;
BYTE	pre_main_mode = 0;
BYTE	aok_flag = 0;
extern BYTE gen_preheat_time_flag;

//-------- pano 관련 변수
extern LONG	pano_stan_prec_angle;		// from command.c
extern LONG	pano_tmj_prec_angle;		// from command.c
extern LONG	pano_tmj2_prec_angle;		// from command.c
extern LONG	pano_sinu_prec_angle;		// from command.c
extern LONG capt_tm_cnt;				// from capture.c

//-------- canine 관련 변수
BYTE	isOnCanineBeam = NO;	// 견치빔 값 적용(1)/비적용(0)
WORD	Silde_potentio_value = 0;		// 견치빔 AD 값	(견치빔의 위치가 전치(앞니)의 뿌리 위치다!)
LONG	Panomode_paxis_value = 0;		// 견치빔 Offset


extern LONG	alex_val;				// from command.c

//----- scout 관련
extern LONG	pre_ct_capt_frame;		// from capture.c
extern LONG	pre_ct_capt_fps;		// from capture.c

//------ 전시회 관련
extern BYTE 	isOnExhibition;		// from EcoX.c

//------ Tracking CO
extern t_ObjectInfo g_Obj;			// from four_axis.c
BYTE g_TcOption;
extern t_TrackingCO g_tc;			// from capture.c

/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
void PrintCmdMode(LONG inst, LONG value)
{
	char strInst[5] = {0};
	strInst[0] = (char)((inst>>24) & 0x000000ff);
	strInst[1] = (char)((inst>>16) & 0x000000ff);
	strInst[2] = (char)((inst>>8) & 0x000000ff);
	strInst[3] = (char)((inst>>0) & 0x000000ff);
	strInst[4] = '\0';

	// UartPrintf("==> [pmm_%s],  value : %ld \r\n", strInst, value);
}


/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
BYTE CmdMode(LONG inst, LONG value)
{
	// todo : isCmdCmpleted와 isCompleted가 비슷해서 헷갈릴 수 있다. 
	// isComplete를 바꾼다.  
	static	BYTE isCmdCompleted =0;
	static	BYTE endCmdMode = 1;
	const BYTE notCompleted = 0;

	
	PrintCmdMode(inst, value);

	switch(inst)
	{	

		case CT:					// [ct__]
		case CT_CHILD:					// [ctch]
			isCmdCompleted = Mode_CT(inst, value); 			
			return isCmdCompleted;

		case PANO_STAN:				// [pano]
		case PANO_BITEWING:			// [biwi]
		case PANO_TMJ1:				// [tmj1]
		case PANO_SINUS:			// [sinu]
			isCmdCompleted = Mode_PANO(inst, value); 	
			return isCmdCompleted;

		case PANO_TMJ2:				// [tmj2]
			isCmdCompleted = Mode_PANO_TMJ2(inst, value); 	
			return isCmdCompleted;				
		
		case SCAN_CEPH_LA:			// [cepl]
		case SCAN_CEPH_PA:			// [cepp]
			isCmdCompleted = Mode_CEPH(inst, value); 	
			return isCmdCompleted;	


		case PREC_COMPLETE:		// [prec]																	
			switch(main_mode)
			{
				case CT_MODE:
				case CT_SCOUT_MODE:	
					isCmdCompleted = Mode_PREC_CT(value); 
					return isCmdCompleted;
  				

  				//  todo : Mode_PREC_CT() 에 통합시킴, 검증후 함수 및 루틴  제외해도 될 듯
				// case CT_SCOUT_MODE:	
				// 	isCmdCompleted = Mode_SCOUT_SET(value); 
				// 	return isCmdCompleted;


				case PANO_STAN_MODE:
				case PANO_BITEWING_MODE:
				case PANO_TMJ1_MODE:
				case PANO_TMJ2_MODE:
					isCmdCompleted = Mode_PREC_PANO(value); 
					return isCmdCompleted;

				case SCAN_CEPH_LA_MODE:
				case SCAN_CEPH_PA_MODE:
					isCmdCompleted = Mode_PREC_CEPH(value);  // todo : input value : main_mode
					return isCmdCompleted;

				default: 	
					PCPuts("Undifined main_mode!!\r\n"); 	
					return endCmdMode;
 			}

		case FIRMWARE_REVISION_NUM:		// [frn?]
			isCmdCompleted = Mode_FIRMWARE_REVISION_NUM(value);	
			return isCmdCompleted;	

		case BRIGHT_FRAME_ACQ:			// [brit]
			isCmdCompleted = Mode_BRIGHT_FRAME_ACQ(value);	
			return isCmdCompleted;	

		case ALIGN_EXPOSE:	// [alex]	// 디텍터는 인터널모드로 설정한다고 가정. 오로지 조사만 되는 명령
			isCmdCompleted = Mode_ALIGN_EXPOSE(value);
			return isCmdCompleted;

		case SCOUT_SET:		// [scst]  // scout prec
			isCmdCompleted = Mode_SCOUT_SET(value);
			return isCmdCompleted;

		default: 
			PCPuts("Not defined Mode-command!!\r\n");
			return endCmdMode;
 	}  

	return notCompleted;
}


//============================================================================================================	
// CT Mode
//============================================================================================================
BYTE Mode_CT(LONG inst, LONG value)
{
	const BYTE isCompleted = 1;
	const BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static taskData_t taskPA, taskRO, taskCO;
    static t_Info4Axis axis4;

	switch(modeStep)
	{
		case STEP1:
			// UartPrintf("step 1\r\n");
			SoundPlay(SOUND_APPLYING_CT); 		// 촬영을 위해 준비중입니다.
			gen_off();	ct_pano_det_off();	s_ceph_det_off(); 
			ring_off();	lamp_off();	LaserOff();

			// todo 변수사용 확인 : scout_mode, ro_end_tm_flag
			exp_ok_flag = NO;	scout_mode = NO;	ro_end_tm_flag = 0;
			
			// task initialize //PA, RO, CO는 각 기능별로 Task 함수로 동작됨
			taskRO.event = EVENT_START; taskRO.offset_pos = gvRO.ct_offset_pos;
			taskPA.event = EVENT_START; taskPA.offset_pos = gvPA.ct_offset_pos;
			taskCO.event = EVENT_START; taskCO.offset_pos = gvCO.ct_offset_pos;

			if(co_option == FOUR_AXIS_CO)
			{
				axis4.hl_offsetPosition_um = g_HL.ct_offset_pos;
				axis4.hr_offsetPosition_um = g_HR.ct_offset_pos;
				axis4.vt_offsetPosition_um = g_VT.ct_offset_pos;
				axis4.vb_offsetPosition_um = g_VB.ct_offset_pos;
			}

			if(inst == CT)
			{
				main_mode = CT_MODE;
				PCPuts("\r\nEcoX CT MODE \r\n");
			}
			else if(inst == CT_CHILD)
			{
				main_mode = CT_CHILD_MODE;	
				PCPuts("\r\nEcoX CT Child MODE \r\n");
			}

			//-----------------------------------------	Dark 출력 시작
			// DARK_FRAME_ACQ is used in timer.c - 10us, align_exp_time_flag 
			Ctrl(DARK_FRAME_ACQ, (LONG) FRAME_CT);  // '1' : CT mode
			
            modeStep = STEP2;
		break;

		case STEP2:
			if(taskRO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] RO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] CO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskPA.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] PA \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_OFFSET) {	PCPuts("[SYS_ERR_03] CO OFFSET \r\n");	modeStep = STEP1;	return isCompleted; }

			if(taskRO.isEnd == YES && taskPA.isEnd == YES && taskCO.isEnd == YES)	
			{
			// UartPrintf("==> modeStep2-1\r\n");
				modeStep = STEP3;
			}
			else modeStep = STEP2;
		break;

		case STEP3:
			if(sound_play_time_end)
			{
			// UartPrintf("==> modeStep3\r\n");
				SoundPlay(SOUND_APPLIED_CT);	//촬영 준비가 되었습니다.
	            LaserOn();					// 수진자 정렬을 위해 레이저를 켬
				modeStep = STEP4;
			}
		break;

		case STEP4:
			if(sound_play_time_end)
			{
			// UartPrintf("modeStep4\r\n");
				pre_main_mode = main_mode;
				PCPuts("[rpm_ct__] \r\n\r\n");
				LCDPuts("[tml_ctmd]\r\n");
				PCPuts("//-------------------- CT MODE is Completed!! \r\n");
	
				modeStep = STEP1;
				return isCompleted;
			}
		break;
		
		default:	PCPuts("Undefined CT STEP! \r\n\r\n");	break;
	}

	Mode_TaskRO(&taskRO);
	Mode_TaskPA(&taskPA);

	if(co_option == FOUR_AXIS_CO) {
		if(g_TcOption == YES) {
			// Mode_TaskCO_TrckingCO(&taskCO);
			Mode_TaskCO_TrckingCO(&taskCO, &g_tc);
		}	
		else {
			Mode_TaskCO_4Axis(&taskCO, &axis4);
		}
	}
	else if(co_option == ONE_AXIS_CO) {
		Mode_TaskCO(&taskCO);
	}

	return notCompleted;
}

//===================================================================================//
// 	CT mode 동작시 RO 동작 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//  pEvent(외부 이벤트)에 따라 CT mode RO 동작을 start 한다. 
//	TasK 함수 : 각 Task별로 독립적으로 구동하여 Multi Task가 가능하게 하기 위한 함수
//===================================================================================//
void Mode_TaskRO(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 70;


	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
		// UartPrintf("==> RO MT_START \r\n");
		  	Ctrl(RO_SET_ORIGIN, (LONG) 0 );
		  	currState = MT_CHK_STOP;
		break;

		case MT_CHK_STOP:
		// UartPrintf("==> RO MT_CHK_STOP \r\n");
			// next State : MT_CHK_ERR or MT_FINISH
			currState = MT_CHK_Stop(IsMotorStop(&gvRO));
			lastTime = TimeNow();		// for CHK_ERR
		break;

		case MT_CHK_ERR:  
			// next State : MT_CHK_STOP or MT_ERR
		// UartPrintf("==> RO MT_CHK_ERR \r\n");
			currState = MT_CHEK_Err(lastTime, &(pTaskData->reqCnt), errReqCnt);
		break;
    
		case MT_ERR:
			// next State : MT_IDLE
			currState = MT_Err(pTaskData);
			// UartPrintf("==> RO MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
		// UartPrintf("==> RO MT_FINISH \r\n");
			// next State : MT_IDLE
			currState = MT_Finish(pTaskData);
		break;

		default:	UartPrintf("Undefined RO State : %d !! \r\n", currState);	break;
	}
}

//============================================================
//  대기(Idle)상태에서 외부 이벤트 start가 들어오면 MT_START 상태로 전이
//============================================================
int MT_Idle(taskData_t *pTaskData)
{
	if(pTaskData->event == EVENT_START) {
		// initailize
		pTaskData->isEnd = NO;
		pTaskData->numError = 0;
		pTaskData->reqCnt = 0;

		// clear event 
		pTaskData->event = EVENT_CLEAR;

		// UartPrintf("==> addr pTaskData %lx \r\n", pTaskData);
		return MT_START;
	}
	
	return MT_IDLE;
}

int MT_Err(taskData_t *pTaskData)
{
	pTaskData->numError = ERR_COM;
		// UartPrintf("==> Err addr pTaskData %lx \r\n", pTaskData);
	return MT_IDLE;
}

int MT_Finish(taskData_t *pTaskData)
{
	pTaskData->isEnd = YES;
	return MT_IDLE;
}

int MT_CHK_Stop(BYTE isOkMotorStop)
{
	if(isOkMotorStop) {
	  // UartPrintf("==> func MT_CHK_STOP-1 \r\n");
		return MT_FINISH;
	}
	else {
	  // UartPrintf("==> func MT_CHK_STOP-2 \r\n");
		return MT_CHK_ERR;
	}

	return MT_CHK_STOP;
}

int MT_CHEK_Err(ULONG lastTime, int *reqCnt, int timeErr)
{

	if( TimePassed(lastTime) > ONE_SECOND ) {
		++*reqCnt;
		if(*reqCnt < timeErr ) {
			return MT_CHK_STOP;
		}
		else {	// 'timeErr'sec 동안 apply가 안되면 에러 리턴
			return MT_ERR;
		}
	}

	return MT_CHK_ERR;

}

//===================================================================================//
// 	CT mode 동작시 PA 동작 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void Mode_TaskPA(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 70;


	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
			// if PA end option이 1이면 원점 잡고 offset이동, 0이면 바로 offset 이동. 
			// todo naming : pa_axis_end_position -> isPAEndOption
			if(pa_axis_end_position)
			{
			// UartPrintf("==> PA MT_START-1 \r\n");
				Ctrl(PA_SET_ORIGIN, (LONG) 0);
				currState = MT_ORIGIN;
			}	
			else
			{
			// UartPrintf("==> PA MT_START-2 \r\n");
				Ctrl(PA_MOVE, pTaskData->offset_pos);
				currState = MT_CHK_STOP;
			}	
		break;

		case MT_ORIGIN:
			if(IsMotorStop(&gvPA))
			{
			// UartPrintf("==> PA MT_ORIGIN-1 \r\n");
				lastTime = TimeNow();
				currState = MT_WAIT_1S;
			}
		break;

		case MT_WAIT_1S:
			if( TimePassed(lastTime) > ONE_SECOND )
			{
			// UartPrintf("==> PA MT_WAIT_1S-1 \r\n");
				Ctrl(PA_MOVE, gvPA.ct_offset_pos);
				currState = MT_CHK_STOP;
			}
		break;

		case MT_CHK_STOP:
			// next State : MT_CHK_ERR or MT_FINISH
			currState = MT_CHK_Stop(IsMotorStop(&gvPA));
			lastTime = TimeNow();		// for CHK_ERR
		break;

		case MT_CHK_ERR:  
			// next State : MT_CHK_STOP or MT_ERR
			currState = MT_CHEK_Err(lastTime, &(pTaskData->reqCnt), errReqCnt);
		break;	

		case MT_ERR:
			// next State : MT_IDLE
			currState = MT_Err(pTaskData);
			// UartPrintf("==> PA MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
			// UartPrintf("==> PA MT_FINISH \r\n");
			// next State : MT_IDLE
			currState = MT_Finish(pTaskData);
		break;

		default:	UartPrintf("Undefined PA State : %d !! \r\n", currState);	break;
	}
}

//===================================================================================//
// 	CT mode 동작시 CO 동작 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void CheckStopError(int *startCheck, unsigned int errTime_sec, int *errStatus)
{
	static int currState = MT_IDLE;
	static ULONG lastTime;
	static int waitTime;


	switch(currState) {
		case MT_IDLE:
			if(*startCheck == YES) {
				// initialize
				waitTime = 0;
				*errStatus = 0;
				lastTime = TimeNow();
				currState = MT_CHK_ERR;
			}
			break;

		case MT_CHK_ERR:
			// 조기 리턴
			if(TimePassed(lastTime) <= ONE_SECOND) {
				break;
			}

			++waitTime;
			if(waitTime < errTime_sec ) {
				// more wait
				lastTime = TimeNow();
				break;
			}
			else {
				// error 
				*errStatus = ERR_COM;
				startCheck = NO;
				currState = MT_IDLE;
			}
			break;
	}

}


void Mode_TaskCO_TrckingCO(taskData_t *pTaskData, t_TrackingCO *p_tc)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	const int errWaitTime = 70;
	// static int reqCnt;
	// static t_TrackingCO tc;
	static t_Position_um position_um;

	static int startErrorCheck;
	static int errStatus;


	switch(currState)
	{
		case MT_IDLE:
			// 조기 리턴
			if(pTaskData->event != EVENT_START) {
				currState = MT_IDLE;
				break;
			}

			// initailize
			pTaskData->event = EVENT_CLEAR;
			pTaskData->isEnd = NO;
			pTaskData->numError = 0;
			// reqCnt = 0;
			errStatus = 0;
			currState = MT_START;
			break;

		case MT_START:
			// 위치 및 TC관련 계산하고, CO CP, CW 위치로 이동시키고
			if( CheckObjValidity(&g_Obj) == NO ) {
				UartPrintf("Check Object info!! \r\n");
				currState = MT_FINISH;
				break;
			}

			p_tc->fpsCT = ct_capt_fps;
			p_tc->captureTime = ct_capt_frame/ct_capt_fps;
			CalculateTrackingCo(&g_Obj, p_tc);
			MoveTrackingCo(p_tc, &position_um);

			startErrorCheck = YES;
			currState = MT_CHK_STOP;
			break;		

		case MT_CHK_STOP:
			if( CheckStop_4Axis() ) {
				startErrorCheck = NO;
				currState = MT_FINISH;
				break;
			}

			if(errStatus == ERR_COM) {
				pTaskData->numError = ERR_COM;
				currState = MT_FINISH;
				break;
			}

			currState = MT_CHK_STOP;
			break;


		case MT_FINISH:
			if( CheckPosition_TC(&position_um))	{
				pTaskData->isEnd = YES;
			}
			else {
				pTaskData->numError = ERR_OFFSET;
			}

			currState = MT_IDLE;
			break;

		default:	PCPuts("Undefined CO State!\r\n");	break;
	}

	CheckStopError(&startErrorCheck, errWaitTime, &errStatus);
}

void Mode_TaskCO_4Axis(taskData_t *pTaskData, t_Info4Axis *pAxis4)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	const int errTime_sec = 70;
	static int reqCnt;
	static t_Position_um position_um;


	switch(currState)
	{
		case MT_IDLE:
			// 조기 리턴
			if(pTaskData->event != EVENT_START) {
				currState = MT_IDLE;
				break;
			}

			// initailize
			pTaskData->event = EVENT_CLEAR;
			pTaskData->isEnd = NO;
			pTaskData->numError = 0;
			reqCnt = 0;
			currState = MT_START;
			break;

		case MT_START:
			// ct offset postion으로 이동 
			Ctrl(HL_MOVE_UM, pAxis4->hl_offsetPosition_um);
			Ctrl(HR_MOVE_UM, pAxis4->hr_offsetPosition_um);
			Ctrl(VT_MOVE_UM, pAxis4->vt_offsetPosition_um);
			Ctrl(VB_MOVE_UM, pAxis4->vb_offsetPosition_um);
			currState = MT_CHK_STOP;
			break;

		case MT_CHK_STOP:
			if( CheckStop_4Axis() ) {
				currState = MT_FINISH;
				break;
			}
			else {
				currState = MT_CHK_ERR;
			}
			lastTime = TimeNow();		// for CHK_ERR
			break;

		case MT_CHK_ERR:
			// 조기리턴
			if( TimePassed(lastTime) <= ONE_SECOND) {
				currState = MT_CHK_ERR;
				break;
			}

			++reqCnt;
			if(reqCnt < errTime_sec ) {
				currState = MT_CHK_STOP;
			}
			else {	// 'timeErr'sec 동안 apply가 안되면 에러 리턴
				pTaskData->numError = ERR_COM;
				currState = MT_IDLE;
			}
			break;


		case MT_FINISH:
			if( CheckOffset_4Axis(pAxis4) ) {
				pTaskData->isEnd = YES;
			}
			else {
				pTaskData->numError = ERR_OFFSET;
			}
			currState = MT_IDLE;
			break;

		default:	PCPuts("Undefined CO State!\r\n");	break;
	}
}

BYTE CheckPosition_TC(t_Position_um *position_um)
{
	// [0.1mm]로 단위 맞추어 비교
	if( g_HL.move_distance == position_um->hl/100 &&
		g_HR.move_distance == position_um->hr/100 &&
		g_VT.move_distance == position_um->vt/100 &&
		g_VB.move_distance == position_um->vb/100 
		) 
		return 1;
	else
		return 0;
}

BYTE CheckOffset_4Axis(t_Info4Axis * pAxis4)
{
	if( g_HL.move_distance == pAxis4->hl_offsetPosition_um/100 &&
		g_HR.move_distance == pAxis4->hr_offsetPosition_um/100 &&
		g_VT.move_distance == pAxis4->vt_offsetPosition_um/100 &&
		g_VB.move_distance == pAxis4->vb_offsetPosition_um/100 ) 
		return 1;
	else
		return 0;
}

BYTE CheckStop_4Axis(void)
{
	if( IsMotorStop(&g_HL) &&
		IsMotorStop(&g_HR) &&
		IsMotorStop(&g_VT) &&
		IsMotorStop(&g_VB) ) 
		return 1;
	else
		return 0;
}


void Mode_TaskCO(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 70;


	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
			// ceph모드에서 CT로 전환할 때, guard를 위치를 위해 원점을 찍은 후 CT offset으로 이동
			if(pre_main_mode == SCAN_CEPH_LA_MODE || pre_main_mode == SCAN_CEPH_PA_MODE )
			{
			// UartPrintf("==> CO MT_START-1 \r\n");
				Ctrl(CO_MOVE, (LONG) GUARD_POS_CT);
			  	currState = MT_GUARD;
			}
			else
			{
			// UartPrintf("==> CO MT_START-2 \r\n");
				Ctrl(CO_MOVE_ACCU, pTaskData->offset_pos);
			  	currState = MT_CHK_STOP;
			}
		break;

		case MT_GUARD:
			if(IsMotorStop(&gvCO))
			{
				Ctrl(CO_MOVE_ACCU, pTaskData->offset_pos);
				currState = MT_CHK_STOP;
			}
		break;

		case MT_CHK_STOP:
			// next State : MT_CHK_ERR or MT_FINISH
			currState = MT_CHK_Stop(IsMotorStop(&gvCO));
			lastTime = TimeNow();		// for CHK_ERR
		break;

		case MT_CHK_ERR:  
			// next State : MT_CHK_STOP or MT_ERR
			currState = MT_CHEK_Err(lastTime, &(pTaskData->reqCnt), errReqCnt);
		break;

		case MT_ERR:
			// next State : MT_IDLE
			currState = MT_Err(pTaskData);
			// UartPrintf("==> CO MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
			if(gvCO.move_distance == (pTaskData->offset_pos/10))
			{
			// UartPrintf("==> CO MT_FINISH \r\n");
				currState = MT_Finish(pTaskData);
			}
			else 
			{
				// todo : offset error를 테스트 할 방법은 무엇??
				pTaskData->numError = ERR_OFFSET;
				currState = MT_IDLE;
			}
		break;

		default:	PCPuts("Undefined CO State!\r\n");	break;
	}
}

/*===================================================================================*/	
// Panorama Mode
/*===================================================================================*/
BYTE Mode_PANO(LONG panoMode, LONG value)
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static taskData_t taskPA, taskRO, taskCO;
	static t_Info4Axis axis4;


	switch(modeStep)
	{
		case STEP1:
			// UartPrintf("panoStep 1\r\n");
			SoundPlay(SOUND_APPLYING_PANO);	// 촬영을 위해 준비중입니다.
			gen_off();	ct_pano_det_off();	s_ceph_det_off();
			ring_off();	lamp_off();	LaserOff();

			exp_ok_flag = NO;	scout_mode = NO;	ro_end_tm_flag = 0;

			// task initialize // PA, RO, CO는 각 기능별로 Task 함수로 코딩됨
			taskRO.event = EVENT_START; taskRO.offset_pos = gvRO.pano_offset_pos;
			taskPA.event = EVENT_START; taskPA.offset_pos = gvPA.pano_offset_pos;
			taskCO.event = EVENT_START; taskCO.offset_pos = gvCO.pano_offset_pos;
			
			if(co_option == FOUR_AXIS_CO)
			{
				axis4.hl_offsetPosition_um = g_HL.pano_offset_pos;
				axis4.hr_offsetPosition_um = g_HR.pano_offset_pos;
				axis4.vt_offsetPosition_um = g_VT.pano_offset_pos;
				axis4.vb_offsetPosition_um = g_VB.pano_offset_pos;
			}

			if(panoMode == PANO_STAN)
			{
				main_mode = PANO_STAN_MODE;
				PCPuts("\r\nEco-X PANORAMA STANDARD MODE \r\n");
				// gvCO.pano_offset_pos = CO_PANO_OFFSET;
				// taskCO.offset_pos = gvCO.pano_offset_pos;
			}
			else if(panoMode == PANO_BITEWING)
			{
				main_mode = PANO_BITEWING_MODE;	
				PCPuts("\r\nEco-X PANORAMA BITEWING MODE \r\n");
				// gvCO.pano_offset_pos = CO_BW_OFFSET;
				// taskCO.offset_pos = gvCO.pano_offset_pos;
			}
			else if(panoMode == PANO_TMJ1)
			{
				main_mode = PANO_TMJ1_MODE;
				PCPuts("\r\nEco-X PANORAMA TMJ1 MODE \r\n");
				// gvCO.pano_offset_pos = CO_PANO_OFFSET;
				// taskCO.offset_pos = gvCO.pano_offset_pos;
			}

			//-----------------------------------------	Dark 출력 시작
			Ctrl(DARK_FRAME_ACQ, (LONG) FRAME_PANO);

			modeStep = STEP2;
		break;

		case STEP2:
			if(taskRO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] RO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] CO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskPA.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] PA \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_OFFSET) {	PCPuts("[SYS_ERR_03] CO OFFSET \r\n");	modeStep = STEP1;	return isCompleted; }

			if(taskRO.isEnd == YES && taskPA.isEnd == YES && taskCO.isEnd == YES)	
			{
			// UartPrintf("==>modeStep 3-1\r\n");
				modeStep = STEP3;
			}
			else { modeStep = STEP2; }
		break;

		case STEP3:
			if(sound_play_time_end)
			{					
			// UartPrintf("==>modeStep 3\r\n");
				SoundPlay(SOUND_APPLIED_PANO);	//촬영 준비가 되었습니다.
	            LaserOn();					// 수진자 정렬을 위해 레이저를 켬
	            if(isOnCanineBeam == YES)		LaserCanine(ON);
				else							LaserCanine(OFF);

				modeStep = STEP4;
			}
		break;

		case STEP4:
			if(sound_play_time_end)
			{
			// UartPrintf("panoStep 4 \r\n");
				pre_main_mode = main_mode;
				if(main_mode == PANO_STAN_MODE) PCPuts("[rpm_pano] \r\n\r\n");
				if(main_mode == PANO_BITEWING_MODE) PCPuts("[rpm_pano] \r\n\r\n");
				if(main_mode == PANO_TMJ1_MODE) PCPuts("[rpm_tmj1] \r\n\r\n");

				LCDPuts("[tml_pnmd]\r\n");
				PCPuts("//-------------------- Pano MODE is Completed!! \r\n");
				
				modeStep = STEP1;	
				return isCompleted;
			}
		break;
		
		default:	PCPuts("Undefined Pano STEP! \r\n\r\n");	break;
	}

	Mode_TaskRO(&taskRO);
	Mode_TaskPA(&taskPA);

	if(co_option == FOUR_AXIS_CO) {
		Mode_TaskCO_4Axis(&taskCO, &axis4);
	}
	else {
		Mode_TaskCO(&taskCO);
	}

	return notCompleted;
}

//===================================================================================//
// 	PANO TMJ2 함수 
//===================================================================================//
BYTE Mode_PANO_TMJ2(LONG panoMode, LONG value)
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;


	switch(modeStep)
	{
		case STEP1:
			if(pre_main_mode == PANO_TMJ1_MODE)
			{
		// UartPrintf("tmj2 Step1-1 \r\n");
				modeStep = STEP2;
			}
			else
			{
		// UartPrintf("tmj2 Step1-2 \r\n");
				PCPuts("\r\nCaution! TMJ1 mode first! \r\n");
				
				modeStep = STEP1;	
				return isCompleted;
			}
		break;

		case STEP2:
		// UartPrintf("tmj2 Step2 \r\n");
			
			gen_off();	ct_pano_det_off();	s_ceph_det_off();
			ring_off();	lamp_off();	LaserOff();
			
			exp_ok_flag = NO;	scout_mode = NO;	ro_end_tm_flag = 0;
			
			PCPuts("\r\nEco-X PANORAMA TMJ2 MODE \r\n");
			main_mode = PANO_TMJ2_MODE;
			
			//--------------------------------------------- 촬영 초기 위치로 이동
			Ctrl(PA_MOVE, gvPA.pano_offset_pos);
			Ctrl(RO_MOVE, pano_tmj2_prec_angle);
			
			modeStep = STEP3;
		break;
		
		case STEP3:
		// UartPrintf("tmj2 Step3 \r\n");
				modeStep = STEP4;
		break;
		
		case STEP4:
		// UartPrintf("tmj2 Step4 \r\n");
			if( (gvPA.org_complete_flag) && (gvPA.stop_flag)
			 && (gvRO.org_complete_flag) && (gvRO.stop_flag) )
				modeStep = STEP6;
		break;

		case STEP6:
		// UartPrintf("tmj2 Step6 \r\n");
			if(sound_play_time_end)
			{
				//tmj2 이후 prec 없이 촬영진행이 되어 안내멘트 구성 변경됨
				SoundPlay(SOUND_CLOSE_EYES);	//안전한 촬영을 위해 눈을 감아 주세요.
				Ctrl(TUBE_NORMAL_MODE, (LONG) 0);
			
				modeStep = STEP7;
			}		
		break;

		case STEP7:
			if(sound_play_time_end)
			{
		// UartPrintf("tmj2 Step end!! \r\n");
				PCPuts("[rpm_tmj2] \r\n\r\n");	LCDPuts("[tml_pnmd]\r\n");
				
				exp_ok_flag = YES;	exp_push = NO;
				
				pre_main_mode = main_mode;
				
				modeStep = STEP1;	
				return isCompleted;
			}
		break;
	}

	return notCompleted;
}


//===================================================================================*/	
// Ceph Mode
/*===================================================================================*/
BYTE Mode_CEPH(LONG cephMode, LONG value)
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static taskData_t taskPA, taskRO, taskCO, taskSD;
	static t_Info4Axis axis4;


	switch(modeStep)
	{
		case STEP1:
			// UartPrintf("cephStep1 \r\n");
			SoundPlay(SOUND_APPLYING_CEPH);	// 촬영을 위해 준비중입니다.
			
			gen_off();	ct_pano_det_off(); ring_off();	lamp_off();	LaserOff();

			exp_ok_flag = NO;	scout_mode = NO; ro_end_tm_flag = 0;

			taskRO.event = EVENT_START; taskRO.offset_pos = gvRO.ceph_offset_pos;
			taskPA.event = EVENT_START; taskPA.offset_pos = gvPA.ceph_offset_pos;
			taskCO.event = EVENT_START; 
			taskSD.event = EVENT_START;

			if(cephMode == SCAN_CEPH_LA)
			{
				PCPuts("\r\nEcoX SCAN-TYPE CEPHALO LATERAL MODE \r\n");
				main_mode = SCAN_CEPH_LA_MODE; PCPuts("[rpm_mode] \r\n");
				taskSD.offset_pos = gvSD.scla_offset_pos;
				taskCO.offset_pos = gvCO.scla_offset_pos;

				if(co_option == (FOUR_AXIS_CO||AXIS_ALL))
				{
					axis4.hl_offsetPosition_um = g_HL.scla_offset_pos;
					axis4.hr_offsetPosition_um = g_HR.scla_offset_pos;
					axis4.vt_offsetPosition_um = g_VT.scla_offset_pos;
					axis4.vb_offsetPosition_um = g_VB.scla_offset_pos;
				}
			}
			
			if(cephMode == SCAN_CEPH_PA)
			{
				PCPuts("\r\nEcoX SCAN-TYPE CEPHALO PA MODE \r\n");
				main_mode = SCAN_CEPH_PA_MODE; PCPuts("[rpm_mode] \r\n");
				taskSD.offset_pos = gvSD.scpa_offset_pos;
				taskCO.offset_pos = gvCO.scpa_offset_pos;

				if(co_option == FOUR_AXIS_CO||AXIS_ALL)
				{
					axis4.hl_offsetPosition_um = g_HL.scpa_offset_pos;
					axis4.hr_offsetPosition_um = g_HR.scpa_offset_pos;
					axis4.vt_offsetPosition_um = g_VT.scpa_offset_pos;
					axis4.vb_offsetPosition_um = g_VB.scpa_offset_pos;
				}
			}

			modeStep = STEP2;
		break;


		case STEP2:
			if(sound_play_time_end)
			{
			// UartPrintf("cephStep7 \r\n");
				SoundPlay(SOUND_CAPT_WAIT);	// 잠시만 기다려 주세요	
				modeStep = STEP3;
			} 
		break;

	
		case STEP3:
			if(taskRO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] RO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskPA.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] PA \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] CO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskSD.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] SD \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_OFFSET) {	PCPuts("[SYS_ERR_03] CO OFFSET \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskSD.numError == ERR_OFFSET) {	PCPuts("[SYS_ERR_03] SD OFFSET \r\n");	modeStep = STEP1;	return isCompleted; }

			if(taskRO.isEnd == YES && taskPA.isEnd == YES && taskCO.isEnd == YES && taskSD.isEnd == YES)
			{
			// UartPrintf("==>modeStep 3-1\r\n");
				modeStep = STEP4;
			}
			else { modeStep = STEP3; }
		break;

		case STEP4:
			if(sound_play_time_end) 
			{
				// UartPrintf("cephStep4 \r\n");
				SoundPlay(SOUND_APPLIED_CEPH);	//촬영 준비가 되었습니다.
				modeStep = STEP5;
			}
		break;
		
		case STEP5:
			if(sound_play_time_end)
			{
			// UartPrintf("cephStep5-1 \r\n");
				if(cephMode == SCAN_CEPH_LA) PCPuts("[rpm_cepl] \r\n\r\n");
				if(cephMode == SCAN_CEPH_PA) PCPuts("[rpm_cepp] \r\n\r\n");
				
				pre_main_mode = main_mode;
				LCDPuts("[tml_cpmd]\r\n");
				PCPuts("//-------------------- Ceph MODE is Completed!! \r\n");
				
				modeStep = STEP1;	
				return isCompleted;
			}
		break;

		default: 	break;
	}

	Mode_CEPH_TaskRO(&taskRO);
	Mode_CEPH_TaskPA(&taskPA);
	Mode_CEPH_TaskSD(&taskSD);

	if(co_option == FOUR_AXIS_CO) {
		Mode_TaskCO_4Axis(&taskCO, &axis4);
	}
	else if(co_option == ONE_AXIS_CO) {
		Mode_CEPH_TaskCO(&taskCO);
	}
	else if(co_option == AXIS_ALL) {
		Mode_TaskCO_4Axis(&taskCO, &axis4);
		Mode_CEPH_TaskCO(&taskCO);
	}
	
	

	return notCompleted;
}

//===================================================================================//
// 	CEPH mode 동작시 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void Mode_CEPH_TaskRO(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 70;

	
	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
			// UartPrintf("==> RO MT_START \r\n");
			Ctrl(RO_MOVE, (LONG)0);  // 0도부근으로 이동
			currState = MT_ORIGIN;
		break;

		case MT_ORIGIN:
			if( IsMotorStop(&gvRO) )
			{
			// UartPrintf("==> RO MT_ORIGIN \r\n");
				lastTime = TimeNow();
				currState = MT_WAIT_1S;
			}
		break;

		case MT_WAIT_1S:
			if(TimePassed(lastTime) > ONE_SECOND)	// 로테이터 역회전에 대비해 1초 쉰다.
			{
			// UartPrintf("==> RO MT_WAIT_1S \r\n");
				Ctrl(RO_MOVE, pTaskData->offset_pos);	
				currState = MT_CHK_STOP;
			}
		break;

		case MT_CHK_STOP:
			// UartPrintf("==> RO MT_CHK_STOP \r\n");
			currState = MT_CHK_Stop( IsMotorStop(&gvRO) );
			lastTime = TimeNow();		// for CHK_ERR
		break;

		case MT_CHK_ERR:  
			currState = MT_CHEK_Err(lastTime, &(pTaskData->reqCnt), errReqCnt);
		break;
    
		case MT_ERR:
			currState = MT_Err(pTaskData);
			// UartPrintf("==> RO MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
			// UartPrintf("==> RO MT_FINISH \r\n");
			currState = MT_Finish(pTaskData);
		break;

		default:	UartPrintf("Undefined RO State : %d !! \r\n", currState);	break;
	}
}

//===================================================================================//
// 	CEPH mode 동작시 PA 동작 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void Mode_CEPH_TaskPA(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 70;
	LONG	roDegree;


	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
		// UartPrintf("==> PA MT_START \r\n");

			// 모드 전환시 RO와 컬럼부 간섭(충돌)문제 : 
			// CT, Pano 모드 -> ceph 모드로 전환시 1차적으로 RO축을 0도 부근으로 한다.
			// CT, Pano PA 위치가 컬럼부 간섭구간(CT위치에서 +20mm) 이내이므로 간섭문제 없음.
			// ceph모드 -> ceph 모드 전환시 RO각도가 12도 이므로 컬럼부 간섭문제 없음. 
			// ro 각도가 90도 이상일때 ceph pa offset 으로 움직이면 RO와 컬럼 충돌 발생
			// ==> ro 각도가 90도 보다 작을 때까지 PA 이동 대기
			roDegree = RO_GetPositionCnt() / RO_1_DEGREE_PULSE ;	// ROpositionCnt를 각도로 환산 
			if(roDegree < 90)
			{
				Ctrl(PA_MOVE, pTaskData->offset_pos);
				currState = MT_CHK_STOP;
			}
		break;

		case MT_CHK_STOP:
		// UartPrintf("==> PA MT_CHK_STOP \r\n");
			currState = MT_CHK_Stop(gvPA.org_complete_flag && gvPA.stop_flag);
			lastTime = TimeNow();		// for CHK_ERR
		break;

		case MT_CHK_ERR:  
			currState = MT_CHEK_Err(lastTime, &(pTaskData->reqCnt), errReqCnt);
		break;	

		case MT_ERR:
			currState = MT_Err(pTaskData);
			// UartPrintf("==> PA MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
		// UartPrintf("==> PA MT_FINISH \r\n");
			currState = MT_Finish(pTaskData);
		break;

		default:	UartPrintf("Undefined PA State : %d !! \r\n", currState);	break;
	}
}

//===================================================================================//
// 	CEPH mode 동작시 CO 동작 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void Mode_CEPH_TaskCO(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 50;


	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
		  // UartPrintf("==> CO MT_START \r\n");
			// ceph모드에서 guard를 위치를 찍은 후 CEPh offset으로 이동
			Ctrl(CO_MOVE, (LONG) GUARD_POS_CEPH);
		  	currState = MT_GUARD;
		break;

		case MT_GUARD:
			if((gvCO.org_complete_flag) && (gvCO.stop_flag))
			{
	  			// UartPrintf("==> CO MT_GUARD\r\n");
				Ctrl(CO_MOVE_ACCU, pTaskData->offset_pos);
				currState = MT_CHK_STOP;
			}
		break;

		case MT_CHK_STOP:
		  	// UartPrintf("==> CO MT_CHK_STOP \r\n");
			currState = MT_CHK_Stop(IsMotorStop(&gvCO));
			lastTime = TimeNow();		// for MT_CHK_ERR
		break;

		case MT_CHK_ERR:  
			currState = MT_CHEK_Err(lastTime, &(pTaskData->reqCnt), errReqCnt);
		break;

		case MT_ERR:
			currState = MT_Err(pTaskData);
			// UartPrintf("==> CO MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
			if(gvCO.move_distance == (pTaskData->offset_pos/10))
			{
				// UartPrintf("==> CO MT_FINISH\r\n");
				// gvCO.move_distance = 0;		// todo why '0'????
				currState = MT_Finish(pTaskData);
			}
			else
			{
				// todo : offset error를 테스트 할 방법은 무엇??
				pTaskData->numError = ERR_OFFSET;
				currState = MT_IDLE;
			}
		break;

		default:	PCPuts("Undefined CO State!\r\n");	break;
	}
}

//===================================================================================//
// 	CEPH mode 동작시 SD 동작 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//===================================================================================//
void Mode_CEPH_TaskSD(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 50;


	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
			// UartPrintf("==> SD MT_START \r\n");
		  	Ctrl(SD_MOVE, pTaskData->offset_pos);
			currState = MT_CHK_STOP;
		break;

		case MT_CHK_STOP:
		  	// UartPrintf("==> SD MT_CHK_STOP \r\n");
			currState = MT_CHK_Stop(IsMotorStop(&gvSD));
			lastTime = TimeNow();		// for CHK_ERR
		break;

		case MT_CHK_ERR:  
			currState = MT_CHEK_Err(lastTime, &(pTaskData->reqCnt), errReqCnt);
		break;

		case MT_ERR:
			currState = MT_Err(pTaskData);
			// UartPrintf("==> SD MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
			if(gvSD.move_distance == pTaskData->offset_pos)
			{
				// UartPrintf("==> SD MT_FINISH\r\n");
				// gvCO.move_distance = 0;		// todo why '0'????
				currState = MT_Finish(pTaskData);
			}
			else
			{
				// todo : offset error를 테스트 할 방법은 무엇??
				pTaskData->numError = ERR_OFFSET;
				currState = MT_IDLE;
			}
		break;

		default:	PCPuts("Undefined SD State!\r\n");	break;
	}
}



/*===================================================================================*/	
 // PREC_COMPLETE
 // 설명 : CT, PANO, CEPH 등 모드를 끝내면(촬영위치를 잡으면 : (apply되었다고 한다)
 // 환자가 머리를 촬영부쪽에 위치를 시킨다. 그 후 PREC 실행 ==> READY상태?
 /*===================================================================================*/

//===================================================================================//
// 	CT PREC
//===================================================================================//
BYTE Mode_PREC_CT(LONG value)
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static LONG precAngle;
	static taskData_t taskRO, taskGen;


	switch(modeStep)
	{
		// gen의 pulse, continuous 모드 선택을 한다. 
		case STEP1:
		// UartPrintf("precStep1\r\n");
			Msg_kVmA();
			SoundPlay(SOUND_DONT_MOVE);		// 촬영중에는 몸을 움직이지 말아 주세요
			LaserOff();					// 촬영 전에는 레이저 끔
			// scout_mode = NO;	
			exp_ok_flag = NO;	// prec를 반복해서 보낼 경우, exp_ok_flag이 이미 YES로 되어 있기 때문에 다른 결과 발생하므로 추가
			software_exp = OFF;	// 실수로 swon을 보냈을 경우를 대비해서 추가. 단, 시험을 위해서 의도적으로 보내는 경우도 있으므로 예외 명령에 추가 등록
			software_rdy = ON;	//software_rdy = OFF;  // 구형 장비와 호환 위해서 임시로 ON 적용 //2018.06.08

			detector_ready = ON;
			// ro_end_tm_flag = 0;		// ??  capture.c에서 사용됨 and timer.c - 5ms tiemr interrupt routine에서 사용됨.

			if(main_mode == CT_SCOUT_MODE)
			{
				// capture.c - cap_exp()에서 CT_MODE의 설정값으로 원복된다.
				pre_ct_capt_frame = ct_capt_frame; 
				pre_ct_capt_fps = ct_capt_fps;
				ct_capt_frame = 400;	ct_capt_fps = 50;
			}

			if((ct_capt_frame/ct_capt_fps) == 8)    precAngle = -400;   // 8초 촬영이면 -40도로 이동
	        else                                    precAngle = -200;   // 나머지 촬영은 -20도로 이동

			taskRO.event = EVENT_START; taskRO.offset_pos = precAngle;
			taskGen.event = EVENT_START; taskGen.genMode = GEN_PULSE_MODE;

			modeStep = STEP3;
		break;

		case STEP3:
			// UartPrintf("==> precStep3\r\n");
			// todo : scout_mode -> main_mode == CT_MODE, main_mode == CT_SCOUT_MODE
			if(main_mode == CT_MODE)		PCPuts("\r\nCT PREC MODE \r\n");
			if(main_mode == CT_SCOUT_MODE)	PCPuts("\r\nCT SCOUT PREC MODE \r\n");

			// if(scout_mode == NO)	PCPuts("\r\nCT PREC MODE \r\n");
			// else					PCPuts("\r\nCT SCOUT MODE \r\n");

			modeStep = STEP4;
		break;

		case STEP4:
			if(sound_play_time_end)
			{
			// UartPrintf("==> precStep4\r\n");
				SoundPlay(SOUND_CLOSE_EYES);	// 안전한 촬영을 위해 눈을 감아 주세요
	            modeStep = STEP5;
			}
		break;

		case STEP5:
			if(taskRO.numError == ERR_COM)  {	PCPuts("[SYS_ERR_05] RO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskGen.numError == ERR_COM) {	gen_ack("ecom"); modeStep = STEP1;	return isCompleted; }

			if(taskRO.isEnd == YES && taskGen.isEnd == YES)	
			{
			// UartPrintf("==> precStep5-1\r\n");
				modeStep = STEP10;
			}
			else
			{
				modeStep = STEP5;	
			} 
		break;

		case STEP10:
			if(sound_play_time_end)
			{
				// UartPrintf("==> precStep10 \r\n");
				exp_ok_flag = YES;	// exp_ok_flag 	== YES일 때에만 조사스위치에 대해 반응함
				exp_push = NO;
				capt_tm_cnt = 0;	// 촬영 시간 출력 용 변수
				
				PCPuts("[rpm_prec] \r\n\r\n");

				if(main_mode == CT_MODE) 
					PCPuts("//-------------------- CT prec is Completed!! \r\n");
				if(main_mode == CT_SCOUT_MODE)
					PCPuts("//-------------------- CT scout prec is Completed!! \r\n");

				// todo : main_mode == CT_SCOUT_MODE 일때 메세지 넣기
				// PCPuts("//-------------------- CT prec is Completed!! \r\n");

				modeStep = STEP1;
				return isCompleted;
			}
		break;

		default:	PCPuts("Undefined PreComplete STEP! \r\n\r\n");	break;
	}

	Prec_TaskGen(&taskGen);
	Prec_TaskRO(&taskRO);

	return notCompleted;
}

//===================================================================================//
// 	Prec RO Task 함수
//===================================================================================//
void Prec_TaskGen(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 5;

	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
			// UartPrintf("==> Gen MT_START \r\n");
			if(gen_option == OFF)
			{
				currState = MT_FINISH;
				break;
			}
			
			if(pTaskData->genMode == GEN_PULSE_MODE) Ctrl(TUBE_PULSE_MODE, NULL);
			if(pTaskData->genMode == GEN_NORMAL_MODE) Ctrl(TUBE_NORMAL_MODE, NULL);

		  	lastTime = TimeNow();
		  	currState = MT_CHK_STOP;
		break;

		case MT_CHK_STOP:
			if(TimePassed(lastTime) > ONE_SECOND)
			{
				if(gen_mode_ack_flag == pTaskData->genMode)	currState = MT_FINISH;
				else		currState = MT_CHK_ERR;
				lastTime = TimeNow();		// for CHK_ERR
			}
		break;

		case MT_CHK_ERR:  
			if(TimePassed(lastTime) > ONE_SECOND)
			{
				++pTaskData->reqCnt;
				if(pTaskData->reqCnt < errReqCnt)
				{
			// UartPrintf("==> Gen MT_CHK_ERR-1 \r\n");
					if(pTaskData->genMode == GEN_PULSE_MODE) Ctrl(TUBE_PULSE_MODE, NULL);
					if(pTaskData->genMode == GEN_NORMAL_MODE) Ctrl(TUBE_NORMAL_MODE, NULL);
					lastTime = TimeNow();
					currState = MT_CHK_STOP;
				}
				else	// 5번 보냈음에도 응답이 없으면 통신 에러로 판단
				{
			// UartPrintf("==> Gen MT_CHK_ERR-2 \r\n");
					currState = MT_ERR;		
				}
			}
		break;

		case MT_ERR:
			// next State : MT_IDLE
			currState = MT_Err(pTaskData);
			// UartPrintf("==> Gen MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
		// UartPrintf("==> Gen MT_FINISH \r\n");
			// next State : MT_IDLE
			currState = MT_Finish(pTaskData);
		break;

		default:	UartPrintf("Undefined Gen State : %d !! \r\n", currState);	break;
	}
}

//===================================================================================//
// 	Prec mode 동작시 RO 동작 Task 함수 , 흐름제어를 상태머신 디자인 패턴으로 구현
//  pEvent(외부 이벤트)에 따라 CT mode RO 동작을 start 한다. 
//	TasK 함수 : 각 Task별로 독립적으로 구동하여 Multi Task가 가능하게 하기 위한 함수
//===================================================================================//
void Prec_TaskRO(taskData_t *pTaskData)
{
	static ULONG lastTime;
	static int currState = MT_IDLE;
	int errReqCnt = 70;


	switch(currState)
	{
		case MT_IDLE:
			currState = MT_Idle(pTaskData);
		break;

		case MT_START:
		// UartPrintf("==> RO MT_START \r\n");
			Ctrl(RO_MOVE, pTaskData->offset_pos);
		  	currState = MT_CHK_STOP;
		break;

		case MT_CHK_STOP:
		// UartPrintf("==> RO MT_CHK_STOP \r\n");
			// next State : MT_CHK_ERR or MT_FINISH
			currState = MT_CHK_Stop(IsMotorStop(&gvRO));
			lastTime = TimeNow();		// for CHK_ERR
		break;

		case MT_CHK_ERR:  
			// next State : MT_CHK_STOP or MT_ERR
		// UartPrintf("==> RO MT_CHK_ERR \r\n");
			currState = MT_CHEK_Err(lastTime, &(pTaskData->reqCnt), errReqCnt);
		break;
    
		case MT_ERR:
			// next State : MT_IDLE
			currState = MT_Err(pTaskData);
			// UartPrintf("==> RO MT_ERR : numError : %d\r\n", pTaskData->numError);
		break;

		case MT_FINISH:
		// UartPrintf("==> RO MT_FINISH \r\n");
			// next State : MT_IDLE
			currState = MT_Finish(pTaskData);
		break;

		default:	UartPrintf("Undefined RO State : %d !! \r\n", currState);	break;
	}
}


//===================================================================================//
// 	PANO PREC
//===================================================================================//
BYTE Mode_PREC_PANO(LONG value)
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static ULONG lastTime;
	static LONG	precAngle;
	static taskData_t taskRO, taskGen;
	static	BYTE chk_CAN_cnt;


	switch(modeStep)
	{
		case STEP1:
		// UartPrintf("==> panoPrec Step1 \r\n");
			Msg_kVmA();
			SoundPlay(SOUND_DONT_MOVE);		// 촬영중에는 몸을 움직이지 말아 주세요
			LaserOff();

			exp_ok_flag = NO; software_exp = OFF; software_rdy = ON;
			detector_ready = ON;
			// ro_end_tm_flag = 0;		// ??  capture.c에서 사용됨 and timer.c - 5ms tiemr interrupt routine에서 사용됨.

			if(isOnCanineBeam)	// 견치빔 값을 적용할때에는 (WillCapture ini 값으로 설정)
			{
			// UartPrintf("==> panoPrec Step1-1 \r\n");
				Silde_potentio_value = ad_read(4);
			}

			modeStep = STEP2;
		break;

		case STEP2:
		// UartPrintf("==> panoPrec Step3 \r\n");
			switch(main_mode)
			{
				case PANO_STAN_MODE:
					PCPuts("\r\nPANO STAN PREC MODE \r\n");
					precAngle = -pano_stan_prec_angle;
					if(isOnCanineBeam) Set_Canine_Values(Silde_potentio_value);
				break;
			
				case PANO_BITEWING_MODE:
					PCPuts("\r\nPANO BITEWING PREC MODE \r\n");
					precAngle = -pano_stan_prec_angle;		// todo : 값 정하기 : 일단 pano_stan 값으로 
					if(isOnCanineBeam) Set_Canine_Values(Silde_potentio_value);
				break;

				case PANO_TMJ1_MODE:
					PCPuts("\r\nPANO TMJ1 PREC MODE \r\n");
					precAngle	= -pano_tmj_prec_angle;
				break;
			
				case PANO_TMJ2_MODE:
					PCPuts("\r\nPANO TMJ2 PREC MODE \r\n");
					precAngle	= pano_tmj2_prec_angle;
				break;

				default: PCPuts("Undefined main_mode\r\n");
				break;

			}

			taskRO.event = EVENT_START; taskRO.offset_pos = precAngle;
			taskGen.event = EVENT_START; taskGen.genMode = GEN_NORMAL_MODE;

			modeStep = STEP3;
		break;

		case STEP3:
			if(sound_play_time_end)
			{
			// UartPrintf("==> panoPrec Step4-1 \r\n");
				SoundPlay(SOUND_CLOSE_EYES);	// 안전한 촬영을 위해 눈을 감아 주세요
				modeStep = STEP4;
			}
		break;

		case STEP4:
			if(taskRO.numError == ERR_COM)  {	PCPuts("[SYS_ERR_05] RO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskGen.numError == ERR_COM) {	gen_ack("ecom"); modeStep = STEP1;	return isCompleted; }

			if(taskRO.isEnd == YES && taskGen.isEnd == YES)	
			{
			// UartPrintf("==> precStep5-1\r\n");
				modeStep = STEP5;
			}
			else
			{
				modeStep = STEP4;	
			} 
		break;

		case STEP5:
			if(sound_play_time_end)
			{
			// UartPrintf("==> panoPrec Step10-1 \r\n");
				exp_ok_flag = YES;	// exp_ok_flag 	== YES일 때에만 조사스위치에 대해 반응함
				exp_push = NO;
				capt_tm_cnt = 0;	// 촬영 시간 출력 용 변수
				
				PCPuts("[rpm_prec] \r\n\r\n");
				PCPuts("//-------------------- Pano prec is Completed!! \r\n");
				
				modeStep = STEP1;	
				return isCompleted;
			}
		break;

		default:	PCPuts("Undefined PANO PreC STEP! \r\n\r\n");	break;
	}

	Prec_TaskGen(&taskGen);
	Prec_TaskRO(&taskRO);

	return notCompleted;
}


void Set_Canine_Values(WORD adcCanine)
{
	//-------------------------------------------------------------------------------------
	// 견치빔용 가변저항의 이동 범위: 40 mm
	// 양쪽 끝 3 mm씩은 비선형 구간이므로 선형 구간인 3 mm ~ 37 mm 사이만 사용)
	// 선형 구간의 전압을 AD 변환하면 4950 ~ 61050
	// 편의상 최대, 최소값을 5000, 60000으로 사용하면 실제 사용 구간은 3.03 mm ~ 36.36 mm
	const LONG slidePotenMax = 60000;
	const LONG slidePotenMin = 5000;
	static LONG temp, value;
	const LONG	maxLimit_PAX = -170;
	
	// Silde_potentio_value
	// adcCaninePosition
	// adcCanineValue

	PCPrintf("Canine beam AD range = %ld ~ %ld \r\n", slidePotenMin, slidePotenMax);
	
	if(adcCanine > slidePotenMax)		// 견치빔 위치 최대값 초과할 경우 (37 mm 초과)
	{
		PCPrintf("Caution!! Slide potentio value( = %u) is Over!! \r\n",adcCanine);
		adcCanine = slidePotenMax;	// 강제로 최대값 고정
	}
	else if(adcCanine > slidePotenMin - 1)
	{
		PCPrintf("Slide potentio value = %u \r\n",adcCanine);
	}
	else
	{									
		PCPrintf("Caution!! Slide potentio value( = %u) is Under!! \r\n",adcCanine);
		adcCanine = slidePotenMin;	// 강제로 최소값 고정
	}
	
	value = (LONG)(adcCanine - slidePotenMin);	// 실제 적용될 값은 최소값만큼 뺀 값
	// value = value - 55050;							// Qface에 있는 코드 : 삭제해야 하는 것 같음.견치빔으로 이동할 수 있는 거리 반대로 변환
	
	temp = value / 165;								// 0.1 mm 단위 로 변환 (165는 slide potentiometer의 전압에 따른 비율)
	PCPrintf("Canine Beam distance = %ld [0.1 mm] \r\n",temp);	// 견치빔에 의한 P 축 이동거리 확인
	

	//
	temp = temp + Panomode_paxis_value;
	value = temp;


	// 조립공차등을 고려하여 p축 (-) 의 최대거리에서 3mm 정도 여유두어야 함.
	// eco x는 -20 mm 이니 최대 허용은 -17mm로 정함(maxLimit_PAX = -170)
	if(value < maxLimit_PAX)		
	{
		PCPuts("[SYS_CHK_06] \r\n");			// pano축 이동거리 초과되어서
		// PCPrintf("Paxis distance = %ld [0.1 mm] \r\n", gvPA.pano_offset_pos);	// -97 표현
		// Ctrl(PA_MOVE, gvPA.pano_offset_pos);		//pano offset 값으로 이동
		PCPrintf("Paxis distance = %ld [0.1 mm] \r\n", maxLimit_PAX);	// -97 표현
		Ctrl(PA_MOVE, maxLimit_PAX);		//pano offset 값으로 이동
	}
	else
	{	
		PCPrintf("Paxis distance = %ld [0.1 mm] \r\n", value);	// -97 표현
		Ctrl(PA_MOVE, value);	
	}
}

//===================================================================================//
// 	PANO PREC
//===================================================================================//
BYTE Mode_PREC_CEPH(LONG value)
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static ULONG lastTime;
	static BYTE genAckCnt = 0;
	static taskData_t taskGen;

	switch(modeStep)
	{
		case STEP1:
			Msg_kVmA();
			SoundPlay(SOUND_DONT_MOVE);		// 촬영중에는 몸을 움직이지 말아 주세요
			LaserOff();					// 촬영 전에는 레이저 끔
			// scout_mode = NO;	
			exp_ok_flag = NO;	// prec를 반복해서 보낼 경우, exp_ok_flag이 이미 YES로 되어 있기 때문에 다른 결과 발생하므로 추가
			software_exp = OFF;	// 실수로 swon을 보냈을 경우를 대비해서 추가. 단, 시험을 위해서 의도적으로 보내는 경우도 있으므로 예외 명령에 추가 등록
			software_rdy = ON;	//software_rdy = OFF;  // 구형 장비와 호환 위해서 임시로 ON 적용 //2018.06.08

			detector_ready = ON;
			// ro_end_tm_flag = 0;		// ??  capture.c에서 사용됨 and timer.c - 5ms tiemr interrupt routine에서 사용됨.

			taskGen.event = EVENT_START; taskGen.genMode = GEN_NORMAL_MODE;

			// Ctrl(TUBE_NORMAL_MODE, NULL);								
            // hand_lamp_mode = 0; // pano mode 램프 동작 시작  // ??
            modeStep = STEP3;
		break;

		case STEP3:
		// UartPrintf("cephPrecStep3 \r\n");
			if(main_mode == SCAN_CEPH_LA_MODE)	PCPuts("\r\nSCAN CEPH LA PREC MODE \r\n");
			if(main_mode == SCAN_CEPH_PA_MODE)	PCPuts("\r\nSCAN CEPH PA PREC MODE \r\n");

			modeStep = STEP4;
		break;


		case STEP4:
			if(sound_play_time_end)
			{
				SoundPlay(SOUND_CLOSE_EYES);	// 안전한 촬영을 위해 눈을 감아 주세요
				modeStep = STEP5;
			}
		break;

		case STEP5:
			if(taskGen.numError == ERR_COM) {	gen_ack("ecom"); modeStep = STEP1;	return isCompleted; }

			if(taskGen.isEnd == YES)	
			{
			// UartPrintf("==> precStep5-1\r\n");
				modeStep = STEP10;
			}
			else
			{
				modeStep = STEP5;	
			} 
		break;

		case STEP10:
			if(sound_play_time_end)
			{
		// UartPrintf("cephPrecStep10 \r\n");
			exp_ok_flag = YES;	// exp_ok_flag 	== YES일 때에만 조사스위치에 대해 반응함
			exp_push = NO;

			capt_tm_cnt = 0;	// 촬영 시간 출력 용 변수
			PCPuts("[rpm_prec] \r\n\r\n");
			PCPuts("//-------------------- Ceph prec is Completed!! \r\n");

			modeStep = STEP1;
			return isCompleted;
			}
		break;

		default:	PCPuts("Undefined Ceph PreComplete STEP! \r\n\r\n");	break;
	}

	Prec_TaskGen(&taskGen);

	return notCompleted;
}

//===================================================================================//
// 	SCOUT PREC
//===================================================================================//
// todo : CT prec 사용과 거의 동일 ==> 코드 중복 제거 필요 (caputure.c 도 수정 필요)
//    ==> Mode_PREC_CT() 에 통합시킴, 검증후 함수 제외해도 될 듯
BYTE Mode_SCOUT_SET(LONG value)		// [scst]
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static LONG precAngle;
	static taskData_t taskRO, taskGen;


	// early return
	if(main_mode != CT_SCOUT_MODE)
	{
		PCPuts("Caution! CT-SCOUT mode only! \r\n");
		
		return	isCompleted;
	}

	switch(modeStep)
	{
		// gen의 pulse, continuous 모드 선택을 한다. 
		case STEP1:
		// UartPrintf("precStep1\r\n");
			Msg_kVmA();
			SoundPlay(SOUND_DONT_MOVE);		// 촬영중에는 몸을 움직이지 말아 주세요
			LaserOff();					// 촬영 전에는 레이저 끔
			// scout_mode = NO;	
			exp_ok_flag = NO;	// prec를 반복해서 보낼 경우, exp_ok_flag이 이미 YES로 되어 있기 때문에 다른 결과 발생하므로 추가
			software_exp = OFF;	// 실수로 swon을 보냈을 경우를 대비해서 추가. 단, 시험을 위해서 의도적으로 보내는 경우도 있으므로 예외 명령에 추가 등록
			software_rdy = ON;	//software_rdy = OFF;  // 구형 장비와 호환 위해서 임시로 ON 적용 //2018.06.08

			detector_ready = ON;
			// ro_end_tm_flag = 0;		// ??  capture.c에서 사용됨 and timer.c - 5ms tiemr interrupt routine에서 사용됨.

			// scout 관련된 셋팅!! 나머지는 PREC_CT와 같다.
			// capture.c cap_exp()에서 CT_MODE의 설정값으로 원복된다.
			// todo
			pre_ct_capt_frame = ct_capt_frame; 
			pre_ct_capt_fps = ct_capt_fps;
			ct_capt_frame = 400;	ct_capt_fps = 50;

			if((ct_capt_frame/ct_capt_fps) == 8)    precAngle = -400;   // 8초 촬영이면 -40도로 이동
	        else                                    precAngle = -200;   // 나머지 촬영은 -20도로 이동

			taskRO.event = EVENT_START; taskRO.offset_pos = precAngle;
			taskGen.event = EVENT_START; taskGen.genMode = GEN_PULSE_MODE;

			modeStep = STEP3;
		break;

		case STEP3:
			// UartPrintf("precStep3\r\n");
			PCPuts("\r\nCT SCOUT MODE \r\n");
			modeStep = STEP4;
		break;

		case STEP4:
			if(sound_play_time_end)
			{
			// UartPrintf("==> precStep4\r\n");
				SoundPlay(SOUND_CLOSE_EYES);	// 안전한 촬영을 위해 눈을 감아 주세요
	            modeStep = STEP5;
			}
		break;

		case STEP5:
			if(taskRO.numError == ERR_COM)  {	PCPuts("[SYS_ERR_05] RO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskGen.numError == ERR_COM) {	gen_ack("ecom"); modeStep = STEP1;	return isCompleted; }

			if(taskRO.isEnd == YES && taskGen.isEnd == YES)	
			{
			// UartPrintf("==> precStep5-1\r\n");
				modeStep = STEP10;
			}
			else
			{
				modeStep = STEP5;	
			} 
		break;

		case STEP10:
			if(sound_play_time_end)
			{
				// UartPrintf("precStep10\r\n");
				exp_ok_flag = YES;	// exp_ok_flag 	== YES일 때에만 조사스위치에 대해 반응함
				exp_push = NO;
				capt_tm_cnt = 0;	// 촬영 시간 출력 용 변수

				PCPuts("[rpm_prec] \r\n\r\n");
				PCPuts("//-------------------- SCOUT prec is Completed!! \r\n");

				modeStep = STEP1;
				return isCompleted;
			}
		break;

		default:	PCPuts("Undefined PreComplete STEP! \r\n\r\n");	break;
	}

	Prec_TaskGen(&taskGen);
	Prec_TaskRO(&taskRO);
	// Mode_TaskRO(&taskRO);

	return notCompleted;
}


//===================================================================================
//
//===================================================================================
BYTE Mode_FIRMWARE_REVISION_NUM(LONG value)
{
	const BYTE isCompleted = 1;
	const BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static ULONG lastTime;

	// CAN통신을 위한 step은 불필요한듯...
	switch(modeStep)
	{
		case STEP1:
			PCPrintf("\r\nMaster = %s \r\n",REVISION_NUM);
			lastTime = TimeNow();
			modeStep = STEP2;
		break;
		
		case STEP2:
			if(TimePassed(lastTime) > 10)	// 50 ms
			{	
				rcan_puts(CO_MOTOR, CAN_FW_REV_NUM, dmy_val, dmy_msg);	
				lastTime = TimeNow();
				modeStep = STEP3;
			}
		break;
		
		case STEP3:
			if(TimePassed(lastTime) > 10)	// 50 ms
			{
				rcan_puts(SD_MOTOR, CAN_FW_REV_NUM, dmy_val, dmy_msg);
				lastTime = TimeNow();
				modeStep = STEP4;
			}
		break;

		case STEP4:
			if(TimePassed(lastTime) > 10)	// 50 ms
			{
				rcan_puts(RO_ADC, CAN_FW_REV_NUM, dmy_val, RO_ADC);
				lastTime = TimeNow();
				modeStep = STEP5;
			}
		break;
		
		case STEP5:
			if(TimePassed(lastTime) > 10)	// 50 ms
			{
				GenPuts("(QFRV)");
				lastTime = TimeNow();
				modeStep = STEP6;
			}
		break;
		
		case STEP6:
			if(TimePassed(lastTime) > ONE_SECOND)	// 50 ms
			{
				PCPuts("\r\n[rpm_frn?] \r\n\r\n");
				modeStep = STEP1;	
				return isCompleted;
			}
		break;

		default:	PCPuts("\r\n Warning! Unknown step in FIRMWARE_REVISION_NUM \r\n");	break;
	}

	return notCompleted;
}


/*===================================================================================*/	
// Bright frame acq
/*===================================================================================*/
BYTE Mode_BRIGHT_FRAME_ACQ(LONG frameMode)	// [brit]
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	
	static ULONG lastTime;
	static BYTE genAckCnt = 0;
	LONG detPeriodCnt = 0;

	
	switch(modeStep)
	{
		case STEP1:
			ring_off();	gen_off();
			exp_ok_flag = NO;	// exp_ok_flag을 NO로 설정하지 않으면 align_exp_time_flag이 동작하지 않을 수 있다.
			
			if( frameMode == FRAME_CT )  // '1' : ct mode
			{
                detPeriodCnt = 100000 / ct_capt_fps;
				SetTriggerPulse_CT(detPeriodCnt);

				Ctrl(TUBE_PULSE_MODE,(LONG) 0);
			}
			else if(frameMode == FRAME_PANO)
			{
                //todo : 아래 주석은 덴트리 9차 소스의 주석처리 된건데, 2번째줄을 포함되어야 하는 듯한느데 확인 
                /*        if(ct_det_optn == V_P_1313) pulse2_period = 526;    // 100000 / 526 = 190.114
                else                        pulse2_period = 500;    // 100000 / 500 = 200*/
				
				trig_pulse_setting(FRAME_PANO);
				Ctrl(TUBE_NORMAL_MODE,(LONG) 0);
			}
			else if(frameMode == FRAME_CEPH)
			{
				pulse2_period = 500;	// 파노모드 기본값
				trig_pulse_setting(FRAME_CEPH);

				Ctrl(TUBE_NORMAL_MODE,(LONG) 0);

			}

            trig_pulse_time	= (brit_time+300)*100;			// 브라이트 영상 받는 시간 보다 0.3 초 더 조사
            align_exp_time_10u = trig_pulse_time-200;		// ???
	
			modeStep = STEP2;
		break;

		case STEP2:	// generator ack wait
		// UartPrintf("britStep2 \r\n");
			if( (gen_mode_ack_flag == GEN_NORMAL_MODE) || (gen_mode_ack_flag == GEN_PULSE_MODE) )
			{
		// UartPrintf("britStep2-1 \r\n");
				rdy_lamp_on();	gen_rdy();
				
				modeStep = STEP3;
			}
		break;
		case STEP3:
			if(gen_preheat_time_flag == NO)
			{
		// UartPrintf("britStep3-1 \r\n");
				exp_lamp_on();
				
				if(gen_mode_ack_flag == GEN_NORMAL_MODE)		gen_cont_exp();
				else if(gen_mode_ack_flag == GEN_PULSE_MODE)	gen_pulsed_exp();
				
				align_exp_time_flag	= YES;
				
				modeStep = STEP4;
			}
		break;
		
		case STEP4:
			if(align_exp_time_cnt >= 20000)	// 조사 시작하고 출력 안정화 된 0.2초 이후부터 디텍터 트리거 출력 시작
			{
		// UartPrintf("britStep4-1 \r\n");
				ring_on();
				
				switch(frameMode)
				{
					case FRAME_CT:
					case FRAME_UFS:		ct_pano_det_capt();	SetGenPulseCnt(0);	break;
					case FRAME_PANO:	ct_pano_det_capt();	SetGenPulseCnt(0);	break;
					case FRAME_CEPH:	s_ceph_det_exp_capt(); 		break;
				}
				modeStep = STEP5;
			}
		break;
		
		case STEP5:
			if(brit_end_flag)
			{
		// UartPrintf("britStep5-1 \r\n");
			    brit_end_flag = NO;
				gen_off();	ring_off();

				s_ceph_det_off();

                ct_pano_det_off(); lamp_off();
                modeStep = STEP1;	
                    						
				brit_ack_flag = YES;
                return isCompleted;
			}
		break;

		default:	PCPuts("\r\n Warning! Unknown step in alex \r\n");	break;
	}

	return notCompleted;
}

/*===================================================================================*/	
// Use : 지정된 msec동안 X-ray를  조사한다. 
//	pulse : [pmc_tupl][pmc_alex_3000]  	==> 자동으로 [pmm_alex_3000]으로 넘어오면서 3초간 X-ray 조사된다. 
//	continuous : [pmc_tunr][pmc_alex_3000] ==> 자동으로 [pmm_alex_3000]으로 넘어오면서 3초간 X-ray 조사된다. 
/*===================================================================================*/
BYTE Mode_ALIGN_EXPOSE(LONG value)	// [alex]
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	LONG detPeriodCnt = 0;


	switch(modeStep)
	{
		case STEP1:
			ring_off();	gen_off();
			exp_ok_flag = NO;	// exp_ok_flag을 NO로 설정하지 않으면 align_exp_time_flag이 동작하지 않을 수 있다.
			
			align_exp_time_10u = alex_val * 100;	// 1 ms 단위를 10 us 단위로 변환
			modeStep = STEP2;
		break;
		
		case STEP2:	// generator ack wait
			if(gen_mode_ack_flag == GEN_PULSE_MODE)
			{
				detPeriodCnt = 100000 / ct_capt_fps;
				SetTriggerPulse_CT(detPeriodCnt);
                
                rdy_lamp_on();	gen_rdy();
				
				modeStep = STEP3;
			}
			else if(gen_mode_ack_flag == GEN_NORMAL_MODE)
			{
				rdy_lamp_on();	gen_rdy();
				
				modeStep = STEP3;
			}
		break;
		
		case STEP3:
			if(gen_preheat_time_flag == NO)
			{
				exp_lamp_on();	ring_on();
				
				if(gen_mode_ack_flag == GEN_NORMAL_MODE)		
				{	
				// UartPrintf("==> alex normal\r\n");
					gen_cont_exp();	
				}
				else if(gen_mode_ack_flag == GEN_PULSE_MODE)	
				{	
				// UartPrintf("==> alex pulse\r\n");
					gen_pulsed_exp();	
					ct_pano_det_capt();	
				}
				
				align_exp_time_flag	= YES;	// 조사 종료는 timer.c에서
				
				modeStep = STEP1;	
			 	// cmd_mode_flag = NO;
				return isCompleted;
			}
		break;
		
		default:	PCPuts("\r\n Warning! Unknown step in alex \r\n");	break;
	}
	return notCompleted;
}


/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
BYTE CmdMode_LCD(LONG inst, LONG value)
{
	static	BYTE isCmdCompleted =0;
	static	BYTE isCompleted =0;
	const BYTE notCompleted = 0;


	// UartPrintf("==> here in CmdMode_LCD() \r\n");
	// 전시회 모드 일경우

	#ifdef EXHIBIT_MODE
	if( (inst == CT) || (inst == PANO_STAN))
	{
		if(isOnExhibition == YES) 
		{
			isOnExhibition = NO;
			UartPrintf("isOnExhibition = NO \r\n");
		}
		else 
		{
			isOnExhibition = YES;
			UartPrintf("isOnExhibition = YES \r\n");
		}

		LCDPuts("[tml_ctmd]\r\n");

		return 1;	// '1' Cmd Mode Completed!!
	}
	#endif


	switch(inst)
	{	
		case CT:					// [ct__]	
			isCmdCompleted = ModeLCD_CT(value); 
			return isCmdCompleted;

		case PANO_STAN:				// [pano]
			isCmdCompleted = ModeLCD_PANO(inst, value); 	
			return isCmdCompleted;
	}

	return notCompleted;
}

/*===================================================================================*/	
// Panorama Mode LCD
/*===================================================================================*/
BYTE ModeLCD_PANO(LONG panoMode, LONG value)
{
	const static BYTE isCompleted = 1;
	const static BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static taskData_t taskPA, taskRO, taskCO;
	

	switch(modeStep)
	{
	
		case STEP1:
			// UartPrintf("panoStep 1\r\n");
			SoundPlay(SOUND_APPLYING_PANO);	// 촬영을 위해 준비중입니다.
			gen_off();	ct_pano_det_off();	s_ceph_det_off();
			ring_off();	lamp_off();	LaserOff();

			exp_ok_flag = NO;	scout_mode = NO;	ro_end_tm_flag = 0;

			// task initialize // PA, RO, CO는 각 기능별로 Task 함수로 코딩됨
			taskRO.event = EVENT_START;	taskRO.offset_pos = gvRO.pano_offset_pos;
			taskPA.event = EVENT_START; taskPA.offset_pos = gvPA.pano_offset_pos;
			taskCO.event = EVENT_START; taskCO.offset_pos = gvCO.pano_offset_pos;


			main_mode = PANO_STAN_MODE;
			PCPuts("\r\nEco-X PANORAMA STANDARD MODE REPOS BUTTON \r\n");

			//-----------------------------------------	Dark 출력 시작
			Ctrl(DARK_FRAME_ACQ, (LONG) FRAME_PANO);

			modeStep = STEP2;
		break;

		case STEP2:
			if(taskRO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] RO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] CO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskPA.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] PA \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_OFFSET) {	PCPuts("[SYS_ERR_03] CO OFFSET \r\n");	modeStep = STEP1;	return isCompleted; }

			if(taskRO.isEnd == YES && taskPA.isEnd == YES && taskCO.isEnd == YES)	
			{
			// UartPrintf("==>modeStep 3-1\r\n");
				modeStep = STEP3;
			}
			else { modeStep = STEP2; }
		break;

		case STEP3:
			if(sound_play_time_end)
			{					
			// UartPrintf("==>modeStep 3\r\n");
				SoundPlay(SOUND_APPLIED_PANO);	//촬영 준비가 되었습니다.
	            LaserOn();					// 수진자 정렬을 위해 레이저를 켬
	            if(isOnCanineBeam == YES)		LaserCanine(ON);
				else							LaserCanine(OFF);

				modeStep = STEP4;
			}
		break;

		case STEP4:
			if(sound_play_time_end)
			{
			// UartPrintf("panoStep 4\r\n");
				pre_main_mode = main_mode;
				PCPuts("[tml_pano] \r\n\r\n");
				LCDPuts("[tml_pnmd]\r\n");
				PCPuts("//-------------------- Pano MODE is Completed!! \r\n");
				
				modeStep = STEP1;	
				return isCompleted;
			}
		break;
		
		default:	PCPrintf("Undefined Pano LCD STEP \r\n");	break;
	}

	Mode_TaskRO(&taskRO);
	Mode_TaskCO(&taskCO);
	Mode_TaskPA(&taskPA);

	return notCompleted;
}


//============================================================================================================	
// CT Mode LCD
//============================================================================================================
BYTE ModeLCD_CT(LONG value)
{
	const BYTE isCompleted = 1;
	const BYTE notCompleted = 0;
	static BYTE modeStep = STEP1;
	static taskData_t taskPA, taskRO, taskCO;

	switch(modeStep)
	{
		case STEP1:
			// UartPrintf("step 1\r\n");
			SoundPlay(SOUND_APPLYING_CT); 		// 촬영을 위해 준비중입니다.
			gen_off();	ct_pano_det_off();	s_ceph_det_off(); 
			ring_off();	lamp_off();	LaserOff();

			exp_ok_flag = NO;	scout_mode = NO;	ro_end_tm_flag = 0;
			
			// task initialize //PA, RO, CO는 각 기능별로 Task 함수로 동작됨
			taskRO.event = EVENT_START; taskRO.offset_pos = gvRO.ct_offset_pos;
			taskPA.event = EVENT_START; taskPA.offset_pos = gvPA.ct_offset_pos;
			taskCO.event = EVENT_START; taskCO.offset_pos = gvCO.ct_offset_pos;

			main_mode = CT_MODE;
			PCPuts("\r\nEcoX CT MODE REPOS BUTTON\r\n");

			//-----------------------------------------	Dark 출력 시작
			Ctrl(DARK_FRAME_ACQ, (LONG) FRAME_CT);  // '1' : CT mode
			
            modeStep = STEP2;
		break;

		case STEP2:
			if(taskRO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] RO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] CO \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskPA.numError == ERR_COM) {	PCPuts("[SYS_ERR_05] PA \r\n");	modeStep = STEP1;	return isCompleted; }
			if(taskCO.numError == ERR_OFFSET) {	PCPuts("[SYS_ERR_03] CO OFFSET \r\n");	modeStep = STEP1;	return isCompleted; }

			if(taskRO.isEnd == YES && taskPA.isEnd == YES && taskCO.isEnd == YES)	
			{
			// UartPrintf("==> modeStep2-1\r\n");
				modeStep = STEP3;
			}
			else modeStep = STEP2;
		break;

		case STEP3:
			if(sound_play_time_end)
			{
			// UartPrintf("==> modeStep3\r\n");
				SoundPlay(SOUND_APPLIED_CT);	//촬영 준비가 되었습니다.
	            LaserOn();					// 수진자 정렬을 위해 레이저를 켬
				modeStep = STEP4;
			}
		break;

		case STEP4:
			if(sound_play_time_end)
			{
			// UartPrintf("modeStep4\r\n");
				pre_main_mode = main_mode;
				PCPuts("[tml_ct__] \r\n\r\n");
				LCDPuts("[tml_ctmd]\r\n");
				PCPuts("//-------------------- CT MODE is Completed!! \r\n");
	
				modeStep = STEP1;
				return isCompleted;
			}
		break;
		
		default:	PCPuts("Undefined CT_LCD STEP! \r\n\r\n");	break;
	}

	Mode_TaskRO(&taskRO);
	Mode_TaskPA(&taskPA);
	Mode_TaskCO(&taskCO);

	return notCompleted;
}


//============================================================================================================	
// 
//============================================================================================================
void Msg_kVmA(void)
{
    if( !kv_value || !ma_value ) PCPuts("Msg: kV or mA not set!! \r\n");
}