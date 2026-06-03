#pragma once
#include <string.h>
#include <wchar.h>
#include "InputManager.h"
#include "Collision.h"

namespace CmdUI // CmdUI 네임스페이스로 슬라이더 관련 함수들을 묶어서 관리
{
	typedef struct ___Slider // 슬라이더의 위치, 길이, 색상, 텍스트, 현재 수치, 최소/최대값, 호버 상태, 드래그 상태를 저장하는 구조체
    {
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
	} SLIDER; // 슬라이더 구조체 정의

    SLIDER* CreateSlider(int x, int y, int w, const wchar_t* label, int minVal, int maxVal, int defaultVal, unsigned short colDef, unsigned short colHov); // 슬라이더 생성 함수 // CreateSlider()는 슬라이더의 위치(x, y), 너비(w), 라벨 텍스트(label), 최소값(minVal), 최대값(maxVal), 기본값(defaultVal), 기본 색상(colDef), 호버 색상(colHov)을 매개변수로 받아 새로운 SLIDER 객체를 동적 메모리로 할당하여 초기화한 후 반환
    bool Update(SLIDER* sld, const INPUTVALUE* input); // 슬라이더 업데이트 함수 // Update()는 슬라이더 객체와 입력 값을 매개변수로 받아 마우스 좌표가 슬라이더 영역 안에 있는지 확인하여 호버 상태를 업데이트하고, 마우스 왼쪽 버튼 클릭 시 드래그 상태를 업데이트. 슬라이더가 드래그 상태일 때 마우스 이동에 따라 현재 수치를 업데이트하며, 입력이 완료되었음을 알리기 위해 true 반환, 그렇지 않으면 false 반환
    void Draw(SLIDER* sld); // 슬라이더 그리기 함수 // Draw()는 슬라이더 객체를 매개변수로 받아 현재 상태에 따라 슬라이더의 색상을 결정하고, 라벨과 슬라이더 바를 화면에 그리며, 드래그 상태일 때 슬라이더 핸들을 표시
    void Release(SLIDER* sld); // 슬라이더 해제 함수 // Release()는 슬라이더 객체를 매개변수로 받아 동적 메모리를 해제하여 슬라이더 객체를 해제
	inline Rect GetSliderRect(SLIDER* sld) { Rect r; r = { (float)sld->x, (float)sld->y, sld->width, 1 }; return r; } // 슬라이더의 충돌 영역을 반환하는 함수 
	inline void SetSliderHover(SLIDER* sld, bool isHover) { if (sld) sld->isHovered = isHover; } // 슬라이더의 호버 상태를 설정하는 함수 // 객체와 상태를 매개변수로 받아 NULL이 아닌 경우에만 isHovered 값을 설정
}
