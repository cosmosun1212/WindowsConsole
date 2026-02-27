#pragma once
#include "TimeManager.h"
#include "InputManager.h"

void InitScreen(int cols, int rows, int color);

void FPS_Gizmo(int x, int y, TIMEMANAGER* pTime, INPUTVALUE* pInput);

void DrawScorePanel(int x, int y, int score, int lines);

void DrawNextBlockBox(int x, int y, int w, int h, int nextType);

void DrawColorBox(int x, int y, int w, int h, unsigned short color);

void DrawGameOverPopup(int centerX, int centerY, int score);