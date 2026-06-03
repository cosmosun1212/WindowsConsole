// 01_08_cmdGameEngine_basedWorkspace_Sound.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

// 사운드 시스템을 활용한 간단한 콘솔 게임 예제입니다. 
// 플레이어가 벽과 충돌할 때 효과음이 재생되고, 버튼과의 상호작용도 포함되어 있습니다.
#include <stdio.h>  // 표준 입출력 라이브러리 
#include <windows.h> // Windows API 헤더 (콘솔 제어, 시간 관리 등)
#include <conio.h> // 콘솔 입출력 헤더 (키 입력 처리)
#include <time.h> // 시간 관련 함수 헤더 (게임 루프 타이밍 관리)
#include <locale.h> // 지역 설정 헤더 (한글 지원)
#include <fmod.h> // FMOD 사운드 라이브러리 헤더 (사운드 재생 및 관리)
#include "ScreenBuffer.h" // 화면 버퍼 관리 헤더 (화면 초기화 및 버퍼 조작)
#include "InputManager.h" // 입력 관리 헤더 (키보드 및 마우스 입력 처리)
#include "TimeManager.h" // 시간 관리 헤더 (게임 루프 타이밍 및 고정 업데이트 관리)
#include "CmdUtil.h" // 커맨드 유틸리티 헤더 (화면 그리기, FPS 표시 등)
#include "Collision.h" // 충돌 감지 헤더 (충돌 체크 함수)
#include "ButtonObject.h" // 버튼 UI 요소 헤더 (버튼 생성 및 업데이트 함수)
#include "Numbers.h"  // 숫자 출력 헤더 (숫자 그리기 함수)
#include "SoundManager.h" // 사운드 관리 헤더 (FMOD 초기화, 사운드 로드 및 재생 함수)

// 사운드 리스트 정의
enum SOUND_LIST { SND_BGM, SND_COIN, SND_GAMEOVER }; // 사운드 ID 열거형 (배경음, 효과음, 게임오버음 등)


using namespace CmdUI; // CmdUI 네임스페이스 사용 (버튼 관련 함수 및 구조체 포함)

// 외부 전역 변수 선언
extern INPUTVALUE g_Input; // 입력 상태를 담는 전역 변수 (키보드 및 마우스 상태)
extern TIMEMANAGER g_Time; // 시간 관리 구조체 (프레임 타이밍 및 고정 업데이트 관리)


bool bIsGameRun = false; // 게임 실행 여부 플래그

BUTTON* btnStart; /// 게임 시작 버튼
BUTTON* btnExit; /// 게임 종료 버튼

Rect player = { 10.0f, 5.0f, 4.0f, 2.0f }; /// 플레이어 (초기 위치와 크기)
Rect wall = { 35.0f, 3.0f, 11.0f, 7.0f }; // 벽 (충돌 대상)

float moveSpeed = 20.0f; // 플레이어 이동 속도 (픽셀/초)
unsigned short playerColor = WHITE; // 플레이어 색상 (흰색)

float g_fCount = 0.f; 

/// 프로그램 실행되고 게임 세팅 전에 실행되는 초기화 함수
void Awake() 
{
	_wsetlocale(LC_ALL, L"korean"); // 콘솔 모드에서 한글이 깨지는 문제 방지
	SetConsoleTitle(L"Dj__Kim's Game Engine"); // 콘솔 창 제목 설정
}

//게임 조기화 함수 
void Start()
{
	ScreenInit(); // 화면 초기화 (버퍼 생성 및 크기 설정)
	InputInit(); // 입력 시스템 초기화 (키보드, 마우스 상태 초기화)
	TimeInit(); // 시간 관리 시스템 초기화 (프레임 타이밍 및 고정 업데이트 설정)
	btnStart = CreateButton(15, 14, 20, 5, L"게임 시작", GRAY, YELLOW); // 시작 버튼 생성 (위치, 크기, 텍스트, 기본 색상, 하이라이트 색상)
    btnExit = CreateButton(45, 14, 20, 5, L"종료 하기", GRAY, RED); // 종료 버튼 생성 (위치, 크기, 텍스트, 기본 색상, 하이라이트 색상)

	SoundInit(); // 사운드 시스템 초기화 
    
    //FMOD 시스템 생성 및 채널 그룹 설정
    SoundLoad(SND_COIN, "../../SoundAsset/coin.mp3", SOUND_GROUP_SFX, false);

	bIsGameRun = true; // 게임 루프 플래그 활성화
}

