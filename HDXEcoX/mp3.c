/*C***************************************************************************
* $RCSfile: mp3.c
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the mp3 source file
*----------------------------------------------------------------------------
* RELEASE:      $Name: mp3     
* Edited  By    $Id: Joony 
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include "includes.h"

/*_____ D E C L A R A T I O N ______________________________________________*/

/*_____ D E F I N E ________________________________________________________*/

/*_____ F U C T I O N ______________________________________________________*/
void SoundPlay(BYTE);
void KO_Korean(BYTE);
void EN_English(BYTE);
void ZH_Chinese(BYTE);
void Voice_Off(void);

/*_____ V A R I A B L E ____________________________________________________*/
extern  BYTE	sound_option;		// language option
BYTE	sound_play_flag = NO;		// 메인문에서 음원출력 필요할 때 한 번만 실행하기 위한 플랙
LONG	sound_play_time;			// 음성안내가 출력되는 시간 (음성 출력이 완료되기 전에 다음 음성이 출력되는 것 방지)
BYTE	sound_play_time_end	= YES;	// 음성안내 출력이 완료되었음을 알기 위한 플랙
BYTE	sound_play_time_flag = NO;	// 음성안내 출력시간 카운트 시작 플랙
BYTE	sound_file_name = 0;
BYTE	mp3_ack_flag = 0;
// BYTE	pre_sound_file_name = 0;	// 같은 파일이 반복되는 걸 방지하기 위한 변수
//BYTE	sdtt_ack_flag = NO;


// todo : 전역변수 지역변수화, sound_file_name  => KO_Korean() 수정필요
// todo : return 값을 sound_play_time 으로...??
void SoundPlay(BYTE soundName)
{
    static BYTE preSoundName = 0;       // 같은 파일이 반복되는 걸 방지하기 위한 변수

   // UartPrintf("Sound Play : now sound : %d, pre sound : %d, endflag : %d \r\n", soundName, preSoundName, sound_play_time_end);

    
    if(soundName == preSoundName)  return; // 같은 파일 반복되는 것 방지
    
    // if(sound_play_time_end)
    // {
        if(soundName == SOUND_STOP)
        {
            MP3Puts("@S\r");
            sound_play_time = SOUND_0_1_SEC / 20;
        }
        else
        {
            MP3Puts("@P3");

            switch(sound_option)
            {
                case 1:     KO_Korean(soundName);        break;
                case 2:     EN_English(soundName);       break;
                case 3:     ZH_Chinese(soundName);       break;
                default:    Voice_Off();        break;
            }
        }

        preSoundName = soundName;  // 음성 출력하고 나서 변경해야 함

        sound_play_time_end = NO;       // sound play가 끝나는 것을확인하기 위한 플래그  
        sound_play_time_flag    = YES;  // sound_play_time만큼 카운트 시작
        sound_play_flag         = NO;   // sound_play 함수 반복 실행 방지
    // }
}


void Voice_Off (void)
{
    switch(sound_file_name)
    {
        case SOUND_CAPT_BGM:		// 촬영 음악
            MP3Puts("WCT\r");
            sound_play_time = SOUND_0_1_SEC / 20;
        break;

        default: MP3Puts("@S\r"); sound_play_time = SOUND_0_1_SEC / 20; break;
    }
}


