#include <stdio.h>
#include <string.h> // wcslen 사용을 위해 필요
#include "GameUI.h"
#include "CmdUtil.h"
#include "ScreenBuffer.h"
#include "TimeManager.h"
#include "InputManager.h"

extern int shapes[7][4][4][2];
extern unsigned short blockColors[8];
#define BLOCK_W 3
#define BLOCK_H 2

void InitScreen(int cols, int rows, int color)
{
    DrawRect(0, 0, cols, rows, color);
}

void FPS_Gizmo(int x, int y, TIMEMANAGER* pTime, INPUTVALUE* pInput)
{
    // 안전장치
    if (pTime == NULL || pInput == NULL) return;

    wchar_t debugMsg[100];

    // 정보 가독성을 위해 양옆에 공백(스페이스바)을 넣어서 배경색이 좀 더 넓게 칠해지도록 함
    swprintf_s(debugMsg, L" FPS: %02d | Mouse: [%02d, %02d] ",
        pTime->fps,
        pInput->mouseX,
        pInput->mouseY
    );

    int msgLen = (int)wcslen(debugMsg);

    // 테두리 그리기  0x000A: 배경 검정(0) + 글자 밝은 초록(A) -> Matrix 느낌!
    DrawRect(x, y, msgLen + 2, 3, 0x000A);

    // 배경색 있는 글자 쓰기
    // 0x001F: 배경 파랑(1) + 글자 밝은 흰색(F) -> DOS 시절의 근본 색상
    SetColor(0x001F);

    // 테두리 안쪽에 출력
    ScreenPrint(x + 1, y + 1, debugMsg);

    // 색상 사용 후 다시 기본(흰색)으로 돌려놓는 매너
    SetColor(0x000F);
}

// 점수 패널 그리기
void DrawScorePanel(int x, int y, int score, int lines)
{
    wchar_t msg[50];
    SetColor(0x000F); // 기본 흰색

    swprintf_s(msg, L"SCORE: %d", score);
    ScreenPrint(x, y, msg);

    swprintf_s(msg, L"LINES: %d", lines);
    ScreenPrint(x, y + 2, msg);
}

// NEXT 블록 박스 그리기
void DrawNextBlockBox(int x, int y, int w, int h, int nextType)
{
    // 제목
    SetColor(0x000F);
    ScreenPrint(x, y - 2, L"NEXT:");

    // 테두리 박스
    DrawRect(x, y, w, h, 0x000F);

    // 중앙 정렬 계산
    int minX = 4, maxX = 0;
    int minY = 4, maxY = 0;

    for (int i = 0; i < 4; i++)
    {
        int bx = shapes[nextType][0][i][0];
        int by = shapes[nextType][0][i][1];
        if (bx < minX) minX = bx;
        if (bx > maxX) maxX = bx;
        if (by < minY) minY = by;
        if (by > maxY) maxY = by;
    }

    int pieceWidth = (maxX - minX + 1) * BLOCK_W;
    int pieceHeight = (maxY - minY + 1) * BLOCK_H;

    int startX = x + (w - pieceWidth) / 2;
    int startY = y + (h - pieceHeight) / 2;

    // 블록 그리기
    unsigned short color = blockColors[nextType + 1];
    for (int i = 0; i < 4; i++)
    {
        int bx = shapes[nextType][0][i][0];
        int by = shapes[nextType][0][i][1];

        int drawX = startX + ((bx - minX) * BLOCK_W);
        int drawY = startY + ((by - minY) * BLOCK_H);

        DrawRect(drawX, drawY, BLOCK_W, BLOCK_H, color);
    }
}

// 내부가 채워진 색상 박스 그리기 (배경 지우기용도)
void DrawColorBox(int x, int y, int w, int h, unsigned short color)
{
    // 내부 채우기
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            SetColor(color);
            ScreenPrint(x + i, y + j, L" ");
        }
    }
    // 테두리 그리기
    DrawRect(x, y, w, h, color);
}

// 게임 오버 팝업 그리기
void DrawGameOverPopup(int screenW, int screenH, int score)
{
    // 화면 중앙 계산
    int w = 30;
    int h = 7;
    int x = (screenW - w) / 2; // 화면 너비가 80이라면 (80-30)/2 = 25
    int y = (screenH - h) / 2; // 화면 높이가 25라면 (25-7)/2 = 9

    // 배경 박스 그리기 (빨강)
    // 0x00CF: 빨강 배경(C) + 밝은 흰색 글자(F)
    DrawColorBox(x, y, w, h, 0x00CF);

    // 텍스트 출력
    SetColor(0x00CF);
    ScreenPrint(x + 10, y + 2, L"GAME OVER");

    wchar_t msg[50];
    swprintf_s(msg, L"Final Score: %d", score);
    // 텍스트 중앙 정렬: x + (박스폭 - 글자길이)/2
    ScreenPrint(x + (w - wcslen(msg)) / 2, y + 4, msg);

    ScreenPrint(x + 8, y + 5, L"PRESS [ENTER]");
}