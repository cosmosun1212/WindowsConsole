#include "SliderObject.h"
#include "ScreenBuffer.h"
#include <stdlib.h>
#include <stdio.h> // swprintf_s 사용

namespace CmdUI
{
    SLIDER* CreateSlider(int x, int y, int w, const wchar_t* label, int minVal, int maxVal, int defaultVal, unsigned short colDef, unsigned short colHov)
    {
        SLIDER* sld = (SLIDER*)malloc(sizeof(SLIDER));
        sld->x = x;
        sld->y = y;
        sld->width = w;
        sld->minValue = minVal;
        sld->maxValue = maxVal;
        sld->currentValue = defaultVal;
        sld->isHovered = false;
        sld->isDragging = false;
        sld->colorDefault = colDef;
        sld->colorHover = colHov;
        wcscpy_s(sld->label, label);
        return sld;
    }

    bool UpdateSlider(SLIDER* sld, const INPUTVALUE* input)
    {
        if (sld == NULL || input == NULL) return false;

        sld->isHovered = (
            input->mouseX >= sld->x && 
            input->mouseX < sld->x + sld->width &&
            input->mouseY == sld->y);

        // 드래그 시작 조건
        if (sld->isHovered && input->isMouseLeftClick)
        {
            sld->isDragging = true;
        }

        // 드래그 종료 조건(Hold 상태가 아닐 때)
        if (!input->isMouseLeftDown)
        {
            sld->isDragging = false;
        }

        // 드래그 중인 상태 처리 (isLeftDown = true 일 때)
        if (sld->isDragging && input->isMouseLeftDown)
        {
            float ratio = (float)(input->mouseX - sld->x) / (float)(sld->width - 1);
            if (ratio < 0.0f) ratio = 0.0f;
            if (ratio > 1.0f) ratio = 1.0f;

            int oldVal = sld->currentValue;
            sld->currentValue = sld->minValue + (int)((sld->maxValue - sld->minValue) * ratio);

            if (oldVal != sld->currentValue) return true;
        }

        return false;
    }

    void DrawSlider(SLIDER* sld)
    {
        if (sld == NULL) return;

        //라벨 출력 (슬라이더 바로 윗줄)
        SetColor(sld->colorDefault);
        ScreenPrint(sld->x, sld->y - 1, sld->label);

        //슬라이더 바(트랙) 그리기
        for (int i = 0; i < sld->width; i++)
        {
            float ratio = (float)i / (float)(sld->width - 1);
            int dotVal = sld->minValue + (int)((sld->maxValue - sld->minValue) * ratio);

            if (dotVal <= sld->currentValue)
            {
                // 채워진 부분
                SetColor(sld->isHovered || sld->isDragging ? sld->colorHover : sld->colorDefault);
                ScreenPrint(sld->x + i, sld->y, L"■");
            }
            else
            {
                // 빈 부분
                SetColor(0x0008); // 어두운 회색
                ScreenPrint(sld->x + i, sld->y, L"─");
            }
        }

        //현재 수치 텍스트 출력
        wchar_t valStr[32];
        swprintf_s(valStr, 32, L" %d", sld->currentValue);
        SetColor(sld->colorDefault);
        ScreenPrint(sld->x + sld->width, sld->y, valStr);
    }

    void ReleaseSlider(SLIDER* sld)
    {
        if (sld == NULL) return;
        free(sld);
    }
}