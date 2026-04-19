#pragma once
#include <string.h>
#include <wchar.h>
#include "InputManager.h"
#include "Collision.h"
namespace CmdUI
{
    typedef struct ___CheckBox {
        int x, y;                    // 위치
        unsigned short colorDefault; // 평상시 색상
        unsigned short colorHover;   // 커서가 올라갔을 때 색상
        wchar_t label[128];          // 설명 텍스트
        bool isChecked;              // 체크 여부
        bool isHovered;              // 커서 호버 여부
    } CHECKBOX;

    CHECKBOX* CreateCheckBox(int x, int y, const wchar_t* label, bool defaultCheck, unsigned short colDef, unsigned short colHov);
    bool UpdateCheckBox(CHECKBOX* chk, const INPUTVALUE* input);
    void DrawCheckBox(CHECKBOX* chk);
    void ReleaseCheckBox(CHECKBOX* chk);

    inline Rect GetCheckBoxRect(CHECKBOX* chk)
    {
        Rect r;
        int textLen = (int)wcslen(chk->label) * 2;
        r = { (float)chk->x, (float)chk->y, 5 + textLen, 1 };
        return r;
    }
    inline void SetCheckBoxHover(CHECKBOX* chk, bool isHover) { if (chk) chk->isHovered = isHover; }
}