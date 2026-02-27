#pragma once
#include <string.h> // wcscpy_s 사용을 위해 필요
#include "InputManager.h"
#include "Collision.h"

// 버튼 구조체 정의
typedef struct ___Button {
    int x, y;               // 위치
    int width, height;      // 크기
    unsigned short colorDefault; // 평상시 색상
    unsigned short colorHover;   // 커서가 올라갔을 때 색상
    wchar_t text[32];       // 버튼 텍스트
    bool isHovered;         // 현재 커서가 위에 있는지 여부
}BUTTON;

BUTTON* CreateButton(int x, int y, int w, int h, const wchar_t* label, unsigned short colDef, unsigned short colHov);
bool UpdateButton(BUTTON* btn, const INPUTVALUE* input);
void DrawButton(BUTTON* btn);
void ReleaseButton(BUTTON* btn);

inline Rect GetButtonRect(BUTTON* btn) { Rect r;  return r = { (float)btn->x, (float)btn->y, btn->width, btn->height }; };
inline void SetButtonHover(BUTTON* btn, bool isHover) { if (btn) btn->isHovered = isHover; }