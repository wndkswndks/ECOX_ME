/***********************************************************************/
/*                                                                     */
/*  FILE        :EcoX_one.c                                                */
/*  DATE        :2019. 8.08                                            */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :H8SX/1544                                             */
/*                                                                     */
/*  Edited By 	:sslim			   									   */
/*                                                                     */
/***********************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include	"includes.h"


/*_____ D E F I N E ________________________________________________________*/

//----------- option 관련 



/*_____ D E C L A R A T I O N ______________________________________________*/
typedef struct
{
	BYTE 	ceph;
} Option_t;




/*_____ F U C T I O N ______________________________________________________*/
static void ad_init(int);

void InitializeBoard(void);
void InitializeSystem(void);
void InitializeMachine(void);
void InitializeCANMotor(void);
void InitializeGen(void);
void FixOrigin_CANMotor(void);

void gen_off(void);
void ct_pano_det_off(void);
void ct_pano_det_capt(void);
void ring_on(void);
void ring_off(void);
void lamp_off(void);
void rdy_lamp_on(void);
void exp_lamp_on(void);
void err_lamp_on(void);
void LaserOff(void);
void LaserOn(void);

void Init_FlashValue(void);
BYTE SetOptionFromFlash(BYTE, BYTE, BYTE, BYTE);
void Set_CANMotorAxisValue(void);
BYTE CalibrateGen(void);
BYTE PrintOption(void);
BYTE CheckAllOrigin(void);
BYTE IsCompletedAllOrigin(void);
void PrintFW_Version(void);
WORD ad_read(WORD ch);
void GenPreheatTime10sec(BYTE *);

void LaserCanine(BYTE);
void LaserZ(BYTE);
void LaserHorizental(BYTE);
void LaserVertical(BYTE);

void CheckStall_RO_PA(void);


void SetInitialAxisValue_HL(void);
void SetInitialAxisValue_HR(void);
void SetInitialAxisValue_VT(void);
void SetInitialAxisValue_VB(void);



/*_____ V A R I B L E S ______________________________________________________*/

//--- option
// todo : interface function : BYTE GetOption(str), , SetOption(str, value)
Option_t optn;

BYTE 	pa_option;
BYTE 	co_option;
BYTE 	sound_option;
BYTE 	ceph_option;
BYTE 	debug_mode;
BYTE 	pa_axis_end_position;
BYTE 	debug_mode = 0;
BYTE 	ct_ro_end_angle;
BYTE 	ct_ro_end_tm;
BYTE	gen_overheat_threshold;
BYTE	ac_option;
BYTE 	gen_option;

BYTE	first_run;
BYTE    g_IsInitializingGen;
// axisCANMotor_t gAxisCO;
// axisCANMotor_t gAxisSD;

varMotor_t 		gvPA;
varMotor_t 		gvRO;
varMotor_t 		gvCO;
varMotor_t  	gvSD;
varMotor_t  	g_HL;
varMotor_t  	g_HR;
varMotor_t  	g_VT;
varMotor_t  	g_VB;



//------ imsi 
BYTE 	co_org_complete_flag;
BYTE gen_preheat_time_flag;		// todo : move to other file
LONG gen_preheat_time_cnt = 0;	// todo : move to other file

BYTE	ring_ack_flag; 			// todo : move to other file	
BYTE	lamp_ack_flag; 			// todo : move to other file	
LONG imsi_l1;
LONG imsi_l2;
LONG imsi_l3;
BYTE isInitializedMachine;

//------ laser 관련 var.
extern BYTE	laser_lmt_cnt_flag;			// from timer.c
extern BYTE isOnCanineBeam;				// from command.c
extern BYTE	lr_lcd_flag;				// from command.c
extern LONG	laser_on_time;					// from command.c
extern LONG	laser_on_time_5m;			// from timer.c


/*F**************************************************************************
* NAME:     InitializeBoard
* PURPOSE:	
*****************************************************************************/
void InitializeBoard(void)
{
	set_imask_ccr(1);		// <machine.h>
	set_ccr(0x80);			// Initialize CCR/Interrupt Disable <machine.h> 
		
	//--------------Port setting dir	
	P1.DDR = 0xED;			//1110	1101
	//P2.DDR = 0x34;			//0011	0100
	P2.DDR = 0x31;			//0011	0001
	P3.DDR = 0xDF;			//1101	1111		
		
	P6.DDR = 0xED;			//1110	1101
		
	PA.DDR = 0x07;			//0000	0111
	PD.DDR = 0x00;			//0000  0000
	PE.DDR = 0xFF;			//1111	1111
	PF.DDR = 0xBF;			//1011  1111
	PH.DDR = 0xFF;			//1111  1111
	PI.DDR = 0xFF;			//1111	1111
	PJ.DDR = 0xFF;			//1111	1111
	PK.DDR = 0xFF;			//1111	1111	
	
	PD.PCR.BYTE = 0xFF;		
	
/****************************************************************************/
/* H8SX/1544 Initialize routine */
/****************************************************************************/	
	/*** SYSTEM ***/
	SCKCR.WORD = 0x0011;	// Iphi,Pphi,Bphi = x8,x4,x4 
	
	SCKCR.BIT.ICK = 0;				/* Set system clock (5 MHz × 8 = 40 MHz) */
	SCKCR.BIT.PCK = 1;				/* Set peripheral clock (5 MHz × 4 = 20 MHz) */
	SCKCR.BIT.BCK = 1;				/* Set external bus clock (5 MHz × 4 = 20 MHz) */
	SBYCR.BIT.SSBY = 1;				/* Software standby mode */
	
	
	INTC.INTCR.BIT.INTM = 0;	 	
	
	//Module Stop Control Registers
	MSTP.CRA.BIT._DMAC		= 0;		// DMA controller
	MSTP.CRA.BIT._TPU		= 0;		// Timer pulse Unit
	MSTP.CRC.BIT._MCPWM		= 0;  		// Motor control PWM
	MSTP.CRC.BIT._RCAN01	= 0;		/* Cancel module-stop mode: RCAN */	
	
	//Standby Control Register
	
	//Output Port Enable(OPE)
	SBYCR.BIT.OPE = 0;	
	

	timer_init();
	serial_init();
	set_imask_ccr(0);			// <machine.h>
	ad_init(4);
	eeprom_init();			// ????
	rcan_init();
	set_imask_exr(0);

	PCPuts(" \r\n \r\n \r\n \r\n \r\n[rpm_pwon]\r\n");	// notify power on to PC
}

/*F**************************************************************************
* NAME:     ad_init
* PURPOSE:	
*****************************************************************************/
void ad_init(int ch)			// set ADC single mode
{
	MSTP.CRA.BIT._AD0 = 0;		// Takes AD out of Module Stop Mode 
	AD0.ADCSR.BIT.CH = ch;		// set input channel
	AD0.ADCSR.BIT.ADST = 1;
}

