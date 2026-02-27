#include "CmdUtil.h"
#include "ScreenBuffer.h"



void DrawRect(int x, int y, int w, int h, unsigned short color)
{
    SetColor(color); // 색상 설정

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
    SetColor(0x0007);
}