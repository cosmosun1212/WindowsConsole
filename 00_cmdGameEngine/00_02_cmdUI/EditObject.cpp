
// EditObject.cpp : EDIT 객체의 생성, 업데이트, 렌더링, 해제 함수 구현
#include "EditObject.h" 
#include "ScreenBuffer.h"
#include <stdlib.h>

namespace CmdUI // CmdUI 네임스페이스로 Edit 관련 함수들을 묶어서 관리
{
	// Edit 생성 함수 // Edit 객체의 위치, 크기, 텍스트, 색상 등을 설정하여 새로운 Edit 객체를 생성하는 함수
    EDIT* CreateEdit(int x, int y, int w, int maxLen, const wchar_t* label, bool encrypted, unsigned short colDef, unsigned short colHov, unsigned short colFoc)
    {
		EDIT* edit = (EDIT*)malloc(sizeof(EDIT)); // EDIT 객체를 가리키는 포인터 선언 및 동적 메모리 할당
		edit->x = x; // Edit의 x 위치 설정
		edit->y = y; // Edit의 y 위치 설정
		edit->width = w; // Edit의 너비 설정
		edit->maxLen = maxLen; // Edit에 입력 가능한 최대 글자 수 설정
		edit->colorDefault = colDef; // Edit의 기본 색상 설정
		edit->colorHover = colHov; // Edit의 호버 색상 설정
		edit->colorFocused = colFoc; // Edit의 포커스 색상 설정

		wcscpy_s(edit->label, label); // Edit의 라벨 텍스트 복사
		edit->text[0] = L'\0'; // Edit의 텍스트 초기화 (빈 문자열)
		edit->textLength = 0; // Edit에 현재 입력된 글자 수 초기화

		edit->isHovered = false; // Edit의 호버 상태 초기화
		edit->isFocused = false; // Edit의 포커스 상태 초기화
		edit->isEncrypted = encrypted; // Edit의 암호화 모드 설정 (입력된 텍스트를 ***로 표시할지 여부)

		edit->lastBlinkTick = GetTickCount(); // Edit의 커서 깜빡임 제어용 타이머 초기화
		edit->showCursor = false; // Edit의 커서 표시 상태 초기화 (처음에는 커서가 보이지 않도록 설정)

		return edit; // 초기화된 Edit 객체의 포인터 반환
    }

	// Edit 업데이트 함수 
    bool Update(EDIT* edit, const INPUTVALUE* input)
    {
        // 안전성 검사: Edit 또는 입력 값이 NULL인 경우 함수 종료
		if (edit == NULL || input == NULL) return false; 

		// 마우스 좌표가 Edit 영역 안에 있는지 확인하여 상태 업데이트
        edit->isHovered = (input->mouseX >= edit->x && input->mouseX < edit->x + edit->width &&
            input->mouseY == edit->y);

		// 클릭 판정: Edit이 호버 상태이고 마우스 왼쪽 버튼이 클릭되었는지 확인하여 포커스 상태 업데이트
        if (input->isMouseLeftClick) 
        {
			if (edit->isHovered) edit->isFocused = true; // 호버 상태이고 마우스 왼쪽 버튼이 클릭되었을 때 포커스 설정
            else                 edit->isFocused = false; // 호버 상태가 아니면 포커스 해제
        }

		// 키보드 입력 처리: Edit이 포커스 상태일 때만 입력 처리
        if (edit->isFocused)
        {
			// Enter 또는 Escape 키 입력 시 포커스 해제
            if ((GetAsyncKeyState(VK_RETURN) & 0x0001) || (GetAsyncKeyState(VK_ESCAPE) & 0x0001))
            {
				edit->isFocused = false; // 포커스 해제
				return true; // 입력이 완료되었음을 알리기 위해 true 반환
            }

			// Backspace 키 입력 시 마지막 글자 삭제
            if (GetAsyncKeyState(VK_BACK) & 0x0001)
            {
				if (edit->textLength > 0) // 현재 입력된 글자 수가 0보다 클 때 삭제 처리
                {
					edit->textLength--; // 마지막 글자 삭제
					edit->text[edit->textLength] = L'\0'; // 문자열 끝 표시 (null terminator)
                }
            }

            // Ctrl + V (클립보드 붙여넣기)
			if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState('V') & 0x0001)) // Ctrl 키가 눌린 상태에서 V 키가 클릭되었는지 확인하여 클립보드에서 텍스트를 붙여넣기 처리
            {
				// 클립보드에서 텍스트 붙여넣기 처리
				if (OpenClipboard(NULL)) // 클립보드 열기 시도
                {
					HANDLE hData = GetClipboardData(CF_UNICODETEXT); // 클립보드에서 유니코드 텍스트 데이터 가져오기
					if (hData) // 클립보드에서 텍스트 데이터를 성공적으로 가져왔는지 확인 
                    {
						wchar_t* clipText = (wchar_t*)GlobalLock(hData); // 클립보드 데이터에 대한 포인터 얻기 (GlobalLock을 사용하여 메모리 잠금)
						if (clipText) // 클립보드 데이터에 대한 포인터가 유효한지 확인
                        {							
                            while (*clipText && edit->textLength < edit->maxLen) // 클립보드에서 가져온 텍스트를 Edit의 텍스트에 추가 (최대 글자 수를 초과하지 않도록 처리)
                            {
								edit->text[edit->textLength++] = *clipText; // 클립보드에서 가져온 텍스트의 현재 글자를 Edit의 텍스트에 추가하고 글자 수 증가
								clipText++; // 클립보드 텍스트의 다음 글자로 이동
                            }
							edit->text[edit->textLength] = L'\0'; // 문자열 끝 표시 (null terminator)
							GlobalUnlock(hData); /// 클립보드 데이터에 대한 잠금 해제 (GlobalUnlock을 사용하여 메모리 잠금 해제)
                        }
                    }
					CloseClipboard(); // 클립보드 닫기
					return true; // 입력이 완료되었음을 알리기 위해 true 반환
                }
            }

