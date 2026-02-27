#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <math.h> 

// 엔진 헤더 파일들
#include "ScreenBuffer.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "CmdUtil.h"
#include "GameUI.h"
#include "Collision.h"
#include "ButtonObject.h"

// 전역 변수 참조
extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;

// ==========================================
// [게임 설정 및 데이터]
// ==========================================
enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER };
GameState currentState = STATE_MENU;

enum Direction { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NONE };

// 뱀 설정
#define MAX_SNAKE_LENGTH 100
Rect snakeBody[MAX_SNAKE_LENGTH];
int snakeLength = 0;
int currentDir = DIR_NONE;
int nextDir = DIR_NONE;

// 타이머 및 속도 설정
float moveTimer = 0.0f;
float moveInterval = 0.3f;

// 먹이 및 점수
Rect food;
int score = 0;

// UI 버튼
BUTTON* btnStart;
BUTTON* btnExit;

// ==========================================
// [함수 정의]
// ==========================================

// 먹이 생성 함수 (그리드 좌표계 적용)
void SpawnFood()
{
    bool validPosition = false;
    while (!validPosition)
    {
        validPosition = true;

        // 뱀 크기(3x2)에 맞춘 그리드 랜덤 좌표 계산
        // 화면 가로 80 / 3 = 약 26칸, 세로 25 / 2 = 약 12칸
        int gridX = rand() % 25;
        int gridY = rand() % 11;

        // 실제 좌표로 변환 (+ 여백 2)
        food.x = (float)(gridX * 3) + 2;
        food.y = (float)(gridY * 2) + 2;

        // 먹이 크기도 뱀과 동일하게 3x2
        food.w = 3;
        food.h = 2;

        // 뱀 몸통과 겹치는지 확인
        for (int i = 0; i < snakeLength; i++)
        {
            if (IsColliding(food, snakeBody[i]))
            {
                validPosition = false;
                break;
            }
        }
    }
}

// 게임 초기화
void ResetGame()
{
    snakeLength = 5; // 초기 길이
    currentDir = DIR_RIGHT;
    nextDir = DIR_RIGHT;

    moveTimer = 0.0f;
    moveInterval = 0.3f; //초기 속도 (0.3초마다 한 칸)

    score = 0;

    // 뱀 초기 배치 (Game Over 방지)
    // 몸통 너비가 3이므로, 3칸씩 띄워서 배치해야 겹치지 않음
    for (int i = 0; i < snakeLength; i++)
    {
        snakeBody[i].x = 40.0f - (i * 3); // 3칸 간격!
        snakeBody[i].y = 12.0f;
        snakeBody[i].w = 3; // 너비 3
        snakeBody[i].h = 2; // 높이 2
    }

    SpawnFood();
}

void Awake()
{
    _wsetlocale(LC_ALL, L"korean");
    SetConsoleTitle(L"Dj__Kim's SNAKE BITE");
    srand((unsigned int)time(NULL));
}

void Start()
{
    ScreenInit();
    InputInit();
    TimeInit();

    btnStart = CreateButton(30, 10, 20, 5, L"게임 시작", 0x0007, 0x000E);
    btnExit = CreateButton(30, 17, 20, 5, L"종료 하기", 0x0007, 0x000C);
}

void FixedUpdate() { /* 물리 연산 필요 시 사용 */ }