/*F**************************************************************************
* NAME:     ad_read
* PURPOSE:	
*****************************************************************************/
WORD ad_read(WORD ch)
{
	WORD ad_value;
	
	AD0.ADCSR.BIT.ADST = 0;		// ch 설정 전에 AD를 정지 해서 오류 방지
	AD0.ADCSR.BIT.CH = ch;		// AD 입력 받을 채널 설정
	AD0.ADCSR.BIT.ADST = 0;		// ch 설정 후에 AD를 정지 해서 오류 방지
	AD0.ADCSR.BIT.ADST = 1;		// AD 시작
	
	while(AD0.ADCSR.BIT.ADST){}	// AD 완료까지 대기(ADST는 완료 되면 자동으로 클리어)
	
	if(AD0.ADCSR.BIT.ADF)		// ADdone 플래그 확인 완료 되있으면
	{

		// ad_value = AD0.ADDRE;	// ADdone 플래그 확인 ad_value에 AD 4번채널 값 저장
								// ADDRA 레지스터는 AD채널 0번값을 저장 하는 레지스터
								// ADDRB 레지스터는 AD채널 1번값을 저장 하는 레지스터
								// ADDRC 레지스터는 AD채널 2번값을 저장 하는 레지스터
								// ADDRD 레지스터는 AD채널 3번값을 저장 하는 레지스터
								// ADDRE 레지스터는 AD채널 4번값을 저장 하는 레지스터
								// ADDRF 레지스터는 AD채널 5번값을 저장 하는 레지스터
								// ADDRG 레지스터는 AD채널 6번값을 저장 하는 레지스터
								// ADDRH 레지스터는 AD채널 7번값을 저장 하는 레지스터
								// 현재는 채널을 4번으로 고정 했지만, 추후 입력 받은 ch 변수로 전환할 필요 있음
		
		if(ch == 4)	ad_value = AD0.ADDRE;
		if(ch == 5) ad_value = AD0.ADDRF;
	}
	else	ad_value = ad_value;
	
	return(ad_value);
}

/*F**************************************************************************
* NAME:     InitializeSystem
* PURPOSE:	
*****************************************************************************/
void InitializeSystem(void)
{
	COLUMN_UP = HIGH;	// Active Low// 혹시나 올라가는것을 대비해서 안죽임 
	COLUMN_DN = HIGH;	// Active Low//  
}

/*F**************************************************************************
* NAME:     generator control function
	X_RAY_RDY		==> Active Low
	X_RAY_EXP		==> Active Low
	X_RAY_PULSE		==> Active HIGH
*****************************************************************************/
// todo relocate to timer.c
void gen_off(void)
{
	X_RAY_RDY = HIGH;	X_RAY_EXP = HIGH;	X_RAY_PULSE = LOW;
	SetGenPulse(OFF);
	SetGenPulseCnt(0);
	gen_preheat_time_flag = NO;	gen_preheat_time_cnt = 0;
}

void gen_rdy(void)
{
	X_RAY_RDY = LOW;	X_RAY_EXP = HIGH;	X_RAY_PULSE = LOW;
	SetGenPulse(OFF);
	gen_preheat_time_flag = YES; // Generator Preheat timer start
}

void gen_cont_exp(void)
{
	X_RAY_RDY = LOW;	X_RAY_EXP = LOW;	X_RAY_PULSE = LOW;
	SetGenPulse(OFF);
}

void gen_pulsed_exp(void)
{	
	X_RAY_RDY = LOW;	X_RAY_EXP = HIGH;	X_RAY_PULSE = LOW;
	SetGenPulse(ON);
	SetDetPulseCnt(0);			// for detector sync
}

/*F**************************************************************************
* NAME:     ct_pano_det control function
*****************************************************************************/
// todo : naming : DetCaptureOff()
// todo relocate to timer.c
void ct_pano_det_off(void)
{
	SetDetPulse(OFF);
	SetDetPulseCnt(0);	

	// RELAY2 = LOW;			// todo : check RELAY2 not used
	// CT_TRIG_PULSE = HIGH;	// todo: delete port is not used 
	VW_EXP_SYNC = HIGH;
}

// todo : naming : DetCaptureOn()
void ct_pano_det_capt(void)
{
	SetDetPulse(ON);
	// ct_pano_det_trig_flag = YES;	// 디텍터 트리거 출력 시작
}


/*F**************************************************************************
* NAME:     ring control function
*****************************************************************************/
void ring_on()	{	CPU_RING = HIGH;	ring_ack_flag = 1;	}
void ring_off()	{	CPU_RING = LOW;		ring_ack_flag = 2;	}

/*F**************************************************************************
* NAME:     laser control function
*****************************************************************************/
// todo : 입력매개변수로 main_mode를 받고 
// 그에 따른 laser on을 할 수 있는 방안 고려
void LaserOn(void)
{
	laser_lmt_cnt_flag = ON;	// 장시간 laser on 상태 방지를 위한 falg


	// 조기리턴 ceph 모드에서는 Laser 동작 안함
	if(main_mode == SCAN_CEPH_LA_MODE || main_mode == SCAN_CEPH_PA_MODE) 
		return;

	LaserVertical(ON);
	LaserHorizental(ON);
	LaserZ(ON);

	// 오송공장 요청으로 디버그 모드에서 견치빔 사용 가능하도록 함 
	if(debug_mode == YES)	
	{
		LaserCanine(ON);
	}

	// CT mode, Ceph mode에서는 canine 빔이 작동 안된다.
	if(main_mode == PANO_STAN_MODE || main_mode == PANO_BITEWING_MODE || 
		main_mode == PANO_TMJ1_MODE || main_mode == PANO_TMJ2_MODE) 
	{
		LaserCanine(ON);	// pano mode에서 별도로 제어
	}
	
	LCDPuts("[tml_lron] \r\n");

	if(lr_lcd_flag == YES)
	{	lr_lcd_flag = NO;	}
	else
	{	PCPuts("[rpm_lron] \r\n");	}
}

void LaserOff(void)
{
	laser_lmt_cnt_flag = OFF;
	
	LaserVertical(OFF);
	LaserHorizental(OFF);
	LaserZ(OFF);		// CAN com. 이기때문에 timer interrupt로 호출되면 무한루프에 빠짐.
	LaserCanine(OFF);

	LCDPuts("[tml_lrof] \r\n");

	if(lr_lcd_flag == YES)
	{	lr_lcd_flag = NO;	}
	else
	{	PCPuts("[rpm_lrof] \r\n");	}
}

void LaserVertical(BYTE sw)
{
	LR_VERTICAL = sw;		// LASER1
}
void LaserHorizental(BYTE sw)
{
	LR_HORIZENTAL = sw;		// LASER2
}

void LaserZ(BYTE sw)
{
	if(sw == OFF)		
	{
		rcan_puts(CO_MOTOR, CAN_LASER_CTRL, (LONG)99, dmy_msg); dly_10us(dly_t);
	}
	else if(sw == ON)
	{
		rcan_puts(CO_MOTOR, CAN_LASER_CTRL, (LONG)ON, dmy_msg); dly_10us(dly_t);
	}	
}
void LaserCanine(BYTE sw)
{
	LR_CANINE = sw;
}


/*F**************************************************************************
* NAME:     lamp control function
*****************************************************************************/
void lamp_off(void)		//평소엔 소등
{
	lamp_ack_flag = 1;				EXP_LAMP_RED		= LOW;	EXP_LAMP_GREEN			= LOW;
	SPREAD_EXP_LAMP_ENABLE = HIGH;	SPREAD_EXP_LAMP_RED	= LOW;	SPREAD_EXP_LAMP_GREEN	= LOW;
}
void rdy_lamp_on(void)	//녹색: ready 상태일 때 점등
{
	lamp_ack_flag = 2;				EXP_LAMP_RED		= LOW;	EXP_LAMP_GREEN			= HIGH;
	SPREAD_EXP_LAMP_ENABLE = LOW;	SPREAD_EXP_LAMP_RED	= LOW;	SPREAD_EXP_LAMP_GREEN	= HIGH;
}
void exp_lamp_on(void)	//황색: expose 상태일 때 점등
{
	lamp_ack_flag = 3;				EXP_LAMP_RED		= HIGH;	EXP_LAMP_GREEN			= HIGH;
	SPREAD_EXP_LAMP_ENABLE = LOW;	SPREAD_EXP_LAMP_RED	= HIGH;	SPREAD_EXP_LAMP_GREEN	= HIGH;
}
void err_lamp_on(void)	//적색: error 상태일 때 점등
{
	lamp_ack_flag = 4;				EXP_LAMP_RED		= HIGH;	EXP_LAMP_GREEN			= LOW;
	SPREAD_EXP_LAMP_ENABLE = LOW;	SPREAD_EXP_LAMP_RED	= HIGH;	SPREAD_EXP_LAMP_GREEN	= LOW;
}

