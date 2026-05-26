#include "CmdUtil.h"
#include "ScreenBuffer.h"

void DrawRect(int x, int y, int w, int h, unsigned short color)
{
    SetScreenColor(color); // 색상 설정

    const wchar_t* TOP_LEFT = L"┌";
    const wchar_t* TOP_RIGHT = L"┐";
    const wchar_t* BT_LEFT = L"└";
    const wchar_t* BT_RIGHT = L"┘";
    const wchar_t* HORIZ = L"─";
    const wchar_t* VERT = L"│";

    //윗줄 그리기
    ScreenPrint(x, y, TOP_LEFT);
    for (int i = 1; i < w - 1; i++) {
        ScreenPrint(x + i, y, HORIZ);
    }
    ScreenPrint(x + w - 1, y, TOP_RIGHT);

    //양옆 벽
    for (int j = 1; j < h - 1; j++) {
        ScreenPrint(x, y + j, VERT);         // 왼쪽 벽
        ScreenPrint(x + w - 1, y + j, VERT); // 오른쪽 벽
    }

    //아랫줄 그리기
    ScreenPrint(x, y + h - 1, BT_LEFT);
    for (int i = 1; i < w - 1; i++) {
        ScreenPrint(x + i, y + h - 1, HORIZ);
    }
    ScreenPrint(x + w - 1, y + h - 1, BT_RIGHT);

    // 색상을 기본(흰색)으로 되돌리기 (선택사항)
    SetScreenColor(0x0007);
}

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

    // 테두리 그리기 
    DrawRect(x, y, msgLen + 2, 3, GREEN);

    // 배경색 있는 글자 쓰기
    SetScreenColor(BK_DBLUE | WHITE);

    // 테두리 안쪽에 출력
    ScreenPrint(x + 1, y + 1, debugMsg);

    // 색상 사용 후 다시 기본(흰색)으로 돌려놓는 매너
    SetScreenColor(WHITE);
}