void KO_Korean (BYTE sound_file_name)
{
    switch(sound_file_name)
    {
        case SOUND_CLOSE_EYES:			//안전한 촬영을 위해 눈을 감아 주세요
            MP3Puts("WGUIDE1\r");
            sound_play_time = SOUND_0_1_SEC * 18;
        break;

        case SOUND_DONT_MOVE:			//촬영중에는 몸을 움직이지 말아 주세요
            MP3Puts("WGUIDE2\r");
            sound_play_time = SOUND_0_1_SEC * 18;
        break;

        case SOUND_CAPT_THANKS:			//촬영에 협조해 주셔서 감사합니다. 수고하셨습니다.
            MP3Puts("WGUIDE3\r");
            sound_play_time = SOUND_0_1_SEC * 20;
        break;

        case SOUND_APPLYING_CT:			//촬영을 위해 준비중입니다.
        case SOUND_APPLYING_PANO:
        case SOUND_APPLYING_CEPH:
            MP3Puts("WWAIT1\r");
            sound_play_time = SOUND_0_1_SEC * 10;
        break;

        case SOUND_CAPT_WAIT:			//잠시만 기다려 주세요
            MP3Puts("WWAIT2\r");
            sound_play_time = SOUND_0_1_SEC * 10;
        break;

        case SOUND_OPEN_EYES:			//눈을 뜨시고 다음 안내가 있을때까지 기다려주세요.
            MP3Puts("WWAIT3\r");
            sound_play_time = SOUND_0_1_SEC * 23;
        break;

        case SOUND_CAPT_BGM:		// 촬영 음악
            MP3Puts("WCT\r");
            sound_play_time = SOUND_0_1_SEC / 20;
        break;

        case SOUND_HELLO:				//안녕하세요
            MP3Puts("WHELLO1\r");
            sound_play_time = SOUND_0_1_SEC * 8;
        break;

        case SOUND_APPLIED_CT:			//촬영 준비가 되었습니다.
        case SOUND_APPLIED_PANO:
        case SOUND_APPLIED_CEPH:
            MP3Puts("WREADY\r");
            sound_play_time = SOUND_0_1_SEC * 10;
        break;

        case SOUND_CAPT_FINISH:			//촬영이 완료 되었습니다.
            MP3Puts("WFINISH\r");
            sound_play_time = SOUND_0_1_SEC * 10;
        break;

        case SOUND_CAPT_STOP:			//촬영이 중단 되었습니다.
            MP3Puts("WSTOP\r");
            sound_play_time = SOUND_0_1_SEC * 10;
        break;

        case SOUND_HAPPYDAY:			//즐거운 하루 되세요.
            MP3Puts("WILLMED\r");
            sound_play_time = SOUND_0_1_SEC * 10;
        break;

        default: MP3Puts("@S\r"); sound_play_time = SOUND_0_1_SEC / 20; break;
    }
}


void EN_English (BYTE sound_file_name)
{
    switch(sound_file_name)
    {
        case SOUND_CLOSE_EYES:          // 안전한 촬영을 위해 눈을 감고 몸을 움직이지 말아주세요			
            MP3Puts("EN_SN_01\r");      // Please close your eyes and don't move for safe scanning.     
            sound_play_time = SOUND_0_1_SEC * 25;
        break;

        case SOUND_DONT_MOVE:           // 아직 촬영중이니 정확한 촬영을 위해 몸을 움직이지 말아주세요           
            MP3Puts("EN_SN_02\r");      //  Please be still to get the exact scan while scanning.
            sound_play_time = SOUND_0_1_SEC * 30;
        break;

        case SOUND_OPEN_EYES:
        case SOUND_CAPT_THANKS:         // 눈을 뜨시고 다음 안내가 있을 때까지 기다려주세요
            MP3Puts("EN_SN_03\r");      // Please open your eyes and wait for further directions.
            sound_play_time = SOUND_0_1_SEC * 25;
        break;

        case SOUND_APPLYING_CT:         // 씨티 촬영 준비중입니다
            MP3Puts("EN_SN_04\r");      // CT scan is being prepared.  
            sound_play_time = SOUND_0_1_SEC * 15;
            break;

        case SOUND_APPLYING_PANO:       // 파노라마 촬영 준비중입니다
            MP3Puts("EN_SN_05\r");      // Panoramic scan is being prepared.
            sound_play_time = SOUND_0_1_SEC * 15;
            break;

        case SOUND_APPLYING_CEPH:       // 세팔로 촬영 준비중입니다  
            MP3Puts("EN_SN_06\r");      // Cephalo scan is being prepared.
            sound_play_time = SOUND_0_1_SEC * 15;
            break;
      
        case SOUND_CAPT_WAIT:           // 잠시만 기다려 주세요			
            MP3Puts("EN_SN_07\r");      // Please wait a moment.      
            sound_play_time = SOUND_0_1_SEC * 15;
        break;

        case SOUND_CAPT_BGM:            // 촬영 음악
            MP3Puts("WCT\r");
            sound_play_time = SOUND_0_1_SEC / 20;
        break;

        case SOUND_APPLIED_CT:          // 씨티 촬영 준비가 완료되었습니다
            MP3Puts("EN_SN_08\r");      // CT scan is ready.
            sound_play_time = SOUND_0_1_SEC * 15;
        break;

        case SOUND_APPLIED_PANO:        // 파노라마 촬영 준비가 완료되었습니다
            MP3Puts("EN_SN_09\r");      // Panoramic scan is ready.
            sound_play_time = SOUND_0_1_SEC * 15;
        break;

        case SOUND_APPLIED_CEPH:        // 세팔로 촬영 준비가 완료되었습니다
            MP3Puts("EN_SN_10\r");      // Cephalo scan is ready.
            sound_play_time = SOUND_0_1_SEC * 15;
        break;

        case SOUND_CAPT_FINISH:         // 촬영이 완료 되었습니다.
            MP3Puts("EN_SN_11\r");      // Thank you. Scanning is now complete.
            sound_play_time = SOUND_0_1_SEC * 25;
        break;

        case SOUND_CAPT_STOP:			// 촬영이 중지되었습니다.
            MP3Puts("EN_SN_12\r");      // Scanning has been stopped. 
            sound_play_time = SOUND_0_1_SEC * 20;
        break;

        case SOUND_HAPPYDAY:            // 안녕하세요. 즐거운 하루 되세요.
            MP3Puts("EN_SN_13\r");      // Hello. welcome. Have a nice day
            sound_play_time = SOUND_0_1_SEC * 15;
        break;

        default: MP3Puts("@S\r"); sound_play_time = SOUND_0_1_SEC / 20; break;
    }
}


