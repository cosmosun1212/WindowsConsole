#include "TextObject.h"
#include <stdlib.h> // malloc, free »ç¿ë

namespace CmdUI
{
    TEXT* CreateText(int x, int y, const wchar_t* label, unsigned short color)
    {
        TEXT* txt = (UI_TEXT*)malloc(sizeof(TEXT));
        txt->x = x;
        txt->y = y;
        txt->color = color;

        wcscpy_s(txt->text, label);

        return txt;
    }

    void DrawTextControl(TEXT* txt)
    {
        if (txt == NULL) return;

        SetColor(txt->color);
        ScreenPrint(txt->x, txt->y, txt->text);
    }

    void ReleaseText(TEXT* txt)
    {
        if (txt == NULL) return;
        free(txt);
    }
}
