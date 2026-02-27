#pragma once
#include "TimeManager.h"
#include "InputManager.h"

void InitScreen(int cols, int rows, int color);

void FPS_Gizmo(int x, int y, TIMEMANAGER* pTime, INPUTVALUE* pInput);