/*F**************************************************************************
* NAME:     
* PURPOSE:	
*****************************************************************************/
void PrintFW_Version(void)
{	
	// todo : CAN board 버전받아서 표시할 수 있도록	

    PCPuts("\r\n***   Welcome HDX WILL EcoX   ***\r\n");
    PCPuts("EcoX-System Rev  = EX1_X1_2004010 \r\n");		// 허가 받은 펌웨어 버전 출력
    PCPuts("EcoX-Motor Rev   = MD0_V1_1904230 \r\n");
    PCPuts("EcoX-ADC Rev     = AD0_V1_2001230 \r\n"); 	// 실제버전 V0.5_1909 => 양산으로 통합 
    PCPuts("EcoX-MP3 Board Rev    = MP3_V0_1408070 \r\n");
    PCPuts("EcoX-Generator Rev    = W90_V1_1803190 \r\n");
    PCPuts("******************************************** \r\n\r\n");	// 허가 받은 펌웨어 버전 출력

    return;			
}
/*F**************************************************************************
* NAME:     

	0 ~ 99:		Byte	ex) 0, 2, 4, 6, 8,...
	100 ~ 299:	WORD	ex) 100, 105, 110, 115,...
	300 ~	 :	LONG	ex) 300, 310, 320, 330,...

*****************************************************************************/
// todo : naming : SetOption()???
// todo !!!!! : 옵션 변수들은 모두 BYTE형으로 정의할 것!!
// option에 대한 기능 주석을 이곳에 달아둘것!!!
// BYTE SetOptionFromFlash(BYTE addr, BYTE optionDefault, BYTE minRange, BYTE maxRange)
void Init_FlashValue(void)
{
	BYTE address;
	BYTE opt_Default;
	write_eeprom_BYTE(64, 0);		// temporary line : for setting  temporary


	sound_option 		= SetOptionFromFlash(0, 0, 0, 4);
	ceph_option 		= SetOptionFromFlash(2, 0, 0, 1);
	debug_mode 			= SetOptionFromFlash(4, 1, 0, 1);
	// gen_overheat_threshold 	= SetOptionFromFlash(6, 54, 0, 63);
	pano_ro_end_angle 	= SetOptionFromFlash(14, 2, 0, 3);
	// pano_nor_tbl_optn 	= SetOptionFromFlash(16, 7, 0, 8);
	// ceph_axis_end_position = SetOptionFromFlash(20, 1, 0, 1);
	ct_ro_end_angle 	= SetOptionFromFlash(22, 0, 0, 5);
	// door_check_option 	= SetOptionFromFlash(24, 0, 0, 1);
	co_option 			= SetOptionFromFlash(28, (BYTE)ONE_AXIS_CO, 0, 9);
	ac_option 			= SetOptionFromFlash(38, 3, 2, 3);
	gen_option 			= SetOptionFromFlash(40, 1, 0, 2);
	// pa_option 			= SetOptionFromFlash(42, (BYTE)LEAD_2_PA, 0, 2);
	// pano_exp_option 	= SetOptionFromFlash(44, 0, 1, 1);
	ct_ro_end_tm 		= SetOptionFromFlash(46, 0, 0, 120);	ct_ro_end_tm_5ms = ct_ro_end_tm * 200;
	// pano_ro_end_tm 		= SetOptionFromFlash(48, 0, 0, 120);	pano_ro_end_tm_5ms = pano_ro_end_tm * 200;
	// ct_det_optn 		= SetOptionFromFlash(52, (BYTE)TD_X_1313, 0, 3);
	// sc_det_optn 		= SetOptionFromFlash(54, (BYTE)TD_A, 0, 1);
	laser_on_time 		= SetOptionFromFlash(60, 90, 0, 90);		laser_on_time_5m = laser_on_time * 200;
	// ceph_capt_end_col_position = SetOptionFromFlash(62, 1, 0, 1);
	pa_axis_end_position = SetOptionFromFlash(64, 1, 0, 1);
	// test_mode_optn 		= SetOptionFromFlash(90, 0, 0, 1);
	
	gen_overheat_threshold = 63;
	ct_ro_end_tm = 0;
	// ac_option = 3;
	

	//---------	옵션에 따른 내부 변수값 정리

	//-------------------------	PA
	// todo : mPA 초기값 셋팅 함수는 motor.c에서 하도록!!
	gvPA.pos_dir 			= PA_POS_DIR;
	gvPA.neg_dir 			= PA_NEG_DIR; 
	gvPA.unit_0_1_pulse		= PA_0_1_MM_PULSE_2;
	gvPA.accel_step			= PA_ACCEL_PULSE_2;
	gvPA.start_speed		= PA_START_PULSE_2;
	gvPA.max_speed			= PA_MAX_PULSE_2;
	gvPA.p_max_pulse		= PA_P_MAX_PULSE_2;
	gvPA.n_max_pulse		= PA_N_MAX_PULSE_2;
	
	//-------
	gvPA.ct_offset_pos		= PA_CT_OFFSET;
	gvPA.pano_offset_pos	= PA_PANO_OFFSET;
	gvPA.ceph_offset_pos	= PA_CEPH_OFFSET;
	
	//-------------------------	RO( 과거 PAN_CTRL_RO option)
	gvRO.pos_dir			= RO_POS_DIR;
	gvRO.neg_dir			= RO_NEG_DIR;
	gvRO.unit_0_1_pulse		= RO_0_1_DEGREE_PULSE;
	gvRO.p_max_pulse		= RO_P_MAX_PULSE;
	gvRO.n_max_pulse		= RO_N_MAX_PULSE;
	
	// gvRO.accel_step		= ro_accel_step;
	// gvRO.start_speed		= ro_start_speed;
	// gvRO.max_speed		= ro_max_speed;
	
	// command_control.c 에 cmd_ctrl()의 ro_stop_flag를 업데이트 시킴
	// ro_stop_flag			= A1_stop_flag;
	// ro_org_complete_flag	= A1_org_complete_flag;

	gvRO.start_speed 		= RO_START_PULSE;
	gvRO.max_speed 			= RO_MAX_PULSE;
	gvRO.ct_offset_pos		= RO_CT_OFFSET;
	gvRO.pano_offset_pos	= RO_PANO_OFFSET;
	// gvRO.accel_step	= RO_ACCEL_PULSE;
	gvRO.ceph_offset_pos	= RO_CEPH_OFFSET;
	

	//-------------------------	CO
	gvCO.pos_dir			= CO_NEG_DIR;
	gvCO.neg_dir			= CO_POS_DIR;
	gvCO.unit_0_1_pulse		= CO_0_1_MM_PULSE_C;
	gvCO.unit_0_01_pulse	= CO_0_01_MM_PULSE_C;
	gvCO.accel_step			= CO_ACCEL_PULSE_C;
	gvCO.start_speed			= CO_START_PULSE_C;
	gvCO.max_speed			= CO_MAX_PULSE_C;
	
	gvCO.p_max_pulse 		= CO_P_MAX_PULSE_C;
	gvCO.n_max_pulse 		= CO_N_MAX_PULSE_C;
	
	gvCO.ct_offset_pos		= CO_CT_OFFSET;
	gvCO.pano_offset_pos	= CO_PANO_OFFSET;

	gvCO.scla_offset_pos	= CO_SCLA_OFFSET;
	gvCO.scpa_offset_pos	= CO_SCPA_OFFSET;

	
	//-------------------------	SD
	gvSD.accel_step			= SD_ACCEL_PULSE;
	gvSD.start_speed		= SD_START_PULSE;
	gvSD.max_speed			= SD_MAX_PULSE;
	
	gvSD.scla_offset_pos	= SD_SCLA_OFFSET;
	gvSD.scpa_offset_pos	= SD_SCPA_OFFSET;

	//------------------------- 4 axis collimator
	SetInitialAxisValue_HL();		
	SetInitialAxisValue_HR();		
	SetInitialAxisValue_VT();		
	SetInitialAxisValue_VB();		

	//------	org_complete_flag
	//-----	PA
	if(pa_option == NOT_EXIST)	gvPA.org_complete_flag = YES;
	else						gvPA.org_complete_flag = NO;
	
	//-----	RO
	gvRO.org_complete_flag = NO;
	
	//-----	CO
	// if(co_option == NOT_EXIST) 	mCO.org_complete_flag = YES;
	// else						mCO.org_complete_flag = NO;


	//-----	SD
	if(ceph_option == NONE_CEPH) 	gvSD.org_complete_flag = YES;
	else						gvSD.org_complete_flag = NO;

}