void Update()
{
    //  메뉴 상태
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

    // 게임 오버 상태
    if (currentState == STATE_GAMEOVER)
    {
        // Enter 누르면 메인 메뉴로 복귀
        if (g_Input.keyState[1] == KEY_DOWN)
            currentState = STATE_MENU;
        return;
    }

    // 게임 플레이 상태

    // 방향 입력 처리
    if (g_Input.keyState[4] == KEY_DOWN && currentDir != DIR_DOWN) nextDir = DIR_UP;
    if (g_Input.keyState[5] == KEY_DOWN && currentDir != DIR_UP)   nextDir = DIR_DOWN;
    if (g_Input.keyState[2] == KEY_DOWN && currentDir != DIR_RIGHT) nextDir = DIR_LEFT;
    if (g_Input.keyState[3] == KEY_DOWN && currentDir != DIR_LEFT)  nextDir = DIR_RIGHT;

    // 이동 타이머 체크
    moveTimer += g_Time.deltaTime;

    if (moveTimer >= moveInterval)
    {
        moveTimer = 0.0f;
        currentDir = nextDir;

        // 꼬리 이동은 뒤에서부터 앞의 좌표를 복사
        for (int i = snakeLength - 1; i > 0; i--)
        {
            snakeBody[i] = snakeBody[i - 1]; // 좌표 및 크기 복사
        }

        // 머리 이동은 방향에 따른 좌표 변경
        switch (currentDir)
        {
        case DIR_UP:    snakeBody[0].y -= 2; break;
        case DIR_DOWN:  snakeBody[0].y += 2; break;
        case DIR_LEFT:  snakeBody[0].x -= 3; break;
        case DIR_RIGHT: snakeBody[0].x += 3; break;
        }

        // 3충돌 체크 (머리 기준)
        Rect* head = &snakeBody[0];

        // 벽 충돌
        if (head->x < 0 || head->x >= 80 || head->y < 0 || head->y >= 25)
        {
            currentState = STATE_GAMEOVER;
        }

        // 자기 자신 충돌
        for (int i = 1; i < snakeLength; i++)
        {
            if (IsColliding(*head, snakeBody[i]))
            {
                currentState = STATE_GAMEOVER;
            }
        }

        // 먹이 충돌
        if (IsColliding(*head, food))
        {
            score += 10;

            // 길이 증가
            if (snakeLength < MAX_SNAKE_LENGTH)
            {
                snakeBody[snakeLength] = snakeBody[snakeLength - 1];
                snakeLength++;
            }

            // 속도 조금씩 증가 (최대 속도 제한)
            if (moveInterval > 0.1f) moveInterval -= 0.005f;

            SpawnFood(); // 새로운 먹이 배치
        }
    }
}

void LateUpdate() {}

void PreRender()
{
    ScreenClear();
    InitScreen(80, 25, 0x000F);
}

void Render()
{
    SetColor(0x000F);

    if (currentState == STATE_MENU)
    {
        ScreenPrint(33, 6, L"== SNAKE BITE ==");
        ScreenPrint(28, 22, L"방향키로 뱀을 조종하세요");
        DrawButton(btnStart);
        DrawButton(btnExit);
    }
    else if (currentState == STATE_GAMEOVER)
    {
        SetColor(0x000C); // 빨간색
        ScreenPrint(35, 10, L"GAME OVER");

        SetColor(0x000F);
        wchar_t scoreMsg[50];
        swprintf_s(scoreMsg, L"Final Score: %d", score);
        ScreenPrint(32, 12, scoreMsg);
        ScreenPrint(26, 14, L"메인 메뉴로 가려면 [Enter]");
    }
    else
    {
        // [게임 화면 그리기]

        // 먹이 (빨간색)
        DrawRect((int)food.x, (int)food.y, food.w, food.h, 0x000C);

        // 뱀 (머리: 노랑, 몸통: 초록)
        for (int i = 0; i < snakeLength; i++)
        {
            if (i == 0)
                DrawRect((int)snakeBody[i].x, (int)snakeBody[i].y, snakeBody[i].w, snakeBody[i].h, 0x000E);
            else
                DrawRect((int)snakeBody[i].x, (int)snakeBody[i].y, snakeBody[i].w, snakeBody[i].h, 0x000A);
        }

        // 3점수 표시 UI
        wchar_t uiMsg[50];
        swprintf_s(uiMsg, L"SCORE: %d / Len: %d", score, snakeLength);
        ScreenPrint(2, 1, uiMsg);
    }
}

void PostRender()
{
    // 디버그 정보 (FPS, 마우스)
    FPS_Gizmo(25, 0, &g_Time, &g_Input);
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
        
        TimeUpdate(); // 시간 갱신
 
        InputUpdate(); // 입력 처리

        // 물리 연산 
        while (IsFixedUpdateTime())
        {
            FixedUpdate();
        }

        // 게임 로직
        Update();
        LateUpdate();

        // 렌더링
        PreRender();
        Render();
        PostRender();

        // 프레임 대기 (입력 반응성 확보)
        WaitForNextFrame();
    }
    Release();
    return 0;
}