/*C***************************************************************************
* $RCSfile: motor_drive.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the motor_drive source file
*----------------------------------------------------------------------------
* RELEASE:      $Name: motor_drive     
* Edited  By    $Id: sslim 
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include 	"includes.h"

/*_____ D E C L A R A T I O N ______________________________________________*/
enum OriginStates
{ 	ST_INIT, ST_IDLE, ST_CHK_POSITION, ST_READY_ZONE, ST_FIND_ORG, 
	ST_FINISH_ORG, ST_GO_ORG, 
};



/*_____ D E F I N E ________________________________________________________*/

//------ CAN ADC 값
// todo : 양산 조립 사양에 따라 ADC 계산값 넣기(가변저항 turn수, RO 유효각도에 따른 turn수...)
#define ADC_DEGREE_0        14034               // 0도 부근의 CAN ADC값
#define ADC_DEGREE_360      50147               // 360도 부근의 CAN ADC값
#define ONE_DEGREE     ((ADC_DEGREE_360 - ADC_DEGREE_0)/360)   // 단위 1도에 대한 CAN ADC 값



/*_____ F U C T I O N ______________________________________________________*/
void RO_SetInitialValue(void);
void PA_SetInitialValue(void);
void PA_MotorStart(BYTE, LONG);
void RO_MotorStart(BYTE, LONG);
void PA_MotorDrive(BYTE, LONG, LONG, LONG);
void RO_MotorDrive(BYTE, LONG, LONG, LONG);
void PA_MotorStop(void);
void RO_MotorStop(void);
void PA_SetOrigin(BYTE);
void RO_SetOrigin(BYTE);
// void PA_MotorSlowStop(void);

//------------ interface functions
LONG RO_GetPositionCnt(void);
LONG PA_GetPositionCnt(void);
LONG RO_GetSpeed(void);
LONG PA_GetSpeed(void);
void PA_UpdateState(void);
void RO_UpdateState(void);

void PA_MotorChangeSpeed(BYTE, LONG);
void RO_MotorChangeSpeed(BYTE, LONG);

//----- pano 관련 func
void PA_increase_stage(BYTE);
void RO_increase_stage(BYTE);
void PA_stop_cnt(LONG);
void RO_stop_cnt(LONG);
void PA_tot_pulse(LONG);
void RO_tot_pulse(LONG);

BYTE IsMotorStop(varMotor_t*);
int ReadDegree_RO(int*);


BYTE RO_p_n_over_flag(void);
BYTE PA_p_n_over_flag(void);

BYTE RO_CheckStall(void);
BYTE PA_CheckStall(void);


/*----- V A R I A B L E ____________________________________________________*/
motor_t		mPA;
motor_t		mRO;

BYTE isStallInitialized_RO;
BYTE isStallInitialized_PA;

// BYTE	A1_temp = 0;
BYTE    A1_table_step_flag = NO;
LONG 	A1_pulse_count = 0;
WORD 	A1_array_count = 0;
BYTE 	resetOrigin_PA = 0;
BYTE 	resetOrigin_RO = 0;

int 	adcDegree_RO_org;	// RO org 에서 adc degree값


// todo : imsi variable



BYTE RO_CheckStall(void)
{
	static 	LONG	posCnt, prePosCnt ;
	static 	WORD	adcValue, preAdcValue;
	static 	ULONG	lastTime;
	signed short deltaAdcValue;
	LONG 	deltaPosCnt, stall_value;


	//------- stall 검출 알고리즘
	// stall 감지되면 return '1'
	// deltaAdcValue / deltaPositionCnt  비율값 값으로 stall_value 결정
	// positionCnt와 adcValue은 비례관계가 있어 안정적인 stall_value를 얻을 수 있음
	// stall_value가 1보다 작으면 stall로 인식
	// 처음 첫 stall_value는 경계값으로 버려야 함
	// 0.25초 마다 stall 감지(실험적으로 결정)
	// 
	//----------------------------------------

	// Motor가 멈춰 있으면 함수 리턴
	if(mRO.stop_flag == YES) return 0;

	if( isStallInitialized_RO == NO)	// 처음 시작때 초기화를 위한 루틴
	{
		UartPrintf("==> RO_CheckStall - 1 \r\n");
		posCnt = RO_GetPositionCnt();
		ReadCanAdc_RO(&adcValue);

		lastTime = TimeNow();
		isStallInitialized_RO = YES;

		return 0;
	}

	// 초기화 후 2번째 함수 호출부터 실행됨
	// 0.25초 마다 stall 감지(실험적으로 결정)
	if( TimePassed(lastTime) > ONE_SECOND/4)
	{
		lastTime = TimeNow();

		prePosCnt = posCnt;
		preAdcValue = adcValue;

		posCnt = RO_GetPositionCnt();
		ReadCanAdc_RO(&adcValue);

		deltaPosCnt = posCnt - prePosCnt;
		deltaAdcValue = adcValue - preAdcValue;
		stall_value = (LONG)deltaAdcValue*100 / (LONG)deltaPosCnt;

		// UartPrintf("==> RO pos : %ld, adc : %u, deltaPosCnt : %ld, deltaAdcValue : %d , stall : %ld  \r\n", 
		// 	posCnt, adcValue, deltaPosCnt, deltaAdcValue, stall_value);

		// stall 감지되면 return '1', 아니면 '0'
		if(stall_value <= 1) 	return 1;
	}

	return 0;
}

