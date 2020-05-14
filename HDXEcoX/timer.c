/*C***************************************************************************
* $RCSfile: timer.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the timer library
*----------------------------------------------------------------------------
* RELEASE:      $Name: timer   
* Edited  By    $Id: sslim
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include "Includes.h"


/*_____ D E C L A R A T I O N ______________________________________________*/

/*_____ D E F I N E ________________________________________________________*/
#define TIME_MAX 	4294967295		// system timer Tick 변수(32bit)의 최대값


/*_____ F U C T I O N ______________________________________________________*/
static void enable_timer(void);
static void disable_timer(void);

void dly_ms(LONG value);
void dly_10us(WORD);

void timer_init(void);
ULONG TimeNow(void);
ULONG TimePassed(ULONG);
ULONG TimeNow_10us(void);
ULONG TimePassed_10us(ULONG);


void SetGenPulse(BYTE);
void SetDetPulse(BYTE);
void SetGenPulseCnt(LONG);
void SetDetPulseCnt(LONG);
LONG GetDetPulseCnt(void);
LONG GetGenPeriodCnt(void);


void s_ceph_det_off(void);
void s_ceph_det_dark_capt(void);
void s_ceph_det_exp_capt(void);

void s_ceph_det_off(void);
void s_ceph_det_dark_capt(void);
void s_ceph_det_exp_capt(void);

void SetTriggerPulse_CT(LONG);
// ULONG Get_TimerCnt_ms(void);

/*_____ V A R I A B L E ____________________________________________________*/
BYTE 	dly_ms_call;		
LONG	dly_ms_cnt;

WORD	one_ms_cnt = 0;
BYTE	one_ms_flag = NO;

ULONG	gSytemTicks;			// 5ms * 32bit ==> about 3,221 days
ULONG	gSytemTicks_10us;		// 10us * 32bit ==> about 11.93 hours

BYTE 	wait_ms_call;		
LONG	wait_ms_cnt;

BYTE	timePassed_10ms_call;
LONG	timePassed_10ms_cnt;

BYTE	checkTimeCall;
LONG	checkTimeCnt_10us;

BYTE	gen_trig_flag = NO;			// pulse output for Generator expose trigger
BYTE	ct_pano_det_trig_flag = NO;		// todo naming : det_pulse_flag // pulse output for Detector external trigger input

LONG 	genPulseCnt;		// Generator pulse counter
LONG 	detPulseCnt;		// Detector pulse counter

LONG	pulse1_period;		// pulse1 -> generator
// LONG	genPulsePeriodCnt;

LONG	pulse2_period;		// pulse2 -> detector
LONG	pulse2_duty;
LONG	sync_cnt;			// for sync with detector & generator & rotator  
LONG 	genSyncCnt;			// CT에서 사용되는 sync_cnt와 같은 기능함

LONG	genPulseHighStartCnt;
LONG	genPulseHighEndCnt;

BYTE	pre_trig_lv = HIGH;
WORD	falling_edge_cnt = 0;

BYTE	canmsg_timeover_check_flag;
BYTE	canmsg_timeover_flag;
ULONG	canmsg_timeover_cnt;

BYTE	ro_end_tm_flag = 0;
ULONG	ro_end_tm_cnt = 0;
LONG	ct_ro_end_tm_5ms;
LONG	pano_ro_end_tm_5ms;

// for control sensor  todo struct motor_t in motor_dirve.h
ULONG	p_org_on_cnt	= 0;
ULONG	p_org_off_cnt	= 0;
ULONG	r_org_on_cnt	= 0;
ULONG	r_org_off_cnt	= 0;
ULONG	r_lmt_on_cnt	= 0;
ULONG	r_lmt_off_cnt	= 0;

LONG	r_lmt_flag	= NO;
ULONG	align_exp_time_cnt = 0;
LONG	trig_pulse_time = 0;


//-------- ceph작업 변수
WORD	ceph_falling_edge_cnt = 0;
BYTE	ceph_pre_trig_lv = HIGH;	// 10us 인터럽트 함수 내에서만 사용됨
WORD    vw_exp_ok_cnt = 0;
BYTE    flag_vw_exp_ok = HIGH;		// 10us 인터럽트 함수 내에서만 사용됨, pano에서 사용되는 듯

