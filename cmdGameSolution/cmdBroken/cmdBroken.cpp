#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <math.h> 

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
// [데이터 구조 정의]
// ==========================================
enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER, STATE_CLEAR };
GameState currentState = STATE_MENU;

// 블록 구조체
typedef struct _Block {
    Rect rect; //위치 정보
    bool isActive; //블록 활성화 여부
    unsigned short color; //색상
} Block;

// ==========================================
// [게임 변수]
// ==========================================
#define BLOCK_ROWS 5
#define BLOCK_COLS 10
#define MAX_BLOCKS (BLOCK_ROWS * BLOCK_COLS)

Block blocks[MAX_BLOCKS]; // 블록 배열
int activeBlockCount = 0; // 남은 블록 개수

Rect paddle = { 35.0f, 22.0f, 10, 2 }; // 플레이어 바
Rect ball = { 40.0f, 20.0f, 3, 2 };    // 공

float paddleSpeed = 35.0f;
float ballSpeedX = 10.0f;
float ballSpeedY = -8.0f; // 위로 발사

int life = 3; // 목숨

BUTTON* btnStart;
BUTTON* btnExit;


// 레벨 초기화를 통해 블록 배치
void SetupLevel()
{
    activeBlockCount = 0;

    // 블록의 크기와 간격 설정
    float startX = 4.0f;
    float startY = 3.0f;
    int bW = 6;
    int bH = 2;
    float gap = 1.0f; // 블록 사이 간격

    for (int y = 0; y < BLOCK_ROWS; y++)
    {
        for (int x = 0; x < BLOCK_COLS; x++)
        {
            int index = y * BLOCK_COLS + x; // 2차원 좌표를 1차원 배열 인덱스로 변환

            blocks[index].rect.x = startX + x * (bW + gap);
            blocks[index].rect.y = startY + y * (bH + gap);
            blocks[index].rect.w = bW;
            blocks[index].rect.h = bH;
            blocks[index].isActive = true;

            // 줄마다 색깔 다르게
            if (y == 0) blocks[index].color = 0x000C; // 빨강
            else if (y == 1) blocks[index].color = 0x000E; // 노랑
            else if (y == 2) blocks[index].color = 0x000A; // 초록
            else if (y == 3) blocks[index].color = 0x000B; // 하늘
            else blocks[index].color = 0x0009; // 파랑

            activeBlockCount++;
        }
    }
}

void ResetBall()
{
    ball.x = paddle.x + (paddle.w / 2) - 1;
    ball.y = paddle.y - 2;
    ballSpeedX = (rand() % 2 == 0) ? 20.0f : -20.0f; // 랜덤 방향
    ballSpeedY = -20.0f; // 위로 쏘기
}

void Awake()
{
    _wsetlocale(LC_ALL, L"korean");
    SetConsoleTitle(L"Dj__Kim's BLOCK BREAKER");
    srand((unsigned int)time(NULL)); // 랜덤 시드
}

void Start()
{
    ScreenInit();
    InputInit();
    TimeInit();

    btnStart = CreateButton(30, 10, 20, 5, L"게임 시작", 0x0007, 0x000E);
    btnExit = CreateButton(30, 17, 20, 5, L"종료 하기", 0x0007, 0x000C);

    SetupLevel();
}

void FixedUpdate() { /* 물리 연산 */ }

