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
// [게임 데이터]
// ==========================================
enum GameState { STATE_MENU, STATE_PLAYING, STATE_CLEAR, STATE_ALL_CLEAR };
GameState currentState = STATE_MENU;

#define MAP_H 10
#define MAP_W 15
#define TILE_W 3
#define TILE_H 2

const char stageData[3][MAP_H][MAP_W + 1] = {
    // Stage 1
    {
        "###############",
        "#             #",
        "#   @         #",
        "#      $      #",
        "#             #",
        "#      .      #",
        "#             #",
        "#             #",
        "#             #",
        "###############"
    },
    // Stage 2
    {
        "###############",
        "#             #",
        "#  ##   ###   #",
        "#  #  $   #   #",
        "#  #   .  #   #",
        "#  # . $  #   #",
        "#  ### @ ##   #",
        "#             #",
        "#             #",
        "###############"
    },
    // Stage 3
    {
        "###############",
        "#####    ######",
        "##### $  ######",
        "##### $  ######",
        "###  @  .  ####",
        "###   .  ######",
        "##### .  ######",
        "###############",
        "###############",
        "###############"
    }
};

char currentMap[MAP_H][MAP_W + 1];
int currentStage = 0;
int playerX, playerY;
int moveCount = 0;

// 맵 그리기 위치 오프셋
const int MAP_OFFSET_X = 17;
const int MAP_OFFSET_Y = 4;

BUTTON* btnStart;
BUTTON* btnExit;

void LoadStage(int stageIndex)
{
    if (stageIndex >= 3)
    {
        currentState = STATE_ALL_CLEAR;
        return;
    }

    moveCount = 0;
    currentStage = stageIndex;

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            char tile = stageData[stageIndex][y][x];
            currentMap[y][x] = tile;
            if (tile == '@') { playerX = x; playerY = y; }
        }
    }
}

void MovePlayer(int dx, int dy)
{
    int nextX = playerX + dx;
    int nextY = playerY + dy;

    if (currentMap[nextY][nextX] == '#') return;

    // 박스 밀기 로직
    if (currentMap[nextY][nextX] == '$' || currentMap[nextY][nextX] == '*')
    {
        int boxNextX = nextX + dx;
        int boxNextY = nextY + dy;
        char behindBox = currentMap[boxNextY][boxNextX];
        if (behindBox == '#' || behindBox == '$' || behindBox == '*') return;

        // 박스 원래 자리 처리
        if (currentMap[nextY][nextX] == '$') currentMap[nextY][nextX] = ' ';
        else if (currentMap[nextY][nextX] == '*') currentMap[nextY][nextX] = '.';

        // 박스 갈 자리 처리
        if (currentMap[boxNextY][boxNextX] == '.') currentMap[boxNextY][boxNextX] = '*';
        else currentMap[boxNextY][boxNextX] = '$';
    }

    // 플레이어 이동 로직
    if (currentMap[playerY][playerX] == '+') currentMap[playerY][playerX] = '.';
    else currentMap[playerY][playerX] = ' ';

    if (currentMap[nextY][nextX] == '.' || currentMap[nextY][nextX] == '*')
        currentMap[nextY][nextX] = '+';
    else
        currentMap[nextY][nextX] = '@';

    playerX = nextX;
    playerY = nextY;
    moveCount++;
}

bool CheckClear()
{
    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            if (currentMap[y][x] == '$') return false;
        }
    }
    return true;
}

void Awake()
{
    _wsetlocale(LC_ALL, L"korean");
    SetConsoleTitle(L"Dj__Kim's SOKOBAN (Layered Architecture Ver.)");
}

void Start()
{
    ScreenInit();
    InputInit();
    TimeInit();

    btnStart = CreateButton(30, 10, 20, 5, L"게임 시작", 0x0007, 0x000E);
    btnExit = CreateButton(30, 17, 20, 5, L"종료 하기", 0x0007, 0x000C);
}

void FixedUpdate() {}

void Update()
{
    if (currentState == STATE_MENU)
    {
        if (UpdateButton(btnStart, &g_Input) || g_Input.keyState[1] == KEY_DOWN)
        {
            LoadStage(0);
            currentState = STATE_PLAYING;
        }
        if (UpdateButton(btnExit, &g_Input)) exit(0);
        return;
    }

    if (currentState == STATE_CLEAR)
    {
        if (g_Input.keyState[1] == KEY_DOWN)
        {
            LoadStage(currentStage + 1);
            if (currentState != STATE_ALL_CLEAR) currentState = STATE_PLAYING;
        }
        return;
    }

    if (currentState == STATE_ALL_CLEAR)
    {
        if (g_Input.keyState[1] == KEY_DOWN) currentState = STATE_MENU;
        return;
    }

    // 게임 플레이 입력
    if (g_Input.keyState[2] == KEY_DOWN) MovePlayer(-1, 0);
    if (g_Input.keyState[3] == KEY_DOWN) MovePlayer(1, 0);
    if (g_Input.keyState[4] == KEY_DOWN) MovePlayer(0, -1);
    if (g_Input.keyState[5] == KEY_DOWN) MovePlayer(0, 1);
    if (GetAsyncKeyState(0x52) & 0x8000) LoadStage(currentStage);

    if (CheckClear()) currentState = STATE_CLEAR;
}

void LateUpdate() {}