/*F**************************************************************************
* NAME:     
* PURPOSE:	
*****************************************************************************/
BYTE SetOptionFromFlash(BYTE addr, BYTE optionDefault, BYTE minRange, BYTE maxRange)
{
	BYTE 	read_byte;

	read_byte = 0;	read_byte = read_eeprom_BYTE(addr);
	if(minRange <= read_byte && read_byte <= maxRange)	return read_byte;
	return optionDefault;

}
/*===================================================================================*/
// option_print - 설정된 옵션의 내용을 출력하고 내부 변수값 설정
/*===================================================================================*/
// option 정보를 담는 구조체로 
// todo : option.system, optin.ceph, option.paAxis...
BYTE PrintOption(void)
{
	// static BYTE isCompleted = 1;

	PCPuts("\r\n//-----------------   Print options\r\n");

	switch(sound_option)
	{
	
		case 0:		PCPuts("Voice guidance	= None \r\n");			break;
		case 1:		PCPuts("Voice guidance	= KO : Korean \r\n");	break;
		case 2:		PCPuts("Voice guidance	= EN : English \r\n");	break;
		case 3:		PCPuts("Voice guidance	= ZH : Chinese \r\n");	break;
		case 4:		PCPuts("Voice guidance	= ES : Spanish \r\n");	break;
		default:	PCPuts("Undefined sound option!! \r\n");		break;
	}

	switch(ceph_option)
	{
		case NONE_CEPH:		PCPuts("Ceph option 	= None-Cephalo \r\n");	break;
		case SCAN_CEPH:		PCPuts("Ceph option 	= Scan-Cephalo \r\n");		break;
		default:	PCPuts("Undefined ceph option!! \r\n");			break;
	}

	switch(debug_mode)
	{
		case 0:		PCPuts("Debug mode  	= OFF \r\n");			break;
		case 1:		PCPuts("Debug mode  	= ON  \r\n");			break;
		default: 	PCPuts("Undefined option!! \r\n");				break;
	}

	switch(pano_ro_end_angle)
	{
		case 0:		PCPuts("Pano RO end angle = 0 degree \r\n");		break;
		case 1:		PCPuts("Pano RO end angle = 90 degree \r\n");		break;
		case 2:		PCPuts("Pano RO end angle = 180 degree \r\n");		break;
		case 3:		PCPuts("Pano RO end angle = None \r\n");			break;
		default: 	PCPuts("Undefined option!! \r\n");					break;
	}

	switch(ct_ro_end_angle)
	{
		case 0:		PCPuts("CT RO end angle 	= 0 degree \r\n");		break;
		case 1:		PCPuts("CT RO end angle 	= 90 degree \r\n");		break;
		case 2:		PCPuts("CT RO end angle 	= 180 degree \r\n");	break;
		case 3:		PCPuts("CT RO end angle 	= 270 degree \r\n");	break;
		case 4:		PCPuts("CT RO end angle 	= 360 degree \r\n");	break;
		case 5:		PCPuts("CT RO end angle 	= None \r\n");			break;
		default: 	PCPuts("Undefined option!! \r\n");					break;
	}

	PCPrintf("Laser on Time 	= %ld sec \r\n", laser_on_time);

	// todo : pa_axis_end_position, ecoX에서는 필요없는 옵션인듯 
	switch(pa_axis_end_position)
	{
		case 0:		PCPuts("PA end position 	= Not moving \r\n");	break;
		case 1:		PCPuts("PA end position 	= Moving \r\n");		break;
		default: 	PCPuts("Undefined option!! \r\n");					break;
	}

	// todo : ac option 에 대한 기능적 spec이 불명확함 ==> 기능 히스토리 확인 및 정리 필요
	switch(ac_option)
	{
		case 2:		PCPuts("Acturator option 	= Forward SMPS \r\n");	break;
		case 3:		PCPuts("Acturator option 	= Reverse SMPS \r\n");	break;
		default: 	PCPuts("Undefined option!! \r\n");					break;
	}

	switch(gen_option)
	{
		case 0:		PCPuts("Generator option 	= OFF \r\n");			break;
		case 1:		PCPuts("Generator option 	= ON \r\n");			break;
		default: 	PCPuts("Undefined option!! \r\n");					break;
	}


	switch(co_option)
	{
		case ONE_AXIS_CO:		PCPuts("CO option  	=  1-Axis CO \r\n");			break;
		case FOUR_AXIS_CO:		PCPuts("CO option  	=  4-Axis CO \r\n");			break;
		case NONE_AXIS_CO:		PCPuts("CO option  	=  None-Axis CO \r\n");			break;
		case AXIS_ALL:			PCPuts("CO option  	=  all-Axis CO \r\n");			break;
		default: 	PCPuts("Undefined option!! \r\n");					break;
	}


	// PCPuts("CT, Pano Detector 	= Vieworks VIVIX \r\n");	break;

	// switch(ceph_option)
	// {
	// 	case 0:		break;		// none-ceph 이면 표시하지 않는다. 
	// 	case 1:		PCPuts("Ceph Detector 	= Teledyne Dalsa Xineos \r\n");	break;
	// }
	// PCPuts("Ceph Detector 	= Teledyne Dalsa Xineos \r\n");
	// PCPuts("X-ray Generator 	= WDG90 \r\n");


	// switch(option)
	// {
	// 	case 0:		PCPuts(" \r\n");		break;
	// 	case 1:		PCPuts(" \r\n");		break;
	// 	case 2:		PCPuts(" \r\n");		break;
	// 	default: 	PCPuts("Undefined option!! \r\n");		break;
	// }




	// PCPuts("pa axis end position	= Not Moving \r\n");
	// PCPuts("gen option		= PAN_CTRL Rev 1.08 \r\n");

	PCPuts("//---------------------------\r\n");
	// return isCompleted;
}

