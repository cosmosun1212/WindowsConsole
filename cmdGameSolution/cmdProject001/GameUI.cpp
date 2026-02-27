#include <stdio.h>
#include <string.h> // wcslen 사용을 위해 필요
#include "GameUI.h"
#include "CmdUtil.h"
#include "ScreenBuffer.h"
#include "TimeManager.h"
#include "InputManager.h"

void InitScreen(int cols, int rows, int color)
{
	DrawRect(0, 0, cols, rows, color);
}

void FPS_Gizmo(int x, int y, TIMEMANAGER* pTime, INPUTVALUE* pInput)
{
    // 안전장치
    if (pTime == NULL || pInput == NULL) return;

    wchar_t debugMsg[100];

    // 정보 가독성을 위해 양옆에 공백(스페이스바)을 넣어서 배경색이 좀 더 넓게 칠해지도록 함
    swprintf_s(debugMsg, L" FPS: %02d | Mouse: [%02d, %02d] ",
        pTime->fps,
        pInput->mouseX,
        pInput->mouseY
    );

    int msgLen = (int)wcslen(debugMsg);

    // 테두리 그리기  0x000A: 배경 검정(0) + 글자 밝은 초록(A) -> Matrix 느낌!
    DrawRect(x, y, msgLen + 2, 3, 0x000A);

    // 배경색 있는 글자 쓰기
    // 0x001F: 배경 파랑(1) + 글자 밝은 흰색(F) -> DOS 시절의 근본 색상
    SetColor(0x001F);

    // 테두리 안쪽에 출력
    ScreenPrint(x + 1, y + 1, debugMsg);

    // 색상 사용 후 다시 기본(흰색)으로 돌려놓는 매너
    SetColor(0x000F);
}