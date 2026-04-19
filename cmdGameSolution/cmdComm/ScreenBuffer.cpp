#include "ScreenBuffer.h"
#include <windows.h>


static int g_nScreenIndex;
static HANDLE g_hScreen[2];
static int g_wColor = 0x0007; // 기본 색상: 흰색



void ScreenInit()
{
    // 콘솔 창 크기 명령
    system("mode con: cols=80 lines=25");

    CONSOLE_CURSOR_INFO cci;
    COORD size = { SCREEN_WIDTH, SCREEN_HEIGHT };
    SMALL_RECT rect = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };

    // 2버퍼 생성
    g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    // 버퍼 크기와 윈도우 크기를 강제로 맞춤
    for (int i = 0; i < 2; i++)
    {
        SetConsoleScreenBufferSize(g_hScreen[i], size); // 화면에 맞추어 버퍼 크기 고정하면 자연스럽게 스크롤바 제거
        SetConsoleWindowInfo(g_hScreen[i], TRUE, &rect); // 창 크기 고정

        // 커서 숨기기
        cci.dwSize = 1;
        cci.bVisible = FALSE;
        SetConsoleCursorInfo(g_hScreen[i], &cci);
    }
}

void ScreenFlipping()
{
	//Sleep(10);
	SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
	g_nScreenIndex = !g_nScreenIndex;
}

void ScreenClear()
{
	COORD Coor = { 0, 0 };
	DWORD dw;
	FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', 80 * 25, Coor, &dw);
	FillConsoleOutputAttribute(g_hScreen[g_nScreenIndex], 0x0007, 80 * 25, Coor, &dw);
}

void ScreenRelease()
{
	CloseHandle(g_hScreen[0]);
	CloseHandle(g_hScreen[1]);
}


void ScreenPrint(int x, int y, const wchar_t* string)
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	int len = (DWORD)wcslen(string);
    int vlen = 0;

    // 특정 문자들은 화면 버퍼를 2칸 차지하므로, 실제로 출력되는 칸 수를 대략적으로 계산
    for (int i = 0; i < len; i++)
    {
		// 한글 음절과 자모는 2칸으로 간주
        if ((string[i] >= 0xAC00 && string[i] <= 0xD7A3) || (string[i] >= 0x1100 && string[i] <= 0x11FF))
        {
            vlen += 2;
        }
		else // 나머지는 1칸으로 간주
        {
            vlen += 1;
		}
    }

	SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition);
	WriteConsoleOutputCharacterW(g_hScreen[g_nScreenIndex], string, len, CursorPosition, &dw);
	FillConsoleOutputAttribute(g_hScreen[g_nScreenIndex], g_wColor, vlen, CursorPosition, &dw);
}

void SetColor(unsigned short color)
{
	g_wColor = color;
}