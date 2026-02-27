#include "ButtonObject.h"
#include "CmdUtil.h"
#include "ScreenBuffer.h"
#include "InputManager.h"

// 1. 버튼 생성 함수 (초기화)
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

// [수정됨] 인풋 시스템을 인자로 받아서 처리
bool UpdateButton(BUTTON* btn, const INPUTVALUE* input)
{
    // 포인터 유효성 검사 (안전장치)
    if (btn == NULL || input == NULL) return false;

    // 마우스가 버튼 범위 안에 있는지 확인
    // (g_Input 대신 인자로 들어온 input-> 사용)
    if (input->mouseX >= btn->x && input->mouseX < btn->x + btn->width &&
        input->mouseY >= btn->y && input->mouseY < btn->y + btn->height)
    {
        SetButtonHover(btn, true);

        // 마우스 왼쪽 클릭 체크
        if (input->isMouseLeftClick) return true;
    }
    else {
        SetButtonHover(btn, false);
    }
    return false;
}

// 3. 버튼 그리기 함수
void DrawButton(BUTTON* btn)
{
    // 상태에 따른 색상 결정
    unsigned short currentColor = btn->isHovered ? btn->colorHover : btn->colorDefault;

    // 1) 박스 그리기 (기존에 만드신 함수 사용)
    DrawRect(btn->x, btn->y, btn->width, btn->height, currentColor);

    // 2) 텍스트 중앙 정렬 계산
    // 버튼의 가운데 - 글자 길이의 절반
    int textLen = (int)wcslen(btn->text);
    int textX = btn->x + (btn->width - textLen) / 2; // 가로 중앙
    int textY = btn->y + (btn->height / 2);          // 세로 중앙

    // 3) 텍스트 출력
    SetColor(currentColor); // 텍스트도 버튼 색상과 맞춤 (원하면 흰색으로 고정 가능)
    ScreenPrint(textX, textY, btn->text);
}

void ReleaseButton(BUTTON* btn)
{
	if (btn == nullptr) return;
    btn = nullptr;
	free(btn); // 동적 메모리 해제
}

