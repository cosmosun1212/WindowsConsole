#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

typedef struct _TimeManager
{
    clock_t prevTime;
    float deltaTime;

    // FPS 계산용
    float frameTime;
    int frameCount;
    int fps;

    // 프레임 고정용 설정
    int targetFPS;
    int targetFrameTime;

    // FixedUpdate용 변수
    float accumulatedTime; // 누적 시간
    float fixedDeltaTime;  // 고정 간격

} TIMEMANAGER;

void TimeInit();
void TimeUpdate();
void WaitForNextFrame();
bool IsFixedUpdateTime();
