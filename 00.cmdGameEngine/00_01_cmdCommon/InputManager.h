#pragma once
#include <windows.h>

//Virtual KeyCode Mapping
enum VK { Esc = 0, Space, Return, Left, Right, Up, Down, MaxCount }; //End는 키보드 매핑 아님 //마지막 개수를 세는 용도
enum KEY_STATE { KEY_FREE = 0, KEY_DOWN, KEY_HOLD, KEY_UP };

typedef struct _InputValue
{
    // 마우스
    int mouseX;
    int mouseY;
    bool isMouseLeftDown;  // 왼쪽 버튼이 현재 눌려있는지
    bool isMouseLeftClick; // 이번 프레임에 클릭(Down) 했는지

    // 키보드 (자주 쓰는 키만 관리)
    KEY_STATE keyState[VK::MaxCount];
}INPUTVALUE;

void InputInit();
void InputUpdate();