BYTE PA_CheckStall(void)
{
	static 	LONG	posCnt, prePosCnt ;
	static 	WORD	adcValue, preAdcValue;
	static 	ULONG	lastTime;
	signed short deltaAdcValue;
	LONG 	deltaPosCnt, stall_value;


	//------- stall 검출 알고리즘
	// stall 감지되면 return '1'
	// deltaAdcValue / deltaPositionCnt  비율값 값으로 stall_value 결정
	// positionCnt와 adcValue은 비례관계가 있어 안정적인 stall_value를 얻을 수 있음
	// stall_value가 1보다 작으면 stall로 인식
	// 처음 첫 stall_value는 경계값으로 버려야 함
	// 0.25초 마다 stall 감지(실험적으로 결정)
	//----------------------------------------

	// Motor가 멈춰 있으면 함수 리턴
	if(mPA.stop_flag == YES) return 0;

	if( isStallInitialized_PA == NO)	// 처음 시작때 초기화를 위한 루틴
	{
		UartPrintf("==> PA_CheckStall - 1 \r\n");
		posCnt = PA_GetPositionCnt();
		ReadCanAdc_PA(&adcValue);

		lastTime = TimeNow();
		isStallInitialized_PA = YES;

		return 0;
	}

	// 초기화 후 2번째 함수 호출부터 실행됨
	// 0.25초 마다 stall 감지(실험적으로 결정)
	if( TimePassed(lastTime) > ONE_SECOND/4)
	{
		lastTime = TimeNow();

		prePosCnt = posCnt;
		preAdcValue = adcValue;

		posCnt = PA_GetPositionCnt();
		ReadCanAdc_PA(&adcValue);

		deltaPosCnt = posCnt - prePosCnt;
		deltaAdcValue = adcValue - preAdcValue;
		stall_value = (LONG)deltaAdcValue*100 / (LONG)deltaPosCnt;

		// UartPrintf("==> PA pos : %ld, adc : %u, deltaPosCnt : %ld, deltaAdcValue : %d , stall : %ld  \r\n", 
		// 	posCnt, adcValue, deltaPosCnt, deltaAdcValue, stall_value);

		if(stall_value <= 1) return 1;
	}

	return 0;
}

//==================================================
// todo : 어느 파일디에 위치해야 하나?? system.c, motor_drive.c
BYTE IsMotorStop(varMotor_t *mt)
{
	return (mt->org_complete_flag && mt->stop_flag);
}

void PA_SetInitialValue(void)
{
	mPA.pos_dir 		= PA_POS_DIR;
	mPA.neg_dir 		= PA_NEG_DIR; 
	mPA.unit_0_1_pulse	= PA_0_1_MM_PULSE_2;
	mPA.min_speedHz		= PA_START_PULSE_2;
	mPA.max_speedHz		= PA_MAX_PULSE_2;
	mPA.accel_step		= PA_ACCEL_PULSE_2;
	mPA.p_maxPulseCnt	= PA_P_MAX_PULSE_2;
	mPA.n_maxPulseCnt	= PA_N_MAX_PULSE_2;
	
	mPA.org_complete_flag = NO;

}
void RO_SetInitialValue(void)
{
	mRO.pos_dir			= RO_POS_DIR;
	mRO.neg_dir			= RO_NEG_DIR;
	mRO.unit_0_1_pulse	= RO_0_1_DEGREE_PULSE;
	mRO.min_speedHz		= RO_START_PULSE;
	mRO.max_speedHz		= RO_MAX_PULSE;
	mRO.accel_step		= RO_ACCEL_PULSE;
	mRO.p_maxPulseCnt	= RO_P_MAX_PULSE;
	mRO.n_maxPulseCnt	= RO_N_MAX_PULSE;
}


//==================================================
void PA_UpdateState(void)
{
	gvPA.stop_flag 			= mPA.stop_flag;
	gvPA.org_complete_flag 	= mPA.org_complete_flag;
}
void RO_UpdateState(void)
{
	gvRO.stop_flag 			= mRO.stop_flag;
	gvRO.org_complete_flag 	= mRO.org_complete_flag;
}

//==================================================
void PA_MotorChangeSpeed(BYTE dir, LONG speed)
{
	// PCPuts("[PA]");
	mPA.setting_cycle = 1;
	PA_MotorStart(dir, speed);
}
void RO_MotorChangeSpeed(BYTE dir, LONG speed)
{
	// PCPuts("[RO]");
	mRO.setting_cycle = 1;
	RO_MotorStart(dir, speed);
}

void PA_increase_stage(BYTE value)
{
	mPA.accel_stage = value;
}

void RO_increase_stage(BYTE value)
{
	mRO.accel_stage = value;
}
//-------------------------
void PA_stop_cnt(LONG value)
{
	mPA.moved_pulseCnt = value;
}

void RO_stop_cnt(LONG value)
{
	mRO.moved_pulseCnt = value;
}
//-------------------------
void PA_tot_pulse(LONG value)
{
	mPA.total_pulseCnt = value;
}

void RO_tot_pulse(LONG value)
{
	mRO.total_pulseCnt = value;
}

//-------- p_over_flag, n_over_flag
BYTE PA_p_n_over_flag(void)
{
	// over_flag가 발생하고 바로 PWM에서 클리어 되는데 
	// cmd_ack에서 [rpm_pamv_####] 표시를 위한 제한 기능에 응답이 늦을듯

	if(mPA.p_over_flag || mPA.n_over_flag)	return 1;

	return 0;
}
BYTE RO_p_n_over_flag(void)
{
	if(mRO.p_over_flag || mRO.n_over_flag)	return 1;

	return 0;
}

