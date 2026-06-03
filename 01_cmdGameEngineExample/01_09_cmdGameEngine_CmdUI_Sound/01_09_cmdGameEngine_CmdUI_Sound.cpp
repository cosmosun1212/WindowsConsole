// 01_09_cmdGameEngine_CmdUI_Sound.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h> // 표준 입출력 라이브러리
#include <windows.h> // Windows API 헤더 (콘솔 제어, 시간 관리 등)
#include <conio.h> // 콘솔 입출력 헤더 (키 입력 처리)
#include <time.h> // 시간 관련 함수 헤더 (게임 루프 타이밍 관리)
#include <locale.h> // 지역 설정 헤더 (한글 지원)
#include <fmod.h> // FMOD 사운드 라이브러리 헤더 (사운드 재생 및 관리)
#include "ScreenBuffer.h" // 화면 버퍼 관리 헤더 (화면 초기화 및 버퍼 조작)
#include "InputManager.h" // 입력 관리 헤더 (키보드 및 마우스 입력 처리)
#include "TimeManager.h" // 시간 관리 헤더 (게임 루프 타이밍 및 고정 업데이트 관리)
#include "Collision.h" // 충돌 감지 헤더 (충돌 체크 함수)
#include "CmdUtil.h" // 커맨드 유틸리티 헤더 (화면 그리기, FPS 표시 등)
#include "TextObject.h" // 텍스트 출력 헤더 (텍스트 그리기 함수)
#include "EditObject.h" // 에디트 박스 헤더 (텍스트 입력 필드)
#include "SliderObject.h" // 슬라이더 헤더 (볼륨 조절 등)
#include "CheckBoxObject.h" // 체크박스 헤더 (옵션 선택)
#include "ButtonObject.h" // 버튼 헤더 (클릭 가능한 UI 요소)
#include "SoundManager.h" // 사운드 관리 헤더 (FMOD 초기화, 사운드 로드 및 재생 함수)


using namespace CmdUI; // CmdUI 네임스페이스 사용 (버튼, 슬라이더, 체크박스 등 UI 요소 관련 함수 및 구조체 포함)

bool isModalOpen = false; // 모달 창 열림 여부 플래그
int count = 0;            // 게임 내에서 사용할 카운트 변수 (예: 클릭 횟수, 시간 경과 등)

// 외부 전역 변수 선언
extern INPUTVALUE g_Input; // 입력 상태를 담는 전역 변수 (키보드 및 마우스 상태)
extern TIMEMANAGER g_Time; // 시간 관리 구조체 (프레임 타이밍 및 고정 업데이트 관리)

bool bIsGameRun = false; // 게임 실행 여부 플래그
float totalPlayTime = 0.0f; // 게임이 시작된 후 경과한 총 시간 (초 단위) 

float g_BgmVolume = 0.5f; // 초기 볼륨 50%
float g_SfxVolume = 0.5f; // 초기 볼륨 50%
bool  g_IsMuted = false; // 사운드 음소거 여부 플래그

// 사운드 리스트 정의
enum SOUND_LIST { SND_BGM, SND_COIN, SND_GAMEOVER }; // 사운드 ID 열거형 (배경음, 효과음, 게임오버음 등)

// UI 요소 포인터 선언
TEXT* titleText = nullptr;          // 게임 제목 텍스트 UI 요소 포인터         
TEXT* volumeStatusText = nullptr;   // 볼륨 상태 텍스트 UI 요소 포인터
SLIDER* sfxVolumeSlider = nullptr;  // 효과음 볼륨 슬라이더 UI 요소 포인터
SLIDER* bgmVolumeSlider = nullptr;  // 배경음 볼륨 슬라이더 UI 요소 포인터
CHECKBOX* bgmMuteCheckBox = nullptr;// 배경음 음소거 체크박스 UI 요소 포인터
EDIT* edtID = nullptr;              // ID 입력 에디트 박스 UI 요소 포인터
BUTTON* playSfxButton = nullptr;    // 효과음 재생 버튼 UI 요소 포인터
BUTTON* exitButton = nullptr;       // 종료 버튼 UI 요소 포인터

