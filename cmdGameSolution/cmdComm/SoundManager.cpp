#include "SoundManager.h"
#include <windows.h>
#pragma comment(lib, "../fmodSound/lib/x64/fmod_vc.lib")

static SOUND_ENGINE g_SoundEngine = { nullptr, {nullptr,}, {false,}, {nullptr,}, { SOUND_GROUP_SFX, }, {nullptr,} };

bool SoundInit()
{
    // FMOD 시스템 생성
    if (FMOD_System_Create(&g_SoundEngine.system, FMOD_VERSION) != FMOD_OK)
        return false;

    // 사운드 카드 초기화 (최대 32개 동시 출력 가능 채널 설정)
    if (FMOD_System_Init(g_SoundEngine.system, MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, NULL) != FMOD_OK)
        return false;

    const char* groupNames[SOUND_GROUP_MAX] = { "BGM", "SFX" };
    FMOD_CHANNELGROUP* masterGroup = nullptr;
    FMOD_System_GetMasterChannelGroup(g_SoundEngine.system, &masterGroup);

    for (int i = 0; i < SOUND_GROUP_MAX; i++)
    {
        // 그룹 생성
        if (FMOD_System_CreateChannelGroup(g_SoundEngine.system, groupNames[i], &g_SoundEngine.groupContainer[i]) == FMOD_OK)
        {
            // 생성된 그룹을 마스터 그룹 아래 자식으로 추가 (계층 구조 형성)
            FMOD_ChannelGroup_AddGroup(masterGroup, g_SoundEngine.groupContainer[i], false, NULL);
        }
    }

    // 슬롯 초기화
    for (int i = 0; i < MAX_SOUND_CHANNELS; i++)
    {
        g_SoundEngine.soundContainer[i] = nullptr;
        g_SoundEngine.isLoaded[i] = false;
        g_SoundEngine.channelContainer[i] = nullptr;
        g_SoundEngine.soundGroupMap[i] = SOUND_GROUP_SFX; // 기본값
    }

    return true;
}

void SoundUpdate()
{
    if (g_SoundEngine.system != nullptr)
    {
        FMOD_System_Update(g_SoundEngine.system);
    }
}

bool SoundLoad(int soundID, const char* filePath, SoundGroup group, bool isStream)
{
    if (g_SoundEngine.system == nullptr) return false;
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return false;

    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.soundContainer[soundID])
    {
        FMOD_Sound_Release(g_SoundEngine.soundContainer[soundID]);
    }

    FMOD_RESULT result;
    if (isStream)
    {
        result = FMOD_System_CreateStream(g_SoundEngine.system, filePath, FMOD_LOOP_NORMAL, NULL, &g_SoundEngine.soundContainer[soundID]);
    }
    else
    {
        result = FMOD_System_CreateSound(g_SoundEngine.system, filePath, FMOD_DEFAULT, NULL, &g_SoundEngine.soundContainer[soundID]);
    }

    if (result != FMOD_OK)
    {
        MessageBoxA(NULL, filePath, "사운드 파일 로드 실패! 경로를 확인하세요.", MB_OK | MB_ICONWARNING);
        g_SoundEngine.isLoaded[soundID] = false;
        return false;
    }

    // 그룹 정보 저장
    g_SoundEngine.soundGroupMap[soundID] = group;
    g_SoundEngine.isLoaded[soundID] = true;
    return true;
}

void SoundPlay(int soundID)
{
    if (g_SoundEngine.system == nullptr) return;
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return;

    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.soundContainer[soundID])
    {
        SoundGroup group = g_SoundEngine.soundGroupMap[soundID];
        FMOD_CHANNELGROUP* targetGroup = g_SoundEngine.groupContainer[group];

        FMOD_System_PlaySound(g_SoundEngine.system, g_SoundEngine.soundContainer[soundID], targetGroup, false, &g_SoundEngine.channelContainer[soundID]);
    }
}

void SoundStop(int soundID)
{
    if (g_SoundEngine.system == nullptr) return;
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return;

    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.channelContainer[soundID])
    {
        FMOD_Channel_Stop(g_SoundEngine.channelContainer[soundID]);
        g_SoundEngine.channelContainer[soundID] = nullptr;
    }
}

void SoundSetVolume(int soundID, float volume)
{
    if (g_SoundEngine.system == nullptr) return;
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return;

    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.channelContainer[soundID])
    {
        FMOD_Channel_SetVolume(g_SoundEngine.channelContainer[soundID], volume);
    }
}

void SoundSetGroupVolume(SoundGroup group, float volume)
{
    if (g_SoundEngine.system == nullptr) return;
    if (group < 0 || group >= SOUND_GROUP_MAX) return;

    if (g_SoundEngine.groupContainer[group])
    {
        FMOD_ChannelGroup_SetVolume(g_SoundEngine.groupContainer[group], volume);
    }
}

void SoundSetMasterVolume(float volume)
{
    if (g_SoundEngine.system == nullptr) return;

    FMOD_CHANNELGROUP* masterGroup = nullptr;
    if (FMOD_System_GetMasterChannelGroup(g_SoundEngine.system, &masterGroup) == FMOD_OK)
    {
        FMOD_ChannelGroup_SetVolume(masterGroup, volume);
    }
}

void SoundRelease()
{
    // 사운드 리소스 해제
    for (int i = 0; i < MAX_SOUND_CHANNELS; i++)
    {
        if (g_SoundEngine.isLoaded[i] && g_SoundEngine.soundContainer[i])
        {
            FMOD_Sound_Release(g_SoundEngine.soundContainer[i]);
            g_SoundEngine.soundContainer[i] = nullptr;
            g_SoundEngine.channelContainer[i] = nullptr;
            g_SoundEngine.isLoaded[i] = false;
        }
    }

    // FMOD 시스템 종료
    if (g_SoundEngine.system)
    {
        FMOD_System_Close(g_SoundEngine.system);
        FMOD_System_Release(g_SoundEngine.system);
        g_SoundEngine.system = nullptr;
    }
}
