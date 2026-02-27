#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <locale.h>

#include "ScreenBuffer.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "CmdUtil.h"
#include "GameUI.h"
#include "Collision.h"
#include "ButtonObject.h"

extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;

// ==========================================
// [게임 상수 및 데이터]
// ==========================================
enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER };
GameState currentState = STATE_MENU;

// [보드 크기] 15칸 x 10줄
#define BOARD_W 15
#define BOARD_H 10

// [블록 디자인] 3x2 크기
#define BLOCK_W 3
#define BLOCK_H 2

// [화면 배치 보정]
// 벽(Index -1)이 화면 좌표 (2, 2)에 그려지게 계산
// X: 5 + (-1 * 3) = 2
// Y: 2 + (-1 * 2) = 2
#define BOARD_OFFSET_X 5
#define BOARD_OFFSET_Y 2

// 게임 보드 (0:빈칸, 1~7:블록색상)
int board[BOARD_H][BOARD_W];

// 테트리스 모양 데이터
int shapes[7][4][4][2] = {
    { { {0,1},{1,1},{2,1},{3,1} }, { {2,0},{2,1},{2,2},{2,3} }, { {0,2},{1,2},{2,2},{3,2} }, { {1,0},{1,1},{1,2},{1,3} } }, // I
    { { {0,0},{0,1},{1,1},{2,1} }, { {1,0},{2,0},{1,1},{1,2} }, { {0,1},{1,1},{2,1},{2,2} }, { {1,0},{1,1},{0,2},{1,2} } }, // J
    { { {2,0},{0,1},{1,1},{2,1} }, { {1,0},{1,1},{1,2},{2,2} }, { {0,1},{1,1},{2,1},{0,2} }, { {0,0},{1,0},{1,1},{1,2} } }, // L
    { { {1,0},{2,0},{1,1},{2,1} }, { {1,0},{2,0},{1,1},{2,1} }, { {1,0},{2,0},{1,1},{2,1} }, { {1,0},{2,0},{1,1},{2,1} } }, // O
    { { {1,0},{2,0},{0,1},{1,1} }, { {1,0},{1,1},{2,1},{2,2} }, { {1,1},{2,1},{0,2},{1,2} }, { {0,0},{0,1},{1,1},{1,2} } }, // S
    { { {1,0},{0,1},{1,1},{2,1} }, { {1,0},{1,1},{2,1},{1,2} }, { {0,1},{1,1},{2,1},{1,2} }, { {1,0},{0,1},{1,1},{1,2} } }, // T
    { { {0,0},{1,0},{1,1},{2,1} }, { {2,0},{1,1},{2,1},{1,2} }, { {0,1},{1,1},{1,2},{2,2} }, { {1,0},{0,1},{1,1},{0,2} } }  // Z
};

unsigned short blockColors[8] = { 0x0000, 0x000B, 0x0009, 0x0006, 0x000E, 0x000A, 0x000D, 0x000C };

struct Piece
{
    int x, y;
    int type;
    int rotation;
};

Piece currentPiece;
int nextPieceType;

float dropTimer = 0.0f;
float dropInterval = 0.8f;
int score = 0;
int linesCleared = 0;

BUTTON* btnStart;
BUTTON* btnExit;


bool IsValid(int px, int py, int pType, int pRot)
{
    for (int i = 0; i < 4; i++)
    {
        int bx = shapes[pType][pRot][i][0];
        int by = shapes[pType][pRot][i][1];

        int targetX = px + bx;
        int targetY = py + by;

        if (targetX < 0 || targetX >= BOARD_W || targetY >= BOARD_H) return false;
        if (targetY >= 0 && board[targetY][targetX] != 0) return false;
    }
    return true;
}

void SpawnPiece()
{
    currentPiece.type = nextPieceType;
    currentPiece.rotation = 0;
    currentPiece.x = (BOARD_W / 2) - 1; // 중앙 정렬
    currentPiece.y = -2;

    nextPieceType = rand() % 7;

    if (!IsValid(currentPiece.x, currentPiece.y, currentPiece.type, currentPiece.rotation))
    {
        currentState = STATE_GAMEOVER;
    }
}