/*F**************************************************************************
* NAME:     
* PURPOSE:	
*****************************************************************************/
//----- 4 axis (HL, HR, VT, VB)
void SetInitialAxisValue_HL(void)
{
	g_HL.pos_dir 			= HL_POS_DIR;
	g_HL.unit_0_1_pulse		= HL_0_1_MM_PULSE;
	g_HL.unit_0_01_pulse	= HL_0_01_MM_PULSE;
	g_HL.p_max_pulse 		= HL_P_MAX_PULSE;
	g_HL.n_max_pulse 		= HL_N_MAX_PULSE;
	g_HL.accel_step 		= HL_ACCEL_PULSE;
	g_HL.start_speed 		= HL_START_PULSE;
	g_HL.max_speed 			= HL_MAX_PULSE;
	g_HL.axisId 			= HL_MOTOR;
	g_HL.ct_offset_pos		= HL_CT_OFFSET;
	g_HL.scla_offset_pos	= HL_SCLA_OFFSET;
	g_HL.scpa_offset_pos	= HL_SCPA_OFFSET;
	g_HL.pano_offset_pos	= HL_PANO_OFFSET;
	strcpy(g_HL.axisName, "hl");
}

void SetInitialAxisValue_HR(void)
{
	g_HR.pos_dir 			= HR_POS_DIR;
	g_HR.unit_0_1_pulse		= HR_0_1_MM_PULSE;
	g_HR.unit_0_01_pulse	= HR_0_01_MM_PULSE;
	g_HR.p_max_pulse 		= HR_P_MAX_PULSE;
	g_HR.n_max_pulse 		= HR_N_MAX_PULSE;
	g_HR.accel_step 		= HR_ACCEL_PULSE;
	g_HR.start_speed 		= HR_START_PULSE;
	g_HR.max_speed 			= HR_MAX_PULSE;
	g_HR.axisId 			= HR_MOTOR;
	g_HR.ct_offset_pos		= HR_CT_OFFSET;
	g_HR.scla_offset_pos	= HR_SCLA_OFFSET;
	g_HR.scpa_offset_pos	= HR_SCPA_OFFSET;
	g_HR.pano_offset_pos	= HR_PANO_OFFSET;
	strcpy(g_HR.axisName, "hr");
}

void SetInitialAxisValue_VT(void)
{
	g_VT.pos_dir 			= VT_POS_DIR;
	g_VT.unit_0_1_pulse		= VT_0_1_MM_PULSE;
	g_VT.unit_0_01_pulse	= VT_0_01_MM_PULSE;
	g_VT.p_max_pulse 		= VT_P_MAX_PULSE;
	g_VT.n_max_pulse 		= VT_N_MAX_PULSE;
	g_VT.accel_step 		= VT_ACCEL_PULSE;
	g_VT.start_speed 		= VT_START_PULSE;
	g_VT.max_speed 			= VT_MAX_PULSE;
	g_VT.axisId 			= VT_MOTOR;
	g_VT.ct_offset_pos		= VT_CT_OFFSET;
	g_VT.scla_offset_pos	= VT_SCLA_OFFSET;
	g_VT.scpa_offset_pos	= VT_SCPA_OFFSET;
	g_VT.pano_offset_pos	= VT_PANO_OFFSET;
	strcpy(g_VT.axisName, "vt");
}

void SetInitialAxisValue_VB(void)
{
	g_VB.pos_dir 			= VB_POS_DIR;
	g_VB.unit_0_1_pulse		= VB_0_1_MM_PULSE;
	g_VB.unit_0_01_pulse	= VB_0_01_MM_PULSE;
	g_VB.p_max_pulse 		= VB_P_MAX_PULSE;
	g_VB.n_max_pulse 		= VB_N_MAX_PULSE;
	g_VB.accel_step 		= VB_ACCEL_PULSE;
	g_VB.start_speed 		= VB_START_PULSE;
	g_VB.max_speed 			= VB_MAX_PULSE;
	g_VB.axisId 			= VB_MOTOR;
	g_VB.ct_offset_pos		= VB_CT_OFFSET;
	g_VB.scla_offset_pos	= VB_SCLA_OFFSET;
	g_VB.scpa_offset_pos	= VB_SCPA_OFFSET;
	g_VB.pano_offset_pos	= VB_PANO_OFFSET;
	strcpy(g_VB.axisName, "vb");
}

