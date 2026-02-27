#pragma once
#include <windows.h>

enum KEY_STATE { KEY_FREE = 0, KEY_DOWN, KEY_HOLD, KEY_UP };

typedef struct _InputValue
{
    // 마우스
    int mouseX;
    int mouseY;
    bool isMouseLeftDown;  // 왼쪽 버튼이 현재 눌려있는지
    bool isMouseLeftClick; // 이번 프레임에 클릭(Down) 했는지

    // 키보드 (자주 쓰는 키만 관리)
    // 0: Space, 1: Enter, 2: Left, 3: Right, 4: Up, 5: Down
    KEY_STATE keyState[6];
}INPUTVALUE;

void InputInit();
void InputUpdate();