// 프로그램 실행되고 게임 세팅 전에 실행되는 초기화 함수
void Awake()
{
    _wsetlocale(LC_ALL, L"korean"); // 콘솔 모드에서 한글이 깨지는 문제 방지
    SetConsoleTitle(L"Dj__Kim's Game Engine"); // 콘솔 창 제목 설정
}

// 게임 초기화 함수
void Start()
{
	ScreenInit(); // 화면 초기화 (버퍼 생성 및 크기 설정)
	InputInit(); // 입력 시스템 초기화 (키보드, 마우스 상태 초기화)
	TimeInit(); // 시간 관리 시스템 초기화 (프레임 타이밍 및 고정 업데이트 설정)
	SoundInit(); // 사운드 시스템 초기화 (FMOD 시스템 생성 및 채널 그룹 설정)

	// 사운드 로드 (사운드 ID, 파일 경로, 그룹, 스트리밍 여부)
	SoundLoad(SND_BGM, "../../SoundAsset/bgm.mp3", SOUND_GROUP_BGM, true); // 배경음 로드 (스트리밍 모드로 로드하여 메모리 사용 최적화)
	SoundLoad(SND_COIN, "../../SoundAsset/coin.mp3", SOUND_GROUP_SFX, false); // 효과음 로드 (메모리에 전체 로드하여 빠른 재생 가능)

	// 배경음 재생 (SND_BGM 사운드 ID로 재생 시작)
    SoundPlay(SND_BGM);

	// UI 요소 생성 (위치, 크기, 텍스트, 색상 등 설정)
	titleText = CreateText(10, 2, L"=== 게임 사운드 테스트 프로그램 ===", WHITE); // 게임 제목 텍스트 생성 (위치, 텍스트, 색상)
	volumeStatusText = CreateText(10, 4, L"배경 음악 소리: 50% [PLAYING]", YELLOW, CmdUI::TEXT_DYNAMIC); // 볼륨 상태 텍스트 생성 (위치, 초기 텍스트, 색상, 동적 텍스트 타입)
	bgmVolumeSlider = CreateSlider(10, 6, 20, L"배경 음악 소리", 0, 100, (int)(g_BgmVolume * 100), WHITE, YELLOW); // 배경음 볼륨 슬라이더 생성 (위치, 크기, 라벨, 최소값, 최대값, 초기값, 기본 색상, 호버 색상)
	sfxVolumeSlider = CreateSlider(10, 8, 20, L"효과음 소리", 0, 100, (int)(g_BgmVolume * 100), WHITE, YELLOW); // 효과음 볼륨 슬라이더 생성 (위치, 크기, 라벨, 최소값, 최대값, 초기값, 기본 색상, 호버 색상)
    bgmMuteCheckBox = CreateCheckBox(10, 12, L"게임 사운드 끄기", false, WHITE, CYAN); // 배경음 음소거 체크박스 생성 (위치, 라벨, 초기 상태, 기본 색상, 체크 색상)
    edtID = CreateEdit(10, 15, 20, 20, L"ID : ", false, WHITE, DYELLOW, BK_DYELLOW | BLACK); // ID 입력 에디트 박스 생성 (위치, 크기, 라벨, 비밀번호 모드, 기본 색상, 포커스 색상, 배경 색상)
    playSfxButton = CreateButton(10, 16, 15, 3, L"코인 소리", WHITE, CYAN); // 효과음 재생 버튼 생성 (위치, 크기, 텍스트, 기본 색상, 호버 색상)
    exitButton = CreateButton(28, 16, 8, 3, L"EXIT", WHITE, RED); // 종료 버튼 생성 (위치, 크기, 텍스트, 기본 색상, 호버 색상)

	bIsGameRun = true; // 게임 루프 플래그 활성화 (게임이 시작되었음을 나타냄)
}

