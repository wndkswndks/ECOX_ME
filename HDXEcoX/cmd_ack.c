/*C***************************************************************************
* $RCSfile: command.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the command source
*----------------------------------------------------------------------------
* RELEASE:      $Name: cmd_ack.c     
* Edited  By    $Id: sslim
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include "includes.h"

/*_____ D E C L A R A T I O N ______________________________________________*/


/*_____ D E F I N E ________________________________________________________*/


/*_____ F U C T I O N ______________________________________________________*/
void CheckState_1ms(void);
void CheckState_10ms(void);
void cmd_ack(void);
void laser_ack(void);

/*_____ V A R I A B L E ____________________________________________________*/
extern BYTE	one_ms_flag;
extern BYTE gen_preheat_time_flag;
BYTE	preheat_ack_flag;

extern BYTE	capt_tm_flag;			// from capture.c
extern LONG capt_tm_cnt;			// from capture.c

//------ ceph관련 var.
BYTE can_seq_stop_time_flag = NO;
ULONG	can_seq_stop_time_cnt = 0;
extern BYTE	ct_mode_value;
extern BYTE isModeCompleted;	// from EcoX_one.c


extern BYTE	laser_ack_flag;			// from timer.c

extern LONG	alex_val;				// from command.c
extern BYTE	alex_ack_flag;			// from timer.c

//------ can com error
extern char    errstr[50][30];		// from command_canmsg.c
extern BYTE    err_save_cnt;	// from command_canmsg.c
BYTE    err_send_cnt = 0;

/*F********************************************************************************************************
* NAME:     CheckState_1ms

* PURPOSE:	매 루프마다 반복할 필요는 없고, 1 ms 마다 한 번씩 반복적으로 수행해야 하는 명령들만 별도 관리

* Notice:	촬영 동작에 방해를 최소화해야 함
***********************************************************************************************************/
// 사용되는 곳은 main()에서 사용됨 while(1) 전에 한번 호출, while()1에서 호출
// one_ms_flag 조건에 따라 호출됨, timer에서 1ms 마다 set됨
// todo : Can use 5ms timer interrupt?
void CheckState_1ms(void)
{
	static ULONG	exp_sw_cnt = 0;
	static int 		cnt_1ms = 0;



	if(one_ms_flag == NO) return;
	one_ms_flag = NO;

	++cnt_1ms;
	


	//----------------- rutin of process flag

	PA_UpdateState();
	RO_UpdateState();




	// 촬영중일 때 꼭 필요한 것들(촬영시 제약된 시간 인터럽트 확보를 위해) 
	if(exp_ok_flag)			// 촬영 중일때 확인이 꼭!! 필요한 것들
	{
		if(capt_tm_flag)	++capt_tm_cnt;
	}
	else if(!exp_ok_flag)	// 촬영중이 아닐때 처리할 것들( 촬영없을때 처리)
	{
		cmd_ack();	


		// CheckStall_RO();

		/*if(err_save_cnt > 0)	// CAN 통신으로 수신하는 err가 발생 했으면
		{
		    if(err_save_cnt != err_send_cnt)
		    {
		        PCPuts(errstr[err_send_cnt]);
		        err_send_cnt ++;
		    }
		    else
		    {
		        err_save_cnt = err_send_cnt = 0;
		    }
		}*/			
	}

	
	////////  촬영여부와 상관없이 확인 필요 ///////

	if( (cnt_1ms % 10) == 0)	// 10ms마다 실행
	{
		CheckState_10ms();
	}


	laser_ack();



	//-----------------------------------------	1 ms 마다 항상 확인이 필요한 것들

	if(can_seq_stop_time_flag)
	{
		++can_seq_stop_time_cnt;
				
		if(can_seq_stop_time_cnt > 9)
		{ 	
			can_seq_stop_time_flag = NO; 	can_seq_stop_time_cnt = 0;	
		}
	}

	if(!EXPOSURE_SW)	// Active LOW
	{
		++exp_sw_cnt;
		
		if(exp_sw_cnt > 2)	exp_sw_flag = ON;	// 조사스위치 입력이 3 ms 이상 LOW일 때만 의도적으로 누른 것으로 판단
	}
	else	{	exp_sw_cnt = 0;	exp_sw_flag = OFF;	}


/*	if(gvRO.move_ack_flag)
	{
		if(gvRO.stop_flag)
		{
			gvRO.move_ack_flag = NO;
			// if( (!A1_p_over_flag) && (!A1_n_over_flag) )
			{
				// if(gvRO.move_value != 0)	
				{	
					PCPrintf("[rpm_romv_%05ld] \r\n",gvRO.move_value);
				}
			}
		}
	}
*/
	
	if(dark_ack_flag)
	{
		dark_ack_flag = NO;

		if(ct_mode_value == FRAME_CT)			PCPuts("[rpm_dark_ct] \r\n");
		else if(ct_mode_value == FRAME_UFS)		PCPuts("[rpm_dark_ufs] \r\n");
		else if(ct_mode_value == FRAME_PANO)	PCPuts("[rpm_dark_pano] \r\n");
		else if(ct_mode_value == FRAME_CEPH)	PCPuts("[rpm_dark_sc] \r\n");
	}
	
	else if(brit_ack_flag)
	{
		brit_ack_flag = NO;

		if(ct_mode_value == FRAME_CT)			PCPuts("[rpm_brit_ct] \r\n");
		else if(ct_mode_value == FRAME_UFS)		PCPuts("[rpm_brit_ufs] \r\n");
		else if(ct_mode_value == FRAME_PANO)	PCPuts("[rpm_brit_pano] \r\n");
		else if(ct_mode_value == FRAME_CEPH)	PCPuts("[rpm_brit_sc] \r\n");
	}
	
}



