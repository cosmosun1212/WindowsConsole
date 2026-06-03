#include "SoundManager.h" // 사운드 관리 헤더 
#include <windows.h> // Windows API 헤더 (MessageBoxA 함수 사용)

// 사운드 엔진 전역 변수 초기화
static SOUND_ENGINE g_SoundEngine = { nullptr, {nullptr,}, {false,}, {nullptr,}, { SOUND_GROUP_SFX, }, {nullptr,} };

// 사운드 시스템 초기화 함수
bool SoundInit()
{
    // FMOD 시스템 생성
	if (FMOD_System_Create(&g_SoundEngine.system, FMOD_VERSION) != FMOD_OK) 
		return false; // FMOD 시스템 초기화 실패 시

    // 사운드 카드 초기화 (최대 32개 동시 출력 가능 채널 설정)
    if (FMOD_System_Init(g_SoundEngine.system, MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, NULL) != FMOD_OK)
        return false;

	const char* groupNames[SOUND_GROUP_MAX] = { "BGM", "SFX" }; // 그룹 이름 배열 (배경음과 효과음 그룹)
	FMOD_CHANNELGROUP* masterGroup = nullptr; // 마스터 채널 그룹 포인터
	FMOD_System_GetMasterChannelGroup(g_SoundEngine.system, &masterGroup); /// FMOD 시스템에서 마스터 채널 그룹 가져오기

	// 그룹 생성 및 마스터 그룹에 추가
	for (int i = 0; i < SOUND_GROUP_MAX; i++) // 각 사운드 그룹에 대해 반복
    {
        // 그룹 생성
        if (FMOD_System_CreateChannelGroup(g_SoundEngine.system, groupNames[i], &g_SoundEngine.groupContainer[i]) == FMOD_OK)
        {
            // 생성된 그룹을 마스터 그룹 아래 자식으로 추가 (계층 구조 형성)
            FMOD_ChannelGroup_AddGroup(masterGroup, g_SoundEngine.groupContainer[i], false, NULL);
        }
    }

	// 사운드 컨테이너 및 로드 상태 초기화 
	for (int i = 0; i < MAX_SOUND_CHANNELS; i++) // 각 사운드 슬롯에 대해 반복
    {
		g_SoundEngine.soundContainer[i] = nullptr; // 사운드 컨테이너 초기화 (nullptr로 설정하여 빈 슬롯 표시)
		g_SoundEngine.isLoaded[i] = false; // 로드 상태 초기화 (false로 설정하여 아직 로드되지 않았음을 표시)
		g_SoundEngine.channelContainer[i] = nullptr; // 채널 컨테이너 초기화 (nullptr로 설정하여 빈 슬롯 표시)
        g_SoundEngine.soundGroupMap[i] = SOUND_GROUP_SFX; // 기본값 (효과음 그룹으로 설정)
    }

	return true; // 사운드 시스템 초기화 성공
}

// 사운드 시스템 업데이트 함수
void SoundUpdate()
{
	// FMOD 시스템 업데이트 (사운드 재생 상태 및 효과 적용 등 처리)
	if (g_SoundEngine.system != nullptr) // FMOD 시스템이 초기화된 경우에만 업데이트 수행
    {
		FMOD_System_Update(g_SoundEngine.system); // FMOD 시스템 업데이트 호출 (사운드 재생 상태 및 효과 적용 등 처리)
    }
}