/*F**************************************************************************
* NAME:     PA_MotorStart
* PURPOSE:  다양한 speed에서도 속도를 유지할 수 있게 하기위해 prescaler등을 셋팅하고 
			timer interrupt를 시작하여 start motor
*****************************************************************************/
void PA_MotorStart(BYTE dir, LONG speedHz)
{
	mPA.stop_flag = NO;

	MOTOR_DIR_A0 = dir;			// 방향 설정 i/o port // naming : pinMotorDirA0, ...
	mPA.dir = dir;			// 인터럽트 루틴내 방향설정 변수
	mPA.speedHz = speedHz;		// A#_slow_stop 함수에서 현재 속도를 확인하기 위해 필요
	
	// 16-bit timer를 사용하므로 speed가 작을 경우에는 CYCLE_TIMER#이 WORD를 넘게 된다.
	// 따라서 speed에 따라 prescaler로 적절히 나눠주어야 작은 speed에 대해서도 펄스를 만들 수 있다.

	//---------------------------------------------------------속도 설정	
	//	만약 speedHz가 1,000이면, A0_cycle	= 20,000,000 / 1,000 -1
	//										= 19,999
	//	INT_TGI0A_TPU0를 보면 CYCLE_TIMER0 = 19999가 되니까
	//	0부터 19999까지 20000번 카운트를 증가해야 펄스 하나 발생하며
	//	Timer 설정에서 prescaler를 1로 설정했으므로,
	//	20,000,000 / 1 / 20,000 = 1,000 Hz
	//	따라서 speed 값이 곧 모터 드라이버에 입력되는 펄스의 주파수와 동일
	if(speedHz > 610)
	{
		TPU0.TCR.BYTE = 0x28;	// TCNT Clear by TGRA compare match. 
								// Counted at rising edge, Internal clock: counts on Pφ/1
		
		mPA.cycle = P_CLK/speedHz - 1;		mPA.duty = mPA.cycle>>1;
	}
	//---------------------------------------------------------속도 설정	
	//	만약 speedHz가 250이면, A0_cycle	= 20,000,000 / (250*4) -1
	//									= 19,999
	//	INT_TGI0A_TPU0를 보면 CYCLE_TIMER0 = 19999가 되니까
	//	0부터 19999까지 20000번 카운트를 증가해야 펄스 하나 발생하며
	//	Timer 설정에서 prescaler를 4로 설정했으므로,
	//	20,000,000 / 4 / 20,000 = 250 Hz
	//	따라서 speed 값이 곧 모터 드라이버에 입력되는 펄스의 주파수와 동일
	else if(speedHz > 152)
	{
		TPU0.TCR.BYTE = 0x29;	// Pφ/4
		
		mPA.cycle = (P_CLK>>2)/speedHz - 1;	mPA.duty = mPA.cycle>>1;
	}
	else if(speedHz > 38)
	{
		TPU0.TCR.BYTE = 0x2A;	// Pφ/16
		
		mPA.cycle = (P_CLK>>4)/speedHz - 1;	mPA.duty = mPA.cycle>>1;
	}
	else if(speedHz > 4)
	{
		TPU0.TCR.BYTE = 0x2B;	// Pφ/64
		
		mPA.cycle = (P_CLK>>6)/speedHz - 1;	mPA.duty = mPA.cycle>>1;
	}
	else	// 5 미만은 CYCLE_TIMER#이 overflow되므로 5로 고정
	{
		speedHz = 5;
		
		TPU0.TCR.BYTE = 0x2B;	// Pφ/64
		
		mPA.cycle = (P_CLK>>6)/speedHz - 1;	mPA.duty = mPA.cycle>>1;
	}
	
	START_TIMER0		= YES;
	EN_INT_TIMER0		= YES;
}
/*F**************************************************************************
* NAME:     PA_MotorDrive
* PURPOSE:
*****************************************************************************/
// !! total_pulse에는 속도에 이른 후에 부드럽게 멈추기위한 pulse 수가 필요하다. 안그러면 덜컹!! 하며 멈춤
// 이는  는 증가하는 puls와 감속하는 (멈추는 ) pulse 
// A0_moved_pulseCnt 에 대입되는 값은 항상 '-1'이다.   -> 변수 제거 가능할듯!!
void PA_MotorDrive(BYTE dir, LONG start_speedHz, LONG dest_speedHz, LONG tot_pulseCnt)
{	
	if(tot_pulseCnt > 1)
	{	
		EN_INT_TIMER0 = NO;
		
		// 전체이동할 거리가 최대속도까지 가속하기엔 부족할 경우에는 최대 속도를 제한시킴
		if(tot_pulseCnt < (dest_speedHz<<1) )	
		{
			dest_speedHz = tot_pulseCnt>>1;		// dest_speedHz가 tot_pulsCnt/2 ???
			if(dest_speedHz < start_speedHz)	dest_speedHz = start_speedHz;
		}
		
		if(start_speedHz < dest_speedHz) 	mPA.accel_stage = 1;
		else								mPA.accel_stage = 0;
		
		mPA.dir				= dir;
		mPA.speedHz 		= start_speedHz;
		mPA.dest_speedHz	= dest_speedHz;
		mPA.total_pulseCnt	= tot_pulseCnt;
		mPA.moved_pulseCnt	= -1;
		
		mPA.setting_cycle = 1;	PA_MotorStart(mPA.dir, mPA.speedHz);  
	}
	else PA_MotorStop();
}
/*F**************************************************************************
* NAME:     PA_MotorSlowStop
* PURPOSE:	
*****************************************************************************/
void PA_MotorSlowStop(void)
{	
	mPA.decrease_pulseCnt = 1;
	
	if(mPA.speedHz <= mPA.min_speedHz)	// 현재 속도가 축의 최소 속도 설정값보다 작으면
	{
		PA_MotorStop();
	}
	else
	{
		if(mPA.speedHz >= mPA.n_maxPulseCnt)	// 포토센서를 벗어나서 움직일 수 있는 최대 거리를 벗어나지 않기 위해
			PA_MotorDrive(mPA.dir, mPA.speedHz, mPA.min_speedHz, mPA.n_maxPulseCnt);
		else
			PA_MotorDrive(mPA.dir, mPA.speedHz, mPA.min_speedHz, mPA.speedHz);
	}
}
/*F**************************************************************************
* NAME:     PA_MotorStop
* PURPOSE:
*****************************************************************************/
void PA_MotorStop(void)
{
	mPA.stop_flag 	= YES;
	isStallInitialized_PA = NO;		// stall 감지 시작 falg

	CYCLE_TIMER0 		= 0;
	DUTY_TIMER0			= 0;
	START_TIMER0		= NO;
	EN_INT_TIMER0 		= NO;
	TPU0.TIOR.WORD 		= 0x5220;
}
/////////////////// 모터 구동 펄스 interrupt //////////////////////////////
/*F**************************************************************************
* NAME:     INT_TGI0A_TPU0
* PURPOSE:
*****************************************************************************/
__interrupt(vect=88) void INT_TGI0A_TPU0(void)
{
	TPU0.TSR.BIT.TGFA = 0;

	// (1) 멈춰진 상태에서 start_speedHz로 가속을 하고, (2) dest_speedHz로 가속을 하고, 
	// (3) 등속을 유지하고, (4) 감속지점부터 감속을 하고 멈춘다.

	// 가속, 등속, 감속등 속도를 제어
	// 이동된 펄스수가 speedHz와 같고, 가속상태이면, dest_speedHz까지 가속하고 등속상태가 된다. 
	if( (mPA.moved_pulseCnt == mPA.speedHz) && (mPA.accel_stage == 1) ) 
	{
		mPA.speedHz += mPA.accel_step;			//점점 가속하기위해 accel_step만큼 누적시키며, 누적된 만큼 가속
		
		if(mPA.speedHz >= mPA.dest_speedHz)
		{
			mPA.accel_stage = 0;				// constant speed status
			mPA.speedHz = mPA.dest_speedHz;	
			mPA.decrease_pulseCnt = mPA.total_pulseCnt - mPA.moved_pulseCnt;
		}
		
		mPA.setting_cycle = 1;
		PA_MotorStart(mPA.dir, mPA.speedHz);	 
	}

	// 감속할 거리에 오고, 등속상태이면 감속
	else if( (mPA.moved_pulseCnt == mPA.decrease_pulseCnt) && (mPA.accel_stage == 0) )
	{
		mPA.speedHz -= mPA.accel_step;
		
		// mPA.min_speedHz는 축별 최소 속도.  naming ; A0_minimun_speedHz
		if(mPA.speedHz < mPA.min_speedHz )	mPA.speedHz = mPA.min_speedHz;
		
		mPA.setting_cycle = 1;
		PA_MotorStart(mPA.dir,mPA.speedHz);
		
		// accel_step만큼 감속할 거리 조정
		mPA.decrease_pulseCnt = mPA.decrease_pulseCnt + mPA.accel_step;
	}
	
	if(mPA.setting_cycle == 1)	// mPA.setting_cycle이 1로 바뀔 때마다 cycle과 duty를 바꿔서 동작
	{
		mPA.setting_cycle = 0;
		
		TIMER_COUNTER0	= 0;
		CYCLE_TIMER0	= (WORD)mPA.cycle;			// 주파수 바꾸는 설정. 스텝뫃터는 주파수를 바꾼다. 
	    DUTY_TIMER0		= (WORD)mPA.duty;
	}


	// 흐름상 위에 위치하면 좋다. 
	// TPU0 인터럽트가 시작해서, 이동할 거리만큼 이동하면서, 이동된 위치를 정한다.. 
	// moved_pulseCnt(이동된 거리), total_pulseCnt(총이동할 거리), pos_cnt(이동된 위치)
	if(mPA.moved_pulseCnt < mPA.total_pulseCnt)	
	{
		++mPA.moved_pulseCnt;
		
		if(mPA.moved_pulseCnt > 0)	// 인터럽트 시작해서 두 번째 루프부터 첫 번째 펄스가 출력되므로
		{
			if(mPA.dir == mPA.pos_dir)
			{
				++mPA.pos_cnt ;

				// todo : 덴트리 구조인데 좀더 분석 필요!! 밖으로 뺄 수 있을 듯함.
				//-----  기구적 p_max 값을 벗어날 때 처리!!
				if(mPA.org_complete_flag)		// 원점 잡는 게 끝난 경우, (원점 잡는게 안끝나면 over될 수 있다??중이 아닌데도
				{
					if(mPA.pos_cnt > mPA.p_maxPulseCnt)	// 기구적인 최대 이동범위 벗어날 경우
					{
						PA_MotorStop();					// 모터 정지하고
						--mPA.pos_cnt;				// 이동하지 않았으므로 카운트 원복시키고
						mPA.p_over_flag	= YES;
						gvPA.p_over_ack_flag = YES;	// 경고문 출력
					}
				}

			}
			else if(mPA.dir == mPA.neg_dir)
			{
				--mPA.pos_cnt;

				//----- 기구적 n_max 값을 벗어날 때 처리!!
				if(mPA.org_complete_flag)		// 원점 잡는 중이 아닌데도
				{
					if(mPA.pos_cnt < -mPA.n_maxPulseCnt)	// 기구적인 최대 이동범위 벗어날 경우
					{
						PA_MotorStop();					// 모터 정지하고
						++mPA.pos_cnt;				// 이동하지 않았으므로 카운트 원복시키고
						mPA.n_over_flag	= YES;
						gvPA.n_over_ack_flag = YES;	// 경고문 출력
					}
				}

			}

			//-------
			if(mPA.p_over_flag)						// 기구적인 이동 최대범위를 벗어낫다가
			{
				if(mPA.pos_cnt <= mPA.p_maxPulseCnt)	mPA.p_over_flag	= NO;	// 이동 가능범위로 돌아왔을 경우에는 Flag Clear
			}	
			else if(mPA.n_over_flag)					// 기구적인 이동 최대범위를 벗어낫다가
			{
				if(mPA.pos_cnt >= -mPA.n_maxPulseCnt)	mPA.n_over_flag	= NO;	// 이동 가능범위로 돌아왔을 경우에는 Flag Clear
			}

		}
	}
	else	PA_MotorStop(); // 필요한 펄스만큼 이동한 뒤에 펄스 출력을 완전히 정지하지 않으면 마지막 펄스 주파수가 계속 출력됨
}


