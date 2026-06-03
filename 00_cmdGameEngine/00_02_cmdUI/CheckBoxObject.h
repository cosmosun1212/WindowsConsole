// 체크박스 UI 요소 구현을 위한 헤더 파일, CHECKBOX 구조체와 관련 함수들을 선언
#pragma once 
#include <string.h>
#include <wchar.h>
#include "InputManager.h"
#include "Collision.h"

// CmdUI 네임스페이스로 체크박스 관련 함수들을 묶어서 관리
namespace CmdUI
{
	typedef struct ___CheckBox // 체크박스의 위치, 색상, 텍스트, 체크 상태, 호버 상태를 저장하는 구조체
    {
        int x, y;                    // 위치
        unsigned short colorDefault; // 기본 색상
        unsigned short colorHover;   // 커서가 올라갔을 때 색상
        wchar_t label[128];          // 설명 텍스트
        bool isChecked;              // 체크 여부
        bool isHovered;              // 커서 호버 여부
	} CHECKBOX; // 체크박스 구조체 정의

	CHECKBOX* CreateCheckBox(int x, int y, const wchar_t* label, bool defaultCheck, unsigned short colDef, unsigned short colHov); // 체크박스 생성 함수 // CreateCheckBox()는 체크박스의 위치(x, y), 텍스트(label), 기본 체크 상태(defaultCheck), 기본 색상(colDef), 호버 색상(colHov)을 매개변수로 받아 새로운 CHECKBOX 객체를 동적 메모리로 할당하여 초기화한 후 반환합니다.
	bool Update(CHECKBOX* chk, const INPUTVALUE* input); // 체크박스 업데이트 함수 // 매 프레임마다 호출되어 입력 상태를 확인하고 체크박스의 호버 상태와 체크 상태를 업데이트하는 함수 // Update()는 체크박스 객체와 입력 값을 매개변수로 받아 마우스 좌표가 체크박스 영역 안에 있는지 확인하여 호버 상태를 업데이트하고, 마우스 왼쪽 버튼 클릭 시 체크 상태를 토글(반전)하여 true를 반환합니다. 그렇지 않으면 false를 반환합니다.
	void Draw(CHECKBOX* chk); // 체크박스 그리기 함수 // 매 프레임마다 호출되어 체크박스를 화면에 그리는 함수 // Draw()는 체크박스 객체를 매개변수로 받아 현재 상태에 따라 체크박스의 색상을 결정하고, 체크 상태에 따라 박스와 텍스트를 화면에 그립니다. 텍스트는 체크박스 오른쪽에 출력됩니다.
	void Release(CHECKBOX* chk); // 체크박스 해제 함수 // 체크박스 객체가 더 이상 필요하지 않을 때 동적 메모리를 해제하는 함수

	// 체크박스의 충돌 영역을 반환하는 함수 
    inline Rect GetCheckBoxRect(CHECKBOX* chk)
    {
		Rect r; // 체크박스의 충돌 영역을 계산하여 반환하는 함수 
		int textLen = (int)wcslen(chk->label) * 2; // 텍스트 길이를 계산하여 체크박스의 너비에 추가 (체크박스 자체는 5칸, 텍스트는 문자 수 * 2칸으로 가정)
		r = { (float)chk->x, (float)chk->y, 5 + textLen, 1 }; // 체크박스의 충돌 영역을 계산하여 반환하는 함수 
		return r; // 체크박스의 충돌 영역을 계산하여 반환하는 함수
    }
	// 체크박스의 호버 상태를 설정하는 함수
	inline void SetCheckBoxHover(CHECKBOX* chk, bool isHover) { if (chk) chk->isHovered = isHover; } // SetCheckBoxHover()는 체크박스 객체와 상태를 매개변수로 받아 체크박스가 NULL이 아닌 경우에만 isHovered 값을 설정합
}
