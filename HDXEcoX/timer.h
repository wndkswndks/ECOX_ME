/*H***************************************************************************
* $RCSfile: timer.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the timer head define
*----------------------------------------------------------------------------
* RELEASE:      $Name: timer.h      
* Edited  By    $Id: sslim 
*****************************************************************************/
#ifndef _TIMER_H
#define _TIMER_H

//------------------------------------------------ TIER: Timer Interrupt Enable Register
#define EN_INT_TIMER0		TPU0.TIER.BIT.TGIEA
#define EN_INT_TIMER1		TPU1.TIER.BIT.TGIEA
#define EN_INT_TIMER3		TPU3.TIER.BIT.TGIEA
#define EN_INT_TIMER4		TPU4.TIER.BIT.TGIEA
#define EN_INT_TIMER5		TPU5.TIER.BIT.TGIEA

//------------------------------------------------ TCNT: Timer Counter
#define TIMER_COUNTER0		TPU0.TCNT
#define TIMER_COUNTER1		TPU1.TCNT

//------------------------------------------------ TGR: Timer General Register
#define	CYCLE_TIMER0		TPU0.TGRA
#define	CYCLE_TIMER1		TPU1.TGRA
#define	CYCLE_TIMER3		TPU3.TGRA
#define	CYCLE_TIMER4		TPU4.TGRA
#define	CYCLE_TIMER5		TPU5.TGRA
#define	DUTY_TIMER0			TPU0.TGRB
#define	DUTY_TIMER1			TPU1.TGRB

//------------------------------------------------ TSTR: Timer Start Register
#define START_TIMER0		TPU.TSTR.BIT.CST0
#define START_TIMER1		TPU.TSTR.BIT.CST1
#define START_TIMER3		TPU.TSTR.BIT.CST3
#define START_TIMER4		TPU.TSTR.BIT.CST4
#define START_TIMER5		TPU.TSTR.BIT.CST5

//------------------------------------------------ Time define
#define ONE_SECOND 				200		// todo relocate


/////////////////////////////////////////////////////////////////////////////
extern void dly_ms(LONG);
extern void dly_10us(WORD);
extern void StartTimeCheck_10us(char *);
extern void EndTimeCheck_10us(void);
extern void timer_init(void);
extern void SetGenPulse(BYTE);
extern void SetGenPulseCnt(LONG);
extern void SetDetPulse(BYTE);
extern void SetDetPulseCnt(LONG);
extern ULONG TimeNow(void);
extern ULONG TimePassed(ULONG);
extern ULONG TimeNow_10us(void);
extern ULONG TimePassed_10us(ULONG);
extern void TestTimer(void);
extern void trig_pulse_setting(BYTE);

extern void s_ceph_det_off(void);
extern void s_ceph_det_dark_capt(void);
extern void s_ceph_det_exp_capt(void);


// tracking collimator
extern ULONG Get_TimerCnt_ms(void);


//////////


extern LONG	r_lmt_flag;
extern BYTE	canmsg_timeover_check_flag;
// extern BYTE	canmsg_timeover_flag;
extern ULONG	canmsg_timeover_cnt;

extern LONG genPulseCnt;		// Generator pulse counter
extern LONG detPulseCnt;		// todo : 글로벌 전역변수로 사용안됨 Detector pulse counter
extern LONG	pulse1_period;		// pulse1 -> generator
extern LONG	pulse2_period;		// pulse2 -> detector
extern LONG	pulse2_duty;
extern WORD	falling_edge_cnt;	// detector

extern BYTE	ro_end_tm_flag;
extern LONG	ct_ro_end_tm_5ms;	// todo naming, relocate
extern LONG	pano_ro_end_tm_5ms;

extern ULONG	align_exp_time_cnt;
extern LONG	trig_pulse_time;

#endif /*_TIMER_H*/