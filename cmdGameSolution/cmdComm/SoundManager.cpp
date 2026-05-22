#include "SoundEngine.h"
#include <windows.h>

static SOUND_ENGINE g_SoundEngine = { nullptr, {nullptr,}, {false,} };

bool SoundInit()
{
    // FMOD 시스템 생성
    if (FMOD_System_Create(&g_SoundEngine.system, FMOD_VERSION) != FMOD_OK) 
        return false;
    
    // 사운드 카드 초기화 (최대 32개 동시 출력 가능 채널 설정)
    if (FMOD_System_Init(g_SoundEngine.system, MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, NULL) != FMOD_OK) 
        return false;
    
    // 슬롯 초기화 확인
    for (int i = 0; i < MAX_SOUND_CHANNELS; i++)
    {
        g_SoundEngine.soundContainer[i] = nullptr;
        g_SoundEngine.isLoaded[i] = false;
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

bool SoundLoad(int soundID, const char* filePath, bool isStream)
{
    if (g_SoundEngine.system == nullptr) return false;
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return false;
    
    // 이미 해당 슬롯에 소리가 있다면 해제 후 새로 로드
    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.soundContainer[soundID])
    {
        FMOD_Sound_Release(g_SoundEngine.soundContainer[soundID]);
    }
    
    FMOD_RESULT result;
    
    if (isStream)
    {
        // 용량이 큰 BGM (스트리밍 + 루프 연사 모드)
        result = FMOD_System_CreateStream(g_SoundEngine.system, filePath, FMOD_LOOP_NORMAL, NULL, &g_SoundEngine.soundContainer[soundID]);
    }
    else
    {
        // 용량이 작은 효과음 (메모리 로딩 + 단발성 재생 모드)
        result = FMOD_System_CreateSound(g_SoundEngine.system, filePath, FMOD_DEFAULT, NULL, &g_SoundEngine.soundContainer[soundID]);
    }
    
    // 로드 실패 시 에러창 알림
    if (result != FMOD_OK)
    {
        MessageBoxA(NULL, filePath, "사운드 파일 로드 실패! 경로를 확인하세요.", MB_OK | MB_ICONWARNING);
        g_SoundEngine.isLoaded[soundID] = false;
        return false;
    }
    
    g_SoundEngine.isLoaded[soundID] = true;
    return true;
}

void SoundPlay(int soundID)
{
    if (g_SoundEngine.system == nullptr) return;
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return;
    
    // 로드된 사운드만 재생
    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.soundContainer[soundID])
    {
        // 마지막 채널 인자는 무조건 NULL 처리
        FMOD_System_PlaySound(g_SoundEngine.system, g_SoundEngine.soundContainer[soundID], NULL, false, NULL);
    }
}

void SoundStop(int soundID)
{
    if (g_SoundEngine.system == nullptr) return;
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return;
    
    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.soundContainer[soundID])
    {
        FMOD_CHANNEL* channel = nullptr;
    }
}

void SoundRelease()
{
    // 사운드 리소스 메모리 해제
    for (int i = 0; i < MAX_SOUND_CHANNELS; i++)
    {
        if (g_SoundEngine.isLoaded[i] && g_SoundEngine.soundContainer[i])
        {
            FMOD_Sound_Release(g_SoundEngine.soundContainer[i]);
            g_SoundEngine.soundContainer[i] = nullptr;
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