void Update()
{
    // 메인 메뉴 상태 (게임 시작 대기)
    if (currentState == STATE_MENU)
    {
        // 시작 버튼이나 엔터를 누르면 -> 게임 시작
        if (UpdateButton(btnStart, &g_Input) || (g_Input.keyState[1] == KEY_DOWN))
        {
            currentState = STATE_PLAYING;
            life = 3;       // 목숨 초기화
            SetupLevel();   // 블록 배치
            ResetBall();    // 공 리셋
        }

        // 종료 버튼
        if (UpdateButton(btnExit, &g_Input)) exit(0);

        return;
    }

    // 게임 오버 또는 클리어 상태 (결과 화면)
    if (currentState == STATE_GAMEOVER || currentState == STATE_CLEAR)
    {
        // 엔터(Enter) 키를 누르면 메인 메뉴로 복귀
        if (g_Input.keyState[1] == KEY_DOWN)
        {
            currentState = STATE_MENU;
        }
        return;
    }

    // 게임 플레이
    // 패들 이동
    if (g_Input.keyState[2] == KEY_HOLD) paddle.x -= paddleSpeed * g_Time.deltaTime;
    if (g_Input.keyState[3] == KEY_HOLD) paddle.x += paddleSpeed * g_Time.deltaTime;

    if (paddle.x < 0) paddle.x = 0;
    if (paddle.x > 80 - paddle.w) paddle.x = 80 - paddle.w;

    // 공 이동
    ball.x += ballSpeedX * g_Time.deltaTime;
    ball.y += ballSpeedY * g_Time.deltaTime;

    // 벽 충돌
    if (ball.x <= 0) { ball.x = 0; ballSpeedX *= -1; }
    if (ball.x >= 80 - ball.w) { ball.x = 80 - ball.w; ballSpeedX *= -1; }
    if (ball.y <= 0) { ball.y = 0; ballSpeedY *= -1; }

    // 바닥 추락 life - 1 
    if (ball.y >= 25)
    {
        life--;
        if (life <= 0)
        {
            currentState = STATE_GAMEOVER; // 게임 오버 상태로 전환
        }
        else
        {
            ResetBall();
            Sleep(500);
        }
    }

    // 패들 충돌
    if (IsColliding(ball, paddle))
    {
        ball.y = paddle.y - ball.h;
        ballSpeedY = -fabs(ballSpeedY);
        float hitPoint = (ball.x + ball.w / 2) - (paddle.x + paddle.w / 2);
        ballSpeedX = hitPoint * 5.0f;
    }

    // 블록 충돌
    for (int i = 0; i < MAX_BLOCKS; i++)
    {
        if (blocks[i].isActive)
        {
            if (IsColliding(ball, blocks[i].rect))
            {
                blocks[i].isActive = false;
                activeBlockCount--;
                ballSpeedY *= -1;

                if (activeBlockCount <= 0) currentState = STATE_CLEAR; // 클리어 상태로 전환
                break;
            }
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

    // [상태별 화면 그리기]
    if (currentState == STATE_MENU)
    {
        // 타이틀 화면
        ScreenPrint(32, 6, L"== BLOCK BREAKER ==");
        DrawButton(btnStart);
        DrawButton(btnExit);
    }
    else if (currentState == STATE_GAMEOVER)
    {
        // 게임 오버 화면
        SetColor(0x000C);
        ScreenPrint(35, 10, L"GAME OVER");

        SetColor(0x000F);
        ScreenPrint(26, 13, L"메인 메뉴로 가려면 [Enter]를 누르세요");

        // 점수 보여주기
        wchar_t scoreMsg[50];
        swprintf_s(scoreMsg, L"Score: %d blocks", MAX_BLOCKS - activeBlockCount);
        ScreenPrint(33, 15, scoreMsg);
    }
    else if (currentState == STATE_CLEAR)
    {
        // 클리어 화면 
        SetColor(0x000B);
        ScreenPrint(35, 10, L"VICTORY!!");

        SetColor(0x000F);
        ScreenPrint(26, 13, L"메인 메뉴로 가려면 [Enter]를 누르세요");
    }
    else
    {
        // 플레이 화면
        DrawRect((int)paddle.x, (int)paddle.y, paddle.w, paddle.h, 0x000B);
        DrawRect((int)ball.x, (int)ball.y, ball.w, ball.h, 0x000F);

        for (int i = 0; i < MAX_BLOCKS; i++)
        {
            if (blocks[i].isActive)
            {
                DrawRect((int)blocks[i].rect.x, (int)blocks[i].rect.y,
                    blocks[i].rect.w, blocks[i].rect.h, blocks[i].color);
            }
        }

        wchar_t uiMsg[50];
        swprintf_s(uiMsg, L"♥ LIFE: %d   ■ LEFT: %d", life, activeBlockCount);
        ScreenPrint(2, 1, uiMsg);
    }
}

void PostRender()
{
    FPS_Gizmo(25, 0, &g_Time, &g_Input); // 우측 상단에 배치
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
        while (IsFixedUpdateTime()) FixedUpdate(); // 물리

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