// 체크박스 UI 요소 구현을 위한 헤더 파일, CHECKBOX 구조체와 관련 함수들을 선언
//
#include "CheckBoxObject.h"
#include "ScreenBuffer.h"
#include <stdlib.h>

namespace CmdUI // CmdUI 네임스페이스로 체크박스 관련 함수들을 묶어서 관리
{
	// 체크박스 생성 함수 // 체크박스의 위치, 텍스트, 색상 등을 설정하여 새로운 체크박스 객체를 생성하는 함수 
    CHECKBOX* CreateCheckBox(int x, int y, const wchar_t* label, bool defaultCheck, unsigned short colDef, unsigned short colHov)
    {
		CHECKBOX* chk = (CHECKBOX*)malloc(sizeof(CHECKBOX)); // CHECKBOX 객체를 가리키는 포인터 선언 및 동적 메모리 할당
		chk->x = x; // 체크박스의 x 위치 설정
		chk->y = y; // 체크박스의 y 위치 설정
		chk->isChecked = defaultCheck; // 체크박스의 기본 체크 상태 설정
		chk->isHovered = false;  // 체크박스의 호버 상태 초기화
		chk->colorDefault = colDef; // 체크박스의 기본 색상 설정
		chk->colorHover = colHov; // 체크박스의 호버 색상 설정
		wcscpy_s(chk->label, label); // 체크박스의 텍스트 복사 (안전한 문자열 복사 함수 사용)
		return chk; // 초기화된 체크박스 객체의 포인터 반환
    }

	// 체크박스 업데이트 함수 
    bool Update(CHECKBOX* chk, const INPUTVALUE* input)
    {
		// 안전성 검사: 체크박스 또는 입력 값이 NULL인 경우 함수 종료
        if (chk == NULL) return false;

		int textLen = (int)wcslen(chk->label) * 2; // 텍스트 길이를 계산하여 체크박스의 너비에 추가 
        // 호버 판정
        chk->isHovered = (input->mouseX >= chk->x && input->mouseX < chk->x + 5 + textLen &&
			input->mouseY == chk->y); // 마우스 좌표가 체크박스 영역 안에 있으면 확인 후 호버 상태 업데이트 (체크박스 자체는 5칸, 텍스트는 문자 수 * 2칸으로 가정)

		// 클릭 판정: 체크박스가 호버 상태이고 마우스 왼쪽 버튼이 클릭되었는지 확인하여 체크 상태 토글
        if (chk->isHovered && input->isMouseLeftClick)
        {
			chk->isChecked = !(chk->isChecked); // 체크 상태 토글 (반전)
			return true; // 클릭이 발생하여 체크 상태가 변경되었으므로 true 반환
        }
		return false; // 클릭이 발생하지 않았거나 체크 상태가 변경되지 않았으므로 false 반환
    }

	// 체크박스 그리기 함수
    void Draw(CHECKBOX* chk)
    {
		if (chk == NULL) return; // 안전성 검사: 체크박스가 NULL인 경우 함수 종료

		unsigned short currentColor = chk->isHovered ? chk->colorHover : chk->colorDefault; // 현재 상태에 따라 체크박스 색상 결정 (상태에 따라 색상 변경)
		SetScreenColor(currentColor); // 체크박스 색상 설정

		// 체크 상태에 따라 박스와 텍스트를 화면에 그리기 (체크박스 자체는 5칸, 텍스트는 체크박스 오른쪽에 출력)
		if (chk->isChecked) ScreenPrint(chk->x, chk->y, L"[■]"); // 체크 상태가 true인 경우 체크된 박스 그리기 (유니코드 U+25A0)
        else                ScreenPrint(chk->x, chk->y, L"[ ]"); // 체크 상태가 false인 경우 체크되지 않은 박스 그리기

		ScreenPrint(chk->x + 4, chk->y, chk->label); // 체크박스 오른쪽에 텍스트 출력
    }

	// 체크박스 해제 함수
    void Release(CHECKBOX* chk)
    {
		if (chk == NULL) return; // 안전성 검사: 체크박스가 NULL인 경우 함수 종료
		free(chk); /// 동적 메모리 해제
    }
}
