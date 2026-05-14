#include "EditObject.h"
#include "ScreenBuffer.h"
#include <stdlib.h>

namespace cmdUI
{
    EDIT* CreateEdit(int x, int y, int w, int maxLen, const wchar_t* label, bool encrypted, unsigned short colDef, unsigned short colHov, unsigned short colFoc)
    {
        EDIT* edit = (EDIT*)malloc(sizeof(EDIT));
        edit->x = x;
        edit->y = y;
        edit->width = w;
        edit->maxLen = maxLen;
        edit->colorDefault = colDef;
        edit->colorHover = colHov;
        edit->colorFocused = colFoc;
        
        wcscpy_s(edit->label, label);
        edit->text[0] = L'\0'; // 빈 문자열로 초기화
        edit->textLength = 0;
        
        edit->isHovered = false;
        edit->isFocused = false;
        edit->isEncrypted = encrypted;
        
        edit->lastBlinkTick = GetTickCount();
        edit->showCursor = false;
        
        return edit;
    }

    bool UpdateEdit(EDIT* edit, const INPUTVALUE* input)
    {
        if (edit == NULL || input == NULL) return false;

        edit->isHovered = (input->mouseX >= edit->x && input->mouseX < edit->x + edit->width &&
                           input->mouseY == edit->y);

        if (input->isMouseLeftClick)
        {
            if (edit->isHovered) edit->isFocused = true;  
            else                 edit->isFocused = false;
        }

        if (edit->isFocused)
        {
            if ((GetAsyncKeyState(VK_RETURN) & 0x0001) || (GetAsyncKeyState(VK_ESCAPE) & 0x0001)) 
            {
                edit->isFocused = false;
                return true; 
            }

            if (GetAsyncKeyState(VK_BACK) & 0x0001)
          {
                if (edit->textLength > 0) 
                {
                    edit->textLength--;
                    edit->text[edit->textLength] = L'\0';
                }
            }

            // Ctrl + V (클립보드 붙여넣기)
            if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState('V') & 0x0001))
              {
                if (OpenClipboard(NULL)) {
                    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
                    if (hData) {
                        wchar_t* clipText = (wchar_t*)GlobalLock(hData);
                        if (clipText) {
                            while (*clipText && edit->textLength < edit->maxLen) {
                                edit->text[edit->textLength++] = *clipText;
                                clipText++;
                            }
                            edit->text[edit->textLength] = L'\0';
                            GlobalUnlock(hData);
                        }
                    }
                    CloseClipboard();
                }
            }

            if (edit->textLength < edit->maxLen)
            {
                if (GetAsyncKeyState(VK_SPACE) & 0x0001) 
                {
                    edit->text[edit->textLength++] = L' ';
                    edit->text[edit->textLength] = L'\0';
                }
                for (int i = '0'; i <= '9'; i++) 
                {
                    if (GetAsyncKeyState(i) & 0x0001)
                    {
                        edit->text[edit->textLength++] = (wchar_t)i;
                        edit->text[edit->textLength] = L'\0';
                    }
                }
                for (int i = 'A'; i <= 'Z'; i++) 
                {
                    if (GetAsyncKeyState(i) & 0x0001) 
                    {
                        bool isShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                        wchar_t ch = isShift ? (wchar_t)i : (wchar_t)(i + 32); // 소문자 변환
                        edit->text[edit->textLength++] = ch;
                        edit->text[edit->textLength] = L'\0';
                    }
                }
            }
        }
        return false;
    }

    void DrawEdit(EDIT* edit)
    {
        if (edit == NULL) return;

        unsigned short currentColor = edit->colorDefault;
        if (edit->isFocused) currentColor = edit->colorFocused;
        else if (edit->isHovered) currentColor = edit->colorHover;

        SetColor(edit->colorDefault);
        ScreenPrint(edit->x, edit->y - 1, edit->label);

        SetColor(currentColor);
        for (int i = 0; i < edit->width; i++) 
        {
            ScreenPrint(edit->x + i, edit->y, L" ");
        }

        wchar_t drawText[256] = L"";
        if (edit->isEncrypted)
        {
            for (int i = 0; i < edit->textLength; i++) drawText[i] = L'*';
            drawText[edit->textLength] = L'\0';
        }
        else
        {
            wcscpy_s(drawText, edit->text);
        }

        if (edit->isFocused) 
        {
            if (GetTickCount() - edit->lastBlinkTick > 500)
            {
                edit->showCursor = !edit->showCursor;
                edit->lastBlinkTick = GetTickCount();
            }
            if (edit->showCursor && edit->textLength < edit->maxLen) 
            {
                wcscat_s(drawText, L"_");
            }
        } 
        else
        {
            edit->showCursor = false;
        }

        int drawLen = (int)wcslen(drawText);
        int startIdx = 0;
        if (drawLen > edit->width) 
        {
            startIdx = drawLen - edit->width;
        }

        SetColor(currentColor); 
        ScreenPrint(edit->x, edit->y, &drawText[startIdx]);
    }

    void ReleaseEdit(EDIT* edit)
    {
        if (edit) free(edit);
    }
}
