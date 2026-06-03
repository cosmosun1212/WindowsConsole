// 버튼의 상태를 설정하는 함수 (커서가 버튼 위에 있는지 여부)
#include "ButtonObject.h"
#include "CmdUtil.h"
#include "ScreenBuffer.h"
#include "InputManager.h"

namespace CmdUI // CmdUI 네임스페이스로 버튼 관련 함수들을 묶어서 관리
{
    // 버튼 생성 함수 // 버튼의 위치, 크기, 텍스트, 색상 등을 설정하여 새로운 버튼 객체를 생성하는 함수

    // CreateButton 함수는 버튼의 위치(x, y), 크기(w, h), 텍스트(label), 기본 색상(colDef), 호버 색상(colHov)을 매개변수로 받아 새로운 BUTTON 객체를 동적 메모리로 할당하여 초기화한 후 반환합니다.
    BUTTON* CreateButton(int x, int y, int w, int h, const wchar_t* label, unsigned short colDef, unsigned short colHov)
    {
        BUTTON* btn; /// 버튼 객체를 가리키는 포인터 선언
        btn = (BUTTON*)malloc(sizeof(BUTTON)); // 동적 메모리 할당
        btn->x = x; // 버튼의 x 위치 설정
        btn->y = y; // 버튼의 y 위치 설정
        btn->width = w; // 버튼의 너비 설정
        btn->height = h; // 버튼의 높이 설정
        btn->colorDefault = colDef; // 버튼의 기본 색상 설정
        btn->colorHover = colHov; // 버튼의 호버 색상 설정
        btn->isHovered = false; // 버튼의 호버 상태 초기화

        // 버튼 텍스트 복사 (안전한 문자열 복사 함수 사용)
        wcscpy_s(btn->text, label);

        return btn; // 초기화된 버튼 객체의 포인터 반환
    }

    // 버튼 업데이트 함수 // 매 프레임마다 호출되어 입력 상태를 확인하고 버튼의 호버 상태를 업데이트하는 함수
    bool Update(BUTTON* btn, const INPUTVALUE* input)
    {
        if (btn == NULL || input == NULL) return false; // 안전성 검사: 버튼 또는 입력 값이 NULL인 경우 함수 종료

        // 마우스 좌표가 버튼 영역 안에 있는지 확인하여 상태 업데이트
        if (input->mouseX >= btn->x && input->mouseX < btn->x + btn->width &&
            input->mouseY >= btn->y && input->mouseY < btn->y + btn->height)
        {
            SetButtonHover(btn, true); // 마우스가 버튼 위에 있으면 호버 상태로 설정

            if (input->isMouseLeftClick) return true; // 마우스 왼쪽 버튼 클릭 시 true 반환
        }
        else // 마우스가 버튼 영역 밖에 있으면 호버 상태 해제
        {
            SetButtonHover(btn, false); // 호버 상태 해제
        }
        return false; // 클릭이 발생하지 않았으므로 false 반환 // 클릭 여부는 Update 함수의 반환값으로 전달
    }

    // 버튼 그리기 함수
    void Draw(BUTTON* btn)
    {
        if (btn == NULL) return; // 안전성 검사: 버튼이 NULL인 경우 함수 종료

        // 현재 상태에 따라 버튼 색상 결정 (호버 상태에 따라 색상 변경)
        unsigned short currentColor = btn->isHovered ? btn->colorHover : btn->colorDefault;

        DrawRect(btn->x, btn->y, btn->width, btn->height, currentColor);// 버튼 사각형 그리기

        // 텍스트 중앙 정렬 계산
        int vlen = 0; // 실제 화면에 출력될 칸 수
        int textLen = (int)wcslen(btn->text);// 텍스트 길이 계산 (문자 수)

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
        int textX = btn->x + (btn->width - vlen) / 2; // 텍스트 시작 X 좌표 계산 (버튼의 왼쪽에서부터 텍스트가 중앙에 오도록)
        int textY = btn->y + (btn->height / 2); // 텍스트 시작 Y 좌표 계산 (버튼의 위쪽에서부터 텍스트가 중앙에 오도록)

        // 텍스트 출력
        SetScreenColor(currentColor); // 텍스트도 버튼 색상과 맞춤
        ScreenPrint(textX, textY, btn->text); // 버튼 텍스트 출력 (계산된 중앙 위치에 텍스트를 그리기)

    }

	// 버튼 해제 함수 // 버튼 객체가 더 이상 필요하지 않을 때 동적 메모리를 해제하는 함수
    void Release(BUTTON* btn)
    {
		if (btn == nullptr) return; // 안전성 검사: 버튼이 NULL인 경우 함수 종료   
		btn = nullptr; // 버튼 포인터를 NULL로 설정하여 더 이상 사용되지 않도록 표시
		free(btn); // 동적 메모리 해제
    }
}