// 블록 고정 (Lock)
void LockPiece()
{
    //  게임 오버 체크
    // 블록을 고정할 때, 더 이상 쌓을 공간이 없으면 게임 오버
    for (int i = 0; i < 4; i++)
    {
        int by = shapes[currentPiece.type][currentPiece.rotation][i][1];
        int targetY = currentPiece.y + by;

        if (targetY < 0)
        {
            currentState = STATE_GAMEOVER;
            return; // 게임 오버시 진행 안 함
        }
    }

    // 블록을 보드에 기록
    for (int i = 0; i < 4; i++)
    {
        int bx = shapes[currentPiece.type][currentPiece.rotation][i][0];
        int by = shapes[currentPiece.type][currentPiece.rotation][i][1];

        int targetX = currentPiece.x + bx;
        int targetY = currentPiece.y + by;

        // 화면 안에 있는 것만 기록
        if (targetY >= 0)
        {
            board[targetY][targetX] = currentPiece.type + 1;
        }
    }

    // 줄 삭제 체크
    for (int y = BOARD_H - 1; y >= 0; y--)
    {
        bool isFull = true;
        for (int x = 0; x < BOARD_W; x++)
        {
            if (board[y][x] == 0) { isFull = false; break; }
        }

        if (isFull)
        {
            // 윗줄들을 당겨옴
            for (int ky = y; ky > 0; ky--)
            {
                for (int kx = 0; kx < BOARD_W; kx++)
                    board[ky][kx] = board[ky - 1][kx];
            }
            // 맨 윗줄 초기화
            for (int kx = 0; kx < BOARD_W; kx++) board[0][kx] = 0;

            y++;
            score += 100;
            linesCleared++;

            if (dropInterval > 0.1f) dropInterval -= 0.02f;
        }
    }

    SpawnPiece();
}

void HardDrop()
{
    while (IsValid(currentPiece.x, currentPiece.y + 1, currentPiece.type, currentPiece.rotation))
    {
        currentPiece.y++;
        score += 2;
    }
    LockPiece();
}

void ResetGame()
{
    for (int y = 0; y < BOARD_H; y++)
        for (int x = 0; x < BOARD_W; x++)
            board[y][x] = 0;

    score = 0;
    linesCleared = 0;
    dropInterval = 0.8f;
    nextPieceType = rand() % 7;
    SpawnPiece();
}

void Awake()
{
    _wsetlocale(LC_ALL, L"korean");
    SetConsoleTitle(L"Dj__Kim's TETRIS (Modified)");
    srand((unsigned int)time(NULL));
}

void Start()
{
    ScreenInit();
    InputInit();
    TimeInit();

    // 버튼 위치도 중앙으로 이동
    btnStart = CreateButton(30, 10, 20, 3, L"게임 시작", 0x0007, 0x000E);
    btnExit = CreateButton(30, 15, 20, 3, L"종료 하기", 0x0007, 0x000C);
}

void FixedUpdate() {}

void Update()
{
    if (currentState == STATE_MENU)
    {
        if (UpdateButton(btnStart, &g_Input) || g_Input.keyState[1] == KEY_DOWN)
        {
            ResetGame();
            currentState = STATE_PLAYING;
        }
        if (UpdateButton(btnExit, &g_Input)) exit(0);
        return;
    }

    if (currentState == STATE_GAMEOVER)
    {
        if (g_Input.keyState[1] == KEY_DOWN) currentState = STATE_MENU;
        return;
    }

    // [게임 플레이 입력]
    if (g_Input.keyState[2] == KEY_DOWN) // Left
    {
        if (IsValid(currentPiece.x - 1, currentPiece.y, currentPiece.type, currentPiece.rotation))
            currentPiece.x--;
    }
    if (g_Input.keyState[3] == KEY_DOWN) // Right
    {
        if (IsValid(currentPiece.x + 1, currentPiece.y, currentPiece.type, currentPiece.rotation))
            currentPiece.x++;
    }
    if (g_Input.keyState[4] == KEY_DOWN) // Rotate
    {
        int nextRot = (currentPiece.rotation + 1) % 4;
        if (IsValid(currentPiece.x, currentPiece.y, currentPiece.type, nextRot))
            currentPiece.rotation = nextRot;
    }
    if (g_Input.keyState[5] == KEY_HOLD) // Soft Drop
    {
        if (IsValid(currentPiece.x, currentPiece.y + 1, currentPiece.type, currentPiece.rotation))
        {
            currentPiece.y++;
            score += 1;
            dropTimer = 0.0f;
        }
    }
    if (g_Input.keyState[0] == KEY_DOWN) // Hard Drop
    {
        HardDrop();
    }

    dropTimer += g_Time.deltaTime;
    if (dropTimer >= dropInterval)
    {
        dropTimer = 0.0f;
        if (IsValid(currentPiece.x, currentPiece.y + 1, currentPiece.type, currentPiece.rotation))
        {
            currentPiece.y++;
        }
        else
        {
            LockPiece();
        }
    }
}

void LateUpdate() {}