// todo naming : Set_CANMotorInitialValue()
void Set_CANMotorAxisValue(void)
{
	gAxisCO.pos_dir			= CO_NEG_DIR;
	gAxisCO.unit_0_1_pulse	= CO_0_1_MM_PULSE_C;
	gAxisCO.p_max_pulse		= CO_P_MAX_PULSE_C;
	gAxisCO.n_max_pulse		= CO_N_MAX_PULSE_C;
	gAxisCO.accel_step		= CO_ACCEL_PULSE_C;
	gAxisCO.start_speed		= CO_START_PULSE_C;
	gAxisCO.max_speed		= CO_MAX_PULSE_C;

	gAxisSD.pos_dir			= SD_POS_DIR;
	gAxisSD.unit_0_1_pulse	= SD_0_1_MM_PULSE;
	gAxisSD.p_max_pulse		= SD_P_MAX_PULSE;
	gAxisSD.n_max_pulse		= SD_N_MAX_PULSE;
	gAxisSD.accel_step		= SD_ACCEL_PULSE;
	gAxisSD.start_speed		= SD_START_PULSE;
	gAxisSD.max_speed		= SD_MAX_PULSE;	
}
/*F**************************************************************************
* NAME:     
* PURPOSE:	
*****************************************************************************/
// todo relocate : command_canmsg.c
void InitializeCANMotor(void)
{
	Set_CANMotorAxisValue();

	// rcan과 serial의 interrupt 중첩이 생기게 되면, serial data가 소실 되는 듯하다. 
	// rcan data의 송수신은 제대로 이루어지는 것을 확인!!
	// 시리얼 송수신을 위해 rcan 송신후 delay를 주어 serial interrupt 실행(?) 시간 확보 
	// todo : CAN Rx ack flag등을 이용하여 serial 출력은 CAN통신 이후에 하는 것도 하나의 방법

	// 1 axis collimator
	if(co_option == ONE_AXIS_CO) {
		rcan_puts(CO_MOTOR, CAN_AXIS_POS_DIR, gAxisCO.pos_dir, dmy_msg); 				dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, gAxisCO.unit_0_1_pulse, dmy_msg);	dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_P_MAX_PULSE, gAxisCO.p_max_pulse, dmy_msg);		dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_N_MAX_PULSE, gAxisCO.n_max_pulse, dmy_msg);		dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_ACCEL_STEP, gAxisCO.accel_step, dmy_msg);			dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_START_SPEED, gAxisCO.start_speed, dmy_msg);		dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_MAX_SPEED, gAxisCO.max_speed, dmy_msg);			dly_10us(70);
	}

	// 4 axis collimator
	else if(co_option == FOUR_AXIS_CO) {
		rcan_puts(HL_MOTOR, CAN_AXIS_POS_DIR, (LONG)HL_POS_DIR, dmy_msg); 				dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, (LONG)HL_0_1_MM_PULSE, dmy_msg);	dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_P_MAX_PULSE, (LONG)HL_P_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_N_MAX_PULSE, (LONG)HL_N_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_ACCEL_STEP, (LONG)HL_ACCEL_PULSE, dmy_msg);			dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_START_SPEED, (LONG)HL_START_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_MAX_SPEED, (LONG)HL_MAX_PULSE, dmy_msg);			dly_10us(70);

		rcan_puts(HR_MOTOR, CAN_AXIS_POS_DIR, (LONG)HR_POS_DIR, dmy_msg); 				dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, (LONG)HR_0_1_MM_PULSE, dmy_msg);	dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_P_MAX_PULSE, (LONG)HR_P_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_N_MAX_PULSE, (LONG)HR_N_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_ACCEL_STEP, (LONG)HR_ACCEL_PULSE, dmy_msg);			dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_START_SPEED, (LONG)HR_START_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_MAX_SPEED, (LONG)HR_MAX_PULSE, dmy_msg);			dly_10us(70);

		rcan_puts(VT_MOTOR, CAN_AXIS_POS_DIR, (LONG)VT_POS_DIR, dmy_msg); 				dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, (LONG)VT_0_1_MM_PULSE, dmy_msg);	dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_P_MAX_PULSE, (LONG)VT_P_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_N_MAX_PULSE, (LONG)VT_N_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_ACCEL_STEP, (LONG)VT_ACCEL_PULSE, dmy_msg);			dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_START_SPEED, (LONG)VT_START_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_MAX_SPEED, (LONG)VT_MAX_PULSE, dmy_msg);			dly_10us(70);

		rcan_puts(VB_MOTOR, CAN_AXIS_POS_DIR, (LONG)VB_POS_DIR, dmy_msg); 				dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, (LONG)VB_0_1_MM_PULSE, dmy_msg);	dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_P_MAX_PULSE, (LONG)VB_P_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_N_MAX_PULSE, (LONG)VB_N_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_ACCEL_STEP, (LONG)VB_ACCEL_PULSE, dmy_msg);			dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_START_SPEED, (LONG)VB_START_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_MAX_SPEED, (LONG)VB_MAX_PULSE, dmy_msg);			dly_10us(70);
	}

	else if(co_option == AXIS_ALL) {

		rcan_puts(CO_MOTOR, CAN_AXIS_POS_DIR, gAxisCO.pos_dir, dmy_msg); 				dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, gAxisCO.unit_0_1_pulse, dmy_msg);	dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_P_MAX_PULSE, gAxisCO.p_max_pulse, dmy_msg);		dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_N_MAX_PULSE, gAxisCO.n_max_pulse, dmy_msg);		dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_ACCEL_STEP, gAxisCO.accel_step, dmy_msg);			dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_START_SPEED, gAxisCO.start_speed, dmy_msg);		dly_10us(70);
		rcan_puts(CO_MOTOR, CAN_AXIS_MAX_SPEED, gAxisCO.max_speed, dmy_msg);			dly_10us(70);
		
		rcan_puts(HL_MOTOR, CAN_AXIS_POS_DIR, (LONG)HL_POS_DIR, dmy_msg); 				dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, (LONG)HL_0_1_MM_PULSE, dmy_msg);	dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_P_MAX_PULSE, (LONG)HL_P_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_N_MAX_PULSE, (LONG)HL_N_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_ACCEL_STEP, (LONG)HL_ACCEL_PULSE, dmy_msg);			dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_START_SPEED, (LONG)HL_START_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HL_MOTOR, CAN_AXIS_MAX_SPEED, (LONG)HL_MAX_PULSE, dmy_msg);			dly_10us(70);

		rcan_puts(HR_MOTOR, CAN_AXIS_POS_DIR, (LONG)HR_POS_DIR, dmy_msg); 				dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, (LONG)HR_0_1_MM_PULSE, dmy_msg);	dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_P_MAX_PULSE, (LONG)HR_P_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_N_MAX_PULSE, (LONG)HR_N_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_ACCEL_STEP, (LONG)HR_ACCEL_PULSE, dmy_msg);			dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_START_SPEED, (LONG)HR_START_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(HR_MOTOR, CAN_AXIS_MAX_SPEED, (LONG)HR_MAX_PULSE, dmy_msg);			dly_10us(70);

		rcan_puts(VT_MOTOR, CAN_AXIS_POS_DIR, (LONG)VT_POS_DIR, dmy_msg); 				dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, (LONG)VT_0_1_MM_PULSE, dmy_msg);	dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_P_MAX_PULSE, (LONG)VT_P_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_N_MAX_PULSE, (LONG)VT_N_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_ACCEL_STEP, (LONG)VT_ACCEL_PULSE, dmy_msg);			dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_START_SPEED, (LONG)VT_START_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VT_MOTOR, CAN_AXIS_MAX_SPEED, (LONG)VT_MAX_PULSE, dmy_msg);			dly_10us(70);

		rcan_puts(VB_MOTOR, CAN_AXIS_POS_DIR, (LONG)VB_POS_DIR, dmy_msg); 				dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, (LONG)VB_0_1_MM_PULSE, dmy_msg);	dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_P_MAX_PULSE, (LONG)VB_P_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_N_MAX_PULSE, (LONG)VB_N_MAX_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_ACCEL_STEP, (LONG)VB_ACCEL_PULSE, dmy_msg);			dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_START_SPEED, (LONG)VB_START_PULSE, dmy_msg);		dly_10us(70);
		rcan_puts(VB_MOTOR, CAN_AXIS_MAX_SPEED, (LONG)VB_MAX_PULSE, dmy_msg);			dly_10us(70);
	}





	// scan ceph
	if(ceph_option == SCAN_CEPH) {
		rcan_puts(SD_MOTOR, CAN_AXIS_POS_DIR, gAxisSD.pos_dir, dmy_msg); 				dly_10us(70);
		rcan_puts(SD_MOTOR, CAN_AXIS_0_1_UNIT_PULSE, gAxisSD.unit_0_1_pulse, dmy_msg);	dly_10us(70);
		rcan_puts(SD_MOTOR, CAN_AXIS_P_MAX_PULSE, gAxisSD.p_max_pulse, dmy_msg);		dly_10us(70);
		rcan_puts(SD_MOTOR, CAN_AXIS_N_MAX_PULSE, gAxisSD.n_max_pulse, dmy_msg);		dly_10us(70);
		rcan_puts(SD_MOTOR, CAN_AXIS_ACCEL_STEP, gAxisSD.accel_step, dmy_msg);			dly_10us(70);
		rcan_puts(SD_MOTOR, CAN_AXIS_START_SPEED, gAxisSD.start_speed, dmy_msg);		dly_10us(70);
		rcan_puts(SD_MOTOR, CAN_AXIS_MAX_SPEED, gAxisSD.max_speed, dmy_msg);			dly_10us(70);
	}

	while(CANBufGetSize() != 0) 	// 위 명령(송신)에 대해서 수신된 CANBuf 비워질때까지 
	{
		ProcessCANAck();
	}

	UartPrintf("//-----  CANMotor Axis is Set!!\r\n\r\n");
}

/*F**************************************************************************
* NAME:     
* PURPOSE:	
*****************************************************************************/
void InitializeMachine()
{
	static ULONG lastTime;
	const BYTE isNotInitialized = 0;
	static BYTE isDonePreheat;
	
	lastTime = TimeNow();

	first_run = 1;
	resetOrigin_PA = ON;
	resetOrigin_RO = OFF;

	while(first_run)
	{
		GenPreheatTime10sec(&isDonePreheat);

		FixOrigin_CANMotor();
		
		if(resetOrigin_PA == ON)	PA_SetOrigin(NO);
		if(resetOrigin_RO == ON)	RO_SetOrigin(NO);

		
		// CheckStall_RO_PA();
		if(isDonePreheat && CheckAllOrigin())
		{		
			first_run = NO;
			isInitializedMachine = YES;
		} 
	
	
		ProcessCmd();
		// InitCmd();				// 메인 함수 실행되기 전에도 EEPROM과 관련한 명령은 받아들여야 옵션별 설정이 가능
		
		ProcessCANAck();
	}

	UartPrintf("//------------------------------------ Initialize Machine!\r\n");
}

/*F**************************************************************************
* NAME:     
* PURPOSE:	
*****************************************************************************/
void CheckStall_RO_PA(void)
{
	if(RO_CheckStall() ==  YES)
	{
		PCPuts("[rpm_stall_RO] \r\n");
		// RO_MotorStop();
	}
	
	if(PA_CheckStall() == YES)
	{
		PCPuts("[rpm_stall_PA] \r\n");
		// PA_MotorStop();
	}
}

