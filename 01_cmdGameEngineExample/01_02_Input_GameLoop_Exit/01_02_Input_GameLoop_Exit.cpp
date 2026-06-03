// 01_02_Input_GameLoop_Exit.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

// 키보드 매핑된 키 : VK_SPACE=0, VK_RETURN=1, VK_LEFT=2, VK_RIGHT=3, VK_UP=4, VK_DOWN=5 
// 키보드 동작 : KEY_DOWN, KEY_HOLD, KEY_UP

#include <windows.h>
#include "ScreenBuffer.h"
#include "InputManager.h"

extern INPUTVALUE g_Input;

int x = 0, y = 0;

int main()
{
    ScreenInit();
    InputInit();

    while (g_Input.keyState[VK::Esc] != KEY_DOWN)
    {
        InputUpdate();

        if (g_Input.keyState[VK::Up] == KEY_STATE::KEY_DOWN)    y -= 1;
        else
        if (g_Input.keyState[VK::Down] == KEY_STATE::KEY_DOWN)   y += 1;
        else
        if (g_Input.keyState[VK::Left] == KEY_STATE::KEY_DOWN ||
            g_Input.keyState[VK::Left] == KEY_STATE::KEY_HOLD)   x -= 1;
        else
        if (g_Input.keyState[VK::Right] == KEY_STATE::KEY_DOWN ||
            g_Input.keyState[VK::Right] == KEY_STATE::KEY_HOLD)  x += 1;


        ScreenClear();

        ScreenPrint(30 + x, 10 + y, L"서울디지텍고등학교");
        ScreenPrint(30 + x, 11 + y, L"2학년 0반 0번 홍길동");

        SetScreenColor(BK_DRED | YELLOW);
        ScreenPrint(0, 0, L"화살표 키보드 [←↑→↓ : Moving , Esc : 종료]");
        SetScreenColor(GRAY);
        ScreenFlipping();

        Sleep(100);
    }

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
