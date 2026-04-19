#pragma once
#include <string.h>
#include <wchar.h>
#include "InputManager.h"
#include "Collision.h"

namespace CmdUI
{
    typedef struct ___Slider {
        int x, y;                    // 위치
        int width;                   // 슬라이더 바 전체 길이
        unsigned short colorDefault; // 평상시 색상
        unsigned short colorHover;   // 커서가 올라가거나 드래그 중일 때 색상
        wchar_t label[128];          // 상단 라벨
        int currentValue;            // 현재 수치
        int minValue;                // 최소값
        int maxValue;                // 최대값
        bool isHovered;              // 호버 여부
        bool isDragging;             // 드래그 중인지 여부
    } SLIDER;

    SLIDER* CreateSlider(int x, int y, int w, const wchar_t* label, int minVal, int maxVal, int defaultVal, unsigned short colDef, unsigned short colHov);
    bool UpdateSlider(SLIDER* sld, const INPUTVALUE* input);
    void DrawSlider(SLIDER* sld);
    void ReleaseSlider(SLIDER* sld);

    inline Rect GetSliderRect(SLIDER* sld) { Rect r; r = { (float)sld->x, (float)sld->y, sld->width, 1 }; return r; }
    inline void SetSliderHover(SLIDER* sld, bool isHover) { if (sld) sld->isHovered = isHover; }
}