// 게임 로직 업데이트 함수 (매 프레임마다 실행)
void Update()
{
	InputUpdate(); // 입력 상태 업데이트 (키보드 및 마우스 상태 갱신)
	TimeUpdate(); // 시간 업데이트 (프레임 타이밍 및 고정 업데이트 관리)
	SoundUpdate(); // 사운드 업데이트 (FMOD 시스템 업데이트)

	bool bBgmSliderChanged = false; // 배경음 볼륨 슬라이더 변경 여부 플래그
	bool bSfxSliderChanged = false; // 효과음 볼륨 슬라이더 변경 여부 플래그
	bool bMuteChanged = false; // 음소거 체크박스 변경 여부 플래그
	bool bEdtIDChanged = false;// ID 입력 에디트 박스 변경 여부 플래그
	bool bPlayClicked = false; // 효과음 재생 버튼 클릭 여부 플래그
	bool bExitClicked = false; // 종료 버튼 클릭 여부 플래그

	// UI 요소 업데이트 및 변경 감지 (각 UI 요소에 대해 Update 함수를 호출하여 상태 변경 여부 확인)
	if (bgmVolumeSlider)  bBgmSliderChanged = Update(bgmVolumeSlider, &g_Input); // 배경음 볼륨 슬라이더 업데이트 및 변경 감지
	if (sfxVolumeSlider)  bSfxSliderChanged = Update(sfxVolumeSlider, &g_Input); // 효과음 볼륨 슬라이더 업데이트 및 변경 감지
	if (bgmMuteCheckBox)  bMuteChanged = Update(bgmMuteCheckBox, &g_Input); // 배경음 음소거 체크박스 업데이트 및 변경 감지
	if (edtID)            bEdtIDChanged = Update(edtID, &g_Input); // ID 입력 에디트 박스 업데이트 및 변경 감지
	if (playSfxButton)    bPlayClicked = Update(playSfxButton, &g_Input); // 효과음 재생 버튼 업데이트 및 클릭 감지
	if (exitButton)       bExitClicked = Update(exitButton, &g_Input); // 종료 버튼 업데이트 및 클릭 감지

	// 볼륨 슬라이더가 변경되었거나 음소거 체크박스가 변경된 경우, 사운드 시스템에 새로운 볼륨 설정 적용
	if ((bBgmSliderChanged && bgmVolumeSlider) || (bSfxSliderChanged && sfxVolumeSlider)) // 볼륨 슬라이더가 변경되었는지 확인
    {
		// 슬라이더의 현재 값을 0.0f ~ 1.0f 범위로 변환하여 볼륨 설정
		g_BgmVolume = bgmVolumeSlider->currentValue / 100.f; // 배경음 볼륨 계산 (슬라이더 값이 0~100이므로 100으로 나누어 0.0~1.0 범위로 변환)
        g_SfxVolume = sfxVolumeSlider->currentValue / 100.f; // 효과음 볼륨 계산 (슬라이더 값이 0~100이므로 100으로 나누어 0.0~1.0 범위로 변환)
		if (!g_IsMuted) // 음소거 상태가 아니라면, 변경된 볼륨 설정을 사운드 시스템에 적용
        {
			SoundSetVolume(SND_BGM, g_BgmVolume); // 배경음 볼륨 설정 (SND_BGM 사운드 ID로 g_BgmVolume 값 적용)
			SoundSetGroupVolume(SOUND_GROUP_SFX, g_SfxVolume); // 효과음 그룹 볼륨 설정 (SOUND_GROUP_SFX 그룹에 g_SfxVolume 값 적용)
        }
    }

	// 음소거 체크박스가 변경된 경우, 사운드 시스템에 음소거 설정 적용
    if (bMuteChanged && bgmMuteCheckBox)
    {
		g_IsMuted = bgmMuteCheckBox->isChecked; // 음소거 여부 업데이트 (체크박스의 isChecked 상태로 g_IsMuted 플래그 설정)
        SoundSetVolume(SND_BGM, g_IsMuted ? 0.0f : g_BgmVolume); // 배경음 음소거 설정 (g_IsMuted가 true이면 0.0f, 아니면 g_BgmVolume)
        SoundSetGroupVolume(SOUND_GROUP_SFX, g_IsMuted ? 0.0f : g_SfxVolume); // 효과음 그룹 음소거 설정 (g_IsMuted가 true이면 0.0f, 아니면 g_SfxVolume)
    }

	// 효과음 재생 버튼이 클릭되었거나, 키보드의 특정 키가 눌렸을 때 효과음 재생
	if (bPlayClicked || (g_Input.keyState[VK::Esc] == KEY_DOWN)) // 효과음 재생 버튼이 클릭되었거나, ESC 키가 눌렸는지 확인
    {
        SoundPlay(SND_COIN); // 효과음 재생 (SND_COIN 사운드 ID로 효과음 재생)
    }

	// 종료 버튼이 클릭되었을 때 게임 루프 종료
	if (bExitClicked) // 종료 버튼이 클릭되었는지 확인
    {
		bIsGameRun = false; // 게임 루프 플래그 비활성화 (게임 종료)
    }

	// 볼륨 상태 텍스트 업데이트 (볼륨 슬라이더나 음소거 체크박스가 변경된 경우, 텍스트에 현재 볼륨과 상태 표시)
	if (volumeStatusText) // 볼륨 상태 텍스트가 존재하는 경우
    {
		wchar_t wszStatus[64]; // 상태 텍스트 버퍼 (현재 볼륨과 음소거 상태를 표시할 문자열)
		swprintf_s(wszStatus, L"BGM Volume: %d%%  [%s]", (int)(g_BgmVolume * 100), g_IsMuted ? L"MUTED" : L"PLAYING"); // 상태 텍스트 포맷팅 (현재 배경음 볼륨을 백분율로 표시하고, 음소거 상태에 따라 "MUTED" 또는 "PLAYING" 표시)
		SetText(volumeStatusText, wszStatus); // 볼륨 상태 텍스트 업데이트 (현재 볼륨과 음소거 상태를 표시하는 문자열로 업데이트)
    }
}

