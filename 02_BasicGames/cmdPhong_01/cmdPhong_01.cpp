
//// 간단한 Pong 게임 구현

//// 필요한 헤더 파일 포함
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <math.h> 

//// 게임 엔진 관련 헤더 파일 포함
#include "ScreenBuffer.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Collision.h"

/// 게임 유틸리티 및 UI 관련 헤더 파일 포함
#include "CmdUtil.h"
#include "ButtonObject.h"

using namespace CmdUI; // CmdUI 네임스페이스 사용

// 다른 cpp파일에 정의된 전역 변수 가져오기
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
float paddleSpeed = 25.0f;  // 플레이어 패들 이동 속도
float aiSpeed = 10.0f;       // AI는 밸런스를 위해 플레이어보다 느리게
float ballSpeedX = 30.0f;    // 공의 X축 속도
float ballSpeedY = 15.0f;    // 공의 Y축 속도

// 점수
int playerScore = 0;    // 플레이어 점수
int enemyScore = 0;     // 적 점수

// UI 버튼
BUTTON* btnStart;  // 게임 시작 버튼      
BUTTON* btnExit; // 종료 버튼

bool bIsGameOver = false; // 게임 오버 상태 플래그


// 공을 초기 위치로 리셋하고, 방향에 따라 속도를 설정하는 함수
void ResetBall(int direction)
{
    // 화면 중앙으로 리셋
	ball.x = 40.0f;  
    ball.y = 12.0f;

    // direction이 1이면 오른쪽, -1이면 왼쪽으로 공이 움직임
    ballSpeedX = (direction == 1) ? 30.0f : -30.0f;
    ballSpeedY = 15.0f; // 약간 위로 쏘거나 랜덤하게 줄 수도 있음
}

// 게임 초기화 함수
void Awake()
{
	_wsetlocale(LC_ALL, L"korean"); // 콘솔에서 한글이 깨지는 문제 방지
	SetConsoleTitle(L"Dj__Kim's PONG GAME"); // 콘솔 창 제목 설정
}


// 게임 시작 시 초기화 함수
void Start()
{
	ScreenInit(); // 화면 버퍼 초기화
	InputInit(); // 입력 시스템 초기화
	TimeInit(); // 시간 관리 시스템 초기화

    // 타이틀 화면용 버튼 생성
    btnStart = CreateButton(30, 10, 20, 5, L"게임 시작", WHITE, YELLOW);
    btnExit = CreateButton(30, 17, 20, 5, L"종료 하기", WHITE, RED);

	bIsGameOver = true; // 게임이 시작되기 전에는 게임 오버 상태로 설정
}

// 고정된 시간 간격으로 실행되는 업데이트 함수
void FixedUpdate()
{
    // 정밀한 물리 연산이 필요하면 여기에 넣지만, 
    // 간단한 Pong은 Update에서 deltaTime으로 처리해도 충분
}

// 매 프레임마다 실행되는 업데이트 함수
void Update()
{
    // 메뉴 화면일 때
    if (currentState == STATE_MENU)
    {
        // 시작 버튼 처리
        if (Update(btnStart, &g_Input) || (g_Input.keyState[VK::Return] == KEY_DOWN)) // 엔터키로 시작
        {
			currentState = STATE_PLAYING; // 게임 상태로 전환
			playerScore = 0; // 플레이어 점수 초기화
			enemyScore = 0; // 적 점수 초기화
			ResetBall(1);  // 공 초기화 (플레이어 쪽으로 발사)
        }

        // 종료 버튼 처리
		if (Update(btnExit, &g_Input) || (g_Input.keyState[VK::Esc] == KEY_DOWN)) // ESC키로 종료   
        {
            bIsGameOver = false;
        }
        return; // 메뉴 상태면 아래 게임 로직 실행 안 함
    }

    // 게임 플레이 중일 때
    // 플레이어 이동 (위: Up, 아래: Down)
	if (g_Input.keyState[VK::Up] == KEY_HOLD) playerPaddle.y -= paddleSpeed * g_Time.deltaTime; // 위로 이동
    if (g_Input.keyState[VK::Down] == KEY_HOLD) playerPaddle.y += paddleSpeed * g_Time.deltaTime; // 아래로 이동

    // 플레이어 화면 밖 방지
	if (playerPaddle.y < 0) playerPaddle.y = 0; // 화면 상단 방지
    if (playerPaddle.y > 25 - playerPaddle.h) playerPaddle.y = 25 - playerPaddle.h; // 화면 하단 방지 


    // 적(AI) 이동
    // 공이 AI보다 위에 있으면 올라가고, 아래에 있으면 내려감
    // 실수할 확률을 주기 위해 중앙(ball.y)이 아니라 약간의 오차를 줘도 됨
    float targetY = ball.y - (enemyPaddle.h / 2); // 공을 패들 중앙에 맞추려 노력함

	if (enemyPaddle.y < targetY) enemyPaddle.y += aiSpeed * g_Time.deltaTime; // 아래로 이동
	if (enemyPaddle.y > targetY) enemyPaddle.y -= aiSpeed * g_Time.deltaTime; // 위로 이동

    // AI 화면 밖 방지
	if (enemyPaddle.y < 0) enemyPaddle.y = 0; // 화면 상단 방지
    if (enemyPaddle.y > 25 - enemyPaddle.h) enemyPaddle.y = 25 - enemyPaddle.h; // 화면 하단 방지


    // 공 이동
    ball.x += ballSpeedX * g_Time.deltaTime; // X축 이동
    ball.y += ballSpeedY * g_Time.deltaTime; // Y축 이동


    // 공 벽 충돌
	if (ball.y <= 0)    // 화면 상단에 닿으면
    {
        ball.y = 0;
        ballSpeedY *= -1; // Y축 방향 반전
    }
	if (ball.y >= 25 - ball.h) // 화면 하단에 닿으면 // 25는 화면 높이
    {
		ball.y = 25 - ball.h; // 겹침 방지
		ballSpeedY *= -1; // Y축 방향 반전
    }

    // 공 패들과 플레이어 충돌
	if (IsColliding(ball, playerPaddle)) // 플레이어 패들과 충돌했을 때
    {
        ball.x = playerPaddle.x + playerPaddle.w; // 밀어내기를 통해 겹침 방지
        ballSpeedX = fabs(ballSpeedX); // 무조건 오른쪽(+)으로 보냄
        ballSpeedX += 2.0f; // 칠 때마다 조금씩 빨라짐
    }

    // 적과 충돌
	if (IsColliding(ball, enemyPaddle)) // 적 패들과 충돌했을 때
    {
        ball.x = enemyPaddle.x - ball.w; // 겹침 방지
        ballSpeedX = -fabs(ballSpeedX); // 무조건 왼쪽(-)으로 보냄
    }

    // 점수 획득 및 리셋
	if (ball.x < 0) // 0은 화면 왼쪽 끝
    {
        // 적 득점
        enemyScore++;
        ResetBall(1); // 플레이어 쪽으로 발사
        ballSpeedX = 30.0f; // 속도 초기화
        Sleep(500); // 득점 후 잠깐 대기
    }
	else if (ball.x > 80) // 80은 화면 너비
    {
        // 플레이어 득점
        playerScore++;
        ResetBall(-1); // 적 쪽으로 발사
        ballSpeedX = -30.0f; // 속도 초기화
        Sleep(500); // 득점 후 잠깐 대기   
    }
}

