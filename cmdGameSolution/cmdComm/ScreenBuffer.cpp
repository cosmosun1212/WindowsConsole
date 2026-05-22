#include "ScreenBuffer.h"
#include <windows.h>
#include <stdio.h> // sprintf_s 사용을 위해 추가

static int g_nScreenIndex;
static HANDLE g_hScreen[2];
static int g_wColor = 0x0007; // 기본 색상: 흰색

static int g_nScreenWidth = DEFAULT_SCREEN_WIDTH;
static int g_nScreenHeight = DEFAULT_SCREEN_HEIGHT;

void ScreenInit(int width, int height)
{
    // 입력받은 크기를 전역 변수에 저장
    g_nScreenWidth = width;
    g_nScreenHeight = height;

    // 콘솔 창 크기 명령 동적 생성 (예: "mode con: cols=120 lines=50")
    char cmd[64];
    sprintf_s(cmd, "mode con: cols=%d lines=%d", g_nScreenWidth, g_nScreenHeight);
    system(cmd);

    CONSOLE_CURSOR_INFO cci;
    COORD size = { (SHORT)g_nScreenWidth, (SHORT)g_nScreenHeight };
    SMALL_RECT rect = { 0, 0, (SHORT)(g_nScreenWidth - 1), (SHORT)(g_nScreenHeight - 1) };

    // 2버퍼 생성
    g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    // 버퍼 크기와 윈도우 크기를 강제로 맞춤
    for (int i = 0; i < 2; i++)
    {
        SetConsoleScreenBufferSize(g_hScreen[i], size); // 화면에 맞추어 버퍼 크기 고정
        SetConsoleWindowInfo(g_hScreen[i], TRUE, &rect); // 창 크기 고정

        // 커서 숨기기
        cci.dwSize = 1;
        cci.bVisible = FALSE;
        SetConsoleCursorInfo(g_hScreen[i], &cci);
    }
}

void ScreenFlipping()
{
    SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
    g_nScreenIndex = !g_nScreenIndex;
}

void ScreenClear()
{
    COORD Coor = { 0, 0 };
    DWORD dw;

    int totalSize = g_nScreenWidth * g_nScreenHeight;

    FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', totalSize, Coor, &dw);
    FillConsoleOutputAttribute(g_hScreen[g_nScreenIndex], 0x0007, totalSize, Coor, &dw);
}

void ScreenRelease()
{
    CloseHandle(g_hScreen[0]);
    CloseHandle(g_hScreen[1]);
}

void ScreenPrint(int x, int y, const wchar_t* string)
{
    DWORD dw;
    COORD CursorPosition = { (SHORT)x, (SHORT)y };
    DWORD len = (DWORD)wcslen(string);
    DWORD vlen = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (string[i] > 0x007F)
        {
            vlen += 2;
        }
        else
        {
            vlen += 1;
        }
    }
    SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition);
    WriteConsoleOutputCharacterW(g_hScreen[g_nScreenIndex], string, len, CursorPosition, &dw);
    FillConsoleOutputAttribute(g_hScreen[g_nScreenIndex], g_wColor, len, CursorPosition, &dw);
    FillConsoleOutputAttribute(g_hScreen[g_nScreenIndex], g_wColor, vlen, CursorPosition, &dw);
}

void SetColor(unsigned short color)
{
    g_wColor = color;
}

int GetScreenWidth()
{
    return g_nScreenWidth;
}

int GetScreenHeight()
{
    return g_nScreenHeight;
}
