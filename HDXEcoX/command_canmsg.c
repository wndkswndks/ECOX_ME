/*C***************************************************************************
* $RCSfile: cansmsg.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the cansmsg source file
*----------------------------------------------------------------------------
* RELEASE:      $Name: cansmsg     
* Edited  By    $Id: sslim
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include "Includes.h"

/*_____ D E C L A R A T I O N ______________________________________________*/


/*_____ D E F I N E ________________________________________________________*/

/*_____ F U C T I O N ______________________________________________________*/
void ProcessCANAck(void);

void CANAck_AXIS_POS_DIR(CANData_t);        
void CANAck_AXIS_0_1_UNIT_PULSE(CANData_t);
void CANAck_AXIS_P_MAX_PULSE(CANData_t);    
void CANAck_AXIS_N_MAX_PULSE(CANData_t);    
void CANAck_AXIS_ACCEL_STEP(CANData_t);     
void CANAck_AXIS_START_SPEED(CANData_t);    
void CANAck_AXIS_MAX_SPEED(CANData_t);      
void CANAck_AXIS_MOVE_CONST(CANData_t); 
void CANAck_AXIS_MOVE(CANData_t);           
void CANAck_AXIS_SLOW_STOP(CANData_t);      
void CANAck_AXIS_CNT(CANData_t);            
void CANAck_AXIS_ERROR(CANData_t);          

void CANAck_FW_REV_NUM(CANData_t); 
void CANAck_LASER_CTRL(CANData_t);
void CANAck_CAN_ADC_CTRL(CANData_t);  

void Set_Stop_Flag (LONG);
int ReadCanAdc_RO(WORD*);
void CanAxisMove(LONG, varMotor_t*);
void CanAxisMoveConst(LONG, varMotor_t*);


/*_____ V A R I A B L E ____________________________________________________*/
LONG    dmy_val = 99;
WORD    dmy_msg = 99;
WORD    dly_t   = 70;

axisCANMotor_t gAxisCO;
axisCANMotor_t gAxisSD;

//------ pano 관련 var.

//------- can com. error
char    errstr[50][30] = { {0,}, {0,}, };
BYTE    err_save_cnt = 0;

//---------- can adc
LONG    adcValue_RO;
LONG    adcValue_PA;
BYTE    isAdcOK_RO;
BYTE    isAdcOK_PA;

extern WORD xf_cnt;     // from capture.c ==> CO, SD CAN position value 동기화를 위해 
extern SWORD tbl_no;    // from capture.c ==> CO, SD CAN position value 동기화를 위해 
// BYTE    adcOK = 0;
static LONG g_ArryCanPosition[100] = {0};


/*F**************************************************************************
* NAME:     
* PURPOSE:  
*****************************************************************************/
// todo naming : ProcessCANAck(), CANData_AXIS_POS_DIR(), CAN DATA, (instruction, value, msg,  ) 
void ProcessCANAck(void)
{
    CANData_t   can;
    // char    str[17] = {0};


    if(CANBufGetSize() == 0 ) return;
    
    CANBufGetData(&can);

    // UartPrintf("==> can inst 0x%x, value 0x%x, msg %d\r\n", can.inst, can.value, can.msg);    
    // print_axis_id();

    switch(can.inst)
    {
        case CAN_AXIS_POS_DIR:          CANAck_AXIS_POS_DIR(can);           break;
        case CAN_AXIS_0_1_UNIT_PULSE:   CANAck_AXIS_0_1_UNIT_PULSE(can);    break;
        case CAN_AXIS_P_MAX_PULSE:      CANAck_AXIS_P_MAX_PULSE(can);       break;
        case CAN_AXIS_N_MAX_PULSE:      CANAck_AXIS_N_MAX_PULSE(can);       break;
        case CAN_AXIS_ACCEL_STEP:       CANAck_AXIS_ACCEL_STEP(can);        break;
        case CAN_AXIS_START_SPEED:      CANAck_AXIS_START_SPEED(can);       break;
        case CAN_AXIS_MAX_SPEED:        CANAck_AXIS_MAX_SPEED(can);         break;
        case CAN_AXIS_MOVE:             CANAck_AXIS_MOVE(can);              break;

        case CAN_AXIS_MOVE_CONST:       CANAck_AXIS_MOVE_CONST(can);        break;
        case CAN_AXIS_SLOW_STOP:        CANAck_AXIS_SLOW_STOP(can);         break;
        case CAN_AXIS_CNT:              CANAck_AXIS_CNT(can);               break;
        case CAN_AXIS_ERROR:            CANAck_AXIS_ERROR(can);             break;
        case CAN_FW_REV_NUM:            CANAck_FW_REV_NUM(can);             break;
        
        case CAN_LASER_CTRL:            CANAck_LASER_CTRL(can);             break;
        case CAN_ADC_CTRL:              CANAck_CAN_ADC_CTRL(can);           break;


        default:    /*PCPuts("Undifined CAN intstruction\r\n");*/ break;
    }
    
}