// =================================================================
// [1단계: PreRender] 배경(맵의 정적인 요소) 그리기
// 벽(#), 바닥( ), 골인 지점(.) 등 변하지 않거나 바닥에 깔리는 것들
// =================================================================
void PreRender()
{
    ScreenClear();
    InitScreen(80, 25, 0x000F);

    if (currentState != STATE_PLAYING && currentState != STATE_CLEAR) return;

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            char tile = currentMap[y][x];
            int drawX = MAP_OFFSET_X + (x * TILE_W);
            int drawY = MAP_OFFSET_Y + (y * TILE_H);

            // [배경 레이어] : 플레이어(@)나 박스($)가 있어도, 그 아래 '바닥'을 먼저 깔아야 함
            if (tile == '#') // 벽
            {
                DrawRect(drawX, drawY, TILE_W, TILE_H, 0x0008);
                SetColor(0x008F);
                ScreenPrint(drawX, drawY, L"###");
                ScreenPrint(drawX, drawY + 1, L"###");
            }
            // 골인 지점에(., +, *) 누가 위에 있든 바닥은 골인 지점임
            else if (tile == '.' || tile == '+' || tile == '*')
            {
                DrawRect(drawX, drawY, TILE_W, TILE_H, 0x0004); // 빨간 바닥
                SetColor(0x004F);
                ScreenPrint(drawX, drawY, L" . ");
                ScreenPrint(drawX, drawY + 1, L"GOAL");
            }
            else // 기본 바닥은 검정, 나머지 (공백, 플레이어, 박스)
            {
                DrawRect(drawX, drawY, TILE_W, TILE_H, 0x0000);
            }
        }
    }
}

// =================================================================
// [2단계: Render] 동적인 오브젝트(캐릭터, 박스) 그리기
// 배경 위에 덧칠해지는 움직이는 요소들
// =================================================================
void Render()
{
    if (currentState != STATE_PLAYING && currentState != STATE_CLEAR) return;

    SetColor(0x000F);

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            char tile = currentMap[y][x];
            int drawX = MAP_OFFSET_X + (x * TILE_W);
            int drawY = MAP_OFFSET_Y + (y * TILE_H);

            // [오브젝트 레이어]
            // 여기서는 벽이나 빈 바닥은 무시하고, '물체'만 그린다.

            if (tile == '$' || tile == '*') // [박스]
            {
                // 골인된 박스(*)는 색을 다르게
                unsigned short boxColor = (tile == '*') ? 0x000B : 0x000E; // 파랑 or 노랑
                unsigned short fontColor = (tile == '*') ? 0x00BF : 0x00E0;

                DrawRect(drawX, drawY, TILE_W, TILE_H, boxColor);
                SetColor(fontColor);
                if (tile == '*') {
                    ScreenPrint(drawX, drawY, L"OK!");
                    ScreenPrint(drawX, drawY + 1, L"^_^");
                }
                else {
                    ScreenPrint(drawX, drawY, L"[=]");
                    ScreenPrint(drawX, drawY + 1, L" | ");
                }
            }
            else if (tile == '@' || tile == '+') // [플레이어]
            {
                // 플레이어(@) 혹은 골 위의 플레이어(+)
                DrawRect(drawX, drawY, TILE_W, TILE_H, 0x000F);
                SetColor(0x00F0);
                ScreenPrint(drawX, drawY, L"O_O");
                ScreenPrint(drawX, drawY + 1, L"bod");
            }
        }
    }
}

// =================================================================
// [3단계: PostRender] UI 및 HUD 그리기
// 무조건 맨 위에 보여야 하는 정보들
// =================================================================
void PostRender()
{
    SetColor(0x000F);

    // 메뉴 UI
    if (currentState == STATE_MENU)
    {
        ScreenPrint(33, 6, L"== SOKOBAN ==");
        ScreenPrint(30, 8, L"Layered Architecture");
        DrawButton(btnStart);
        DrawButton(btnExit);
    }
    // 올 클리어 UI
    else if (currentState == STATE_ALL_CLEAR)
    {
        SetColor(0x000E);
        ScreenPrint(30, 10, L"CONGRATULATIONS!");
        ScreenPrint(28, 15, L"[Enter] 메인 메뉴로");
    }
    // 인게임 HUD
    else
    {
        // 상단 정보창
        SetColor(0x000F);
        wchar_t uiMsg[50];
        swprintf_s(uiMsg, L"STAGE: %d   MOVES: %d", currentStage + 1, moveCount);
        ScreenPrint(2, 2, uiMsg);

        ScreenPrint(60, 2, L"[R]: Reset");
        ScreenPrint(60, 3, L"[WASD]: Move");

        // 스테이지 클리어 알림 (맵과 캐릭터 위에 뜸)
        if (currentState == STATE_CLEAR)
        {
            DrawRect(25, 10, 30, 5, 0x0009); // 파란 박스 배경
            SetColor(0x009F);
            ScreenPrint(33, 11, L"STAGE CLEAR!");
            ScreenPrint(30, 13, L"PRESS [ENTER] >>");
        }
    }

    // 디버그 및 화면 출력
    FPS_Gizmo(50, 0, &g_Time, &g_Input);
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
    TimeInit();

    while (1)
    {
        TimeUpdate();
        InputUpdate();
        while (IsFixedUpdateTime()) FixedUpdate();

        Update();
        LateUpdate();

        // 렌더링 파이프라인
        PreRender();  // 1층: 배경 (벽, 바닥)
        Render();     // 2층: 오브젝트 (캐릭터, 박스)
        PostRender(); // 3층: UI (점수, 메뉴)

        WaitForNextFrame();
    }
    Release();
    return 0;
}