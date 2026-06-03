#include "InputManager.h" // 입력 관리 헤더 (키보드 및 마우스 입력 처리)
#include <Windows.h> // Windows API 헤더 (콘솔 입력 처리, GetAsyncKeyState 함수 등)

// 키보드 가상 키 코드 매핑 (자주 사용하는 키들을 열거형으로 관리)
int g_KeyMapping[VK::MaxCount] = { VK_ESCAPE, VK_SPACE, VK_RETURN, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN };

INPUTVALUE g_Input; // 입력 상태를 담는 전역 변수 (키보드 및 마우스 상태)
HANDLE g_hInput; // 콘솔 입력 핸들 (GetStdHandle로 초기화)

// 입력 시스템 초기화 함수
void InputInit()
{
    // 입력 핸들 얻기
    g_hInput = GetStdHandle(STD_INPUT_HANDLE);

    // 마우스 입력을 받도록 콘솔 모드 설정
    DWORD dwMode;
    GetConsoleMode(g_hInput, &dwMode);
    // ENABLE_QUICK_EDIT_MODE를 끄고(~), 마우스 입력을 켠다(|).
	dwMode |= ENABLE_MOUSE_INPUT; // 마우스 입력 활성화
	dwMode &= ~ENABLE_QUICK_EDIT_MODE; // 퀵 에디트 모드 비활성화 
	dwMode |= ENABLE_EXTENDED_FLAGS; // 확장 플래그 활성화 (필수)

	SetConsoleMode(g_hInput, dwMode); // 변경된 모드 설정

    // 변수 초기화
    g_Input.mouseX = 0;
    g_Input.mouseY = 0;
	g_Input.isMouseLeftDown = false; // 왼쪽 버튼이 눌려있는지 여부 초기화
	g_Input.isMouseLeftClick = false; // 이번 프레임에 클릭(Down) 했는지 여부 초기화

	// 키보드 상태 초기화 (모든 키를 KEY_FREE 상태로 설정)
    for (int i = 0; i < VK::MaxCount; i++) g_Input.keyState[i] = KEY_FREE;
}

void InputUpdate()
{
    // 이벤트 방식으로 마우스 처리
    DWORD dwEvents;
    GetNumberOfConsoleInputEvents(g_hInput, &dwEvents); // 대기 이벤트 개수 확인

    if (dwEvents > 0)
    {
        INPUT_RECORD* rec = new INPUT_RECORD[dwEvents];
        DWORD dwRead;
        ReadConsoleInput(g_hInput, rec, dwEvents, &dwRead);

        for (DWORD i = 0; i < dwRead; i++)
        {
            if (rec[i].EventType == MOUSE_EVENT)
            {
                // 마우스 위치 업데이트
                g_Input.mouseX = rec[i].Event.MouseEvent.dwMousePosition.X;
                g_Input.mouseY = rec[i].Event.MouseEvent.dwMousePosition.Y;

                // 마우스 버튼 상태
                if (rec[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
                {
                    if (!g_Input.isMouseLeftDown) g_Input.isMouseLeftClick = true; // 방금 눌림
                    else g_Input.isMouseLeftClick = false; // 계속 누르는 중

                    g_Input.isMouseLeftDown = true;
                }
                else
                {
                    g_Input.isMouseLeftDown = false;
                    g_Input.isMouseLeftClick = false;
                }
            }
        }
        delete[] rec;
    }
    else 
    {
        g_Input.isMouseLeftClick = false; // 이벤트가 없으면 클릭 플래그 해제
    }

    // 키보드 처리 (GetAsyncKeyState 상태 체크)
    for (int i = 0; i < VK::MaxCount; i++)
    {
        if (GetAsyncKeyState(g_KeyMapping[i]) & 0x8000) // 현재 눌려 있다면
        {
            if (g_Input.keyState[i] == KEY_FREE || g_Input.keyState[i] == KEY_UP)
                g_Input.keyState[i] = KEY_DOWN; // 방금 눌림
            else
                g_Input.keyState[i] = KEY_HOLD; // 누르고 있음
        }
        else // 안 눌려 있다면
        {
            if (g_Input.keyState[i] == KEY_DOWN || g_Input.keyState[i] == KEY_HOLD)
                g_Input.keyState[i] = KEY_UP;   // 방금 뗌
            else
                g_Input.keyState[i] = KEY_FREE; // 계속 안 눌림
        }
    }
}
