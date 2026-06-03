//

#pragma once
#include <string.h> // wcscpy_s 사용을 위해 필요
#include "InputManager.h"
#include "Collision.h"

// CmdUI 네임스페이스로 버튼 관련 함수들을 묶어서 관리
namespace CmdUI 
{
    // 버튼의 위치, 크기, 색상, 텍스트, 호버 상태를 저장하는 구조체
	typedef struct ___Button 
    {
        int x, y;               // 위치
        int width, height;      // 크기
        unsigned short colorDefault; // 기본 색상
        unsigned short colorHover;   // 커서가 올라갔을 때 색상
        wchar_t text[32];       // 버튼 텍스트
        bool isHovered;         // 현재 커서가 위에 있는지 여부
	}BUTTON; // 버튼 구조체 정의

	BUTTON* CreateButton(int x, int y, int w, int h, const wchar_t* label, unsigned short colDef, unsigned short colHov); // 버튼 생성 함수 // CreateButton()는 버튼의 위치(x, y), 크기(w, h), 텍스트(label), 기본 색상(colDef), 호버 색상(colHov)을 매개변수로 받아 새로운 BUTTON 객체를 동적 메모리로 할당하여 초기화한 후 반환합니다.
	bool Update(BUTTON* btn, const INPUTVALUE* input); // 버튼 업데이트 함수 // 매 프레임마다 호출되어 입력 상태를 확인하고 버튼의 상태를 업데이트하는 함수 // Update()는 버튼 객체와 입력 값을 매개변수로 받아 마우스 좌표가 버튼 영역 안에 있는지 확인하여 상태를 업데이트하고, 마우스 왼쪽 버튼 클릭 시 true를 반환합니다. 그렇지 않으면 false를 반환합니다. 
	void Draw(BUTTON* btn); // 버튼 그리기 함수 // 매 프레임마다 호출되어 버튼을 화면에 그리는 함수 // Draw 함수는 버튼 객체를 매개변수로 받아 현재 상태에 따라 버튼 색상을 결정하고, 버튼 사각형과 텍스트를 화면에 그립니다. 텍스트는 버튼의 중앙에 정렬되어 출력됩니다.
    void Release(BUTTON* btn); // 버튼 해제 함수 // 버튼 객체가 더 이상 필요하지 않을 때 동적 메모리를 해제하는 함수

    inline Rect GetButtonRect(BUTTON* btn) { Rect r;  return r = { (float)btn->x, (float)btn->y, btn->width, btn->height }; }; // 버튼의 충돌 영역을 반환하는 함수 // GetButtonRect()는 버튼 객체를 매개변수로 받아 버튼의 위치와 크기를 기반으로 Rect 구조체를 생성하여 반환합니다. Rect는 버튼의 충돌 영역을 나타냅니다.
	inline void SetButtonHover(BUTTON* btn, bool isHover) { if (btn) btn->isHovered = isHover; } // 버튼의 호버 상태를 설정하는 함수 // SetButtonHover()는 버튼 객체와 상태를 매개변수로 받아 버튼의 isHovered 멤버를 업데이트합니다. 버튼이 NULL이 아닌 경우에만 isHovered 값을 설정합니다.
}