// 게임 로직 업데이트 후, 화면에 그리기 전에 실행되는 함수
void LateUpdate()
{

}

// 화면에 그리기 전에 매 프레임마다 실행되는 함수
void PreRender()
{
	ScreenClear(); // 화면 버퍼 초기화 (이전 프레임의 잔상 제거)
}

int currentTab = 0; // 현재 선택된 탭 인덱스 (0: BGM, 1: SFX, 2: Mute) - UI 요소 간 키보드 탐색에 사용

// 게임 화면에 그리는 함수 (매 프레임마다 호출)
void Render()
{
	if (titleText)        Draw(titleText);   // 게임 제목 텍스트 그리기 (화면에 게임 제목 표시)
	if (volumeStatusText) Draw(volumeStatusText); // 볼륨 상태 텍스트 그리기 (현재 볼륨과 음소거 상태 표시)
	if (bgmVolumeSlider)  Draw(bgmVolumeSlider); // 배경음 볼륨 슬라이더 그리기 (화면에 배경음 볼륨 조절 슬라이더 표시)
	if (sfxVolumeSlider)  Draw(sfxVolumeSlider); // 효과음 볼륨 슬라이더 그리기 (화면에 효과음 볼륨 조절 슬라이더 표시)
	if (bgmMuteCheckBox)  Draw(bgmMuteCheckBox); // 배경음 음소거 체크박스 그리기 (화면에 배경음 음소거 옵션 표시)
	if (edtID)            Draw(edtID); 	// ID 입력 에디트 박스 그리기 (화면에 ID 입력 필드 표시)
	if (playSfxButton)    Draw(playSfxButton); // 효과음 재생 버튼 그리기 (화면에 효과음 재생 버튼 표시)
	if (exitButton)       Draw(exitButton); // 종료 버튼 그리기 (화면에 종료 버튼 표시)
}

