#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <math.h> // abs 함수 사용

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
// [게임 상태 관리]
// ==========================================
enum GameState { STATE_MENU, STATE_PLAYING };
GameState currentState = STATE_MENU;

// ==========================================
// [게임 오브젝트 데이터]
// ==========================================
Rect playerPaddle = { 2.0f, 10.0f, 2, 6 };    // 왼쪽 플레이어
Rect enemyPaddle = { 76.0f, 10.0f, 2, 6 };    // 오른쪽 적(AI)
Rect ball = { 40.0f, 12.0f, 3, 2 };           // 공

// 속도 및 물리 변수3
float paddleSpeed = 25.0f;
float aiSpeed = 10.0f;       // AI는 밸런스를 위해 플레이어보다 느리게
float ballSpeedX = 30.0f;
float ballSpeedY = 15.0f;

// 점수
int playerScore = 0;
int enemyScore = 0;

// UI 버튼
BUTTON* btnStart;
BUTTON* btnExit;

// ==========================================
// [함수 정의]
// ==========================================

// 공을 중앙으로 리셋하는 함수
void ResetBall(int direction)
{
    ball.x = 40.0f;
    ball.y = 12.0f;
    // direction이 1이면 오른쪽, -1이면 왼쪽으로 공이 움직임
    ballSpeedX = (direction == 1) ? 30.0f : -30.0f;
    ballSpeedY = 15.0f; // 약간 위로 쏘거나 랜덤하게 줄 수도 있음
}

void Awake()
{
    _wsetlocale(LC_ALL, L"korean");
    SetConsoleTitle(L"Dj__Kim's PONG GAME");
}

void Start()
{
    ScreenInit();
    InputInit();
    TimeInit();

    // 타이틀 화면용 버튼 생성
    btnStart = CreateButton(30, 10, 20, 5, L"게임 시작", 0x0007, 0x000E);
    btnExit = CreateButton(30, 17, 20, 5, L"종료 하기", 0x0007, 0x000C);
}

void FixedUpdate()
{
    // 정밀한 물리 연산이 필요하면 여기에 넣지만, 
    // 간단한 Pong은 Update에서 deltaTime으로 처리해도 충분
}

