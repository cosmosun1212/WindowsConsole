#include "CheckBoxObject.h"
#include "ScreenBuffer.h"
#include <stdlib.h>

namespace CmdUI
{
    CHECKBOX* CreateCheckBox(int x, int y, const wchar_t* label, bool defaultCheck, unsigned short colDef, unsigned short colHov)
    {
        CHECKBOX* chk = (CHECKBOX*)malloc(sizeof(CHECKBOX));
        chk->x = x;
        chk->y = y;
        chk->isChecked = defaultCheck;
        chk->isHovered = false;
        chk->colorDefault = colDef;
        chk->colorHover = colHov;
        wcscpy_s(chk->label, label);
        return chk;
    }

    bool UpdateCheckBox(CHECKBOX* chk, const INPUTVALUE* input)
    {
        if (chk == NULL || input == NULL) return false;

        int textLen = (int)wcslen(chk->label) * 2;

        chk->isHovered = (
            input->mouseX >= chk->x && 
            input->mouseX < chk->x + 5 + textLen &&
            input->mouseY == chk->y);

        if (chk->isHovered && input->isMouseLeftClick)
        {
            chk->isChecked = !(chk->isChecked);
            return true;
        }
        return false;
    }

    void DrawCheckBox(CHECKBOX* chk)
    {
        if (chk == NULL) return;

        unsigned short currentColor = chk->isHovered ? chk->colorHover : chk->colorDefault;
        SetColor(currentColor);

        // 체크 상태에 따라 박스 출력
        if (chk->isChecked) ScreenPrint(chk->x, chk->y, L"[■]");
        else                ScreenPrint(chk->x, chk->y, L"[ ]");

        // 텍스트 출력
        ScreenPrint(chk->x + 4, chk->y, chk->label);
    }

    void ReleaseCheckBox(CHECKBOX* chk)
    {
        if (chk == NULL) return;
        free(chk);
    }
}