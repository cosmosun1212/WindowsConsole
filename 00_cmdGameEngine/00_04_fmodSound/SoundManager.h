#pragma once
#include <fmod.h> // FMOD 라이브러리 헤더

#define MAX_SOUND_CHANNELS 32 // 최대 사운드 채널 수 (동시 재생 가능한 사운드 수 제한)

// 사운드 그룹 열거형 정의 (배경음과 효과음으로 구분)
enum SoundGroup
{
    SOUND_GROUP_MENU,   // 메뉴 사운드 그룹 (예: 메뉴 클릭, 선택 등 UI 관련 사운드)
    SOUND_GROUP_BGM,    // 배경음별
    SOUND_GROUP_SFX,    // 효과음별
    SOUND_GROUP_MAX     // 그룹 수
};    

// 사운드 관리 구조체 정의
typedef struct ___SoundEngine 
{
    FMOD_SYSTEM* system; // FMOD 시스템 포인터
    FMOD_SOUND* soundContainer[MAX_SOUND_CHANNELS]; // 사운드 파일들을 담을 배열
    bool         isLoaded[MAX_SOUND_CHANNELS];       // 해당 슬롯에 소리가 로드되었는지 여부
    FMOD_CHANNEL* channelContainer[MAX_SOUND_CHANNELS]; // 사운드 채널 배열
    SoundGroup soundGroupMap[MAX_SOUND_CHANNELS]; // 사운드 그룹 매핑 배열
    FMOD_CHANNELGROUP* groupContainer[SoundGroup::SOUND_GROUP_MAX]; // 채널 그룹 배열
}SOUND_ENGINE; // 사운드 엔진 구조체 정의

bool SoundInit(); // 사운드 시스템 초기화 함수
void SoundUpdate(); // 사운드 시스템 업데이트 함수
void SoundRelease(); // 사운드 시스템 해제 함수

bool SoundLoad(int soundID, const char* filePath, SoundGroup group, bool isStream); // 사운드 로드 함수
void SoundPlay(int soundID); // 사운드 재생 함수
void SoundStop(int soundID); // 사운드 정지 함수
void SoundSetVolume(int soundID, float volume); // 사운드 볼륨 설정 함수
void SoundSetGroupVolume(SoundGroup group, float volume); // 사운드 그룹 볼륨 설정 함수
void SoundSetMasterVolume(float volume); // 마스터 볼륨 설정 함수

SoundGroup GetSoundGroup(int soundID); // 사운드 그룹 조회 함수 (사운드 ID에 해당하는 그룹 반환)