			// 일반적인 키 입력 처리 (영문자, 숫자, 공백 등)
			if (edit->textLength < edit->maxLen) // 현재 입력된 글자 수가 최대 글자 수보다 작은 경우에만 입력 처리
            {
				if (GetAsyncKeyState(VK_SPACE) & 0x0001) // Space 키가 클릭되었는지 확인하여 Edit의 텍스트에 공백 추가
                {
					edit->text[edit->textLength++] = L' '; // Space 키 입력 처리
					edit->text[edit->textLength] = L'\0'; // 문자열 끝 표시 (null terminator)
                }
				// 숫자 키 입력 처리 (0~9)
                for (int i = '0'; i <= '9'; i++)
                {
                    if (GetAsyncKeyState(i) & 0x0001) // 숫자 키가 클릭되었는지 확인
                    {
                        edit->text[edit->textLength++] = (wchar_t)i; // 숫자 키 입력 처리
                        edit->text[edit->textLength] = L'\0'; // 문자열 끝 표시 (null terminator)
                    }
                }
				// 영문자 키 입력 처리 (A~Z, Shift 키 상태에 따라 대문자 또는 소문자로 처리)
                for (int i = 'A'; i <= 'Z'; i++)
                {
					if (GetAsyncKeyState(i) & 0x0001) // 영문자 키가 클릭되었는지 확인
                    {
						bool isShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; // Shift 키가 눌린 상태인지 확인하여 대문자 또는 소문자로 처리
						wchar_t ch = isShift ? (wchar_t)i : (wchar_t)(i + 32); // Shift 키가 눌린 상태에서는 대문자, 그렇지 않으면 소문자로 처리 (ASCII 코드에서 대문자와 소문자의 차이는 32)
						edit->text[edit->textLength++] = ch; // 영문자 키 입력 처리
						edit->text[edit->textLength] = L'\0'; // 문자열 끝 표시 (null terminator)
                    }
                }
            }
        }
		return false; // 입력이 완료되지 않았으므로 false 반환
    }

	// Edit 그리기 함수
    void Draw(EDIT* edit)
    {
		if (edit == NULL) return; // 안전성 검사: Edit이 NULL인 경우 함수 종료

		// 현재 상태에 따라 Edit 색상 결정 (포커스 > 호버 > 기본 색상)
		unsigned short currentColor = edit->colorDefault; // 기본 색상으로 초기화
		if (edit->isFocused) currentColor = edit->colorFocused; // Edit이 포커스 상태일 때 포커스 색상 설정
		else if (edit->isHovered) currentColor = edit->colorHover; // Edit이 호버 상태일 때 호버 색상 설정

		SetScreenColor(edit->colorDefault); // Edit의 기본 색상으로 설정하여 라벨 텍스트 그리기
		ScreenPrint(edit->x, edit->y - 1, edit->label); // Edit의 라벨 텍스트 출력 (Edit의 바로 위에 위치하도록 y 좌표에서 1을 빼서 출력)

		SetScreenColor(currentColor); // Edit의 현재 색상으로 설정하여 입력창 배경 그리기 (공백 문자로 Edit의 너비만큼 채워서 그리기)
		for (int i = 0; i < edit->width; i++) // Edit의 너비만큼 공백 문자로 입력창 배경 그리기
        {
			ScreenPrint(edit->x + i, edit->y, L" "); // Edit의 x 좌표에서 i만큼 이동하여 공백 문자 출력 (입력창 배경을 채우기 위해 반복)
        }

		wchar_t drawText[256] = L""; // Edit의 텍스트를 화면에 그릴 때 사용할 버퍼 (암호화 모드일 때는 ***로 표시하기 위해 별도의 버퍼 사용)

        // Edit이 암호화 모드일 때 입력된 텍스트를 ***로 표시하기 위해 drawText 버퍼에 별표로 채우기
		if (edit->isEncrypted) 
        {
			for (int i = 0; i < edit->textLength; i++) drawText[i] = L'*'; // Edit의 textLength만큼 drawText 버퍼에 별표로 채우기 (입력된 글자 수만큼 별표로 표시)
            drawText[edit->textLength] = L'\0'; // 문자열 끝 표시 (null terminator)
        }
		else // Edit이 암호화 모드가 아닐 때는 입력된 텍스트를 그대로 drawText 버퍼에 복사
        {
			wcscpy_s(drawText, edit->text); // Edit의 텍스트를 drawText 버퍼에 복사
        }

		// Edit이 포커스 상태일 때 커서 깜빡임 제어 (500ms마다 커서 표시 상태 토글)
        if (edit->isFocused)
        {
			if (GetTickCount() - edit->lastBlinkTick > 500) /// GetTickCount()를 사용 커서 깜빡임 제어
            {
				edit->showCursor = !edit->showCursor; // 500ms마다 커서 표시 상태 토글 
				edit->lastBlinkTick = GetTickCount(); // GetTickCount()를 사용 깜빡임 시간 업데이트
            }
			if (edit->showCursor && edit->textLength < edit->maxLen) // 커서가 보이는 상태이고 현재 입력된 글자 수가 최대 글자 수보다 작은 경우에만 커서 표시
            {
				wcscat_s(drawText, L"_"); // Edit의 텍스트 뒤에 커서 표시
            }
        }
		else //// Edit이 포커스 상태가 아닐 때는 커서 표시 해제
        {
			edit->showCursor = false; // Edit이 포커스 상태가 아닐 때는 커서 표시 해제
        }

		int drawLen = (int)wcslen(drawText); /// Edit의 텍스트를 화면에 그릴 때 사용할 버퍼의 길이 계산
		int startIdx = 0; // Edit의 텍스트를 화면에 그릴 때 사용할 버퍼의 시작 인덱스 초기화
		if (drawLen > edit->width) // Edit의 텍스트가 입력창의 너비보다 길 때는 텍스트의 뒷부분이 보이도록 시작 인덱스 계산
        {
			startIdx = drawLen - edit->width; // Edit의 텍스트가 입력창의 너비보다 길 때는 텍스트의 뒷부분이 보이도록 시작 인덱스 계산 (텍스트의 뒷부분이 입력창에 보이도록 하기 위해 시작 인덱스를 텍스트 길이에서 입력창 너비를 뺀 값으로 설정)
        }

		SetScreenColor(currentColor); // Edit의 현재 색상으로 설정하여 텍스트 그리기
		ScreenPrint(edit->x, edit->y, &drawText[startIdx]); // Edit의 x 좌표에서 startIdx부터 drawText 버퍼의 텍스트를 출력 (입력창에 보이는 부분만 출력)
    }

	// Edit 해제 함수
    void Release(EDIT* edit)
    {
		if (edit == NULL) return; // 안전성 검사: Edit이 NULL인 경우 함수 종료
		free(edit); // 동적 메모리 해제
    }
}