BYTE	s_ceph_det_trig_flag = NO;  // todo 
LONG	det_trig_cnt;
LONG	det_tbl_no;
BYTE	det_trig_sub_step;
extern BYTE	dark_frame_acq_flag;	// from command.c
BYTE	table_loop_flag	= NO;
BYTE 	pano_5000_cnt = 0;
BYTE	ct_mode_value = 0;

//-------- laser 관련  var.
BYTE	laser_lmt_cnt_flag = OFF;
LONG	laser_on_time_5m = 18000;	// 90 s (5 ms 단위)
LONG	laser_on_cnt = 0;
BYTE	laser_ack_flag = 0;

//------- alignment
BYTE	alex_ack_flag;

//------
LONG testCnt = 0;
int isOn_1sec = 0;

//---- for testing  dynamic 4 axis 
BYTE	g_OnTimer_10us;
ULONG	g_TimerCnt_10us;
ULONG 	g_TimePeriod_10us;
BYTE 	g_OnSampleTime;

/*F**************************************************************************
* NAME:     Timer_init
* PURPOSE:  총 16개의 타이머를 쓸수 있으며, 0,3 channel에서 4개씩 1,2,4,5 channel에서 2개씩
* TSYR : Timer Synchronous Register
* TIOR : Timer I/O Control Register
* TMDR : Timer Mode Register
* TIER : Timer Interrupt Enable Register
* TCR : Timer Control Register
*****************************************************************************/
void timer_init(void)
{
	enable_timer();
	
	TPU.TSYR.BYTE   = 0x00;			// Ch 0,1,2,3,4 Asyncronization
	
	TPU0.TIOR.WORD 	= 0x5220;       // TGR0 B/A/C/D: 0/1output at compare match
  	TPU0.TMDR.BYTE 	= 0x02;        	// TGRA & TGRC Bbuffer operation. PWM mode 1
   	TPU0.TIER.BYTE 	= 0x00;         // TIER Interrupte Disable
	TPU0.TCR.BYTE 	= 0x28;         // TCNT Clear by TGRA compare match. Counted at rising edge, Internal clock: counts on Pφ/1
	
    TPU1.TIOR.BYTE 	= 0x52;         // TGR1 B/A:0/1output at compare match
    TPU1.TMDR.BYTE 	= 0x02;         // TGRA & TGRB Bbuffer operation. PWM mode 1
   	TPU1.TIER.BYTE 	= 0x00;         // TIER Interrupte Disable
	TPU1.TCR.BYTE 	= 0x28;         // TCNT Clear by TGRA compare match. Counted at rising edge, Internal clock: counts on Pφ/1
	
	TPU2.TIOR.BYTE 	= 0x52;         // TGR2 B/A:0/1output at compare match
    TPU2.TMDR.BYTE 	= 0x02;         // TGRA & TGRB Bbuffer operation. PWM mode 1
   	TPU2.TIER.BYTE 	= 0x00;         // TIER Interrupte Disable
    TPU2.TCR.BYTE 	= 0x28;         // TCNT Clear by TGRA compare match. Counted at rising edge, Internal clock: counts on Pφ/1
	
	TPU3.TMDR.BYTE 	= 0x0002;       // TGRA & TGRB/ TGRC & TGRD Bbuffer operation. PWM mode 1  
	TPU3.TIER.BYTE 	= 0x00;         // TIER Interrupte Disable
	TPU3.TCR.BYTE 	= 0x29;         // TCNT Clear by TGRA compare match. Counted at rising edge, Internal clock: counts on Pφ/4
	
	// TPU4.TIOR.BYTE =  0x52;         // TGR4 B/A:0/1output at compare match  // AEC기능 timer를 위해  주석(Qface기준)
    TPU4.TMDR.BYTE 	= 0x02;         // TGRA & TGRC Bbuffer operation. PWM mode 1
   	TPU4.TIER.BYTE 	= 0x00;         // TIER Interrupte Disable
    TPU4.TCR.BYTE 	= 0x29;         // TCNT Clear by TGRA compare match. Counted at rising edge, Internal clock: counts on Pφ/4

    TPU5.TMDR.BYTE 	= 0x02;			// TGRA & TGRC Bbuffer operation. PWM mode 1
   	TPU5.TIER.BYTE 	= 0x00;         // TIER Interrupte Disable
	TPU5.TCR.BYTE 	= 0x2A;			// TCNT Clear by TGRA compare match. Counted at rising edge, Internal clock: counts on Pφ/16
	
	// 10 us setting
	CYCLE_TIMER3 		= 49;		// TGRA Buffer Set  50번 카운트 증가해야 인터럽트 한번 발생(10us); 20,000,000/4 => 5,000,000 Hz
	START_TIMER3 		= YES;		// 1 / 5,000,000 Hz = 0.0000002sec = 0.2us, 0.2 * 50 = 10us
	EN_INT_TIMER3		= YES;

	// 100 us setting
	CYCLE_TIMER4 		= 499;		// TGRA Buffer Set  500번 카운트 증가해야 인터럽트 한번 발생(100us); 20,000,000/4 => 5,000,000 Hz
	START_TIMER4 		= YES;		// 1 / 5,000,000 Hz = 0.0000002sec = 0.2us, 0.2 * 500 = 100us
	EN_INT_TIMER4		= YES;


	// 5ms setting	
	CYCLE_TIMER5 		= 6249;		// TGRA Buffer Set  25000번 카운트 증가해야 인터럽트 한번 발생(5ms); 20,000,000/16 => 1,250,000 Hz
	START_TIMER5 		= YES;		// 1 / 1,250,000 Hz = 0.0000008sec = 0.0008ms, 0.0008 * 6250 = 5ms
	EN_INT_TIMER5		= YES;
}