/*F********************************************************************************************************
* NAME:     
* PURPOSE:	
***********************************************************************************************************/
void laser_ack(void)
{
	switch(laser_ack_flag)
	{
		case 0:		break;	// initial value

		// LaserZ()의 경우 CAN com. 이어서 timer interrupt에서 반응이 안되어 이곳에서 처리
		case 99:		// turn off all lasers	
			LaserOff();
			laser_ack_flag = 0;
		break;	
			
		default:	PCPuts("Undefined laser_ack_flag! \r\n");	break;
	}


}


/*F********************************************************************************************************
* NAME:     CheckState_10ms
* PURPOSE:	10 ms 마다 카운트를 증가시켜도 충분한 것들
***********************************************************************************************************/
// CheckState_1ms()에서 호출되면서 switch()을 통해 10번을 돌아가며 해당 state check.
// todo : 5ms interrupt를 사용하는 방법!!
void CheckState_10ms(void)
{
	static ULONG	gen_preheat_time_cnt  = 0;
	static LONG		sound_play_time_cnt	= 0;		// todo naming : timeCnt_SoundPlay
	static int		cnt_10ms = 0;

	++cnt_10ms;

	if(cnt_10ms % 100 == 0) // 1초
	{
		// UartPrintf("==> 10ms func : 1 sec \r\n");
	}

	// gen preheat하는 루틴... 
	// todo : ==> system.c 에 옮겨져야 한다.
	if(gen_preheat_time_flag)
	{
		++gen_preheat_time_cnt;
		if(first_run == NO)
		{
			if(gen_preheat_time_cnt > 250)	// 예열 2.5 초
			{	gen_preheat_time_cnt = 0;	gen_preheat_time_flag = NO;	}
		}
	}


	switch(cnt_10ms%10)
	{
		 
		case 1:
			
		break;
		
		case 2:

		break;

		default: 	break;
	}
}


