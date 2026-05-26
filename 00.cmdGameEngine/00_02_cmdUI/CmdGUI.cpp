#include "CmdGUI.h"
#include <wchar.h>

namespace CmdGui
{
    namespace Style
    {
        BoxStyleType BoxStyle = BoxStyle_Default;
        CheckBoxStyleType CheckBoxStyle = CheckBoxStyle_Rect;

        // 윈도우 16진수 색상 기반으로 초기화 (배경색 + 글자색)
        unsigned short MainColor = 0x0007;
        unsigned short Button = 0x008F;        // 회색 배경 + 흰 글자
        unsigned short ButtonHovered = 0x002F; // 초록 배경 + 흰 글자
        unsigned short ButtonActive = 0x00AF;  // 연두 배경 + 흰 글자

        unsigned short Link = 0x0007;
        unsigned short LinkHovered = 0x000A;
        unsigned short LinkActive = 0x000E;

        unsigned short SliderGrab = 0x009F;
        unsigned short SliderGrabHovered = 0x00BF;
        unsigned short Slider = 0x004F;
        unsigned short SliderHovered = 0x00CF;

        unsigned short InputText = 0x00F0;
        unsigned short InputTextText = 0x008F;

        void ThemeBlue() 
        {
            Button = 0x001F; ButtonHovered = 0x009F; ButtonActive = 0x00EF;
            SliderGrab = 0x009F; Slider = 0x001F;
        }
        void ThemeDark()
        {
            Button = 0x008F; ButtonHovered = 0x0070; ButtonActive = 0x00F0;
            SliderGrab = 0x008F; Slider = 0x0008;
        }
    }

    void Text(int x, int y, const wchar_t* text, unsigned short color)
    {
        SetScreenColor(color);
        ScreenPrint(x, y, text);
    }

    void Box(int x, int y, int w, int h, unsigned short color, bool fill)
    {
        SetScreenColor(color);

        const wchar_t* LT = L"┌"; 
        const wchar_t* RT = L"┐";
        const wchar_t* LB = L"└"; 
        const wchar_t* RB = L"┘";
        const wchar_t* H = L"─";  
        const wchar_t* V = L"│";

        if (Style::BoxStyle == Style::BoxStyle_Single) 
        {
            LT = L"┌"; RT = L"┐"; LB = L"└"; RB = L"┘";
        }
        else if (Style::BoxStyle == Style::BoxStyle_Block) 
        {
            LT = RT = LB = RB = H = V = L"■";
        }
        else if (Style::BoxStyle == Style::BoxStyle_Empty) 
        {
            LT = RT = LB = RB = H = V = L" ";
        }

        // 테두리 및 배경 렌더링
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                if (j == 0 && i == 0) ScreenPrint(x + i, y + j, LT);
                else if (j == 0 && i == w - 1) ScreenPrint(x + i, y + j, RT);
                else if (j == h - 1 && i == 0) ScreenPrint(x + i, y + j, LB);
                else if (j == h - 1 && i == w - 1) ScreenPrint(x + i, y + j, RB);
                else if (j == 0 || j == h - 1) ScreenPrint(x + i, y + j, H);
                else if (i == 0 || i == w - 1) ScreenPrint(x + i, y + j, V);
                else if (fill) ScreenPrint(x + i, y + j, L" "); // 내부 색상 채우기
            }
        }
    }

    bool InvisibleButton(int x, int y, int w, int h)
    {
        // InputManager의 g_Input 변수의 마우스 좌표를 사용하여 충돌 판정
        bool isHovered = (  g_Input.mouseX >= x && 
                            g_Input.mouseX < x + w &&
                            g_Input.mouseY >= y && 
                            g_Input.mouseY < y + h );
        return (isHovered && g_Input.isMouseLeftClick);
    }

    bool Button(int x, int y, int w, int h, const wchar_t* text)
    {
        bool isHovered = (  g_Input.mouseX >= x && 
                            g_Input.mouseX < x + w &&
                            g_Input.mouseY >= y && 
                            g_Input.mouseY < y + h );

        // 키상태를 직접 가져와 드래그 및 클릭 판정
        bool isMouseDown = isHovered && (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
        bool isClicked = isHovered && g_Input.isMouseLeftClick;

        // 상태에 따른 색상 결정
        unsigned short drawColor = Style::Button;
        if (isHovered) drawColor = Style::ButtonHovered;
        if (isMouseDown) drawColor = Style::ButtonActive;

        // 버튼 배경 그리기
        Box(x, y, w, h, drawColor, true);

        // 중앙에 텍스트 그리기
        int textLen = wcslen(text);
        int textX = x + (w - textLen) / 4;
        int textY = y + (h / 2);
        Text(textX, textY, text, drawColor);

        return isClicked;
    }

    void CheckBox(int x, int y, const wchar_t* text, bool* variable)
    {
        int textLen = wcslen(text);
        bool isHovered = (g_Input.mouseX >= x && g_Input.mouseX < x + 4 + textLen && g_Input.mouseY == y);
        bool isClicked = isHovered && g_Input.isMouseLeftClick;

        if (isClicked) 
        {
            *variable = !(*variable); // true -> false, false -> true 토글
        }

        unsigned short color = isHovered ? Style::ButtonHovered : Style::Button;

        if (*variable) Text(x, y, L"[■]", color);
        else           Text(x, y, L"[ ]", color);

        Text(x + 4, y, text, color);
    }

    void SliderInt(int x, int y, int w, const wchar_t* label, int* variable, int min, int max)
    {
        bool isHovered = (g_Input.mouseX >= x && g_Input.mouseX < x + w && g_Input.mouseY == y);
        bool isDown = isHovered && (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

        // 마우스 드래그에 따른 값 계산
        if (isDown) 
        {
            float ratio = (float)(g_Input.mouseX - x) / (float)(w - 1);
            if (ratio < 0.0f) ratio = 0.0f;
            if (ratio > 1.0f) ratio = 1.0f;
            *variable = min + (int)((max - min) * ratio);
        }

        // 라벨 출력
        Text(x, y - 1, label, Style::MainColor);

        // 슬라이더 트랙 그리기
        for (int i = 0; i < w; i++) 
        {
            float ratio = (float)i / (float)(w - 1);
            int currentVal = min + (int)((max - min) * ratio);

            if (currentVal <= *variable) 
            {
                Text(x + i, y, L"■", isHovered ? Style::SliderGrabHovered : Style::SliderGrab);
            }
            else 
            {
                Text(x + i, y, L"─", Style::Slider);
            }
        }

        // 수치 텍스트 출력
        wchar_t valStr[32];
        swprintf_s(valStr, L" %d", *variable);
        Text(x + w, y, valStr, Style::MainColor);
    }
}