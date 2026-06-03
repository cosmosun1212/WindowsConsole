// 화면 버퍼 관리 기능 구현 파일

#include "ScreenBuffer.h"
#include <windows.h> // Windows API 헤더 (콘솔 버퍼 및 커서 관리 등)
#include <stdio.h> // sprintf_s 사용을 위해 추가

static int g_nScreenIndex; // 현재 활성화된 화면 버퍼 인덱스 (0 또는 1)
static HANDLE g_hScreen[2]; // 화면 버퍼 핸들 배열
static int g_wColor = GRAY; // 기본 색상: 회색

static int g_nScreenWidth = DEFAULT_SCREEN_WIDTH; // 화면 너비
static int g_nScreenHeight = DEFAULT_SCREEN_HEIGHT; // 화면 높이    

// 화면 초기화 함수
void ScreenInit(int width, int height) // 화면 크기를 매개변수로 받아 초기화
{
    // 입력받은 크기를 전역 변수에 저장
	g_nScreenWidth = width;     // 화면 너비 설정
	g_nScreenHeight = height;   // 화면 높이 설정

	// 콘솔 창 크기 조정 명령어 생성 및 실행
	char cmd[64]; // 명령어 버퍼
	sprintf_s(cmd, "mode con: cols=%d lines=%d", g_nScreenWidth, g_nScreenHeight);// 콘솔 모드 설정 명령어 생성 (예: "mode con: cols=80 lines=25")
	system(cmd); // 시스템 명령어 실행 (콘솔 창 크기 조정)

	// 커서 정보 구조체 및 화면 크기/윈도우 크기 설정
    CONSOLE_CURSOR_INFO cci;
	COORD size = { (SHORT)g_nScreenWidth, (SHORT)g_nScreenHeight };// 화면 버퍼 크기 설정 (너비, 높이)
	SMALL_RECT rect = { 0, 0, (SHORT)(g_nScreenWidth - 1), (SHORT)(g_nScreenHeight - 1) };// 콘솔 창 크기 설정 (왼쪽, 위, 오른쪽, 아래)

	//더블 버퍼링을 위해 두 개의 콘솔 화면 버퍼를 생성
	g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); // 첫 번째 화면 버퍼 생성 // 메인 버퍼 화면 생성
    g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); // 두 번째 화면 버퍼 생성 // 백 버퍼 화면 생성

	// 각 화면 버퍼에 대해 버퍼 크기와 창 크기를 설정하고 커서를 숨김 //
	for (int i = 0; i < 2; i++) // 두 개의 화면 버퍼에 대해 반복
    {
        SetConsoleScreenBufferSize(g_hScreen[i], size); // 화면에 맞추어 버퍼 크기 고정
        SetConsoleWindowInfo(g_hScreen[i], TRUE, &rect); // 창 크기 고정

		// 커서 정보 설정: dwSize : 1, bVisible : FALSE (커서 숨김)
        cci.dwSize = 1;
        cci.bVisible = FALSE;
		SetConsoleCursorInfo(g_hScreen[i], &cci); // 커서 정보 적용 
    }
}

// 화면 버퍼를 전환하는 함수 (더블 버퍼링 구현) // 현재 활성화된 화면 버퍼로 전환하고 인덱스 토글
void ScreenFlipping() 
{
	SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);// 현재 활성화된 화면 버퍼로 전환
	g_nScreenIndex = !g_nScreenIndex; // 화면 버퍼 인덱스 토글 (0에서 1로, 1에서 0으로)
}

// 화면을 공백으로 채우고 색상을 초기화하는 함수
void ScreenClear()
{
	COORD Coor = { 0, 0 }; // 화면의 시작 좌표 (0, 0) 설정
	DWORD dw; // 출력된 문자 수를 저장할 변수

	int totalSize = g_nScreenWidth * g_nScreenHeight; // 화면 전체 크기 계산 (너비 * 높이)

	// 현재 활성화된 화면 버퍼에 공백 문자로 화면을 채우고 색상을 초기화
	FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', totalSize, Coor, &dw); // 화면 전체를 공백 문자로 채움
    FillConsoleOutputAttribute(g_hScreen[g_nScreenIndex], GRAY, totalSize, Coor, &dw); // 화면 전체 색상을 초기화
}

// 화면 버퍼 핸들을 닫는 함수 (자원 해제) 
void ScreenRelease()
{
	CloseHandle(g_hScreen[0]); // 첫 번째 화면 버퍼 핸들 닫기 // 메인 버퍼 화면 핸들 닫기
	CloseHandle(g_hScreen[1]); // 두 번째 화면 버퍼 핸들 닫기 // 백 버퍼 화면 핸들 닫기
}

// 화면에 문자열을 출력하는 함수 (x, y 좌표와 문자열을 매개변수로 받아 출력)
void ScreenPrint(int x, int y, const wchar_t* string) 
{
	DWORD dw; // 출력된 문자 수를 저장할 변수
	COORD CursorPosition = { (SHORT)x, (SHORT)y }; // 출력할 위치 설정 (x, y 좌표)
	DWORD len = (DWORD)wcslen(string); // 출력할 문자열의 길이 계산 (문자열 길이)
	DWORD vlen = 0; // 가변 길이 계산을 위한 변수 (유니코드 문자 처리 시 2칸 차지하는 경우 고려)

	// 문자열의 각 문자에 대해 가변 길이 계산 (유니코드 문자는 2칸 차지)
	for (size_t i = 0; i < len; i++) 
    {
		// 유니코드 문자는 2칸 차지하므로 가변 길이 계산 // ASCII 문자는 1칸, 유니코드 문자는 2칸으로 계산
		if (string[i] > 0x007F) // 유니코드 문자인 경우 // ASCII 문자는 0x007F 이하, 유니코드 문자는 그 이상
        {
			vlen += 2; // 유니코드 문자는 2칸 차지
        }
		else // ASCII 문자인 경우
        {
            vlen += 1; // ASCII 문자는 1칸 차지
        }
    }
	SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition); // 커서 위치 설정
	WriteConsoleOutputCharacterW(g_hScreen[g_nScreenIndex], string, len, CursorPosition, &dw);// 문자열 출력
	FillConsoleOutputAttribute(g_hScreen[g_nScreenIndex], g_wColor, len, CursorPosition, &dw); // 출력된 문자열의 색상 설정 (현재 색상으로 설정) // 문자열 길이 대신 가변 길이로 색상 설정 (유니코드 문자가 1칸 차지하는 경우 고려)
	FillConsoleOutputAttribute(g_hScreen[g_nScreenIndex], g_wColor, vlen, CursorPosition, &dw); // 문자열 길이 대신 가변 길이로 색상 설정 (유니코드 문자가 2칸 차지하는 경우 고려)
}

// 화면 색상을 설정하는 함수 (색상 값을 매개변수로 받아 설정)
void SetScreenColor(unsigned short color)
{
	g_wColor = color; // 현재 색상 변수에 설정된 색상 값 저장
}

// 화면 너비를 반환하는 함수	
int GetScreenWidth()
{
	return g_nScreenWidth; // 화면 너비 반환
}

// 화면 높이를 반환하는 함수
int GetScreenHeight()
{
	return g_nScreenHeight; // 화면 높이 반환
}
