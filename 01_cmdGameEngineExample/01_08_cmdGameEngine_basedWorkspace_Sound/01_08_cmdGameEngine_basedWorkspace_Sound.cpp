// 01_08_cmdGameEngine_basedWorkspace_Sound.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include "ScreenBuffer.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "CmdUtil.h"
#include "Collision.h"
#include "ButtonObject.h"
#include "Numbers.h"
#include "SoundManager.h"
#include "../../00.cmdGameEngine/fmodSound_Lib/Header/fmod.h"
#pragma comment(lib, "../../00.cmdGameEngine/fmodSound_Lib/Library/x64/fmod_vc.lib")

#define SND_COIN 100

using namespace CmdUI;

extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;


bool bIsGameRun = false;

BUTTON* btnStart;
BUTTON* btnExit;

Rect player = { 10.0f, 5.0f, 4.0f, 2.0f };     // 플레이어
Rect wall = { 35.0f, 3.0f, 11.0f, 7.0f };     // 벽(장애물)

float moveSpeed = 20.0f;
unsigned short playerColor = WHITE; // 평소 흰색

float g_fCount = 0.f;


void Awake()
{
    _wsetlocale(LC_ALL, L"korean");
    SetConsoleTitle(L"Dj__Kim's Game Engine");
}

void Start()
{
    ScreenInit();
    InputInit();
    TimeInit(); // g_Time 초기화
    btnStart = CreateButton(15, 14, 20, 5, L"게임 시작", GRAY, YELLOW);
    btnExit = CreateButton(45, 14, 20, 5, L"종료 하기", GRAY, RED);

    SoundInit();
    SoundLoad(SND_COIN, "../../Sound/coin.mp3", SOUND_GROUP_SFX, false);

    bIsGameRun = true;
}

void FixedUpdate()
{
    /* 물리 연산 */
}

void Update()
{
    SoundUpdate();
    float prevX = player.x;
    float prevY = player.y;

    // deltaTime을 사용하여 프레임 독립적 이동 구현
    if (g_Input.keyState[VK::Left] == KEY_HOLD) player.x -= moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[VK::Right] == KEY_HOLD) player.x += moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[VK::Up] == KEY_HOLD) player.y -= moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[VK::Down] == KEY_HOLD) player.y += moveSpeed * g_Time.deltaTime;
    if (IsColliding(player, wall))
    {
        player.x = prevX;
        player.y = prevY;
        if ((g_fCount += moveSpeed * g_Time.deltaTime) >= 100) g_fCount = 0.f;
    }

    // 시작 버튼
    bool isClickStart = Update(btnStart, &g_Input); // 마우스 체크
    Rect startRect = GetButtonRect(btnStart);


    if (g_Input.keyState[VK::Return] == KEY_DOWN)  SoundPlay(SND_COIN);
    // 플레이어 충돌 체크
    if (IsColliding(player, startRect))
    {
        SetButtonHover(btnStart, true); // 충돌 후 색 변경
        // 엔터키 누르면 클릭한 것과 똑같이 처리
         isClickStart = true;
    }

    if (isClickStart)
    {                
        MessageBox(NULL, L"게임을 시작합니다!", L"알림", MB_OK);
    }


    // 종료 버튼
    bool isClickExit = Update(btnExit, &g_Input);
    Rect exitRect = GetButtonRect(btnExit);

    if (IsColliding(player, exitRect))
    {
        SetButtonHover(btnExit, true); // 색깔 켜기
        if (g_Input.keyState[1] == KEY_DOWN) isClickExit = true;
    }

    if (isClickExit)
    {
        bIsGameRun = false;
    }

}

void LateUpdate()
{
    // 화면 밖으로 못 나가게 가두기
    if (player.x < 0) player.x = 0;
    if (player.x > 75) player.x = 75;
    if (player.y < 0) player.y = 0;
    if (player.y > 23) player.y = 23;
}

void PreRender()
{
    ScreenClear();
    InitScreen(80, 25, 0x000F);
}

void Render()
{
    SetScreenColor(WHITE);
    ScreenPrint(g_Input.mouseX, g_Input.mouseY, L"+");

    DrawNumber((int)wall.x + 2, (int)wall.y + 1, (int)(g_fCount / 10), L"■", RED);
    DrawNumber((int)wall.x + 6, (int)wall.y + 1, ((int)g_fCount % 10), L"■", RED);
    DrawRect((int)wall.x, (int)wall.y, wall.w, wall.h, DGRAY); // 벽 (회색)
    DrawRect((int)player.x, (int)player.y, player.w, player.h, playerColor); // 플레이어

    Draw(btnStart);
    Draw(btnExit);
}

void PostRender()
{
    FPS_Gizmo(50, 21, &g_Time, &g_Input);
    ScreenFlipping();
}

void Release()
{
    SoundRelease();
    Release(btnStart);
    Release(btnExit);
    ScreenRelease();
}

int main(void)
{
    Awake();
    Start();

    while (bIsGameRun)
    {

        TimeUpdate(); // 시간 측정 및 누적


        InputUpdate(); // 입력 감지

        // 물리 연산 (FixedUpdate)고정 업데이트 할 시간인 동안 계속 반복
        while (IsFixedUpdateTime())
        {
            FixedUpdate();
        }

        // 게임 로직
        Update();
        LateUpdate();

        // 그리기
        PreRender();
        Render();
        PostRender();

        // 입력 반응시간 확보
        WaitForNextFrame();
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
