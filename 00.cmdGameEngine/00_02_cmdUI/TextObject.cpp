#include "TextObject.h"
#include <stdlib.h> // malloc, free »ç¿ë

namespace CmdUI
{
    TEXT* CreateText(int x, int y, const wchar_t* label, unsigned short color, TEXT_TYPE type)
    {
        TEXT* txt = (TEXT*)malloc(sizeof(TEXT));
        if (txt == NULL) return NULL;

        txt->x = x;
        txt->y = y;
        txt->color = color;
        txt->type = type;

        if (type == TEXT_STATIC)
        {
            int len = (int)wcslen(label) + 1;
            txt->staticStr = (wchar_t*)malloc(sizeof(wchar_t) * len);

            if (txt->staticStr != NULL)
            {
                wcscpy_s(txt->staticStr, len, label);
            }
        }
        else if (type == TEXT_DYNAMIC)
        {
            wcscpy_s(txt->dynamicStr, 128, label);
        }

        return txt;
    }

    void SetText(TEXT* txt, const wchar_t* newLabel)
    {
        if (txt == NULL || newLabel == NULL) return;
        if (txt->type == TEXT_STATIC) 
        {
            wcscpy_s(txt->staticStr, sizeof(txt->staticStr), newLabel);
            return;
        }

        wcscpy_s(txt->dynamicStr, 128, newLabel);
    }

    void Draw(TEXT* txt)
    {
        if (txt == NULL) return;

        SetScreenColor(txt->color);

        if (txt->type == TEXT_STATIC) 
        {
            if (txt->staticStr) ScreenPrint(txt->x, txt->y, txt->staticStr);
        }
        else 
        {
            ScreenPrint(txt->x, txt->y, txt->dynamicStr);
        }
    }

    void Release(TEXT* txt)
    {
        if (txt == NULL) return;

        if (txt->type == TEXT_STATIC && txt->staticStr != NULL)
        {
            free(txt->staticStr);
        }

        free(txt);
    }
}