// 사운드 로드 함수
bool SoundLoad(int soundID, const char* filePath, SoundGroup group, bool isStream)
{
	if (g_SoundEngine.system == nullptr) return false; // FMOD 시스템이 초기화되지 않은 경우 로드 실패
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return false; // 유효하지 않은 사운드 ID인 경우 로드 실패

	// 이미 로드된 사운드가 있다면 해제
	if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.soundContainer[soundID]) // 해당 슬롯에 이미 사운드가 로드되어 있는 경우
    {
        FMOD_Sound_Release(g_SoundEngine.soundContainer[soundID]); // 기존 사운드 해제
        g_SoundEngine.soundContainer[soundID] = nullptr; // 슬롯 초기화
        g_SoundEngine.isLoaded[soundID] = false; // 로드 상태 초기화
    }

	// 사운드 로드 (스트리밍 여부에 따라 FMOD 함수 선택)
	FMOD_RESULT result; // FMOD 함수 호출 결과 저장 변수

	// 스트리밍 모드로 로드할 경우 FMOD_System_CreateStream 사용, 그렇지 않으면 FMOD_System_CreateSound 사용
    if (isStream)
    {
		// 스트리밍 모드로 사운드 로드 (메모리 사용 최적화, 긴 사운드에 적합)
        result = FMOD_System_CreateStream(g_SoundEngine.system, filePath, FMOD_LOOP_NORMAL, NULL, &g_SoundEngine.soundContainer[soundID]);
    }
	else // 일반 모드로 로드할 경우 FMOD_System_CreateSound 사용 (메모리에 전체 로드, 빠른 재생 가능)
    {
        // 일반 모드로 사운드 로드 (짧은 사운드에 적합)
        result = FMOD_System_CreateSound(g_SoundEngine.system, filePath, FMOD_DEFAULT, NULL, &g_SoundEngine.soundContainer[soundID]);
    }

	// 로드 실패 시 경고 메시지 출력 및 로드 상태 초기화
	if (result != FMOD_OK) // 사운드 로드 실패한 경우
    {
		// 경고 메시지 출력 (사운드 파일 로드 실패, 경로 확인 요청)
        MessageBoxA(NULL, filePath, "사운드 파일 로드 실패! 경로를 확인하세요.", MB_OK | MB_ICONWARNING);
		g_SoundEngine.isLoaded[soundID] = false; // 로드 상태 초기화 (false로 설정하여 로드 실패 표시)
		return false; // 로드 실패 반환
    }

	// 사운드 로드 성공 시 그룹 매핑 및 로드 상태 설정
	g_SoundEngine.soundGroupMap[soundID] = group; // 사운드 그룹 매핑 (사운드 ID에 해당하는 그룹 설정)
	g_SoundEngine.isLoaded[soundID] = true; // 로드 상태 설정 (true로 설정하여 로드 성공 표시)
	return true; // 로드 성공 반환
}

// 사운드 재생 함수
void SoundPlay(int soundID) 
{
	// FMOD 시스템이 초기화되지 않았거나, 유효하지 않은 사운드 ID인 경우 재생하지 않음
	if (g_SoundEngine.system == nullptr) return; // FMOD 시스템이 초기화되지 않은 경우 재생 실패

    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return; // 유효하지 않은 사운드 ID인 경우 재생 실패

	// 사운드가 로드되어 있고, 해당 사운드에 대한 채널이 없는 경우에만 재생 시도
	if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.soundContainer[soundID]) 
    {
		// 사운드 그룹에 해당하는 채널 그룹 가져오기
		SoundGroup group = g_SoundEngine.soundGroupMap[soundID]; // 사운드 ID에 해당하는 그룹 가져오기
		FMOD_CHANNELGROUP* targetGroup = g_SoundEngine.groupContainer[group]; // 그룹에 해당하는 채널 그룹 가져오기

		// 사운드 재생 (지정된 그룹에 속한 채널에서 재생, 일시정지 상태로 시작하지 않음)
        FMOD_System_PlaySound(g_SoundEngine.system, g_SoundEngine.soundContainer[soundID], targetGroup, false, &g_SoundEngine.channelContainer[soundID]);
    }
}

// 사운드 정지 함수
void SoundStop(int soundID)
{
    if (g_SoundEngine.system == nullptr) return; // FMOD 시스템이 초기화되지 않은 경우 정지 실패
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return; // 유효하지 않은 사운드 ID인 경우 정지 실패

	// 사운드가 로드되어 있고, 해당 사운드에 대한 채널이 존재하는 경우에만 정지 시도
    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.channelContainer[soundID])
    {
		FMOD_Channel_Stop(g_SoundEngine.channelContainer[soundID]); // 사운드 정지 (해당 채널에서 재생 중인 사운드 정지)
		g_SoundEngine.channelContainer[soundID] = nullptr; // 채널 컨테이너 초기화 (nullptr로 설정하여 빈 슬롯 표시)
    }
}

