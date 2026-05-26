#pragma once
#pragma once
#include <Windows.h>
#include <string>
#include "ScreenBuffer.h"
#include "InputManager.h"

extern INPUTVALUE g_Input;

namespace CmdGui
{
    namespace Style  // 색상 및 스타일 정의
    {
        enum BoxStyleType { BoxStyle_Default, BoxStyle_Single, BoxStyle_Corners, BoxStyle_Block, BoxStyle_Empty };
        extern BoxStyleType BoxStyle;

        enum CheckBoxStyleType { CheckBoxStyle_Rect, CheckBoxStyle_Round, CheckBoxStyle_X };
        extern CheckBoxStyleType CheckBoxStyle;

        // UI 기본 색상값
        extern unsigned short MainColor;
        extern unsigned short Button;
        extern unsigned short ButtonHovered;
        extern unsigned short ButtonActive;

        extern unsigned short Link;
        extern unsigned short LinkHovered;
        extern unsigned short LinkActive;

        extern unsigned short SliderGrab;
        extern unsigned short SliderGrabHovered;

        extern unsigned short Slider;
        extern unsigned short SliderHovered;

        extern unsigned short InputText;
        extern unsigned short InputTextText;

        // 테마 설정 함수
        void ThemeBlue();
        void ThemeDark();
    }

    // UI 그리기 및 컨트롤 함수
    void Text(int x, int y, const wchar_t* text, unsigned short color = 0x000F); // 기본 텍스트 출력
    void Box(int x, int y, int w, int h, unsigned short color, bool fill = true); // 박스 테두리 및 배경 그리기
    bool InvisibleButton(int x, int y, int w, int h); // 투명 버튼 (클릭 감지용)
    bool Button(int x, int y, int w, int h, const wchar_t* text); // 일반 버튼 (클릭 시 true 반환)
    void CheckBox(int x, int y, const wchar_t* text, bool* variable); // 체크박스 (클릭 시 true/false 반전)
    void SliderInt(int x, int y, int w, const wchar_t* label, int* variable, int min = 0, int max = 100); // 정수형 슬라이더 바 (마우스 드래그로 수치 조절)
}