/*F**************************************************************************
* NAME:     
* PURPOSE:	
*****************************************************************************/
// Gen과 serial 통신을 하고 응답에 따라 처리가 달라짐. 
// 통신 영향 처리, 링버퍼 처리 control 필요
// todo GenBuf clear
void InitializeGen(void)
{
	static BYTE 	genStep = STEP1;
	static BYTE  	isCompleted;
	static LONG 	lastTime;

	g_IsInitializingGen = YES;

	while(g_IsInitializingGen)
	{
		switch(genStep)
		{
			case STEP1:		
				// gen_option이 off 되면 이 상태 함수 완료
				if(gen_option == OFF) 	
				{
					genStep = STEP5; 
					break;
				}
				
				Ctrl(TUBE_TEMP_REQ, (LONG)0);
				Ctrl(GEN_TRIG_TIME_START, (LONG) 700);
				Ctrl(GEN_TRIG_TIME_END, (LONG) 2000);

				lastTime = TimeNow();		
				genStep = STEP2;
				break;

			case STEP2:
				if(TimePassed(lastTime) > ONE_SECOND)	
				{
					gen_ack("ecom");			// Gen Communication Error
					break;	
				}

				if(gen_ack_flag == 17)			// Ack of TUBE_TEMP_REQ
				{
					gen_ack_flag = 0;						// gen_ack_flag 초기화???
					Ctrl(INVERTER_BUZZER_ON, (LONG)0);	// Generator Inverter Board Buzzer On Mode when X-Ray Exposed

					lastTime = TimeNow();
					genStep = STEP3;
				}
				break;
			
			case STEP3:
				if(TimePassed(lastTime) > ONE_SECOND)	
				{
					gen_ack("ecom");			// Gen Communication Error
					break;	
				}

				if(gen_ack_flag == 15)			// Ack of INVERTER_BUZZER_ON
				{
					gen_ack_flag = 0;
					genStep = STEP4;
				}
				break;
			
			case STEP4:
				if( isCompleted = CalibrateGen())   
				{
					genStep = STEP5;
				}
				break;

			case STEP5:
					genStep = STEP0;
					g_IsInitializingGen = NO; 
				break;
		}

		if(gen_err_flag)		// gen_err가 발생하면 현재 while 루프를 빠져나가 처리된다.
		{
			PCPuts("Error Generator\r\n");
			PA_MotorStop();	RO_MotorStop();	
			g_IsInitializingGen = NO;
		}

		ProcessGenAck();
	}

	while(gen_err_flag)
	{
		err_lamp_on();
		SoundPlay(SOUND_STOP);
				
		ProcessCANAck();		// CANmotor Data 처리
		InitCmd();
	}

	return;
}

/*===================================================================================*/
// gen_cal - 설정된 옵션의 내용을 출력하고 내부 변수값 설정
//
//	1 	kV	⇒	27.7 		1.0  mA	⇒	129 
//
//	50 	kV	⇒	1385 		4.0  mA	⇒	516 
//	55 	kV	⇒	1524 		4.5  mA	⇒	581 
//	60 	kV	⇒	1662 		5.0  mA	⇒	645 
//	65 	kV	⇒	1801 		5.5  mA	⇒	710 
//	70 	kV	⇒	1939 		6.0  mA	⇒	774 
//	75 	kV	⇒	2078 		6.5  mA	⇒	839 
//	80 	kV	⇒	2216 		7.0  mA	⇒	903 
//	85 	kV	⇒	2355 		7.5  mA	⇒	968 
//	90 	kV	⇒	2493 		8.0  mA	⇒	1032 
//	95 	kV	⇒	2632 		8.5  mA	⇒	1097 
//	100 kV	⇒	2770 		9.0  mA	⇒	1161 
//	105 kV	⇒	2909 		9.5	 mA	⇒	1226 
//	110 kV	⇒	3047 		10.0 mA	⇒	1290 
//
//	27.7 * 100 kV	= 2770  => "(XV100)(DV2770)"
//	129 * 10 mA		= 1290  => "(XA100)(DA2770)"
/*===================================================================================*/
// generator calibration
// ??? 어떤 기능을 위해 사용되는가? calibration이 되는가?
// ??? canmsg_timeover_flag는 무슨 의미인가? 캔통신과 gen_cal()는 어떤 연관??
// gen_cal 함수에서만 쓰임.
// todo : 함수를 generator(system.c)관련 쪽으로 옮겨야..


BYTE CalibrateGen(void)
{
	static BYTE cali_Step = STEP1;
	static BYTE	temp_val = 0;
   	static BYTE	temp_ADD = 0;
   	static WORD	temp_cal = 0;
   	WORD read_word;
   	char *string;
   	static BYTE isCompleted = 1;


	switch(cali_Step)
	{
		char str[19] = {0};
		
		case STEP1:
			PCPuts("\r\n// Generator Calibration Parameters\r\n");
					
			temp_val = 50;		// 최소 관전압 50 kV
			temp_ADD = 100;		// 최소 관전압에 대한 캘리브레이션 DA 값이 저장된 EEPROM의 주소
			temp_cal = 1385;	// 최소 관전압에 대한 캘리브레이션 값
			
			cali_Step = STEP3;
		break;
				
		case STEP2:		
			temp_val = temp_val + 5;	// 50 + 5 = 55
			temp_ADD = temp_ADD + 5;	// 100 + 5 = 105
			switch(temp_val)
			{
				case 55:	temp_cal = 1524;	break;
				case 60:	temp_cal = 1662;	break;
				case 65:	temp_cal = 1801;	break;
				case 70:	temp_cal = 1939;	break;
				case 75:	temp_cal = 2078;	break;
				case 80:	temp_cal = 2216;	break;
				case 85:	temp_cal = 2355;	break;
				case 90:	temp_cal = 2493;	break;
				default:	PCPuts("\r\n Warning! Unknown DV value! \r\n");	break;
			}		
			cali_Step = STEP3;
		break;
		
		case STEP3:     // generator kv 설정
			read_word = 0;	read_word = read_eeprom_WORD(temp_ADD);				
			if( (read_word == 0) || (read_word == -1) )		PCPuts("Default	");

		// UartPrintf("==> kv : %03ld\r\n", (LONG)temp_val);
			GenPrintf("(SETV%04ld)",(LONG)temp_val);	// 입력한 kV값에 대하여
			cali_Step = STEP4;
		break;

		case STEP4:     // generator kv 응답 확인
			if(gen_ack_flag == 1)
			{
				gen_ack_flag = 0;	
				cali_Step = STEP5;
			}
		break;

		case STEP5:     // generator dv 설정
			read_word = 0;	read_word = read_eeprom_WORD(temp_ADD);
		// UartPrintf("gen dv from eeprom : %d\r\n", read_word);
			if( (read_word == 0) || (read_word == -1) )
			{
				GenPrintf("(SDVV%04ld)",(LONG)temp_cal);	
				write_eeprom_WORD(temp_ADD, temp_cal);
			}
			else
			{
				GenPrintf("(SDVV%04ld)",(LONG)read_word);
			}
			cali_Step = STEP6;
		break;

		case STEP6:     // generator dv 응답 확인
			if(gen_ack_flag == 12)	// 제네레이터에서 캘리브레이션 값 입력에 대한 응답이 오면 다음 값들을 계속 입력
			{
				gen_ack_flag = 0;	

				if(temp_val < 90)	{									cali_Step = STEP2;	}	// kV 최대값이 아니면 다음 값들을 계속 입력
				else				{	temp_val = 30; temp_ADD = 155;	cali_Step = STEP7;	}	// kV 최대값이면 mA 캘리브레이션 값 입력하러 이동
			}
		break;

		case STEP7:
			// temp_val = temp_val + 5;	// 35 + 5 = 40
			// temp_ADD = temp_ADD + 5;	// 160 + 5 = 165

			temp_val = temp_val + 10;	// 35 + 5 = 40
			temp_ADD = temp_ADD + 10;	// 160 + 5 = 165
		// UartPrintf("step7-1 : temp_val : %d, add : %d \r\n", temp_val, temp_ADD);
			switch(temp_val)
			{
				case 40:	temp_cal = 516;		break;	
				case 45:	temp_cal = 581;		break;
				case 50:	temp_cal = 645;		break;
				case 55:	temp_cal = 710;		break;
				case 60:	temp_cal = 774;		break;
				case 65:	temp_cal = 839;		break;
				case 70:	temp_cal = 903;		break;
				case 75:	temp_cal = 968;		break;
				case 80:	temp_cal = 1032;	break;
				case 85:	temp_cal = 1097;	break;
				case 90:	temp_cal = 1161;	break;
				case 95:	temp_cal = 1226;	break;
				case 100:	temp_cal = 1290;	break;
				default:	PCPuts("\r\n Warning! Unknown DA value! \r\n");	break;
			}
			cali_Step = STEP8;
		break;

		case STEP8:     // generator ma 설정
			read_word = 0;	read_word = read_eeprom_WORD(temp_ADD);
			if( (read_word == 0) || (read_word == -1) )		PCPuts("Default	");

		// UartPrintf("==> ma : %03ld\r\n", (LONG)temp_val);
			GenPrintf("(SETA%04ld)",(LONG)temp_val);	// 입력한 mA값에 대하여

			cali_Step = STEP9;
		break;

		case STEP9:     // generator ma 응답 확인
			if(gen_ack_flag == 2)
			{
				gen_ack_flag = 0;	

				cali_Step = STEP10;
			}
		break;

		case STEP10:    // generator da 설정
			read_word = 0;	read_word = read_eeprom_WORD(temp_ADD);
		// UartPrintf("gen da from eeprom : %d\r\n", read_word);
			if( (read_word == 0) || (read_word == -1) )	
			{
				GenPrintf("(SDVA%04ld)",(LONG)temp_cal);	
				write_eeprom_WORD(temp_ADD, temp_cal);
			}
			else
			{
				GenPrintf("(SDVA%04ld)",(LONG)read_word);
			}
			cali_Step = STEP11;	
		break;

		case STEP11:    // generator da 응답 확인
			if(gen_ack_flag == 11)	// 제네레이터에서 캘리브레이션 값 입력에 대한 응답이 오면 다음 값들을 계속 입력
			{
				gen_ack_flag = 0;	
					
				if(temp_val < 100)	cali_Step = STEP7;	// mA 최대값이 아니면 다음 값들을 계속 입력
				else				cali_Step = STEP12;	// mA 최대값이면 종료
			}
		break;

		case STEP12:		
			PCPuts("--------------------------------------// \r\n\r\n\r\n");
			cali_Step = STEP0;
			return isCompleted;
		break;
		
		default:	break;
	}
	return 0;
}