// 고정된 시간 간격으로 물리 연산이나 게임 로직 업데이트가 필요한 경우 사용
void FixedUpdate()
{
    /* 물리 연산 */
}

// 매 프레임마다 게임 로직 업데이트
void Update()
{
    SoundUpdate(); // FMOD 오디오 스트리밍 업데이트 (절대 멈추면 안 됨)

	// 플레이어의 이전 위치 저장 (충돌 시 되돌리기 위해)
    float prevX = player.x;
    float prevY = player.y;

	/// 키 입력에 따른 플레이어 이동 처리 (키가 눌려있는 동안 지속적으로 이동)
    if (g_Input.keyState[VK::Left] == KEY_HOLD)  player.x -= moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[VK::Right] == KEY_HOLD) player.x += moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[VK::Up] == KEY_HOLD)    player.y -= moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[VK::Down] == KEY_HOLD)  player.y += moveSpeed * g_Time.deltaTime;

	// 플레이어와 벽의 충돌 체크
    if (IsColliding(player, wall))
    {
		// 충돌 시 플레이어를 이전 위치로 되돌리기
        player.x = prevX;
        player.y = prevY;

		// 충돌 효과로 카운트 증가 (0~99 사이에서 반복)
        if ((g_fCount += moveSpeed * g_Time.deltaTime) >= 100) g_fCount = 0.f;

		SoundPlay(SND_COIN); // 충돌 시 효과음 재생
    }


	bool isClickStart = Update(btnStart, &g_Input); // 버튼 업데이트 (마우스 위치에 따른 호버 상태 업데이트 및 클릭 감지)
	Rect startRect = GetButtonRect(btnStart); // 버튼의 충돌 영역 가져오기

    // 플레이어가 버튼에 충돌했는지 체크
    if (IsColliding(player, startRect))
    {
		SetButtonHover(btnStart, true); // 플레이어가 버튼 위에 있으면 호버 상태로 변경 (색상 변경)
    }

	// 플레이어가 버튼 위에 있고 엔터 키가 눌렸는지 체크
    if (g_Input.keyState[VK::Return] == KEY_DOWN)
    {
		isClickStart = true; // 엔터 키가 눌렸다면 클릭으로 간주
    }

	// 클릭이 감지되면 효과음 재생
    if (isClickStart)
    {
		SoundPlay(SND_COIN); // 클릭 시 효과음 재생

		isClickStart = false; // 클릭 상태 초기화 (한 번만 반응하도록)
    }

	// 종료 버튼 업데이트 및 충돌 체크
    bool isClickExit = Update(btnExit, &g_Input);
    Rect exitRect = GetButtonRect(btnExit);

    if (IsColliding(player, exitRect))
    {
        SetButtonHover(btnExit, true);
		// 플레이어가 종료 버튼 위에 있고 엔터 키가 눌렸는지 체크
        if (g_Input.keyState[VK::Return] == KEY_DOWN) isClickExit = true;
    }

	// 종료 버튼 클릭 시 게임 루프 종료
    if (isClickExit)
    {
        bIsGameRun = false;
    }
}

// 게임 로직 업데이트 후, 화면에 그리기 전에 실행되는 함수
void LateUpdate()
{
	// 플레이어가 화면 밖으로 나가지 않도록 위치 보정
    if (player.x < 0) player.x = 0;
    if (player.x > 75) player.x = 75;
    if (player.y < 0) player.y = 0;
    if (player.y > 23) player.y = 23;
}

