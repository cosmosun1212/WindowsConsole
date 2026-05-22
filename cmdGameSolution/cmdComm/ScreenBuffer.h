#pragma once

#include <tchar.h>

const int DEFAULT_SCREEN_WIDTH = 80;
const int DEFAULT_SCREEN_HEIGHT = 25;

void ScreenInit(int width = DEFAULT_SCREEN_WIDTH, int height = DEFAULT_SCREEN_HEIGHT);

void ScreenFlipping();
void ScreenClear();
void ScreenRelease();
void ScreenPrint(int x, int y, const wchar_t* string);
void SetColor(unsigned short color);

int GetScreenWidth();
int GetScreenHeight();