/*F**************************************************************************
* NAME:     RO_MotorStart
* PURPOSE:  다양한 speed에서도 속도를 유지할 수 있게 하기위해 prescaler등을 셋팅하고 
			timer interrupt를 시작하여 start motor
*****************************************************************************/
void RO_MotorStart(BYTE dir, LONG speedHz)
{
	mRO.stop_flag = NO;
	
	MOTOR_DIR_A1 = dir;
	mRO.dir = dir;
	mRO.speedHz = speedHz;
	
	if(speedHz > 610)
	{
		TPU1.TCR.BYTE = 0x28;	mRO.cycle = P_CLK/speedHz - 1;		mRO.duty = mRO.cycle>>1;
	}
	else if(speedHz > 152)
	{
		TPU1.TCR.BYTE = 0x29;	// Pφ/4
		mRO.cycle = (P_CLK>>2)/speedHz - 1;		mRO.duty = mRO.cycle>>1;
	}
	else if(speedHz > 38)
	{
		TPU1.TCR.BYTE = 0x2A;	// Pφ/16
		mRO.cycle = (P_CLK>>4)/speedHz - 1;		mRO.duty = mRO.cycle>>1;
	}
	else if(speedHz > 4)
	{
		TPU1.TCR.BYTE = 0x2B;	// Pφ/64
		mRO.cycle = (P_CLK>>6)/speedHz - 1;		mRO.duty = mRO.cycle>>1;
	}
	else	// 5 미만은 CYCLE_TIMER#이 overflow되므로 5로 고정
	{
		speedHz = 5;
		TPU1.TCR.BYTE = 0x2B;	// Pφ/64
		mRO.cycle = (P_CLK>>6)/speedHz - 1;		mRO.duty = mRO.cycle>>1;
	}
	
	START_TIMER1		= YES;
	EN_INT_TIMER1		= YES;
}
/*F**************************************************************************
* NAME:     RO_MotorDrive
* PURPOSE:
*****************************************************************************/
void RO_MotorDrive(BYTE dir, LONG start_speedHz, LONG dest_speedHz, LONG tot_pulseCnt)
{	
	if(tot_pulseCnt > 1)
	{	
		EN_INT_TIMER1 = NO;
		
		if(tot_pulseCnt < (dest_speedHz<<1) )	
		{
			dest_speedHz = tot_pulseCnt>>1;
			if(dest_speedHz < start_speedHz)	dest_speedHz = start_speedHz;
		}
		
		if(start_speedHz < dest_speedHz) 	mRO.accel_stage = 1;
		else								mRO.accel_stage = 0;
		
		mRO.dir				= dir;
		mRO.speedHz 			= start_speedHz;
		mRO.dest_speedHz		= dest_speedHz;
		mRO.total_pulseCnt	= tot_pulseCnt;
		mRO.moved_pulseCnt	= -1;
		
		mRO.setting_cycle = 1;	RO_MotorStart(mRO.dir, mRO.speedHz);  
	}
	else RO_MotorStop();
}
/*F**************************************************************************
* NAME:     PA_MotorSlowStop
* PURPOSE:	
*****************************************************************************/
void RO_MotorSlowStop(void)
{	
	LONG	safetyMagin_2degree;

	mRO.decrease_pulseCnt = 1;
	
	if(mRO.speedHz <= mRO.min_speedHz)	RO_MotorStop();
	else
	{
		if(mRO.speedHz >= mRO.n_maxPulseCnt)
		{
			safetyMagin_2degree = 20*RO_0_1_DEGREE_PULSE;	// 펄스 제한에 걸리지 않기 위한 여유 2 도 고려
			RO_MotorDrive(mRO.dir, mRO.speedHz, mRO.min_speedHz, 
							mRO.n_maxPulseCnt - safetyMagin_2degree);
		}	
		else
			RO_MotorDrive(mRO.dir, mRO.speedHz, mRO.min_speedHz, mRO.speedHz);
	}
}
/*F**************************************************************************
* NAME:     RO_MotorStop
* PURPOSE:
*****************************************************************************/
void RO_MotorStop(void)
{
	mRO.stop_flag 	= YES;
	isStallInitialized_RO = NO;		// stall 감지 시작 falg

	CYCLE_TIMER1 		= 0;
	DUTY_TIMER1			= 0;
	START_TIMER1		= NO;
	EN_INT_TIMER1 		= NO;
	TPU1.TIOR.BYTE 		= 0x52;
}
/*F**************************************************************************
* NAME:     INT_TGI1A_TPU1
* PURPOSE:
*****************************************************************************/
__interrupt(vect=93) void INT_TGI1A_TPU1(void)
{
	TPU1.TSR.BIT.TGFA = 0;

	// todo naming, structure : A1_table_step_flag, A1_pulse_arr, A1_array_count
	// 가속을 위해 table이 사용됨. 등속상태가 되면 벗어남.
	if(A1_table_step_flag)  // CT 촬영을 위한 회전인 경우(CT Ultra Fast scan??)
    {
		if(mRO.accel_stage == 1)
		{
			++ A1_pulse_count;		// arr[i]와 arr[i+1]사이의 펄스수 => 해당 pulseCnt 만큼 속도 증가

			// arr[i]가 지정한 pulseCnt까지 도달하면..
			if(A1_pulse_count >= A1_pulse_arr[A1_array_count])
			{
				++ A1_array_count;
				mRO.speedHz = (LONG)A1_speed_arr[A1_array_count];
				mRO.setting_cycle = 1;	RO_MotorStart(mRO.dir,mRO.speedHz);
				A1_pulse_count = 0;
			}

			// dset speed까지 도달하면 등속
			if(mRO.speedHz >= mRO.dest_speedHz)
			{
				mRO.accel_stage = 0;
				mRO.speedHz = mRO.dest_speedHz;	//등속
				mRO.decrease_pulseCnt = mRO.total_pulseCnt - mRO.moved_pulseCnt;
				A1_array_count = A1_array_count + 1;
			}
		}
		else if(mRO.moved_pulseCnt >= mRO.decrease_pulseCnt && mRO.accel_stage == 0)
		{
			++ A1_pulse_count;

			if(A1_pulse_count >= A1_pulse_arr[A1_array_count])
			{
				-- A1_array_count;
				mRO.speedHz = (LONG)A1_speed_arr[A1_array_count];
				mRO.setting_cycle = 1;	RO_MotorStart(mRO.dir,mRO.speedHz);
				A1_pulse_count = 0;
			}

			if(mRO.speedHz < mRO.min_speedHz )	mRO.speedHz = mRO.min_speedHz;	// 축 별 최소 속도
		}
    }
    else // CT Ultra Fast Scan 촬영을 위한 회전 중이 아닐 때
    {
		// (1) 멈춰진 상태에서 start_speedHz로 가속을 하고, (2) dest_speedHz로 가속을 하고, 
		// (3) 등속을 유지하고, (4) 감속지점부터 감속을 하고 멈춘다.
		if( (mRO.moved_pulseCnt == mRO.speedHz) && (mRO.accel_stage == 1) ) 
		{
			mRO.speedHz += mRO.accel_step;			//점점 가속하기위해 accel_step만큼 누적시키며, 누적된 만큼 가속
			
			if(mRO.speedHz >= mRO.dest_speedHz)
			{
				mRO.accel_stage = 0;				// constant speed status
				mRO.speedHz = mRO.dest_speedHz;	
				mRO.decrease_pulseCnt = mRO.total_pulseCnt - mRO.moved_pulseCnt;
			}
			
			mRO.setting_cycle = 1;
			RO_MotorStart(mRO.dir, mRO.speedHz);	 
		}
		else if( (mRO.moved_pulseCnt == mRO.decrease_pulseCnt) && (mRO.accel_stage == 0) )
		{
			mRO.speedHz -= mRO.accel_step;
			
			if(mRO.speedHz < mRO.min_speedHz )	mRO.speedHz = mRO.min_speedHz;
			
			mRO.setting_cycle = 1;
			RO_MotorStart(mRO.dir,mRO.speedHz);
			
			mRO.decrease_pulseCnt = mRO.decrease_pulseCnt + mRO.accel_step;
		}
    }



	if(mRO.setting_cycle == 1)
	{
		mRO.setting_cycle = 0;
		
		TIMER_COUNTER1	= 0;
		CYCLE_TIMER1	= (WORD)mRO.cycle;
	    DUTY_TIMER1		= (WORD)mRO.duty;
	}

	if(mRO.moved_pulseCnt < mRO.total_pulseCnt)	
	{
		++mRO.moved_pulseCnt;
		
		if(mRO.moved_pulseCnt > 0)	// 인터럽트 시작해서 두 번째 루프부터 첫 번째 펄스가 출력되므로
		{
			if(mRO.dir == mRO.pos_dir)
			{
				++mRO.pos_cnt ;
	
				//-----  기구적 p_max 값을 벗어날 때 처리!!
				if(mRO.org_complete_flag)
				{	
					if(mRO.pos_cnt > mRO.p_maxPulseCnt)	
					{	
						RO_MotorStop();	
						--mRO.pos_cnt;	
						mRO.p_over_flag = YES;	
						gvRO.p_over_ack_flag = YES;
					}	
				}
			}
			else if(mRO.dir == mRO.neg_dir)
			{
				--mRO.pos_cnt;


				if(mRO.org_complete_flag)
				{	
					if(mRO.pos_cnt < -mRO.n_maxPulseCnt)
					{	
						RO_MotorStop();	
						++mRO.pos_cnt;	
						mRO.n_over_flag = YES;	
						gvRO.n_over_ack_flag = YES;
					}	
				}

			}

			if(mRO.p_over_flag)		
			{	
				if(mRO.pos_cnt <= mRO.p_maxPulseCnt)	mRO.p_over_flag = NO;	
			}
			else if(mRO.n_over_flag)	
			{	
				if(mRO.pos_cnt >= -mRO.n_maxPulseCnt)	mRO.n_over_flag = NO;
			}
		}
	}
	else	RO_MotorStop();
}