void TestTimer()
{


	if(isOn_1sec == YES)
	{
		isOn_1sec = NO;
		UartPrintf("100us timer interrupt - 1sec! : testCnt : %ld \r\n", testCnt);
	}

}

/*F**************************************************************************
* NAME:     INT_TGI4A_TPU4  
* PURPOSE:  100us timer interrupt
*****************************************************************************/
__interrupt(vect=106) void INT_TGI4A_TPU4(void)
{
	TPU4.TSR.BIT.TGFA = 0;


	check_exp_AEC_mode();


	if((++testCnt % 10000) == 0)
	{
		isOn_1sec = YES;
		// UartPrintf("==> Timer4 interrutp\r\n");
	}

}

void SetTimer_Dynamic4Axis(ULONG timerCnt_ms)
{
	g_TimePeriod_10us = timerCnt_ms * 100;	
}


/*F**************************************************************************
* NAME:     INT_TGI3A_TPU3
* PURPOSE: 10us 주기로 interrupt 발생 : CYCLE_TIMER3 = 49 (10us)
*****************************************************************************/
__interrupt(vect=101) void INT_TGI3A_TPU3(void)
{
	TPU3.TSR.BIT.TGFA = 0;

	++gSytemTicks_10us;

	if(dly_ms_call)		++dly_ms_cnt;

	if(wait_ms_call) 	++wait_ms_cnt;

	if(checkTimeCall)	++checkTimeCnt_10us;

	if(g_OnTimer_10us) {
		++g_TimerCnt_10us;
		// if(g_TimerCnt_10us >= 20) {
		if(g_TimerCnt_10us >= g_TimePeriod_10us) {
			g_TimerCnt_10us = 0;
			g_OnSampleTime = ON;
		}
	}

	++one_ms_cnt;	
    if(one_ms_cnt > 99)	{	one_ms_cnt = 0;	one_ms_flag = YES;	}


	if(ct_pano_det_trig_flag)	// Detector Trigger Input Signal Generation
	{
	    // Falling edge detect	
	    // VW_EXP_SYNC 포트는 회로적으로 역전되어 있음. port = High  ==> 0V, Low ==> 5V

		if(detPulseCnt >= pulse2_period)	detPulseCnt = 0;	//period times equal to setting period times

 		if(detPulseCnt < (pulse2_period - pulse2_duty) ) 	VW_EXP_SYNC = HIGH;	// 회로적으로 0V
		else												VW_EXP_SYNC = LOW;	// 회로적으로 5V

		++detPulseCnt;
	}

    // gen_pulsed_exp()가 호출되면, set 된다. 
	if(gen_trig_flag)			// Generator Trigger Input Signal Generation
	{
		if(genPulseCnt >= pulse1_period)	genPulseCnt = 0;

		if( (genPulseHighStartCnt <= genPulseCnt) && (genPulseCnt < genPulseHighEndCnt) )	
				X_RAY_PULSE	= ON;
		else 	X_RAY_PULSE	= OFF;

		++genPulseCnt;
	}

	// detector TD_X_2301		
	if(s_ceph_det_trig_flag)
	{
		if(detPulseCnt >= pulse2_period)	detPulseCnt = 0;

		if(detPulseCnt < pulse2_duty)	S_CEPH_TRIG = LOW;
		else							S_CEPH_TRIG = HIGH;

		++detPulseCnt;
	}


	if(exp_ok_flag)
	{
		++pano_5000_cnt;
		if(pano_5000_cnt > 19)		// 5000 Hz 단위로 테이블을 읽어서 실행
		{
			pano_5000_cnt = 0;	table_loop_flag	= YES;
		}

		if(CPU_RING)	// ring on 시간 동안 출력되는 디텍터 트리거 폴링 엣지의 개수 측정
		{
			// CT_TRIG_PULSE는 H/W 포트로 사용되지 않는다. 
			// todo : VW_EXP_SYNC를 대신 사용하는 것이 바람직	
			if( (!pre_trig_lv) && (VW_EXP_SYNC) )	++falling_edge_cnt;	// 디텍터에 연결되는 커넥터 출력은 MCU 출력과 반대
			pre_trig_lv = VW_EXP_SYNC;

			if( (!ceph_pre_trig_lv) && (S_CEPH_TRIG) )	++ceph_falling_edge_cnt;
			ceph_pre_trig_lv = S_CEPH_TRIG;

			// VIVIX-D 0606C의 EXP_OK 신호가 동작하고(Read-out LOW = Diode On)
            if(!VW_0606C_EXP_OK && flag_vw_exp_ok)  { flag_vw_exp_ok = LOW; ++vw_exp_ok_cnt; }
            else                                    flag_vw_exp_ok = VW_0606C_EXP_OK;
		
		}
	}
	else	// 10 us 타이머에 영향을 주지 않기 위해서 촬영 중에 필요없는 기능은 사용 안 함
	{
		
		// 이렇게 검출알고리즘을 짠 이유는 뭘까?? if(조건문이 불필요한듯 한데...)
	    // 히스테리시스를 위해 org_on_cnt가 필요??
	    // todo : timer에서 센서 검출 알고리즘을 구현할 필요가 있는가??
		if(!PA_ORG_SENSOR)				//if detect SENSOR
		{
			++p_org_on_cnt;
			p_org_off_cnt = 0;
			if( (mPA.onOrgSensor == NO) && (p_org_on_cnt > 1) )	mPA.onOrgSensor = YES;
		}
		else if(PA_ORG_SENSOR)			//if not detect SENSOR
		{
			p_org_on_cnt = 0;
			++p_org_off_cnt;
			if( (mPA.onOrgSensor == YES) && (p_org_off_cnt > 1) )	mPA.onOrgSensor = NO;
		}

		if(!RO_ORG_SENSOR)
		{
			++r_org_on_cnt;
			r_org_off_cnt = 0;
			if( (mRO.onOrgSensor == NO) && (r_org_on_cnt > 1) )	mRO.onOrgSensor = YES;
		}
		else if(RO_ORG_SENSOR)
		{
			r_org_on_cnt = 0;
			++r_org_off_cnt;
			if( (mRO.onOrgSensor == YES) && (r_org_off_cnt > 1) )	mRO.onOrgSensor = NO;
		}

		if(!RO_LMT_SENSOR)
		{
			++r_lmt_on_cnt;
			r_lmt_off_cnt = 0;
			if( (r_lmt_flag == NO) && (r_lmt_on_cnt > 1) )	r_lmt_flag = YES;
		}
		else if(RO_LMT_SENSOR)
		{
			r_lmt_on_cnt = 0;
			++r_lmt_off_cnt;
			if( (r_lmt_flag == YES) && (r_lmt_off_cnt > 1) )	r_lmt_flag = NO;
		}


		if(align_exp_time_flag)
		{
			++align_exp_time_cnt;
			
			if(align_exp_time_cnt > align_exp_time_10u)
			{
				align_exp_time_cnt = 0;	align_exp_time_flag	= NO;
				
				switch(align_acq)
				{
					case ALIGN_EXPOSE:		alex_ack_flag = YES; align_acq = 0;
											gen_off();	lamp_off();
											ring_off();	ct_pano_det_off();	break;

					// case CT:				// todo : 사용이 되는지 확인
					// case PANO_STAN:			// todo : 사용이 되는지 확인
					// case PANO_TMJ1:			// todo : 사용이 되는지 확인
					case DARK_FRAME_ACQ:	dark_ack_flag = YES; align_acq = 0;
											ct_pano_det_off(); 
											s_ceph_det_off();
											break;

					case BRIGHT_FRAME_ACQ:	brit_end_flag = YES; align_acq = 0;
											s_ceph_det_off();
											break;
					default:	break;
				}
			}
		}

		// if(align_trg_time_flag)	    ++align_trg_time_10u;
	}
}

