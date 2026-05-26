#pragma once
#pragma once
#include <string.h>
#include <wchar.h>
#include <windows.h>
#include "InputManager.h" 

namespace CmdUI
{
    typedef struct ___Edit {
        int x, y;                    // 위치
        int width;                   // 입력창의 가로 길이
        int maxLen;                  // 입력 가능한 최대 글자 수
        unsigned short colorDefault; // 평상시 색상
        unsigned short colorHover;   // 마우스를 올렸을 때 색상
        unsigned short colorFocused; // 클릭해서 입력 중일 때(포커스) 색상

        wchar_t label[128];          // 상단에 표시될 제목
        wchar_t text[256];           // 실제로 입력된 텍스트 데이터
        int textLength;              // 현재 입력된 글자 수

        bool isHovered;              // 마우스 오버 여부
        bool isFocused;              // 현재 입력 활성화 여부
        bool isEncrypted;            // 비밀번호 모드 (*** 로 표시)

        DWORD lastBlinkTick;         // 커서 깜빡임 제어용 타이머
        bool showCursor;             // 현재 커서(_)가 보이는 상태인지
    } EDIT;

    EDIT* CreateEdit(int x, int y, int w, int maxLen, const wchar_t* label, bool encrypted, unsigned short colDef, unsigned short colHov, unsigned short colFoc);
    bool Update(EDIT* edit, const INPUTVALUE* input);
    bool WWWideInput(EDIT* edit);
    void Draw(EDIT* edit);
    void Release(EDIT* edit);
}