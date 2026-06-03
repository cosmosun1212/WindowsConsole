
// TIMEMANAGER 구조체와 함수 선언 포함
#include "TimeManager.h" 
// C 표준 라이브러리 헤더 포함
#include <stdlib.h> // malloc, free 등
#include <stdio.h> // printf 등
#include <time.h> 

TIMEMANAGER g_Time; // 게임 전체에서 사용할 시간 관리 구조체

// 시간 관리 초기화 함수
void TimeInit() 
{
	// 초기값 설정
	g_Time.prevTime = clock(); // 현재 시간으로 초기화
	g_Time.deltaTime = 0.0f; // 프레임 간 시간 차이 초기화
	g_Time.frameTime = 0.0f; // FPS 계산용 시간 초기화
	g_Time.frameCount = 0; // FPS 계산용 프레임 카운트 초기화
	g_Time.fps = 0; // 초기 FPS 값 설정

    // 물리 업데이트 목표 (0.02초 == 50회/초)
	g_Time.accumulatedTime = 0.0f; // 누적 시간 초기화 
	g_Time.fixedDeltaTime = 0.02f; // 고정 간격 설정 (0.02초)

    // 렌더링 목표 (30FPS)
	g_Time.targetFPS = 30; // 목표 FPS 설정
	g_Time.targetFrameTime = 1000 / g_Time.targetFPS;  // 목표 프레임 시간 계산 (밀리초 단위)
}

// 시간 관리 업데이트 함수 // 매 프레임마다 호출되어 시간 계산 및 FPS 업데이트
void TimeUpdate() 
{
	clock_t currentTime = clock(); // 현재 시간 측정

    // 이전 프레임과의 시간 차이(초 단위) 계산	// deltaTime 계산: (현재 시간 - 이전 시간) / CLOCKS_PER_SEC
	g_Time.deltaTime = (float)(currentTime - g_Time.prevTime) / CLOCKS_PER_SEC; // deltaTime 계산: (현재 시간 - 이전 시간) / CLOCKS_PER_SEC
	g_Time.prevTime = currentTime; // 이전 시간 업데이트: 현재 시간을 이전 시간으로 저장

	// FPS 계산 (1초마다 갱신) // frameTime에 deltaTime 누적, frameCount 증가, 1초마다 fps 갱신
	g_Time.frameCount++; // 프레임 카운트 증가 
	g_Time.frameTime += g_Time.deltaTime; // frameTime에 deltaTime 누적 
	if (g_Time.frameTime >= 1.0f) // 1초가 지났는지 확인 // 1초가 지났다면 FPS 계산 및 초기화
    {
		g_Time.fps = g_Time.frameCount; // fps 갱신: 1초 동안의 프레임 수를 fps로 설정
		g_Time.frameCount = 0; // 프레임 카운트 초기화
		g_Time.frameTime = 0.0f; // frameTime 초기화	
    }

    // 물리 연산용 시간 누적   // deltaTime을 accumulatedTime에 누적
    g_Time.accumulatedTime += g_Time.deltaTime;
}

// 고정 시간 간격(0.02초)마다 true 반환하여 FixedUpdate 실행 여부 결정
bool IsFixedUpdateTime()
{
	// 누적 시간이 고정 간격(0.02초) 이상인지 확인
    if (g_Time.accumulatedTime >= g_Time.fixedDeltaTime)
    {
		// 고정 간격이 지났다면 누적 시간에서 고정 간격을 빼고 true 반환
		g_Time.accumulatedTime -= g_Time.fixedDeltaTime; // 누적 시간에서 고정 간격을 빼서 다음 고정 업데이트까지의 시간을 계산
		return true; // 고정 간격이 지났으므로 FixedUpdate 실행 허용
    }
	
    return false; // 고정 간격이 아직 안 지났다면 false 반환
}

/// 현재 프레임이 목표 프레임 시간(33ms)보다 빨리 끝났다면, 다음 프레임까지 대기하는 함수
void WaitForNextFrame()
{
	while (true) // 무한 루프를 돌면서 현재 시간이 목표 프레임 시간보다 오래 걸렸는지 확인
    {
		clock_t now = clock(); // 현재 시간 측정	
		clock_t frameDuration = now - g_Time.prevTime; // 현재 시간과 이전 프레임 시간의 차이 계산 (프레임 지속 시간)

		if (frameDuration >= g_Time.targetFrameTime)  // 목표 프레임 시간(33ms)보다 오래 걸렸다면
        {
			break;  // 충분히 시간이 지났으므로 다음 프레임으로 넘어감
        }
    }
}