/*F**************************************************************************
* NAME:     
* PURPOSE:	
*****************************************************************************/
// CANmotor 2개이면 전력분사을 위해 스텝별로 할 필요가 없을 듯
void FixOrigin_CANMotor(void)
{
	static BYTE	originStep = STEP1;
	static ULONG lastTime;

	switch(originStep)
	{
		case STEP0:
			originStep = STEP0;
			break;

		case STEP1:
			lastTime = TimeNow();
			originStep = STEP2;
			break;

		case STEP2:
			if(TimePassed(lastTime) > ONE_SECOND)
			{
				if(co_option == ONE_AXIS_CO) {
					if(!gvCO.org_complete_flag)		Ctrl(CO_MOVE, (LONG)0);
				}

				if(co_option == FOUR_AXIS_CO) {					
					if(!g_VT.org_complete_flag)		Ctrl(VT_MOVE, (LONG)0);
					if(!g_HL.org_complete_flag)		Ctrl(HL_MOVE, (LONG)0);
					if(!g_HR.org_complete_flag)		Ctrl(HR_MOVE, (LONG)0);
					if(!g_VB.org_complete_flag)		Ctrl(VB_MOVE, (LONG)0);
				}

				if(co_option == AXIS_ALL) {	

					if(!gvCO.org_complete_flag)		Ctrl(CO_MOVE, (LONG)0);
					
					if(!g_VT.org_complete_flag)		Ctrl(VT_MOVE, (LONG)0);
					if(!g_HL.org_complete_flag)		Ctrl(HL_MOVE, (LONG)0);
					if(!g_HR.org_complete_flag)		Ctrl(HR_MOVE, (LONG)0);
					if(!g_VB.org_complete_flag)		Ctrl(VB_MOVE, (LONG)0);
				}

				

				if(ceph_option == SCAN_CEPH) {
					if(!gvSD.org_complete_flag)		Ctrl(SD_MOVE, (LONG)0);
				}
				
				originStep = STEP3;				
			}
			break;

		case STEP3:
			originStep = STEP0;
			break;
		
		default:	break;
	}

	return;
}


BYTE IsCompletedAllOrigin(void)
{
	// PA, RO 상태 플래그 업데이트 
	PA_UpdateState();
	RO_UpdateState();

	// PA, RO 원점 안잡히면 false 잡히면 다음단계
	if( !gvPA.org_complete_flag || !gvRO.org_complete_flag)
		return false;

	// co_option에 따라 CO 원점 안잡히면 false, 잡히면 다음단계(co-)
	if(co_option == ONE_AXIS_CO) {
		if( !gvCO.org_complete_flag )
			return false;
	}
	if(co_option == FOUR_AXIS_CO) {
		if( !g_HL.org_complete_flag ||
			!g_HR.org_complete_flag ||
		 	!g_VT.org_complete_flag ||
		 	!g_VB.org_complete_flag
		 	)
		 	return false;
	}

	if(co_option == AXIS_ALL) {
		if( !gvCO.org_complete_flag )
			return false;
			
		if( !g_HL.org_complete_flag ||
			!g_HR.org_complete_flag ||
		 	!g_VT.org_complete_flag ||
		 	!g_VB.org_complete_flag
		 	)
		 	return false;
	}

	// ceph_option에 따라 SD 원점 안잡히면 false,  잡히면 다음단계
	if(ceph_option == SCAN_CEPH) {
		if(!gvSD.org_complete_flag)
			return false;
	}

	// 이 단계까지 오면 true 리턴
	return true;

	// return (gvPA.org_complete_flag) && 
	// 	   (gvRO.org_complete_flag) && 
	// 	   (gvCO.org_complete_flag) && 
	// 	   (gvSD.org_complete_flag);
}

BYTE CheckAllOrigin(void)
{
	static BYTE isChecked = 0;

	if( isChecked )	 return;

	if(IsCompletedAllOrigin())
	{
		// UartPrintf("==> Fixing Origin of axis are completed!!\r\n");
		isChecked = 1;
		return 1;
	}
}

void GenPreheatTime10sec(BYTE *isDonePreheat) 
{
	static BYTE preheatStep = STEP1;
	static ULONG lastTime;

	switch(preheatStep) {
		case STEP0:
			preheatStep = STEP0;
			break;

		case STEP1:
			gen_rdy();	
			rdy_lamp_on();
			lastTime = TimeNow();
			preheatStep = STEP2;
			break;

		case STEP2:
			if(TimePassed(lastTime) > ONE_SECOND * 10) {
				gen_off();
				lamp_off();
				*isDonePreheat = YES;
				preheatStep = STEP0;			
			}
			break;
		
		default:	break;
	}
}
