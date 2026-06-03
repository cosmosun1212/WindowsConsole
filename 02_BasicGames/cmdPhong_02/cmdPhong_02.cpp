// 02_cmdPhong_Complete.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include "WorkSpace.h"

GameState currentState = STATE_MENU;
GameState previousState = STATE_MENU;
Difficulty currentDiff = DIFF_NORMAL;

bool bIsGameRun = false;
float totalPlayTime = 0.0f;

float g_BgmVolume = 0.5f;
float g_SfxVolume = 0.5f;
bool g_IsMuted = false;

Rect playerPaddle = { 2.0f, 10.0f, 2, 6 };
Rect enemyPaddle = { 76.0f, 10.0f, 2, 6 };
Rect ball = { 40.0f, 12.0f, 3, 2 };

float paddleSpeed = 25.0f;
float aiSpeed = 10.0f;
float ballSpeedX = 30.0f;
float ballSpeedY = 15.0f;

int playerScore = 0;
int enemyScore = 0;
int playerSkin = 0;

TEXT* txtMenuTitle = nullptr;
BUTTON* btnStart = nullptr;
BUTTON* btnSetting = nullptr;
BUTTON* btnExit = nullptr;

TEXT* txtSettingTitle = nullptr;
TEXT* volumeStatusText = nullptr;
SLIDER* bgmVolumeSlider = nullptr;
SLIDER* sfxVolumeSlider = nullptr;
CHECKBOX* bgmMuteCheckBox = nullptr;
BUTTON* btnPrevSkin = nullptr;
BUTTON* btnNextSkin = nullptr;
BUTTON* btnTestSFX = nullptr;
BUTTON* btnBack = nullptr;

int main(void)
{
    Awake();
    Start();

    while (bIsGameRun)
    {
        Update();
        LateUpdate();

        PreRender();
        Render();
        PostRender();

        WaitForNextFrame();
        totalPlayTime += g_Time.deltaTime;
    }

    Release();
    return 0;
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