/*F**************************************************************************
* NAME:     INT_TGI5A_TPU5
* PURPOSE: 5ms 주기로 interrupt 발생 : CYCLE_TIMER5 = 6249 (5ms)
*****************************************************************************/
// vector 110 TGI5A TPU5
__interrupt(vect=110) void INT_TGI5A_TPU5(void)
{
	static int cnt_5ms = 0;
	static LONG sound_play_time_cnt = 0;

	TPU5.TSR.BIT.TGFA = 0;

	++gSytemTicks;

	++cnt_5ms;

	if(timePassed_10ms_call) 	++timePassed_10ms_cnt;

	if(exp_ok_flag)
	{
	}
	else
	{
		// 혹시라도 사용자가 레이저를 켜고 장시간 방치할 경우를 대비해서, 
		// 일정 시간 지나면 자동으로 꺼지도록 함
		// todo : systemTick을 이용해 인터럽트를 사용하지 않고도 구현할 수 있다. 
		if(laser_lmt_cnt_flag) 
		{
			++laser_on_cnt;
			
			if(laser_on_cnt > laser_on_time_5m)	
			{
				laser_ack_flag = 99;	// laser off 
			}
		}
		else	laser_on_cnt = 0;	// 항상 이쪽으로 들어온다. 
	}


	// todo delete : canmsg_timeover_check_flag를 사용하는 곳이 없다. 
	if(canmsg_timeover_check_flag)
	{
		++canmsg_timeover_cnt;
		if(canmsg_timeover_cnt > 199)
		{	canmsg_timeover_cnt = 0;	canmsg_timeover_check_flag = NO;	canmsg_timeover_flag = YES;	}
	}


	if(cnt_5ms % 2 == 0)   // 10ms 마다 실행
	{
		// todo : systemTick을 이용해 인터럽트를 사용하지 않고도 구현할 수 있다. 
		if(sound_play_time_flag)		// sound play time 
		{
			// UartPrintf("sound : play time : %ld, time cnt : %ld \r\n", sound_play_time, sound_play_time_cnt);
			++sound_play_time_cnt;
			if(sound_play_time_cnt > sound_play_time) 	// sound_play_time = 200 은 1초,  MP3.c에서 설정된다.
			{	
				sound_play_time_cnt = 0;	
				sound_play_time_flag = NO;	
				sound_play_time_end = YES;	
			}
		}
	}

	
}