// 사운드 볼륨 설정 함수
void SoundSetVolume(int soundID, float volume)
{
	// FMOD 시스템이 초기화되지 않았거나, 유효하지 않은 사운드 ID인 경우 볼륨 설정하지 않음
    if (g_SoundEngine.system == nullptr) return; // FMOD 시스템이 초기화되지 않은 경우 볼륨 설정 실패
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return; // 유효하지 않은 사운드 ID인 경우 볼륨 설정 실패
    if (g_SoundEngine.isLoaded[soundID] && g_SoundEngine.channelContainer[soundID])
    {
		FMOD_Channel_SetVolume(g_SoundEngine.channelContainer[soundID], volume); // 사운드 볼륨 설정 (해당 채널에서 재생 중인 사운드의 볼륨을 volume 값으로 설정)
    }
}

// 사운드 그룹 볼륨 설정 함수
void SoundSetGroupVolume(SoundGroup group, float volume)
{
    if (g_SoundEngine.system == nullptr) return; // FMOD 시스템이 초기화되지 않은 경우 볼륨 설정 실패
    if (group < 0 || group >= SOUND_GROUP_MAX) return; // 유효하지 않은 사운드 그룹인 경우 볼륨 설정 실패
	if (g_SoundEngine.groupContainer[group]) // 해당 그룹의 채널 그룹이 존재하는 경우에만 볼륨 설정 시도
    {
        FMOD_ChannelGroup_SetVolume(g_SoundEngine.groupContainer[group], volume); // 사운드 그룹 볼륨 설정 (해당 그룹의 모든 채널 볼륨을 volume 값으로 설정)
    }
}

// 마스터 볼륨 설정 함수
void SoundSetMasterVolume(float volume)
{
	// FMOD 시스템이 초기화되지 않은 경우 볼륨 설정하지 않음
    if (g_SoundEngine.system == nullptr) return;

	// 마스터 채널 그룹 가져오기
    FMOD_CHANNELGROUP* masterGroup = nullptr; 

	// FMOD 시스템에서 마스터 채널 그룹 가져오기
	if (FMOD_System_GetMasterChannelGroup(g_SoundEngine.system, &masterGroup) == FMOD_OK) // 마스터 채널 그룹을 성공적으로 가져온 경우에만 볼륨 설정 시도
    {
		FMOD_ChannelGroup_SetVolume(masterGroup, volume); // 마스터 볼륨 설정 (마스터 채널 그룹의 볼륨을 volume 값으로 설정하여 전체 사운드 볼륨 조절)
    }
}

// 사운드 시스템 해제 함수
void SoundRelease()
{
	// 모든 사운드 해제
	for (int i = 0; i < MAX_SOUND_CHANNELS; i++) // 각 사운드 슬롯에 대해 반복
    {
		// 해당 슬롯에 사운드가 로드되어 있는 경우에만 해제 시도
        if (g_SoundEngine.isLoaded[i] && g_SoundEngine.soundContainer[i]) 
        {
			// 사운드 해제 (FMOD_Sound_Release 함수를 호출하여 해당 사운드 리소스 해제)
            FMOD_Sound_Release(g_SoundEngine.soundContainer[i]); //
			g_SoundEngine.soundContainer[i] = nullptr; // 슬롯 초기화 (nullptr로 설정하여 빈 슬롯 표시)
			g_SoundEngine.channelContainer[i] = nullptr; // 채널 컨테이너 초기화 (nullptr로 설정하여 빈 슬롯 표시)
            g_SoundEngine.isLoaded[i] = false; // 로드 상태 초기화 (false로 설정하여 로드되지 않은 상태 표시)
        }
    }

	// 모든 채널 그룹 해제
    if (g_SoundEngine.system)
    {
		FMOD_System_Close(g_SoundEngine.system); // FMOD 시스템 닫기 (모든 채널 그룹 및 사운드 리소스 해제)
		FMOD_System_Release(g_SoundEngine.system); // FMOD 시스템 해제 (FMOD 시스템 리소스 완전히 해제)
		g_SoundEngine.system = nullptr; // FMOD 시스템 포인터 초기화 (nullptr로 설정하여 초기화되지 않은 상태 표시)
    }
}

// 사운드 그룹 조회 함수 (사운드 ID에 해당하는 그룹 반환)
SoundGroup GetSoundGroup(int soundID) // 사운드 ID에 해당하는 그룹 반환 함수
{
    if (soundID < 0 || soundID >= MAX_SOUND_CHANNELS) return SOUND_GROUP_SFX; // 유효하지 않은 사운드 ID인 경우 기본값으로 효과음 그룹 반환
    return g_SoundEngine.soundGroupMap[soundID]; // 사운드 ID에 해당하는 그룹 반환
}