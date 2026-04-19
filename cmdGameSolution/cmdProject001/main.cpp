#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include "../cmdComm/ScreenBuffer.h"
#include "../cmdComm/InputManager.h"
#include "../cmdComm/TimeManager.h"
#include "../cmdComm/CmdUtil.h"
#include "../cmdComm/GameUI.h"
#include "../cmdComm/Collision.h"
#include "../cmdComm/cmdUI/ButtonObject.h"

extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;

BUTTON* btnStart;
BUTTON* btnExit;

Rect player = { 10.0f, 5.0f, 4, 2 };     // 플레이어
Rect wall = { 35.0f, 3.0f, 10, 6 };     // 벽(장애물)

float moveSpeed = 20.0f;
unsigned short playerColor = 0x000F; // 평소 흰색


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
    btnStart = CreateButton(30, 10, 20, 5, L"게임 시작", 0x0007, 0x000E);
    btnExit = CreateButton(30, 17, 20, 5, L"종료 하기", 0x0007, 0x000C);
}

void FixedUpdate()
{
    /* 물리 연산 */
}

void Update()
{
    float prevX = player.x;
    float prevY = player.y;

    // deltaTime을 사용하여 프레임 독립적 이동 구현
    if (g_Input.keyState[2] == KEY_HOLD) player.x -= moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[3] == KEY_HOLD) player.x += moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[4] == KEY_HOLD) player.y -= moveSpeed * g_Time.deltaTime;
    if (g_Input.keyState[5] == KEY_HOLD) player.y += moveSpeed * g_Time.deltaTime;
    if (IsColliding(player, wall))
    {
        player.x = prevX;
        player.y = prevY;
    }

    // 시작 버튼
    bool isClickStart = UpdateButton(btnStart, &g_Input); // 마우스 체크
    Rect startRect = GetButtonRect(btnStart);

    // 플레이어 충돌 체크
    if (IsColliding(player, startRect))
    {
        SetButtonHover(btnStart, true); // 충돌 후 색 변경

        // 엔터키 누르면 클릭한 것과 똑같이 처리
        if (g_Input.keyState[1] == KEY_DOWN) isClickStart = true;
    }

    if (isClickStart)
    {
        MessageBox(NULL, L"게임을 시작합니다!", L"알림", MB_OK);
    }


    // 종료 버튼
    bool isClickExit = UpdateButton(btnExit, &g_Input);
    Rect exitRect = GetButtonRect(btnExit);

    if (IsColliding(player, exitRect))
    {
        SetButtonHover(btnExit, true); // 색깔 켜기
        if (g_Input.keyState[1] == KEY_DOWN) isClickExit = true;
    }

    if (isClickExit)
    {
        exit(0);
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
    SetColor(0x000F);
    ScreenPrint(g_Input.mouseX, g_Input.mouseY, L"+");
    
    DrawRect((int)wall.x, (int)wall.y, wall.w, wall.h, 0x0008); // 벽 (회색)
    DrawRect((int)player.x, (int)player.y, player.w, player.h, playerColor); // 플레이어

    DrawButton(btnStart);
    DrawButton(btnExit);
}

void PostRender()
{
    FPS_Gizmo(25, 0, &g_Time, &g_Input);
    ScreenFlipping();
}

void Release()
{
    ReleaseButton(btnStart);
    ReleaseButton(btnExit);
    ScreenRelease();
}

int main(void)
{
    Awake();
    Start();
    TimeInit();

    while (1)
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

        // 기다림을 통해 입력 반응시간 확보
        WaitForNextFrame();
    }
    Release();
    return 0;
}