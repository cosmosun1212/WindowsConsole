// 01_04_Time_FrameLimit.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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

BUTTON* btnFrameLimit = nullptr;
bool bIsLimitsButton = false;
wchar_t strMessage[128];

bool g_bEnableLimit = false;

int main()
{
    ScreenInit(80, 25);
    InputInit();
    TimeInit();

    btnFrameLimit = CreateButton(35, 15, 20, 3, L"  Click Me!!!     ", YELLOW, BK_DYELLOW | BLACK);


    while (g_Input.keyState[VK::Esc] != KEY_DOWN) // ESC 누르면 종료
    {
        TimeUpdate();
        InputUpdate();

        if (Update(btnFrameLimit, &g_Input))
        {
            if (bIsLimitsButton)    wcscpy_s(btnFrameLimit->text, L"  Limit: free     ");
            else wcscpy_s(btnFrameLimit->text, L"  Limit: 30fps    ");
            bIsLimitsButton = !bIsLimitsButton;  //토글
        }

        ScreenClear();

        swprintf_s(strMessage, L" FPS: %02d | Mouse: [%02d, %02d] ",
            g_Time.fps, g_Input.mouseX, g_Input.mouseY);

        int msgLen = (int)wcslen(strMessage);

        DrawRect(0, 0, msgLen + 2, 3, GREEN);

        SetScreenColor(0x001F);

        ScreenPrint(0 + 1, 0 + 1, strMessage);

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