void init_A1_table_value (void)
{
	if(debug_mode)
		PCPrintf("<A1pc = %ld, A1ac = %d> \r\n", A1_pulse_count, A1_array_count);

	A1_pulse_count = 0;
	A1_array_count = 0;
}

/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
LONG RO_GetPositionCnt(void)
{
	return mRO.pos_cnt;
}
LONG RO_GetSpeed(void)
{
	return mRO.speedHz;
}
/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
LONG PA_GetPositionCnt()
{
	return mPA.pos_cnt;
}
LONG PA_GetSpeed(void)
{
	return mPA.speedHz;
}
				
/*F**************************************************************************
* NAME:
// origin senor, potentiometer ADC CAN driver      
// Rotoator 최대 max =  포텐션미터 7.75 turn
// 포텐션미터는 10턴
// 센서 감지 : (-80도 ~ 0도) or  (270도 ~ 360도)
*****************************************************************************/
// todo : 함수 위치 변경? motor_drive.c 쪽이 아닌거 같다. 
int ReadDegree_RO(int *adcDegree)
{
	WORD adcValue;
	static int errTime;
	static ULONG lastTime;


	if( ReadCanAdc_RO(&adcValue) == YES )
	{
		*adcDegree =  (int)( (adcValue - (LONG)ADC_DEGREE_0) / (LONG)ONE_DEGREE );
		errTime = 0;
    	return 1;	// adc 정상변환
	}
	else 	
	{	
		// 지정된 시간을 넘으면 SENSOR 에러 메세지 출력
		if( TimePassed(lastTime) > ONE_SECOND )
		{
			++errTime;
			if(errTime <= 5)	// Sensor 인식 지정된 시간
			{
				lastTime = TimeNow();
			}
			else
			{
				// RO CAN ADC에서 값을 읽어와야 하는데 읽지 못한 경우
				PCPuts("[SEN_ERR_16] RO ADC\r\n");
				errTime = 0;
				lastTime = 0;
			}
		}

	return 0;	// adc 변환 안됨 - error반환
	}
}