/*F**************************************************************************
* NAME:     
* PURPOSE: Interface Functions
*****************************************************************************/
void SetGenPulse(BYTE sw)
{
	gen_trig_flag = sw;
}

void SetGenPulseCnt(LONG value)
{
	genPulseCnt = value;
}

void SetDetPulse(BYTE sw)
{
	ct_pano_det_trig_flag = sw;
}

void SetDetPulseCnt(LONG value)
{
	detPulseCnt = value;
}

LONG GetDetPulseCnt(void)
{
	return detPulseCnt;
}

LONG GetGenPeriodCnt(void)
{
	return pulse1_period;
}

/*F**************************************************************************
* NAME:     scan_ceph_detector control function
	S_CEPH_EN	==> Active HIGH
*****************************************************************************/
// todo delete : RELAY2 is not connected pin
// ENABLE_OFF  -> defined '1'
void s_ceph_det_off(void)
{
	S_CEPH_TRIG = HIGH;	RELAY2 = LOW;	S_CEPH_EN = 1; // ENABLE_OFF;
	s_ceph_det_trig_flag = NO;
	det_trig_sub_step = SUB_STEP1;	det_trig_cnt = 0;	det_tbl_no = 0;
	dark_frame_acq_flag = NO;
}

void s_ceph_det_dark_capt(void)
{
	s_ceph_det_trig_flag = YES;
	det_trig_sub_step = SUB_STEP1;	det_trig_cnt = 0;	det_tbl_no = 0;
	dark_frame_acq_flag = YES;
}

void s_ceph_det_exp_capt(void)
{
	s_ceph_det_trig_flag = YES;
	det_trig_sub_step = SUB_STEP1;	det_trig_cnt = 0;	det_tbl_no = 0;
	dark_frame_acq_flag = NO;
}



