#include "TimeManager.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// 전역 변수 정의
TIMEMANAGER g_Time;

void TimeInit()
{
    g_Time.prevTime = clock();
    g_Time.deltaTime = 0.0f;
    g_Time.frameTime = 0.0f;
    g_Time.frameCount = 0;
    g_Time.fps = 0;

    // 물리 업데이트 목표 (0.02초 = 50회/초)
    g_Time.accumulatedTime = 0.0f;
    g_Time.fixedDeltaTime = 0.02f;

    // 렌더링 목표 (30FPS)
    g_Time.targetFPS = 30;
    g_Time.targetFrameTime = 1000 / g_Time.targetFPS;
}

void TimeUpdate()
{
    clock_t currentTime = clock();

    // 이전 프레임과의 시간 차이(초 단위) 계산
    g_Time.deltaTime = (float)(currentTime - g_Time.prevTime) / CLOCKS_PER_SEC;
    g_Time.prevTime = currentTime;

    // FPS 계산 (1초마다 갱신)
    g_Time.frameCount++;
    g_Time.frameTime += g_Time.deltaTime;
    if (g_Time.frameTime >= 1.0f)
    {
        g_Time.fps = g_Time.frameCount;
        g_Time.frameCount = 0;
        g_Time.frameTime = 0.0f;
    }

    // 물리 연산용 시간 누적   // deltaTime을 accumulatedTime에 누적
    g_Time.accumulatedTime += g_Time.deltaTime;
}

// [물리 연산 타이밍 체크 함수]
// FixedUpdate가 실행되어야 할지 결정
bool IsFixedUpdateTime()
{
    // 누적된 시간이 고정 간격(0.02초)보다 많은가? 확인
    if (g_Time.accumulatedTime >= g_Time.fixedDeltaTime)
    {
        // 많다면, 고정 간격만큼 소비하고 true 반환 FixedUpdate 실행
        g_Time.accumulatedTime -= g_Time.fixedDeltaTime;
        return true;
    }
    // 부족하다면 false 반환
    return false;
}

// [프레임 제한 함수]
// 목표 프레임 시간을 맞추기 위해 강제로 대기(Sleep)합니다.
void WaitForNextFrame()
{
    clock_t now = clock();
    clock_t frameDuration = now - g_Time.prevTime;

    // 시간이 남았을 때 -> 남은 만큼만 쉼
    if (frameDuration < g_Time.targetFrameTime)
    {
        Sleep(g_Time.targetFrameTime - frameDuration);
    }
    // 시간이 부족할 때 -> 최소한의 입력 반응성을 위해 목표 시간만큼 쉼
    else
    {
        Sleep(g_Time.targetFrameTime);
    }
}