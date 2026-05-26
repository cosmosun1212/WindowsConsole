#include "ButtonObject.h"
#include "CmdUtil.h"
#include "ScreenBuffer.h"
#include "InputManager.h"

namespace CmdUI
{
    //버튼 생성 함수 (초기화)
    BUTTON* CreateButton(int x, int y, int w, int h, const wchar_t* label, unsigned short colDef, unsigned short colHov)
    {
        BUTTON* btn;
        btn = (BUTTON*)malloc(sizeof(BUTTON)); // 동적 메모리 할당
        btn->x = x;
        btn->y = y;
        btn->width = w;
        btn->height = h;
        btn->colorDefault = colDef;
        btn->colorHover = colHov;
        btn->isHovered = false;

        // 문자열 복사 (안전한 함수 사용)
        wcscpy_s(btn->text, label);

        return btn;
    }

    bool Update(BUTTON* btn, const INPUTVALUE* input)
    {
        if (btn == NULL || input == NULL) return false;

        if (input->mouseX >= btn->x && input->mouseX < btn->x + btn->width &&
            input->mouseY >= btn->y && input->mouseY < btn->y + btn->height)
        {
            SetButtonHover(btn, true);

            if (input->isMouseLeftClick) return true;
        }
        else
        {
            SetButtonHover(btn, false);
        }
        return false;
    }

    // 버튼 그리기 함수
    void Draw(BUTTON* btn)
    {
        if (btn == NULL) return; 

        // 상태에 따른 색상 결정
        unsigned short currentColor = btn->isHovered ? btn->colorHover : btn->colorDefault;

        // 박스 그리기
        DrawRect(btn->x, btn->y, btn->width, btn->height, currentColor);

        // 텍스트 중앙 정렬 계산
        int vlen = 0; // 실제 화면에 출력될 칸 수
        int textLen = (int)wcslen(btn->text);

        for (int i = 0; i < textLen; i++)
        {
            // btn->text[i]로 한 글자씩 검사! 
            // 0x007F(아스키코드 끝값)보다 크면 한글/한자 같은 2바이트 문자입니다.
            if (btn->text[i] > 0x007F)
            {
                vlen += 2; // 한글은 2칸
            }
            else
            {
                vlen += 1; // 영어/숫자/공백은 1칸
            }
        }

        // 중앙 정렬 계산
        int textX = btn->x + (btn->width - vlen) / 2;
        int textY = btn->y + (btn->height / 2);

        // 텍스트 출력
        SetScreenColor(currentColor); // 텍스트도 버튼 색상과 맞춤
        ScreenPrint(textX, textY, btn->text);
    }

    void Release(BUTTON* btn)
    {
        if (btn == nullptr) return;
        btn = nullptr;
        free(btn); // 동적 메모리 해제
    }
}

