
// CmdUtil.cpp : Windows Console Mode에서 사용할 유틸리티 함수들을 구현하는 파일
#include "CmdUtil.h"
#include "ScreenBuffer.h" //

void DrawRect(int x, int y, int w, int h, unsigned short color)
{
	SetScreenColor(color); // 색상 설정

	// 테두리 그리기에 사용할 문자 정의 (유니코드 박스 드로잉 문자)
	const wchar_t* TOP_LEFT = L"┌"; // 왼쪽 위 모서리       //유니코드 U+250C
    const wchar_t* TOP_RIGHT = L"┐"; // 오른쪽 위 모서리    //유니코드 U+2510
    const wchar_t* BT_LEFT = L"└"; // 왼쪽 아래 모서리      //유니코드 U+2514
    const wchar_t* BT_RIGHT = L"┘"; // 오른쪽 아래 모서리   //유니코드 U+2518
    const wchar_t* HORIZ = L"─"; // 수평선                 //유니코드 U+2500
    const wchar_t* VERT = L"│"; // 수직선                  //유니코드 U+2502

	// 테두리 그리기
	ScreenPrint(x, y, TOP_LEFT); // 왼쪽 위 모서리
    
    // 윗줄 그리기 //수평선 그리기 (왼쪽과 오른쪽 모서리를 제외한 부분)
	for (int i = 1; i < w - 1; i++) 
    {
		ScreenPrint(x + i, y, HORIZ); 
    }
	ScreenPrint(x + w - 1, y, TOP_RIGHT); // 오른쪽 위 모서리

	// 양쪽 벽 그리기 //수직선 그리기 (윗줄과 아랫줄을 제외한 부분)
    for (int j = 1; j < h - 1; j++) 
    {
		ScreenPrint(x, y + j, VERT);         // 왼쪽 벽
        ScreenPrint(x + w - 1, y + j, VERT); // 오른쪽 벽
    }

	// 아랫줄 그리기 //수평선 그리기 (왼쪽과 오른쪽 모서리를 제외한 부분)
    ScreenPrint(x, y + h - 1, BT_LEFT); 
	// 수평선 그리기 (왼쪽과 오른쪽 모서리를 제외한 부분)
    for (int i = 1; i < w - 1; i++) 
    {
        ScreenPrint(x + i, y + h - 1, HORIZ);
    }
	ScreenPrint(x + w - 1, y + h - 1, BT_RIGHT); // 오른쪽 아래 모서리

	// 색상 사용 후 다시 기본(흰색)으로 돌려놓는 매너
    SetScreenColor(0x0007);
}

// 화면 전체를 단일 색상으로 채우는 함수
void InitScreen(int cols, int rows, int color)
{
	DrawRect(0, 0, cols, rows, color); // 전체 화면을 color 색상으로 채우는 사각형 그리기
}

// FPS와 마우스 좌표를 화면에 표시하는 디버그용 함수 
// x, y: 출력할 위치의 좌표, pTime: 시간 관리 구조체 포인터, pInput: 입력 관리 구조체 포인터
void FPS_Gizmo(int x, int y, TIMEMANAGER* pTime, INPUTVALUE* pInput) 
{
	// 안전성 검사: 포인터가 NULL인 경우 함수 종료
    if (pTime == NULL || pInput == NULL) return;

	wchar_t debugMsg[128]; // 디버그 메시지를 저장할 버퍼 (128자까지)

	// 디버그 메시지 작성: FPS와 마우스 좌표를 문자열로 작성
    swprintf_s(debugMsg, L" FPS: %02d | Mouse: [%02d, %02d] ",
        pTime->fps,
        pInput->mouseX,
        pInput->mouseY
    );

	int msgLen = (int)wcslen(debugMsg); // 메시지 길이 계산 (문자 수)

	DrawRect(x, y, msgLen + 2, 3, GREEN); // 메시지 길이에 맞춰 사각형 그리기 (여유 공간 포함)

	SetScreenColor(BK_DBLUE | WHITE); // 메시지 텍스트 색상 설정 (파란색 배경에 흰색 글씨)

	ScreenPrint(x + 1, y + 1, debugMsg); // 메시지 출력 (사각형 안에 텍스트 출력)

	SetScreenColor(WHITE); // 메시지 출력 후 색상 기본값으로 복원 (흰색)
}