/*F**************************************************************************
* NAME:     
* PURPOSE:  Check Time of function execution
*****************************************************************************/
void StartTimeCheck_10us(char *str)
{
	UartPrintf("\r\n==> [ %s ]  TimeCheck start!!\r\n", str);
	checkTimeCall = 1;
	checkTimeCnt_10us = 0;
}

void EndTimeCheck_10us(void)
{
	checkTimeCall = 0;
	UartPrintf("==> EndTimeCheck : %ld us \r\n", checkTimeCnt_10us*10);
}


/*F**************************************************************************
* NAME:     time tick을 이용한 TimeNow(), TimePassed(since) 함수
* PURPOSE:  TimeNow() 시점부터 지난 시간을 받아서 시간흐름 처리
* Usage:  
*			lastTime = TimeNow();
*			while(TimePassed(lastTime) > 100)  {....}
*****************************************************************************/
ULONG TimeNow(void)
{
	return gSytemTicks;
}

ULONG TimePassed(ULONG since)
{
	ULONG	now = gSytemTicks;

	if (now >= since ) 	return (now - since);

	// 32bit gSystemTicks 변수의 최대값 초과시
	// 32bit * 5ms ==> about 3,221 days
	return ((now + TIME_MAX + 1 ) - since);
}

// 정밀 시간 측정 및 제어를 위해 10us timer 인터럽트 사용
ULONG TimeNow_10us(void)
{
	return gSytemTicks_10us;
}

ULONG TimePassed_10us(ULONG since)
{
	ULONG	now = gSytemTicks_10us;

	if (now >= since ) 	return (now - since);

	// 32bit gSystemTicks 변수의 최대값 초과시
	// 32bit * 10us ==> about 11.93 hours
	return ((now + TIME_MAX + 1 ) - since);
}

/*F**************************************************************************
* NAME:     dly_ms
* PURPOSE:
*****************************************************************************/
void dly_ms(LONG value)
{
	value = value*100;
	if(value == 0) return;
	dly_ms_call = 1;
	dly_ms_cnt = 0;
	
	while(dly_ms_cnt < ((value << 1) >> 1));
	dly_ms_call = 0;
}

/*F**************************************************************************
* NAME:     dly_10us
* PURPOSE:
*****************************************************************************/
void dly_10us(WORD value)
{
	if(value == 0) return;
	dly_ms_call = 1;
	dly_ms_cnt = 0;
	
	while(dly_ms_cnt < ((value << 1) >> 1));           // ????
	dly_ms_call = 0;
}
/*F**************************************************************************
* NAME:     enable_timer
* PURPOSE:
*****************************************************************************/
void enable_timer(void)
{
	MSTP.CRA.BIT._TPU = 0;
}
/*F**************************************************************************
* NAME:     disable_timer
* PURPOSE:
*****************************************************************************/
void disable_timer(void)
{
	MSTP.CRA.BIT._TPU = 1;
}


/****************************************************************************
* PURPOSE: 촬영을 위한 Detector Trigger Pulse를 셋팅하고, 
* Generator pulse의 sync를 맞추기위한 값을 계산한다. 
*****************************************************************************/
void SetTriggerPulse_CT(LONG detPeriodCnt)
{
	int framePerSec = 0;


    // Vieworks VIVIX-D 0606C 
    // 덴트리 코드 timer.c 인터럽트에서 조건을 '>='으로 바꾸어 'pulse2_period - 1' 형태를 없앰 
    pulse2_period = detPeriodCnt;
    pulse2_duty	= 150;	// '150'은 1.5ms를 의미, spec에는 trig Active time(trig duty)이 1ms 이상 

	framePerSec = 100000 / pulse2_period;	// '100000'은 10us timer interrupt에서 초당 pulseCnt
    PCPrintf("Vieworks VIVIX-D 0606C Binning(2x2) %d fps \r\n", framePerSec);
    
    // xray pulse
    pulse1_period	= pulse2_period;
	genPulseHighStartCnt = gen_trig_start_time;
    genPulseHighEndCnt  = gen_trig_end_time;
	genSyncCnt = (gen_trig_end_time - gen_trig_start_time) / 2 + gen_trig_start_time;
	PCPrintf("WDG90 Period %ldms, On Start %ldms, On End %ldms\r\n",
					pulse1_period/10, genPulseHighStartCnt/10, genPulseHighEndCnt/10);

    if(debug_mode)
    {
		PCPrintf("Msg: gen_period: %ld, gtts: %ld, gtte: %ld,  gen_sync: %ld \r\n",
					pulse1_period, genPulseHighStartCnt, genPulseHighEndCnt, genSyncCnt);
    }
}