void RO_SetOrigin(BYTE isInitialized)
{
	static int currState = ST_INIT;
	static int adcDegree, readyDegree;
	static LONG moveDegree, moveCnt;
	static ULONG lastTime;
	static int errTime;


	switch(currState)
	{
		case ST_INIT:
			// adc값을 읽고
            if(ReadDegree_RO(&adcDegree) != YES)	break;

			// UartPrintf("==> RO_ORG ST_INIT : adcDegree %d \r\n", adcDegree);
			// PCPrintf("RO adcDegree : %d \r\n", adcDegree);
			if(isInitialized == NO)
			{
				// 전원켜고 초기 first Run인 경우 
				// ready Degree(org 찾아기기위한 준비 각도)는 초기 ADC 오차등을 고려해 -20도를 설정한다. 
				readyDegree = adcDegree_RO_org - 20;
			}
			else if(isInitialized == YES)
			{
				// adcDegree_RO_org가 셋팅 된 후에는 초기(양산) 오차가 없으믈 
				// ready Degree는  원점잡는 시간을 단축하기 위해 -10도를 설정한다. 
				readyDegree = adcDegree_RO_org - 10;	
			}
			currState = ST_CHK_POSITION;
		break;

		case ST_CHK_POSITION:	// 현재 위치를 체크 하고 READY_DEGREE으로 이동한다. 
			if(adcDegree < readyDegree)		
			{
			// UartPrintf("==> RO_ORG ST_CHK_POSITION -1 \r\n");
				moveDegree = (LONG) (readyDegree - adcDegree);	// 
				moveCnt = RO_1_DEGREE_PULSE * moveDegree;
                RO_MotorDrive(RO_POS_DIR,mRO.min_speedHz,mRO.max_speedHz,moveCnt);
				// LCDPuts("[Org1_2-1] \r\n");   PCPuts("[Org1_2-1] \r\n");
				currState = ST_READY_ZONE;
			}
			else if(adcDegree >= readyDegree)	
			{
			// UartPrintf("==> RO_ORG ST_CHK_POSITION -2 \r\n");
				moveDegree = (LONG) (adcDegree - readyDegree);
                moveCnt = RO_1_DEGREE_PULSE * moveDegree;		// 이동할 위치 계산 : 현재 위치 - (대략 -20도 위치))
                RO_MotorDrive(RO_NEG_DIR,mRO.min_speedHz,mRO.max_speedHz,moveCnt);
				// LCDPuts("[Org1_2-2] \r\n");   PCPuts("[Org1_2-2] \r\n");
                currState = ST_READY_ZONE;
			}
		break;

		case ST_READY_ZONE:		// RO가 멈추면 반대방향 전환을 위한 1초를 쉬고 
			if(mRO.stop_flag)
			{
			// UartPrintf("==> RO_ORG ST_READY_ZONE \r\n");
				lastTime = TimeNow();
				currState = ST_GO_ORG;
			}
		break;

		case ST_GO_ORG:			// 센서쪽으로 천천히 이동 , ADC 오차를 고려해 100도 정도 움직이게 함
			if(TimePassed(lastTime) > ONE_SECOND)
			{
			// UartPrintf("==> RO_ORG ST_GO_ORG \r\n");
				moveDegree = (LONG) 100;
                moveCnt = RO_1_DEGREE_PULSE * moveDegree;
                RO_MotorDrive(RO_POS_DIR,mRO.min_speedHz,mRO.min_speedHz,moveCnt);
                // LCDPuts("[Org1_6-2] \r\n"); PCPuts("[Org1_6-2] \r\n");
				currState = ST_FIND_ORG;
			}
		break;

		case ST_FIND_ORG:
			if(!mRO.onOrgSensor)	//센서가 감지되지 않으면 ==> 센서에 닿으면
			{
			// UartPrintf("==> RO_ORG ST_FIND_ORG \r\n");
				RO_MotorStop();
				lastTime = TimeNow();
				currState = ST_FINISH_ORG;
			}
		break;

		case ST_FINISH_ORG:
			if(mRO.stop_flag)
			{
				// 정지후에도 RO축이 미세히 흔들리기때문에 
				// 정확한 RO ADC를 얻기위해 물리적으로 정지할때를 기다림.(실험적으로 1초정도 소요)
				if(TimePassed(lastTime) < ONE_SECOND)	break;	

				if(ReadDegree_RO(&adcDegree_RO_org) != YES)	break;

			// UartPrintf("==> RO_ORG ST_FINISH_ORG \r\n");
				mRO.pos_cnt = 0;	// 원점 다 잡은 위치를 가상의 0도로 설정. 앞으로 이 값으로 축의 위치 판단

				mRO.org_complete_flag = YES;	// 원점 완료 flag 셋
				resetOrigin_RO = OFF;				// reset 완료되어 clear
				PCPuts("[rpm_romv_00000]	[ro_end] \r\n");
				currState = ST_INIT;

				PCPrintf("Msg: RO ADC value at RO origin : %d degree \r\n", adcDegree_RO_org);
			}
		break;
	}

}


