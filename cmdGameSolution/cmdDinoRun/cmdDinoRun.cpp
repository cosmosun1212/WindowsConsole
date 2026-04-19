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
#include "./cmdUI/ButtonObject.h"

extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;

// ==========================================
// [게임 데이터]
// ==========================================
enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER };
GameState currentState = STATE_MENU;

// 물리 상수
const float GRAVITY = 70.0f;
const float JUMP_FORCE = 35.0f;
const float GROUND_Y = 18.0f;
const float FIXED_TIMESTEP = 0.02f; // 물리 연산은 무조건 0.02초(50 FPS) 기준

typedef struct _Dino 
{
    Rect rect;
    float velocityY;
    bool isGrounded;
} Dino;

typedef struct _Cactus 
{
    Rect rect;
    bool isActive;
} Cactus;

Dino dino;
#define MAX_CACTUS 3
Cactus cacti[MAX_CACTUS];

float gameSpeed = 30.0f;
float score = 0.0f;

// Update와 FixedUpdate를 이어주는 신호 변수
bool isJumpKeyPressed = false;

BUTTON* btnStart;
BUTTON* btnExit;


void ResetGame()
{
    dino.rect.x = 10.0f;
    dino.rect.y = GROUND_Y;
    dino.rect.w = 4;
    dino.rect.h = 4;
    dino.velocityY = 0.0f;
    dino.isGrounded = true;

    for (int i = 0; i < MAX_CACTUS; i++)
    {
        cacti[i].isActive = true;
        cacti[i].rect.w = 3;
        cacti[i].rect.h = 3;
        cacti[i].rect.y = GROUND_Y + 1;
        cacti[i].rect.x = 80.0f + (i * 40.0f);
    }

    gameSpeed = 30.0f;
    score = 0.0f;
    isJumpKeyPressed = false;
}

void Awake()
{
    _wsetlocale(LC_ALL, L"korean");
    SetConsoleTitle(L"Dj__Kim's DINO RUN (Physics Ver.)");
    srand((unsigned int)time(NULL));
}

void Start()
{
    ScreenInit();
    InputInit();
    TimeInit();

    btnStart = CreateButton(30, 8, 20, 5, L"달리기 시작", 0x0007, 0x000E);
    btnExit = CreateButton(30, 15, 20, 5, L"종료 하기", 0x0007, 0x000C);
}


// deltaTime 대신 고정된 시간(FIXED_TIMESTEP)을 사용합니다.
void FixedUpdate()
{
    if (currentState != STATE_PLAYING) return;

    // 점프 처리 (Update에서 받은 신호 처리)
    if (isJumpKeyPressed && dino.isGrounded)
    {
        dino.velocityY = -JUMP_FORCE;
        dino.isGrounded = false;
        isJumpKeyPressed = false; // 신호 소비 (껐음)
    }

    // 중력 적용 (공식: 속도 += 가속도 * 시간)
    dino.velocityY += GRAVITY * FIXED_TIMESTEP;

    // 공룡 이동 (공식: 위치 += 속도 * 시간)
    dino.rect.y += dino.velocityY * FIXED_TIMESTEP;

    // 바닥 충돌 (Ground Check)
    if (dino.rect.y >= GROUND_Y)
    {
        dino.rect.y = GROUND_Y;
        dino.velocityY = 0.0f;
        dino.isGrounded = true;
        isJumpKeyPressed = false; // 땅에 닿으면 점프 예약 취소
    }

    // 장애물 이동 (무한 스크롤)
    for (int i = 0; i < MAX_CACTUS; i++)
    {
        cacti[i].rect.x -= gameSpeed * FIXED_TIMESTEP; // 고정 시간 사용

        // 화면 밖 재활용 로직
        if (cacti[i].rect.x < -5)
        {
            float maxX = 80.0f;
            for (int j = 0; j < MAX_CACTUS; j++) {
                if (cacti[j].rect.x > maxX) maxX = cacti[j].rect.x;
            }
            cacti[i].rect.x = maxX + 25.0f + (rand() % 20);
        }

        // 충돌 판정
        Rect dinoHitbox = dino.rect;
        dinoHitbox.x += 1; dinoHitbox.w -= 2;
        dinoHitbox.y += 1; dinoHitbox.h -= 1;

        if (IsColliding(dinoHitbox, cacti[i].rect))
        {
            currentState = STATE_GAMEOVER;
        }
    }

    // 난이도 상승
    score += 10.0f * FIXED_TIMESTEP;
    gameSpeed += 1.0f * FIXED_TIMESTEP;
}

