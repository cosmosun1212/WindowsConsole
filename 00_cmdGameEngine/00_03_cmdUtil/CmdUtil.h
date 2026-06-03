#pragma once
#include "CmdColor.h"
#include "TimeManager.h"
#include "InputManager.h"

void DrawRect(int x, int y, int w, int h, unsigned short color); // 사각형 그리기 함수 (x, y: 시작 좌표, w: 너비, h: 높이, color: 색상)

void InitScreen(int cols, int rows, int color); // 화면 초기화 함수 (cols: 열 수, rows: 행 수, color: 색상)

void FPS_Gizmo(int x, int y, TIMEMANAGER* pTime, INPUTVALUE* pInput); // FPS와 마우스 좌표를 화면에 표시하는 디버그용 함수 (x, y: 출력 위치, pTime: 시간 관리 구조체 포인터, pInput: 입력 관리 구조체 포인터)