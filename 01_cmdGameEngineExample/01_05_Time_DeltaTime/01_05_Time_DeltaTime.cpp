// 01_05_Time_DeltaTime.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include <windows.h>
#include <stdio.h>
#include "InputManager.h"
#include "ScreenBuffer.h"
#include "TimeManager.h"
#include "CmdUtil.h"
#include "ButtonObject.h"

using namespace CmdUI;

extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;

// UI 및 제어 변수
BUTTON* btnFrameLimit = nullptr;
wchar_t strMessage[128];
wchar_t strPlayerInfo[128];
bool bIsLimitsButton = false; 

float playerX = 40.0f;
float playerY = 10.0f;
float moveSpeed = 25.0f; 

int main()
{
    ScreenInit(80, 25);
    InputInit();
    TimeInit(); 

    btnFrameLimit = CreateButton(30, 18, 20, 3, L"   Limit: free    ", YELLOW, BK_DYELLOW | BLACK);

    while (g_Input.keyState[VK::Esc] != KEY_DOWN)
    {
        TimeUpdate();
        InputUpdate();

        if (Update(btnFrameLimit, &g_Input))
        {
            if (bIsLimitsButton)
            {
                wcscpy_s(btnFrameLimit->text, L"   Limit: free    ");
            }
            else
            {
                wcscpy_s(btnFrameLimit->text, L"   Limit: 30fps   ");
            }
            bIsLimitsButton = !bIsLimitsButton; // 상태 스위칭
        }


        if (g_Input.keyState[VK::Up]    == KEY_DOWN || g_Input.keyState[VK::Up]    == KEY_HOLD) playerY -= moveSpeed * ((btnFrameLimit)?g_Time.deltaTime:1);
        if (g_Input.keyState[VK::Down]  == KEY_DOWN || g_Input.keyState[VK::Down]  == KEY_HOLD) playerY += moveSpeed * ((btnFrameLimit)?g_Time.deltaTime:1);
        if (g_Input.keyState[VK::Left]  == KEY_DOWN || g_Input.keyState[VK::Left]  == KEY_HOLD) playerX -= moveSpeed * ((btnFrameLimit)?g_Time.deltaTime:1);
        if (g_Input.keyState[VK::Right] == KEY_DOWN || g_Input.keyState[VK::Right] == KEY_HOLD) playerX += moveSpeed * ((btnFrameLimit)?g_Time.deltaTime:1);


        if (playerX < 0.0f)  playerX = 0.0f;
        if (playerX > 78.0f) playerX = 78.0f;
        if (playerY < 4.0f)  playerY = 4.0f;  
        if (playerY > 24.0f) playerY = 24.0f;

        ScreenClear();

        swprintf_s(strMessage, L" FPS: %02d | DeltaTime: %f 초 | Mode: %s ",
            g_Time.fps, g_Time.deltaTime, bIsLimitsButton ? L"30FPS" : L"Free");
        int msgLen = (int)wcslen(strMessage);

        DrawRect(0, 0, msgLen + 2, 3, GREEN);
        SetScreenColor(0x001F);
        ScreenPrint(1, 1, strMessage);

        // 플레이어 정밀 좌표계 정보 디버깅 출력
        swprintf_s(strPlayerInfo, L" Player X: %05.2f | Y: %05.2f (방향키로 시합 테스트)", playerX, playerY);
        SetScreenColor(YELLOW);
        ScreenPrint(0, 3, strPlayerInfo);

        SetScreenColor(0x000B);
        ScreenPrint((int)playerX, (int)playerY, L"㉿");

        SetScreenColor(WHITE);
        Draw(btnFrameLimit);

        ScreenFlipping();

        if (bIsLimitsButton) WaitForNextFrame(); 
        else Sleep(1);
    }

    if (btnFrameLimit) Release(btnFrameLimit);
    ScreenRelease();

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
