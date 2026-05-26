#pragma once
#include "CmdColor.h"
#include "TimeManager.h"
#include "InputManager.h"

void DrawRect(int x, int y, int w, int h, unsigned short color);

void InitScreen(int cols, int rows, int color);

void FPS_Gizmo(int x, int y, TIMEMANAGER* pTime, INPUTVALUE* pInput);