/*F**************************************************************************
* NAME:    
* PURPOSE: read CAN ADC value
*****************************************************************************/
int ReadCanAdc_PA(WORD *adcValue)
{
    ULONG lastTime;


    isAdcOK_PA = NO;
    
    rcan_puts(PA_ADC, (WORD)CAN_ADC_CTRL, (LONG)dmy_val, PA_ADC);   dly_10us(70);
    lastTime = TimeNow();

    while(1)
    {
        ProcessCANAck();

        if(isAdcOK_PA)
        {
            *adcValue = (WORD)adcValue_PA;
            return 1;
        }

        // 지정된 시간까지 ADC 값을 얻지 못하면 통신에러 출력후 함수 빠져나옴
        if( TimePassed(lastTime) > ONE_SECOND/4 )
        {
            PCPuts("[CAN_ERR_02] CAN ADC PA \r\n");
            return 0;
        }

    }
}

int ReadCanAdc_RO(WORD *adcValue)
{
    ULONG lastTime;


    isAdcOK_RO = NO;
    
    rcan_puts(RO_ADC, (WORD)CAN_ADC_CTRL, (LONG)dmy_val, RO_ADC);   dly_10us(70);
    lastTime = TimeNow();

    while(1)
    {
        ProcessCANAck();

        if(isAdcOK_RO)
        {
            *adcValue = (WORD)adcValue_RO;
            return 1;
        }

        // 지정된 시간까지 ADC 값을 얻지 못하면 통신에러 출력후 함수 빠져나옴
        if( TimePassed(lastTime) > ONE_SECOND/4 )
        {
            PCPuts("[CAN_ERR_02] CAN ADC RO \r\n");
            return 0;
        }

    }
}

void CANAck_CAN_ADC_CTRL(CANData_t can)
{
    switch(can.msg)
    {
        case RO_ADC:
            adcValue_RO = can.value;
            isAdcOK_RO = YES;
        break;

        case PA_ADC:
            adcValue_PA = can.value;
            isAdcOK_PA = YES;
        break;

        default:    PCPuts("CAN_ADC_CTRL Undefined ID !! \r\n"); break;
    }
}


void PrintAxisId(WORD axisId)
{
    switch(axisId)
    {
        case CO_MOTOR:  PCPuts("DZ ");   break;
        case SD_MOTOR:  PCPuts("SD ");   break;
        case HL_MOTOR:  PCPuts("HL ");   break;
        case HR_MOTOR:  PCPuts("HR ");   break;
        case VT_MOTOR:  PCPuts("VT ");   break;
        case VB_MOTOR:  PCPuts("VB ");   break;
        default:    break;
    }
}


