#pragma once
#include "../fmodSound/Header/fmod.h"

#define MAX_SOUND_CHANNELS 32

    // 사운드 관리 구조체
typedef struct ___SoundEngine {
    FMOD_SYSTEM* system;
    FMOD_SOUND* soundContainer[MAX_SOUND_CHANNELS]; // 사운드 파일들을 담을 배열
    bool         isLoaded[MAX_SOUND_CHANNELS];       // 해당 슬롯에 소리가 로드되었는지 여부
    } 

bool SoundInit();
void SoundUpdate();
void SoundRelease();

bool SoundLoad(int soundID, const char* filePath, bool isStream);
void SoundPlay(int soundID);
void SoundStop(int soundID);

