#include "TextObject.h"
#include <stdlib.h> // malloc, free »ç¿ë

namespace CmdUI
{
    UI_TEXT* CreateText(int x, int y, const wchar_t* label, unsigned short color)
    {
        UI_TEXT* txt = (UI_TEXT*)malloc(sizeof(UI_TEXT));
        txt->x = x;
        txt->y = y;
        txt->color = color;

        wcscpy_s(txt->text, label);

        return txt;
    }

    void DrawTextControl(UI_TEXT* txt)
    {
        if (txt == NULL) return;

        SetColor(txt->color);
        ScreenPrint(txt->x, txt->y, txt->text);
    }

    void ReleaseText(UI_TEXT* txt)
    {
        if (txt == NULL) return;
        free(txt);
    }
}