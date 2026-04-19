#pragma once
#include <string.h>
#include <wchar.h>
#include "ScreenBuffer.h"

namespace CmdUI
{
    // 텍스트 구조체 정의
    typedef struct ___UIText {
        int x, y;               // 위치
        unsigned short color;   // 색상
        wchar_t text[128];      // 출력할 문자열
    } UI_TEXT;

    UI_TEXT* CreateText(int x, int y, const wchar_t* label, unsigned short color);
    void DrawTextControl(UI_TEXT* txt);
    void ReleaseText(UI_TEXT* txt);
}