/*F**************************************************************************
* NAME:     PA_SetOrigin
* PURPOSE:
*****************************************************************************/
// PA 축 원점 위치 잡기 함수
// naming : PA_OriginPosition(), PA_MoveOriginPosition()...
// first_run은 system.c 초기화때 셋되고, main루프 while(first_run )이 끝날 때 끝나고
// command_control.c 에서 PC에 의해 set과 리셋된다.
// todo naming : first run에서 사용. 원점을 셋팅하는데 함수.  PA_FixOrigin()
// 원점으로 이동하는데 함수 필요 --> PA_MoveOrigin(), PA_FixOrigine(), PA_FistFixOrigin()
void PA_SetOrigin(BYTE isInitialized)
{
	static BYTE orgStep = STEP1;
	static ULONG lastTime;
	LONG tolerance;

	switch(orgStep)		// ?? orgSTEP1이 언제 설정되는지 파악이 안됨.
	{
		
		case STEP1:		// ?? 
			mPA.org_complete_flag = NO;

			// org sensor가 감지되지 않으면 원점방향(negative dir)으로 최대속도로 움직이고
			// org sensor가 감지되면 원점방향(positive dir)으로 최소속도로 움직인다. 
			if(!mPA.onOrgSensor)
			{
				tolerance = 30*mPA.unit_0_1_pulse;		// 기구공차 3 mm 고려
				PA_MotorDrive(mPA.neg_dir, mPA.min_speedHz, mPA.max_speedHz, mPA.p_maxPulseCnt + tolerance);	
				orgStep = STEP3;
				// PCPuts("[Org0_1-1] \r\n");
			}
			else
			{
				// PA위치가 RO 회전시 컬럼충돌 위치일 경우  예방
				// 장비초기화(전원인가)때만 PA축이 원점선서를 인지하면 RO resetOrine 시작하게끔
				if(isInitialized == NO)	resetOrigin_RO = ON;	

				tolerance = 30 * mPA.unit_0_1_pulse;
				PA_MotorDrive(mPA.pos_dir, mPA.min_speedHz, mPA.min_speedHz, mPA.n_maxPulseCnt + tolerance);
				orgStep = STEP2;
				// PCPuts("[Org0_1-2] \r\n");		// PA축 step 1-1 이라는 뜻
			}

		break;
		
		// org sensor를 지나서 다시 positive dir로 미세히 움직이다가(step1), 
		// 센서를 벗어나면 바로 멈추고 마무리 step
		case STEP2:
			if(!mPA.onOrgSensor)
			{
				// PCPuts("[Org0_2-1] \r\n");
				PA_MotorStop();				//센서 벗어나지마자 멈춤, A0_stop_flag 셋됨, 유일하게 이 함수에서!!
				orgStep = STEP6;
				// if(debug_mode)	PCPuts("[Org0_2] \r\n");
			}
			else if(mPA.stop_flag)	// 최대까지 움직였는데도 원점 센서가 검출이 안되면
			{
				// PCPuts("[Org0_2-2] \r\n");
				PCPuts("[SEN_ERR_02] 11 \r\n");	orgStep = STEP10;
			}
		break;
		
		// // 센서 검출될 때까지 센서쪽으로 움직이다가, 검출되면
		case STEP3:
			if(mPA.onOrgSensor)		
			{
				PA_MotorSlowStop();		// 천천히 멈추기 시작
				orgStep = STEP4;
				// PCPuts("[Org0_3-1] \r\n");
				// if(debug_mode)	PCPuts("[Org0_3] \r\n");
			}
			else if(mPA.stop_flag)	// 움직일 만큼 다 움직였는데도 원점 센서가 검출이 안되면
			{
				// PCPuts("[Org0_3-3] \r\n");
				PCPuts("[SEN_ERR_03] 11 \r\n");	orgStep = STEP10;
			}
		break;
		
		case STEP4:
			if(mPA.stop_flag)		// 완전히 멈췄으면
			{
				// PCPuts("[Org0_4-1] \r\n");

				// stall 감지 초기화, 간혹 플래그가 clear가 안되 초기화가 안되어 stall 감지를 한다. 
				// 버그는 PA_MotorSlowStop()과 연관이 있는듯
				isStallInitialized_PA = NO;		

				if(mPA.dir == mPA.neg_dir)	lastTime = TimeNow(); 
				orgStep = STEP5;
				// if(debug_mode)	PCPuts("[Org0_4] \r\n");
			}
		break;
		
		case STEP5:
			if(TimePassed(lastTime) > ONE_SECOND)	// 반대 방향으로 움직이기 전에 1 초 쉬었다가(기구적 완충을 위해)
			{
				// PCPuts("[Org0_5-1] \r\n");
				orgStep = STEP1;
				// if(debug_mode)	PCPuts("[Org0_5] \r\n");
			}
		break;
		
		case STEP6:
			if(mPA.stop_flag)		// 완전히 멈췄으면
			{	
				// PCPuts("[Org0_6-1] \r\n");
				mPA.pos_cnt = 0;	// 원점 다 잡은 위치를 가상의 0점으로 설정. 앞으로 이 값으로 축의 위치 판단
				mPA.org_complete_flag = YES;
				resetOrigin_PA = 0;		// reset 완료되어 clear
				orgStep = STEP1; 		// 이러면 default로 감.
				PCPuts("[rpm_pamv_0000] [pa_end]\r\n");
			}
		break;
		
		default:	break;
	}
}

