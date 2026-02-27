#pragma once
#include <tchar.h>

// 화면 크기 상수 정의
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;

void ScreenInit();
void ScreenFlipping();
void ScreenClear();
void ScreenRelease();
void ScreenPrint(int x, int y, const wchar_t* string);
void SetColor(unsigned short color);