void CANAck_AXIS_POS_DIR(CANData_t can)
{
    int match = false;
    WORD axisId;

    axisId = can.msg;
    PrintAxisId(axisId);
    PCPuts("Positive Direc   ");

    switch(can.msg) {
        case CO_MOTOR:  if(can.value == gAxisCO.pos_dir)    match = true; else match = false; break;
        case SD_MOTOR:  if(can.value == gAxisSD.pos_dir)    match = true; else match = false; break;
        case HL_MOTOR:  if(can.value == HL_POS_DIR)         match = true; else match = false; break;
        case HR_MOTOR:  if(can.value == HR_POS_DIR)         match = true; else match = false; break;
        case VT_MOTOR:  if(can.value == VT_POS_DIR)         match = true; else match = false; break;
        case VB_MOTOR:  if(can.value == VB_POS_DIR)         match = true; else match = false; break;
        default:    PCPuts("Undefined CAN ID !!");   break;
    }

    if(match == true) {
        PCPuts("OK\r\n");
    }    
    if(match == false) {
        PCPuts("Error!!!\r\n");
    }   

}

void CANAck_AXIS_0_1_UNIT_PULSE(CANData_t can)
{
    int match = false;
    WORD axisId;

    axisId = can.msg;
    PrintAxisId(axisId);
    PCPuts("0.1 Unit Pulse   ");

    switch(can.msg) {
        case CO_MOTOR:  if(can.value == gAxisCO.unit_0_1_pulse) match = true; else match = false; break;
        case SD_MOTOR:  if(can.value == gAxisSD.unit_0_1_pulse) match = true; else match = false; break;
        case HL_MOTOR:  if(can.value == HL_0_1_MM_PULSE)        match = true; else match = false; break;
        case HR_MOTOR:  if(can.value == HR_0_1_MM_PULSE)        match = true; else match = false; break;
        case VT_MOTOR:  if(can.value == VT_0_1_MM_PULSE)        match = true; else match = false; break;
        case VB_MOTOR:  if(can.value == VB_0_1_MM_PULSE)        match = true; else match = false; break;
        default:    PCPuts("Undefined CAN ID !!");   break;
    }

    if(match == true) {
        PCPuts("OK\r\n");
    }    
    if(match == false) {
        PCPuts("Error!!!\r\n");
    }
}