/*F**************************************************************************
* NAME:     trig_pulse_setting
* PURPOSE:
*****************************************************************************/
// detctor option과 촬영모드(CT, pano)등에 따른 값들을 셋팅 : detctor와 gen의 트리거 주기, duty, pulse count 등을 셋팅
// pulse2_period, pulse2_duty, pulse1_period, genPulseHighStartCnt, genPulseHighEndCnt, syn_cnt
// todo : magic num. : CT_NORMAL 1, CT_UFS 2, 
// todo : naming : SetGen_Detector_TrigTiming(). SetTrigPulse()
// todo reloact : timer.c
// todo naming : sync_cnt -> sync_cnt_gen
// todo nameing : captureMode -> frameMode
void trig_pulse_setting(BYTE captureMode)
{
	ct_mode_value = captureMode;
	
/*	if(captureMode == FRAME_CT)		// CT Normal
	{
        // Vieworks VIVIX-D 0606C 
        // timer.c 인터럽트에서 조건을 '>='으로 바꾸어 읽기 쉽게 바꿈 
        pulse2_period	= pulse2_period;	//'pulse2_period - 1' ==> pulse2_period;
        pulse2_duty		= 150;	// 10us * 150 = 1.5ms, spec에는 tirg Active time(trig duty)이 1ms 이상 
        
        pulse1_period	= pulse2_period;
		
		genPulseHighStartCnt = gen_trig_start_time;			// read-out end			ex)1600
        genPulseHighEndCnt  = gen_trig_end_time;			// read-out start		ex)2000
		
		sync_cnt = gen_timming;				
        PCPuts("Vieworks VIVIX-D 0606C Binning(2x2) CT \r\n");

        if(debug_mode)
        {
			PCPrintf("Msg: genPulseHighStartCnt: %04ld, genPulseHighEndCnt: %04ld \r\n",genPulseHighStartCnt, genPulseHighEndCnt);
			PCPrintf("Msg: gen period: %04ld, gen sync_cnt: %04ld \r\n",pulse1_period ,sync_cnt);
        }
		
       
	}
	else if(captureMode == FRAME_UFS)	// CT UFS
	{
       // Vieworks VIVIX-D 0606C
        pulse2_period	= pulse2_period;
        pulse2_duty		= 150;	// EXP_SYNC 신호의 최소 Active time은 1ms
        pulse1_period	= pulse2_period;

		genPulseHighStartCnt = gen_trig_start_time;			// read-out end			ex)1600
        genPulseHighEndCnt  = gen_trig_end_time;			// read-out start		ex)2000
		
		sync_cnt = gen_timming;				
        PCPuts("Vieworks VIVIX-D 0606C Binning(2x2) UFS \r\n");

		if(debug_mode)
		{
			PCPrintf("Msg: genPulseHighStartCnt: %04ld, genPulseHighEndCnt: %04ld \r\n",genPulseHighStartCnt, genPulseHighEndCnt);
			PCPrintf("Msg: gen period: %04ld, gen sync_cnt: %04ld \r\n",pulse1_period ,sync_cnt);
		}		


	}
*/
	// else if(captureMode == FRAME_PANO)	// PANO
	if(captureMode == FRAME_PANO)	// PANO
	{
        pulse2_period = pulse2_period;

        // Vieworks VIVIX-D 0606C
        pulse2_duty	= 150;	// EXP_SYNC 신호의 최소 Active time은 1ms
        PCPuts("Vieworks VIVIX-D 0606C Binning(2x2) CF Panorama \r\n");
	}
	else if(captureMode == FRAME_CEPH)	// Scan Ceph
	{
		pulse2_period = 500;
		pulse2_period = pulse2_period;
		pulse2_duty	= 200;	// 'HIGH' time should be not more than 1 ms
		
		PCPuts("Teledyne DALSA Xineos-2301 CEPH Full 200fps \r\n");

		s_ceph_det_off();
	}

	X_RAY_PULSE	= LOW;	RELAY1 = LOW;	genPulseCnt = 0;
	ct_pano_det_off();
}