void Update()
{
    // 메뉴 화면일 때
    if (currentState == STATE_MENU)
    {
        // 시작 버튼 처리
        if (UpdateButton(btnStart, &g_Input) || (g_Input.keyState[1] == KEY_DOWN)) // 엔터키로 시작
        {
            currentState = STATE_PLAYING;
            playerScore = 0;
            enemyScore = 0;
            ResetBall(1);
        }

        // 종료 버튼 처리
        if (UpdateButton(btnExit, &g_Input))
        {
            exit(0);
        }
        return; // 메뉴 상태면 아래 게임 로직 실행 안 함
    }

    // 게임 플레이 중일 때
    // 플레이어 이동 (위: Up, 아래: Down)
    if (g_Input.keyState[4] == KEY_HOLD) playerPaddle.y -= paddleSpeed * g_Time.deltaTime;
    if (g_Input.keyState[5] == KEY_HOLD) playerPaddle.y += paddleSpeed * g_Time.deltaTime;

    // 플레이어 화면 밖 방지
    if (playerPaddle.y < 0) playerPaddle.y = 0;
    if (playerPaddle.y > 25 - playerPaddle.h) playerPaddle.y = 25 - playerPaddle.h;


    // 적(AI) 이동
    // 공이 AI보다 위에 있으면 올라가고, 아래에 있으면 내려감
    // 실수할 확률을 주기 위해 중앙(ball.y)이 아니라 약간의 오차를 줘도 됨
    float targetY = ball.y - (enemyPaddle.h / 2); // 공을 패들 중앙에 맞추려 노력함

    if (enemyPaddle.y < targetY) enemyPaddle.y += aiSpeed * g_Time.deltaTime;
    if (enemyPaddle.y > targetY) enemyPaddle.y -= aiSpeed * g_Time.deltaTime;

    // AI 화면 밖 방지
    if (enemyPaddle.y < 0) enemyPaddle.y = 0;
    if (enemyPaddle.y > 25 - enemyPaddle.h) enemyPaddle.y = 25 - enemyPaddle.h;


    // 공 이동
    ball.x += ballSpeedX * g_Time.deltaTime;
    ball.y += ballSpeedY * g_Time.deltaTime;


    // 공 벽 충돌
    if (ball.y <= 0)
    {
        ball.y = 0;
        ballSpeedY *= -1; // Y축 방향 반전
    }
    if (ball.y >= 25 - ball.h) // 25는 화면 높이
    {
        ball.y = 25 - ball.h;
        ballSpeedY *= -1;
    }

    // 공 패들과 플레이어 충돌
    if (IsColliding(ball, playerPaddle))
    {
        ball.x = playerPaddle.x + playerPaddle.w; // 밀어내기를 통해 겹침 방지
        ballSpeedX = fabs(ballSpeedX); // 무조건 오른쪽(+)으로 보냄
        ballSpeedX += 2.0f; // 칠 때마다 조금씩 빨라짐
    }

    // 적과 충돌
    if (IsColliding(ball, enemyPaddle))
    {
        ball.x = enemyPaddle.x - ball.w; // 겹침 방지
        ballSpeedX = -fabs(ballSpeedX); // 무조건 왼쪽(-)으로 보냄
    }

    // 점수 획득 및 리셋
    if (ball.x < 0)
    {
        // 적 득점
        enemyScore++;
        ResetBall(1); // 플레이어 쪽으로 발사
        ballSpeedX = 30.0f; // 속도 초기화
        Sleep(500); // 득점 후 잠깐 대기
    }
    else if (ball.x > 80)
    {
        // 플레이어 득점
        playerScore++;
        ResetBall(-1); // 적 쪽으로 발사
        ballSpeedX = -30.0f;
        Sleep(500);
    }
}

void LateUpdate()
{
    // 게임 종료 조건 (5점 내기)
    if (playerScore >= 5 || enemyScore >= 5)
    {
        MessageBox(NULL, playerScore >= 5 ? L"승리했습니다!" : L"패배했습니다...", L"게임 종료", MB_OK);
        currentState = STATE_MENU; // 타이틀로 복귀
        playerScore = 0;
        enemyScore = 0;
    }
}

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
        // 타이틀 화면 그리기
        ScreenPrint(35, 6, L"== PONG ==");
        ScreenPrint(28, 7, L"Up/Down 키로 이동하세요");
        DrawButton(btnStart);
        DrawButton(btnExit);
    }
    else
    {
        // 게임 화면 그리기

        // 게임판 중앙선 그리기
        for (int i = 0; i < 25; i += 2) ScreenPrint(40, i, L"|");

        // 점수판 그리기
        wchar_t scoreBuffer[30];
        swprintf_s(scoreBuffer, L"%d  :  %d", playerScore, enemyScore);
        ScreenPrint(36, 2, scoreBuffer);

        // 오브젝트 그리기
        DrawRect((int)playerPaddle.x, (int)playerPaddle.y, playerPaddle.w, playerPaddle.h, 0x000B); // 플레이어(하늘색)
        DrawRect((int)enemyPaddle.x, (int)enemyPaddle.y, enemyPaddle.w, enemyPaddle.h, 0x000C);    // 적(빨간색)
        DrawRect((int)ball.x, (int)ball.y, ball.w, ball.h, 0x000E);          // 공(노란색)
    }
}

void PostRender()
{
    // 기즈모는 어디서든 보이면 좋음
    FPS_Gizmo(25, 22, &g_Time, &g_Input);
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

        // 물리 연산 루프
        while (IsFixedUpdateTime())
        {
            FixedUpdate();
        }

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