void ZH_Chinese (BYTE sound_file_name)
{
    switch(sound_file_name)
    {
        case SOUND_CLOSE_EYES:			//안전한 촬영을 위해 눈을 감아 주세요
            MP3Puts("ZH_SN_02\r");
            sound_play_time = SOUND_0_1_SEC * 18;
        break;

        case SOUND_DONT_MOVE:			//촬영중에는 몸을 움직이지 말아 주세요
            MP3Puts("ZH_SN_03\r");
            sound_play_time = SOUND_0_1_SEC * 18;
        break;

        case SOUND_CAPT_THANKS:			//촬영에 협조해 주셔서 감사합니다. 수고하셨습니다.
            MP3Puts("ZH_SN_04\r");
            sound_play_time = SOUND_0_1_SEC * 14;
        break;

        case SOUND_APPLYING_CT:         //촬영을 위해 준비중입니다.
        case SOUND_APPLYING_PANO:
        case SOUND_APPLYING_CEPH:
            MP3Puts("ZH_SN_05\r");
            sound_play_time = SOUND_0_1_SEC * 13;
        break;

        case SOUND_CAPT_WAIT:			//잠시만 기다려 주세요
            MP3Puts("ZH_SN_06\r");
            sound_play_time = SOUND_0_1_SEC * 8;
        break;

        case SOUND_OPEN_EYES:			//눈을 뜨시고 다음 안내가 있을때까지 기다려주세요.
            MP3Puts("ZH_SN_07\r");
            sound_play_time = SOUND_0_1_SEC * 21;
        break;

        case SOUND_CAPT_BGM:		// 촬영 음악
            MP3Puts("WCT\r");
            sound_play_time = SOUND_0_1_SEC / 20;
        break;

        case SOUND_HELLO:				//안녕하세요
            MP3Puts("ZH_SN_11\r");
            sound_play_time = SOUND_0_1_SEC * 7;
        break;

        case SOUND_APPLIED_CT:          //촬영 준비가 되었습니다.
        case SOUND_APPLIED_PANO:
        case SOUND_APPLIED_CEPH:		
            MP3Puts("ZH_SN_13\r");
            sound_play_time = SOUND_0_1_SEC * 9;
        break;

        case SOUND_CAPT_FINISH:			//촬영이 완료 되었습니다.
            MP3Puts("ZH_SN_14\r");
            sound_play_time = SOUND_0_1_SEC * 9;
        break;

        case SOUND_CAPT_STOP:			//촬영이 중단 되었습니다.
            MP3Puts("ZH_SN_15\r");
            sound_play_time = SOUND_0_1_SEC * 10;
        break;

        default: MP3Puts("@S\r"); sound_play_time = SOUND_0_1_SEC / 20; break;
    }
}