/*F**************************************************************************
* NAME:   
* PURPOSE:
*****************************************************************************/
// todo : need test function of fixing starting point(Origin position)
void TestMotor()
{
	LONG	total_distance;
	BYTE	dir = 1; // 0 : positive, 1 : negative

	LONG	pa_0_1_unit_pulse = 320;
	LONG	pa_start_speed = 9800;
	LONG	pa_max_speed = 33760;
	LONG	pa_move_value = 1000;

	LONG	ro_1_degree_pulse = 1093;
	LONG	ro_start_speed = 2187;
	LONG	ro_max_speed = 17000;
	LONG	ro_move_value = 10;


/*
	total_distance = pa_move_value * pa_0_1_unit_pulse;	
	total_distance = total_distance - mPA.pos_cnt;

	while(1)
	{	
		UartPrintf("before : dir : %d, pcnt1 : %5ld, pcnt2 : %5ld, pcnt3 : %5ld, pcnt4 : %5ld, pcnt5 : %5ld \r\n", 
			dir, pcnt1, pcnt2, pcnt3, pcnt4, pcnt5);
		// UartPrintf("dir : %d", dir);
		PA_MotorDrive(dir, pa_start_speed, pa_max_speed, total_distance);
		dly_ms(3000);
		++dir;
		dir = dir % 2;

		UartPrintf("after : dir : %d, pcnt1 : %5ld, pcnt2 : %5ld, pcnt3 : %5ld, pcnt4 : %5ld, pcnt5 : %5ld \r\n", 
			dir, pcnt1, pcnt2, pcnt3, pcnt4, pcnt5);
	}
*/
	total_distance = ro_move_value * ro_1_degree_pulse;	
	total_distance = total_distance - mRO.pos_cnt;

	while(1)
	{	

		RO_MotorDrive(dir, ro_start_speed, ro_max_speed, total_distance);
		dly_ms(3000);
		++dir;
		dir = dir % 2;

	}

}