void CANAck_AXIS_P_MAX_PULSE(CANData_t can)
{
    int match = false;
    WORD axisId;

    axisId = can.msg;
    PrintAxisId(axisId);
    PCPuts("P Max Pulse      ");

    switch(can.msg) {
        case CO_MOTOR:  if(can.value == gAxisCO.p_max_pulse)    match = true; else match = false; break;
        case SD_MOTOR:  if(can.value == gAxisSD.p_max_pulse)    match = true; else match = false; break;
        case HL_MOTOR:  if(can.value == HL_P_MAX_PULSE)         match = true; else match = false; break;
        case HR_MOTOR:  if(can.value == HR_P_MAX_PULSE)         match = true; else match = false; break;
        case VT_MOTOR:  if(can.value == VT_P_MAX_PULSE)         match = true; else match = false; break;
        case VB_MOTOR:  if(can.value == VB_P_MAX_PULSE)         match = true; else match = false; break;
        default:    PCPuts("Undefined CAN ID !!");   break;
    }

    if(match == true) {
        PCPuts("OK\r\n");
    }    
    if(match == false) {
        PCPuts("Error!!!\r\n");
    }
}
void CANAck_AXIS_N_MAX_PULSE(CANData_t can)
{
    int match = false;
    WORD axisId;

    axisId = can.msg;
    PrintAxisId(axisId);
    PCPuts("N Max Pulse      ");

    switch(can.msg) {
        case CO_MOTOR:  if(can.value == gAxisCO.n_max_pulse)    match = true; else match = false; break;
        case SD_MOTOR:  if(can.value == gAxisSD.n_max_pulse)    match = true; else match = false; break;
        case HL_MOTOR:  if(can.value == HL_N_MAX_PULSE)         match = true; else match = false; break;
        case HR_MOTOR:  if(can.value == HR_N_MAX_PULSE)         match = true; else match = false; break;
        case VT_MOTOR:  if(can.value == VT_N_MAX_PULSE)         match = true; else match = false; break;
        case VB_MOTOR:  if(can.value == VB_N_MAX_PULSE)         match = true; else match = false; break;
        default:    PCPuts("Undefined CAN ID !!");   break;
    }

    if(match == true) {
        PCPuts("OK\r\n");
    }    
    if(match == false) {
        PCPuts("Error!!!\r\n");
    }
}
void CANAck_AXIS_ACCEL_STEP(CANData_t can)
{
    int match = false;
    WORD axisId;

    axisId = can.msg;
    PrintAxisId(axisId);
    PCPuts("Accel Step       ");

    switch(can.msg) {
        case CO_MOTOR:  if(can.value == gAxisCO.accel_step)     match = true; else match = false; break;
        case SD_MOTOR:  if(can.value == gAxisSD.accel_step)     match = true; else match = false; break;
        case HL_MOTOR:  if(can.value == HL_ACCEL_PULSE)         match = true; else match = false; break;
        case HR_MOTOR:  if(can.value == HR_ACCEL_PULSE)         match = true; else match = false; break;
        case VT_MOTOR:  if(can.value == VT_ACCEL_PULSE)         match = true; else match = false; break;
        case VB_MOTOR:  if(can.value == VB_ACCEL_PULSE)         match = true; else match = false; break;
        default:    PCPuts("Undefined CAN ID !!");   break;
    }

    if(match == true) {
        PCPuts("OK\r\n");
    }    
    if(match == false) {
        PCPuts("Error!!!\r\n");
    }
}
void CANAck_AXIS_START_SPEED(CANData_t can)
{
    int match = false;
    WORD axisId;

    axisId = can.msg;
    PrintAxisId(axisId);
    PCPuts("Start Speed      ");

    switch(can.msg) {
        case CO_MOTOR:  if(can.value == gAxisCO.start_speed)    match = true; else match = false; break;
        case SD_MOTOR:  if(can.value == gAxisSD.start_speed)    match = true; else match = false; break;
        case HL_MOTOR:  if(can.value == g_HL.start_speed)       match = true; else match = false; break;
        case HR_MOTOR:  if(can.value == g_HR.start_speed)       match = true; else match = false; break;
        case VT_MOTOR:  if(can.value == g_VT.start_speed)       match = true; else match = false; break;
        case VB_MOTOR:  if(can.value == g_VB.start_speed)       match = true; else match = false; break;
        default:    PCPuts("Undefined CAN ID !!");   break;
    }

    if(match == true) {
        PCPuts("OK\r\n");
    }    
    if(match == false) {
        PCPuts("Error!!!\r\n");
    }
}
void CANAck_AXIS_MAX_SPEED(CANData_t can)
{
    int match = false;
    WORD axisId;


    axisId = can.msg;
    PrintAxisId(axisId);
    PCPuts("Max Speed        ");

    switch(can.msg) {
        case CO_MOTOR:  if(can.value == gAxisCO.max_speed)      match = true; else match = false; break;
        case SD_MOTOR:  if(can.value == gAxisSD.max_speed)      match = true; else match = false; break;
        case HL_MOTOR:  if(can.value == g_HL.max_speed)         match = true; else match = false; break;
        case HR_MOTOR:  if(can.value == g_HR.max_speed)         match = true; else match = false; break;
        case VT_MOTOR:  if(can.value == g_VT.max_speed)         match = true; else match = false; break;
        case VB_MOTOR:  if(can.value == g_VB.max_speed)         match = true; else match = false; break;
        default:    PCPuts("Undefined CAN ID !!");   break;
    }

    if(match == true) {
        PCPuts("OK\r\n");
    }    
    if(match == false) {
        PCPuts("Error!!!\r\n");
    }
}
void CANAck_AXIS_MOVE_CONST(CANData_t can)
{
        switch(can.msg)
    {
      
        case CO_MOTOR:
            if(can.value == 0) gvCO.stop_flag = YES;
            if(debug_mode)  {   PCPrintf("[rpm_comc_%ld] \r\n", can.value);  }
        break;
          
        case SD_MOTOR:
            if(can.value == 0) gvSD.stop_flag = YES;
            if(debug_mode)  {   PCPrintf("[rpm_sdmc_%ld] \r\n", can.value);  }
        break;
              
        case HL_MOTOR:
            CanAxisMoveConst(can.value, &g_HL);
            break;

        case HR_MOTOR:
            CanAxisMoveConst(can.value, &g_HR);
            break;

        case VT_MOTOR:
            CanAxisMoveConst(can.value, &g_VT);
            break;

        case VB_MOTOR:
            CanAxisMoveConst(can.value, &g_VB);
            break;


        default:    PCPuts("CAN_AXIS_MOVE_CONST Undefined ID!! \r\n");    break;
    }
}


