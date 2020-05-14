/*H***************************************************************************
* $RCSfile: mp3.h
*-----------------------------------------------------------------------------
* PURPOSE:
* This file contains the mp3 header 
*----------------------------------------------------------------------------
* RELEASE:      $Name:mp3.h      
* Edited  By    $Id: Joony
*****************************************************************************/
#ifndef _MP3_HEADER
#define	_MP3_HEADER

/****************************************************************************
*Voice Message
*****************************************************************************/
// 괄호는 qface음성 파일 기준
enum SoundStates {
	SOUND_CLOSE_EYES,				// 안전한 촬영을 위해 눈을 감아 주세요
	SOUND_DONT_MOVE,				// 촬영중에는 몸을 움직이지 말아 주세요
	SOUND_CAPT_THANKS,				// 촬영에 협조해 주셔서 감사합니다. 수고하셨습니다.
	SOUND_APPLYING_CT,              // (씨티) 촬영 준비중입니다
	SOUND_APPLYING_PANO,            // (파노라마) 촬영 준비중입니다
	SOUND_APPLYING_CEPH,            // (세팔로) 촬영 준비중입니다
	SOUND_APPLIED_CT,               // (씨티) 촬영 준비가 완료되었습니다
	SOUND_APPLIED_PANO,             // (파노라마) 촬영 준비가 완료되었습니다
	SOUND_APPLIED_CEPH,         	// (세팔로) 촬영 준비가 완료되었습니다
	SOUND_CAPT_WAIT,				// 잠시만 기다려 주세요
	SOUND_OPEN_EYES,				// 눈을 뜨시고 다음 안내가 있을때까지 기다려주세요.
	SOUND_CAPT_BGM,					// 촬영 음악
	SOUND_HELLO,					// 안녕하세요
	SOUND_CAPT_FINISH,				// 촬영이 완료 되었습니다.
	SOUND_CAPT_STOP,				// 촬영이 중단 되었습니다. 
	SOUND_HAPPYDAY,					// 즐거운 하루 되세요.
	SOUND_STOP						// 재생 정지
};
/****************************************************************************/
#define	SOUND_0_1_SEC	20

extern BYTE	sound_play_flag;
extern LONG	sound_play_time;
extern BYTE	sound_play_time_end;
extern BYTE	sound_play_time_flag;
extern BYTE	mp3_ack_flag;

extern void SoundPlay(BYTE);

#endif