#pragma once
#include <string.h>
#include <wchar.h>
#include "ScreenBuffer.h"

namespace CmdUI
{
    //Text Type 정의
    enum TEXT_TYPE {
        TEXT_STATIC = 0, // Static Text //정적 문자열
        TEXT_DYNAMIC     // Dynamic Text //동적 문자열 
    };

    // 텍스트 구조체 정의

    typedef struct ___Text {
        int x, y;               // 위치
        unsigned short color;   // 색상
        TEXT_TYPE type;         // 정적/동적 타입 저장

        union {
            wchar_t* staticStr;      // STATIC: 딱 글자 크기만 메모리 할당
            wchar_t  dynamicStr[128]; // DYNAMIC: 128자 고정 버퍼 확보
        };
    } TEXT;

    TEXT* CreateText(int x, int y, const wchar_t* label, unsigned short color, TEXT_TYPE type = TEXT_STATIC);
    void SetText(TEXT* txt, const wchar_t* newLabel);
    void Draw(TEXT* txt);
    void Release(TEXT* txt);

    inline void SetColor(TEXT* txt, const int color) { txt->color = color; }
    inline TEXT_TYPE GetType(TEXT* txt) { return txt->type; }
}