/*F********************************************************************************************************
* NAME:     cmd_ack

* PURPOSE:	Ack 출력 시점이 명령어 입력을 받자마자 되지 않고 특정 이벤트 이후에 출력되는 경우,
			또는 타이머 내부에서 출력해야 하는 경우에는 별도 관리

* Notice:	동시에 일어날 수 없는 일에 대해서만 else if 문을 적용해야 함
			Ack 출력이 촬영 동작에 방해가 되지 않아야 함
***********************************************************************************************************/
// one_ms_cmd()에서 x-ray가 조사되지 않을 때 호출됨.  1ms
void cmd_ack(void)
{
 	char str[14] = {0};
	LONG temp;
	static BYTE cmd_ack_cnt = 0;
	
    /* if(dz_option == DC_MOTOR)
	{
		dz_CT_limit_stop();	dz_PANO_limit_stop();
		
		if(!dz_stop_flag)
		{
			++dz_stop_cnt;
			if(dz_stop_cnt > 6000)	// 6초 이내에 멈추지 않으면
			{
				dz_stop();
				if(DZ_DIR == DZ_CT)		com4_puts("[SEN_ERR_12] 14 \r\n");	
				else if(DZ_DIR == DZ_PANO)	com4_puts("[SEN_ERR_14] 14 \r\n");	
			}
		}
		else	dz_stop_cnt = 0;
	}*/
	
	++cmd_ack_cnt;
	
	switch(cmd_ack_cnt%20)
	{

		case 8:
			if(alex_ack_flag)
			{
				alex_ack_flag = NO;
				
				if(align_exp_time_10u > 0)
				{
					PCPrintf("[rpm_alex_%05ld",alex_val);
					
					if(gen_mode_ack_flag == GEN_NORMAL_MODE)		PCPuts("c] \r\n");
					else if(gen_mode_ack_flag == GEN_PULSE_MODE)	PCPuts("p] \r\n");
				}
				else	PCPuts("Alex manually stop! \r\n");
			}
			// else if(align_check_ack_flag)
			// {
			// 	align_check_ack_flag = NO;
			// 	PCPrintf("[rpm_alck_%04ld] \r\n", align_check_pulse_cnt*100/CO_SCAN_CEPH_PA_MODE_CAPT_AREA_TOTAL_PULSE);
			// }
		break;

		case 9:
			// 하나의 축에서는 하나의 조건만 성립하므로 else
			// todo 정리 : 다른 방식으로 rpm 메세지를 전달할 수 있을 듯.
			if(gvPA.move_ack_flag)
			{
				if(gvPA.stop_flag)
				{
					gvPA.move_ack_flag = NO;

					// if( PA_p_n_over_flag() == NO )	// 기구적으로 이동 가능한 범위 내에서 움직일 때에만
					{
						// if(gvPA.move_value)	
						{	
							PCPrintf("[rpm_pamv_%04ld] \r\n",gvPA.move_value); 
						}
					}
				}
			}
			else if(gvPA.p_over_ack_flag)	{	gvPA.p_over_ack_flag = NO;	PCPuts("[SYS_CHK_02] 11 \r\n");	}
			else if(gvPA.n_over_ack_flag)	{	gvPA.n_over_ack_flag = NO;	PCPuts("[SYS_CHK_03] 11 \r\n");	}
		break;

		case 11:
			if(gvRO.move_ack_flag)
			{
				if(gvRO.stop_flag)
				{
					gvRO.move_ack_flag = NO;

					// if( RO_p_n_over_flag() == NO )	// 기구적으로 이동 가능한 범위 내에서 움직일 때에만
					{
						// if(gvRO.move_value != 0)	
						{	
							PCPrintf("[rpm_romv_%04ld] \r\n",gvRO.move_value);
						}
					}
				}
			}
			else if(gvRO.p_over_ack_flag)	{	gvRO.p_over_ack_flag = NO;	PCPuts("[SYS_CHK_02] 12 \r\n");	}
			else if(gvRO.n_over_ack_flag)	{	gvRO.n_over_ack_flag = NO;	PCPuts("[SYS_CHK_03] 12 \r\n");	}
		break;
	}

}