// 화면에 그리기 전에 매 프레임마다 실행되는 함수
void PreRender()
{
	ScreenClear(); // 화면 버퍼 초기화 (이전 프레임의 잔상 제거)
	InitScreen(80, 25, WHITE); // 화면 버퍼 초기화 (80x25 크기, 기본 색상 설정)
}

// 실제로 화면에 그리는 함수 (매 프레임마다 호출)
void Render()
{
	// 디버그 정보 출력 (마우스 위치와 클릭 상태)
    SetScreenColor(WHITE);
    ScreenPrint(g_Input.mouseX, g_Input.mouseY, L"+");

	// 카운트 숫자 그리기 (벽과 충돌할 때마다 증가하는 카운트를 2자리로 표시)
    DrawNumber((int)wall.x + 2, (int)wall.y + 1, (int)(g_fCount / 10), L"■", RED);
    DrawNumber((int)wall.x + 6, (int)wall.y + 1, ((int)g_fCount % 10), L"■", RED);
    DrawRect((int)wall.x, (int)wall.y, wall.w, wall.h, DGRAY); // 벽 (회색)
    DrawRect((int)player.x, (int)player.y, player.w, player.h, playerColor); // 플레이어

	// 버튼 그리기
    Draw(btnStart);
    Draw(btnExit);
}

// 화면에 그린 후, 버퍼를 실제 화면에 출력하기 전에 매 프레임마다 실행되는 함수
void PostRender()
{
	FPS_Gizmo(49, 21, &g_Time, &g_Input); // FPS 및 입력 상태 표시 (화면 오른쪽 하단)
	ScreenFlipping(); // 화면 버퍼를 실제 화면에 출력 (플리핑)
}

// 프로그램 종료 시 리소스 해제 및 정리 작업을 수행하는 함수
void Release()
{
	SoundRelease(); // 사운드 시스템 리소스 해제
	Release(btnStart); // 시작 버튼 리소스 해제
	Release(btnExit); // 종료 버튼 리소스 해제
	ScreenRelease(); // 화면 버퍼 리소스 해제
}

// 프로그램 실행되고 게임 세팅 전에 실행되는 초기화 함수
int main(void)
{
	Awake(); // 초기화 함수 호출 (콘솔 설정 및 타이틀 설정)
	Start(); // 게임 초기화 함수 호출 (화면, 입력, 시간, 사운드 초기화 및 버튼 생성)

	// 게임 루프: 게임이 실행되는 동안 계속 반복
    while (bIsGameRun)
    {
		TimeUpdate(); // 시간 업데이트 (프레임 타이밍 계산 및 고정 업데이트 준비)
		InputUpdate(); // 입력 업데이트 (키보드, 마우스 상태 갱신)

        // 물리 연산 (FixedUpdate)고정 업데이트 할 시간인 동안 계속 반복
        while (IsFixedUpdateTime())
        {
			FixedUpdate(); // 고정된 시간 간격으로 물리 연산이나 게임 로직 업데이트가 필요한 경우 실행되는 함수
        }

		// 게임 로직 업데이트
		Update(); // 매 프레임마다 게임 로직 업데이트
		LateUpdate(); // 게임 로직 업데이트 후, 화면에 그리기 전에 실행되는 함수

		PreRender(); // 화면에 그리기 전에 매 프레임마다 실행되는 함수 (화면 버퍼 초기화)
		Render(); // 실제로 화면에 그리는 함수 (매 프레임마다 호출)
		PostRender(); // 화면에 그린 후, 버퍼를 실제 화면에 출력하기 전에 매 프레임마다 실행되는 함수 (FPS 표시 및 플리핑)

		WaitForNextFrame(); // 다음 프레임까지 대기 (프레임 타이밍 조절)
    }
	Release(); // 프로그램 종료 시 리소스 해제 및 정리 작업을 수행하는 함수
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
