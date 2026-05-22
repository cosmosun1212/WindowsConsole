#pragma once
#include "../fmodSound/inc/fmod.h"

#define MAX_SOUND_CHANNELS 32

enum SoundGroup {
    SOUND_GROUP_BGM,    // 배경음별
    SOUND_GROUP_SFX,    // 효과음별
    SOUND_GROUP_MAX     // 그룹 수
};

// 사운드 관리 구조체
typedef struct ___SoundEngine {
    FMOD_SYSTEM* system;
    FMOD_SOUND* soundContainer[MAX_SOUND_CHANNELS]; // 사운드 파일들을 담을 배열
    bool         isLoaded[MAX_SOUND_CHANNELS];       // 해당 슬롯에 소리가 로드되었는지 여부
    FMOD_CHANNEL* channelContainer[MAX_SOUND_CHANNELS];
    SoundGroup soundGroupMap[MAX_SOUND_CHANNELS];
    FMOD_CHANNELGROUP* groupContainer[SoundGroup::SOUND_GROUP_MAX];
}SOUND_ENGINE;

bool SoundInit();
void SoundUpdate();
void SoundRelease();

bool SoundLoad(int soundID, const char* filePath, SoundGroup group, bool isStream);
void SoundPlay(int soundID);
void SoundStop(int soundID);

void SoundSetVolume(int soundID, float volume);
void SoundSetGroupVolume(SoundGroup group, float volume);
void SoundSetMasterVolume(float volume);