void CanAxisMoveConst(LONG value, varMotor_t *axis)
{
    if(value == 0) {
        axis->stop_flag = YES;
    }

    if(debug_mode) {
       PCPrintf("[rpm_%smc_%ld] \r\n", axis->axisName);  
    }
}

void CanAxisMove(LONG canValue, varMotor_t *axis)
{
    LONG        axis_0_1mm = 0;
    const int   SET_ORIGIN = 0;


    if(canValue == SET_ORIGIN) {
        axis->org_complete_flag = YES;
        PCPrintf("[rpm_%smv_0000]   [%s_end] \r\n", axis->axisName, axis->axisName);
    }
    else {
        axis_0_1mm = canValue / axis->unit_0_1_pulse;
        PCPrintf("[rpm_%smv_%04ld] \r\n", axis->axisName, axis_0_1mm);
    }

    axis->move_distance = axis_0_1mm;
    axis->stop_flag = YES;
}

void CANAck_AXIS_MOVE(CANData_t can)
{
    LONG    temp;
    WORD    axisId;
    const int   SET_ORIGIN = 0;


    axisId = can.msg;

    if(can.value == 0) temp = can.value;

    switch(axisId)
    {
        case CO_MOTOR:
            // todo : 향후 확인후 CanAxisMove()로 대체
            if(can.value == SET_ORIGIN) {
                gvCO.org_complete_flag = YES; 
                PCPuts("[rpm_comv_0000]  [co_end] \r\n");    
            }
            else {
                temp = can.value / gvCO.unit_0_1_pulse; 
                PCPrintf("[rpm_comv_%04ld] \r\n", temp);
            }
            
            gvCO.stop_flag = YES;     
            gvCO.move_distance = temp;
            break;

        case SD_MOTOR:
            // todo : 향후 확인후 CanAxisMove()로 대체
            if(can.value == SET_ORIGIN) {
                gvSD.org_complete_flag = YES; 
                PCPuts("[rpm_sdmv_0000]  [sd_end] \r\n");    
            }
            else {
                temp = can.value / gvSD.unit_0_1_pulse; 
                PCPrintf("[rpm_sdmv_%04ld] \r\n", temp);
            }
            
            gvSD.stop_flag = YES;     
            gvSD.move_distance = temp;
            break;

        case HR_MOTOR:
            CanAxisMove(can.value, &g_HR);
            break;

        case HL_MOTOR:
            CanAxisMove(can.value, &g_HL);
            break;

        case VT_MOTOR:
            CanAxisMove(can.value, &g_VT);
            break;

        case VB_MOTOR:
            CanAxisMove(can.value, &g_VB);
            break;
            
        default:    PCPuts("CAN_AXIS_MOVE Undefined ID!! \r\n");  break;
    }
}

void CANAck_AXIS_SLOW_STOP(CANData_t can)
{
    PCPuts("CANAck_AXIS_SLOW_STOP : not yet coding\r\n");
}

void CANAck_AXIS_CNT(CANData_t can)
{


    switch(can.msg)
    {
        case CO_MOTOR:
            PCPrintf("CO Position Count : %ld \r\n", can.value);
            break;

        case SD_MOTOR:
            PCPrintf("SD Position Count : %ld\r\n", can.value);
            break;

        case HL_MOTOR:
            PCPrintf("HL Position : %ld [um] \r\n", can.value*10/HL_0_01_MM_PULSE);
            break;

        case HR_MOTOR:
            PCPrintf("HR Position : %ld [um] \r\n", can.value*10/HR_0_01_MM_PULSE);
            break;

        case VT_MOTOR:
            PCPrintf("VT Position Count : %ld\r\n", can.value);
            break;

        case VB_MOTOR:
            PCPrintf("VB Position Count : %ld\r\n", can.value);
            break;


        default:    PCPuts("Undefined CAN Driver ID \r\n"); break;
    }
}