void Update()
{
    // [메뉴 / 게임오버]
    if (currentState == STATE_MENU)
    {
        if (UpdateButton(btnStart, &g_Input) || g_Input.keyState[0] == KEY_DOWN)
        {
            ResetGame();
            currentState = STATE_PLAYING;
        }
        if (UpdateButton(btnExit, &g_Input)) exit(0);
        return;
    }

    if (currentState == STATE_GAMEOVER)
    {
        if (g_Input.keyState[0] == KEY_DOWN || g_Input.keyState[1] == KEY_DOWN)
            currentState = STATE_MENU;
        return;
    }

    // [게임 플레이 입력 체크]
    if (g_Input.keyState[0] == KEY_DOWN) // 스페이스바
    {
        if (dino.isGrounded)
        {
            isJumpKeyPressed = true; // 점프 요청 신호 보냄
        }
    }
}

// 모든 물리 계산이 끝난 후 점검
void LateUpdate()
{
    if (currentState != STATE_PLAYING) return;

    // 화면 이탈 방지
    // 물리 엔진(FixedUpdate)에서 공룡을 하늘 높이 날려도 화면 밖으로 못 나가게 막음
    if (dino.rect.y < 0)
    {
        dino.rect.y = 0; // 천장에 머리 쿵
        dino.velocityY = 0.0f; // 위로 올라가려는 힘 제거
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
        ScreenPrint(33, 6, L"== DINO RUN ==");
        ScreenPrint(26, 20, L"Spacebar: 점프 / Jump");
        DrawButton(btnStart);
        DrawButton(btnExit);
    }
    else if (currentState == STATE_GAMEOVER)
    {
        SetColor(0x000C);
        ScreenPrint(35, 10, L"GAME OVER");
        SetColor(0x000F);

        wchar_t scoreMsg[50];
        swprintf_s(scoreMsg, L"Distance: %.0f m", score);
        ScreenPrint(33, 12, scoreMsg);
        ScreenPrint(26, 14, L"다시 하려면 [Space] or [Enter]");
    }
    else
    {
        // [게임 화면]
        for (int i = 0; i < 80; i++) ScreenPrint(i, (int)GROUND_Y + 4, L"_");

        DrawRect((int)dino.rect.x, (int)dino.rect.y, dino.rect.w, dino.rect.h, 0x000A);
        ScreenPrint((int)dino.rect.x + 2, (int)dino.rect.y + 1, L"●");

        for (int i = 0; i < MAX_CACTUS; i++)
        {
            DrawRect((int)cacti[i].rect.x, (int)cacti[i].rect.y,
                cacti[i].rect.w, cacti[i].rect.h, 0x000E);
        }

        wchar_t uiMsg[50];
        swprintf_s(uiMsg, L"SCORE: %.0f   SPEED: %.1f", score, gameSpeed);
        ScreenPrint(2, 2, uiMsg);
    }
}

void PostRender()
{
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
    TimeInit();

    while (1)
    {
        TimeUpdate();
        InputUpdate();

        // 물리 연산 루프 1초 = FixedUpdate(0.02초)로 50번 실행
        // 엔진 내부의 IsFixedUpdateTime() 함수가 이 횟수를 맞춰 줌
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