void PreRender()
{
    ScreenClear();
    InitScreen(80, 25, 0x000F);

    if (currentState != STATE_PLAYING && currentState != STATE_GAMEOVER) return;

    // 보드 테두리 그리기
    for (int y = -1; y < BOARD_H + 1; y++)
    {
        for (int x = -1; x < BOARD_W + 1; x++)
        {
            int drawX = BOARD_OFFSET_X + (x * BLOCK_W);
            int drawY = BOARD_OFFSET_Y + (y * BLOCK_H);

            if (x == -1 || x == BOARD_W || y == BOARD_H)
            {
                DrawRect(drawX, drawY, BLOCK_W, BLOCK_H, 0x0008); // 회색 벽
            }
            else if (y >= 0 && y < BOARD_H)
            {
                int blockType = board[y][x];
                if (blockType != 0)
                {
                    unsigned short color = blockColors[blockType];
                    DrawRect(drawX, drawY, BLOCK_W, BLOCK_H, 0x000F);
                }
                else
                {
                    // 빈칸은 검은색, 가이드 점 제거
                    DrawRect(drawX, drawY, BLOCK_W, BLOCK_H, 0x0000);
                }
            }
        }
    }
}

void Render()
{
    if (currentState != STATE_PLAYING) return;

    // 고스트 블록 그리기
    int ghostY = currentPiece.y;
    while (IsValid(currentPiece.x, ghostY + 1, currentPiece.type, currentPiece.rotation))
    {
        ghostY++;
    }

    for (int i = 0; i < 4; i++)
    {
        int bx = shapes[currentPiece.type][currentPiece.rotation][i][0];
        int by = shapes[currentPiece.type][currentPiece.rotation][i][1];
        int drawX = BOARD_OFFSET_X + ((currentPiece.x + bx) * BLOCK_W);
        int drawY = BOARD_OFFSET_Y + ((ghostY + by) * BLOCK_H);

        if (ghostY + by >= 0)
        {
            //DrawRect(drawX, drawY, BLOCK_W, BLOCK_H, 0x0008); // 짙은 회색
            ScreenPrint(drawX, drawY, L":::");
        }
    }

    // 현재 블록 그리기
    unsigned short pColor = blockColors[currentPiece.type + 1];
    for (int i = 0; i < 4; i++)
    {
        int bx = shapes[currentPiece.type][currentPiece.rotation][i][0];
        int by = shapes[currentPiece.type][currentPiece.rotation][i][1];
        int targetY = currentPiece.y + by;

        if (targetY >= 0)
        {
            int drawX = BOARD_OFFSET_X + ((currentPiece.x + bx) * BLOCK_W);
            int drawY = BOARD_OFFSET_Y + (targetY * BLOCK_H);
            DrawRect(drawX, drawY, BLOCK_W, BLOCK_H, pColor);
            SetColor((pColor & 0xF0) | 0x0F);
            //ScreenPrint(drawX, drawY, L"[ ]");
            //ScreenPrint(drawX, drawY + 1, L" : ");
        }
    }
}

void PostRender()
{
    SetColor(0x000F);

    if (currentState == STATE_MENU)
    {
        ScreenPrint(33, 6, L"== TETRIS ==");
        ScreenPrint(30, 8, L"Custom Board");
        DrawButton(btnStart);
        DrawButton(btnExit);
    }
    else
    {
        // UI 위치 계산 (보드 오른쪽 10칸 띄움)
        int uiX = BOARD_OFFSET_X + (BOARD_W * BLOCK_W) + 10;

        // 점수판 함수 호출
        DrawScorePanel(uiX, 4, score, linesCleared);

        // NEXT 블록 함수 호출
        // (위치: uiX, 11 / 크기: 16x8 / 타입: nextPieceType)
        DrawNextBlockBox(uiX, 11, 16, 8, nextPieceType);

        // 게임 오버 함수 호출
        if (currentState == STATE_GAMEOVER)
        {
            // 화면 크기(80, 25)와 표시 점수 인수를 넘기면 화면 중앙에 팝업띄움
            DrawGameOverPopup(80, 25, score);
        }
    }

    FPS_Gizmo(51, 0, &g_Time, &g_Input);
    ScreenFlipping();
}

void Release()
{
    ReleaseButton(btnStart);
    ReleaseButton(btnExit);
    ScreenRelease();
}

int main(void)
{
    Awake();
    Start();
    TimeInit(); // 타임 초기화 확인

    while (1)
    {
        TimeUpdate();
        InputUpdate();
        while (IsFixedUpdateTime()) FixedUpdate();

        Update();
        LateUpdate();

        PreRender();
        Render();
        PostRender();

        WaitForNextFrame();
    }
    Release();
    return 0;
}