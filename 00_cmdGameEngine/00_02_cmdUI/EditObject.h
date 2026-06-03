#pragma once
#pragma once
#include <string.h>
#include <wchar.h>
#include <windows.h>
#include "InputManager.h" 

namespace CmdUI // CmdUI 네임스페이스로 Edit 관련 함수들을 묶어서 관리
{
	typedef struct ___Edit // Edit의 위치, 크기, 색상, 텍스트, 상태 등을 저장하는 구조체
    {
        int x, y;                    // 위치
        int width;                   // 입력창의 가로 길이
        int maxLen;                  // 입력 가능한 최대 글자 수
        unsigned short colorDefault; // 평상시 색상
        unsigned short colorHover;   // 마우스를 올렸을 때 색상
        unsigned short colorFocused; // 클릭해서 입력 중일 때(포커스) 색상

        wchar_t label[128];          // 상단에 표시될 제목
        wchar_t text[256];           // 실제로 입력된 텍스트 데이터
        int textLength;              // 현재 입력된 글자 수

        bool isHovered;              // 마우스 오버 여부
        bool isFocused;              // 현재 입력 활성화 여부
        bool isEncrypted;            // 비밀번호 모드 (*** 로 표시)

        DWORD lastBlinkTick;         // 커서 깜빡임 제어용 타이머
        bool showCursor;             // 현재 커서(_)가 보이는 상태인지
	} EDIT; // Edit 구조체 정의

	EDIT* CreateEdit(int x, int y, int w, int maxLen, const wchar_t* label, bool encrypted, unsigned short colDef, unsigned short colHov, unsigned short colFoc); // Edit 생성 함수 // CreateEdit()는 Edit의 위치(x, y), 너비(w), 최대 글자 수(maxLen), 라벨 텍스트(label), 암호화 여부(encrypted), 기본 색상(colDef), 호버 색상(colHov), 포커스 색상(colFoc)을 매개변수로 받아 새로운 EDIT 객체를 동적 메모리로 할당하여 초기화한 후 반환
	bool Update(EDIT* edit, const INPUTVALUE* input); // Edit 업데이트 함수 // Update()는 Edit 객체와 입력 값을 매개변수로 받아 마우스 좌표가 Edit 영역 안에 있는지 확인하여 호버 상태를 업데이트하고, 마우스 왼쪽 버튼 클릭 시 포커스 상태를 업데이트. Edit가 포커스 상태일 때 키보드 입력을 처리하여 텍스트를 업데이트하며, Enter 또는 Escape 키 입력 시 포커스 해제 및 true 반환, 그렇지 않으면 false 반환
	void Draw(EDIT* edit); // Edit 그리기 함수 // Draw()는 Edit 객체를 매개변수로 받아 현재 상태에 따라 Edit의 색상을 결정하고, 라벨과 입력창을 화면에 그리고  입력된 텍스트는 암호화 모드일 때 ***로 표시하며, 포커스 상태일 때 커서 깜빡임을 제어하여 표시
    void Release(EDIT* edit); // Edit 해제 함수 // Release()는 Edit 객체를 매개변수로 받아 동적 메모리를 해제하여 Edit 객체를 해제
}