// 게임 종료 조건 체크 및 처리 함수
void LateUpdate()
{
    // 게임 종료 조건 (5점 내기)
	if (playerScore >= 5 || enemyScore >= 5) /// 플레이어 또는 적이 5점 이상이면 게임 종료
    {
        MessageBox(NULL, playerScore >= 5 ? L"승리했습니다!" : L"패배했습니다...", L"게임 종료", MB_OK);
        currentState = STATE_MENU; // 타이틀로 복귀
        playerScore = 0; // 점수 초기화
        enemyScore = 0; // 점수 초기화   
    }
}

// 화면 그리기 함수
void PreRender()
{
	ScreenClear(); // 화면 초기화
	InitScreen(80, 25, WHITE); // 화면 크기 80x25, 기본 색상 흰색으로 설정
}

// 화면 그리기 함수
void Render()
{   
    SetScreenColor(WHITE); // 기본 색상 흰색으로 설정

	if (currentState == STATE_MENU) // 메뉴 화면일 때
    {
        // 타이틀 화면 그리기
        ScreenPrint(35, 6, L"== PONG ==");  //WHITE
        ScreenPrint(28, 7, L"Up/Down 키로 이동하세요"); 
		Draw(btnStart); // 게임 시작 버튼 그리기
		Draw(btnExit); // 종료 버튼 그리기
    }
	else  // 게임 플레이 화면일 때
    {
        // 게임 화면 그리기

        // 게임판 중앙선 그리기
        for (int i = 0; i < 25; i += 2) ScreenPrint(40, i, L"|");

        // 점수판 그리기
        wchar_t scoreBuffer[30];
		swprintf_s(scoreBuffer, L"%d  :  %d", playerScore, enemyScore); // 플레이어 점수 : 적 점수 형식으로 출력
		ScreenPrint(36, 2, scoreBuffer); // 점수판 위치는 화면 상단 중앙 근처

        // 오브젝트 그리기
        DrawRect((int)playerPaddle.x, (int)playerPaddle.y, playerPaddle.w, playerPaddle.h, 0x000B); // 플레이어(하늘색)
        DrawRect((int)enemyPaddle.x, (int)enemyPaddle.y, enemyPaddle.w, enemyPaddle.h, 0x000C);    // 적(빨간색)
        DrawRect((int)ball.x, (int)ball.y, ball.w, ball.h, 0x000E);          // 공(노란색)
    }
}

// 화면 버퍼에 그린 내용을 실제 콘솔 창에 출력하는 함수
void PostRender()
{
#ifdef _DEBUG
    // 기즈모는 어디서든 보이면 좋음
    FPS_Gizmo(25, 22, &g_Time, &g_Input);
#endif	
	ScreenFlipping(); // 화면 버퍼에 그린 내용을 실제 콘솔 창에 출력
}

// 게임 종료 시 자원 해제 함수
void Release()
{
	Release(btnStart); // 게임 시작 버튼 자원 해제
	Release(btnExit); // 종료 버튼 자원 해제
	ScreenRelease(); // 화면 버퍼 자원 해제
}

// 프로그램 진입점
int main(void)
{
	Awake(); // 게임 초기화
	Start(); // 게임 시작 시 초기화

	// 게임 루프
	while (bIsGameOver) // 게임 오버 상태가 아닐 때 계속 실행
    {
		TimeUpdate();  // 시간 업데이트
		InputUpdate(); // 입력 업데이트

		Update();      // 게임 로직 업데이트
		LateUpdate();  // 게임 종료 조건 체크 및 처리

		PreRender();   /// 화면 그리기 준비
		Render();      // 화면 그리기    
		PostRender();  // 화면 버퍼에 그린 내용을 실제 콘솔 창에 출력

		WaitForNextFrame(); // 다음 프레임까지 대기 (프레임 고정)
    }
	Release(); // 게임 종료 시 자원 해제
    return 0; // 프로그램 종료
}