// 디버그 정보 출력 및 화면 버퍼 플리핑 (화면에 그린 내용을 실제로 표시)
void PostRender()
{
	wchar_t dbgMsg[100]; // 디버그 메시지 버퍼 (마우스 위치와 클릭 상태를 표시할 문자열)

	// 디버그 메시지 포맷팅 (현재 마우스 X, Y 위치와 왼쪽 클릭 상태를 문자열로 포맷팅)
    swprintf_s(dbgMsg, L"MouseX: %d | MouseY: %d | Click: %d", g_Input.mouseX, g_Input.mouseY, g_Input.isMouseLeftClick); 

	SetScreenColor(YELLOW); // 디버그 메시지 색상 설정 (노란색)
	ScreenPrint(0, 0, dbgMsg); // 디버그 메시지 출력 (화면 좌측 상단에 마우스 위치와 클릭 상태 표시)

	ScreenFlipping(); // 화면 버퍼 플리핑 (화면에 그린 내용을 실제로 표시)
}

// 프로그램 종료 시 리소스 해제 함수
void Release()
{
	if (titleText)        Release(titleText); // 게임 제목 텍스트 리소스 해제 (메모리 해제 및 정리)
	if (volumeStatusText) Release(volumeStatusText); // 볼륨 상태 텍스트 리소스 해제 (메모리 해제 및 정리)
	if (bgmVolumeSlider)  Release(bgmVolumeSlider); // 배경음 볼륨 슬라이더 리소스 해제 (메모리 해제 및 정리)
    if (sfxVolumeSlider)  Release(sfxVolumeSlider); // 효과음 볼륨 슬라이더 리소스 해제 (메모리 해제 및 정리)
    if (bgmMuteCheckBox)  Release(bgmMuteCheckBox); // 배경음 음소거 체크박스 리소스 해제 (메모리 해제 및 정리)
    if (edtID)            Release(edtID); // ID 입력 에디트 박스 리소스 해제 (메모리 해제 및 정리)
    if (playSfxButton)    Release(playSfxButton); // 효과음 재생 버튼 리소스 해제 (메모리 해제 및 정리)
    if (exitButton)       Release(exitButton); // 종료 버튼 리소스 해제 (메모리 해제 및 정리)
    SoundRelease(); // 사운드 시스템 리소스 해제 (메모리 해제 및 정리)
    ScreenRelease(); // 화면 시스템 리소스 해제 (메모리 해제 및 정리)
}

// 프로그램의 진입점인 main 함수
int main(void)
{
	Awake(); // 초기화 함수 호출 (콘솔 설정 및 타이틀 설정)
	Start(); // 게임 초기화 함수 호출 (화면, 입력, 시간, 사운드 초기화 및 UI 요소 생성)

	// 게임 루프 (게임이 실행되는 동안 계속 반복)
    while (bIsGameRun)
    {
		Update(); // 게임 로직 업데이트 (입력 처리, 시간 업데이트, 사운드 업데이트, UI 상태 업데이트 등)
		LateUpdate(); // 게임 로직 업데이트 후, 화면에 그리기 전에 실행되는 함수 (예: 위치 보정, 상태 체크 등)

		PreRender(); // 화면에 그리기 전에 매 프레임마다 실행되는 함수 (화면 버퍼 초기화 등)
		Render(); // 게임 화면에 그리는 함수 (UI 요소 그리기 등)
		PostRender(); // 디버그 정보 출력 및 화면 버퍼 플리핑 (화면에 그린 내용을 실제로 표시)

		WaitForNextFrame(); // 다음 프레임까지 대기 (프레임 타이밍 조절)

		totalPlayTime += g_Time.deltaTime; // 게임이 시작된 후 경과한 총 시간 업데이트 (g_Time.deltaTime은 이번 프레임의 경과 시간)
    }
	Release(); // 프로그램 종료 시 리소스 해제 함수 호출 (메모리 해제 및 정리)
	return 0; // 프로그램 정상 종료
}
// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