void CANAck_AXIS_ERROR(CANData_t can)
{

    if(exp_ok_flag)         
    {
        // 촬영중에 CAN error를  시리얼통신하지 않고 촬영 이후에 출력하기 위한 문자열 저장
        // sprintf(errstr[err_save_cnt], "CANerr[%d] : ID = %d, errNo =  %ld \r\n",err_save_cnt, can.msg, can.value);
        // err_save_cnt ++;


        // sprintf(errCAN[errCnt], "CANerr[%d] : ID = %d, errNo =  %ld \r\n",err_save_cnt, can.msg, can.value);
        // errCnt++;
    }
    else
    {
        switch(can.value)
        {
            // 1 : p_max_pulse over
            // 2 : n_max_pulse over
            // 3 : p+n max_pulse over
            // 4,5,6,7은 ecox에서 필요없음
            // 4 : ORG센서가 인식되지 않아야 하는데 인식된 경우
            // 5 : ORG센서가 인식되어야 하는데 인식되지 않은 경우
            // 6 : LMT센서가 인식되어야 하는데 인식되지 않은 경우
            // 7 : LMT센서가 인식되지 않아야 하는데 인식된 경우 
          
            case 1: 
                Set_Stop_Flag(can.value);
                PCPrintf("[SYS_CHK_02] %d \r\n", can.msg);     
                break;

            case 2: 
                Set_Stop_Flag(can.value);
                PCPrintf("[SYS_CHK_03] %d \r\n", can.msg);     
                break;

            case 3: 
                PCPrintf("[SYS_CHK_04] %d \r\n", can.msg);     
                break;
        
            case 8: 
                PCPrintf("STALL CAN MOTOR : %d \r\n", can.msg); 
                break;

            // 21 : can ADC 보드에서 WatchDog 상황이 발생할때 보냄 
            case 21:    
                PCPrintf("[CAN_ERR_04] WatchDog! : %d \r\n", can.msg);    
                break;
                
            default:    
                PCPrintf("[CAN_ERR_03] %d \r\n", can.msg);  
                break;
        }

        rcan_puts(can.msg, CAN_AXIS_ERROR, can.value, dmy_msg);   dly_10us(dly_t);

        UartPrintf("CAN error[%d]  ID = %d, errCode =  %ld \r\n", err_save_cnt+1, can.msg, can.value);
        // if(debug_mode)  {   PCPrintf("ID = %d, # =  %ld \r\n", can.msg, can.value);   }
    }



}
void CANAck_FW_REV_NUM(CANData_t can)
{
    // todo : ADC 보드에 대한 것 추가
    switch(can.msg)
    {
        case CO_MOTOR:  PCPrintf("CO  = MD0_V1_%07ld \r\n", can.value);  break;
        case SD_MOTOR:  PCPrintf("SD  = MD0_V1_%07ld \r\n", can.value);  break;
        case RO_ADC:    PCPrintf("RO  = AD0_V1_%07ld \r\n", can.value);  break;
        default:    PCPuts("CAN_FIRMWARE_REVISION_NUM Undefined ID!! \r\n"); break;
    }
}

void CANAck_LASER_CTRL(CANData_t can)
{
    // PCPrintf("CAN_LASER_CTRL %ld %d \r\n", can.value, can.msg);
    if(debug_mode)  {  PCPrintf("Msg: CAN_LASER_CTRL %ld %d \r\n", can.value, can.msg); }
}


/*F**************************************************************************
* NAME:    
* PURPOSE: 
*****************************************************************************/
void Set_Stop_Flag (LONG axis_value)
{
    switch(axis_value)
    {
        case CO_MOTOR:  gvCO.stop_flag = YES; break;
        case SD_MOTOR:  gvSD.stop_flag = YES; break;
        default: break;
    }
}

