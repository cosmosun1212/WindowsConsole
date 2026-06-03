
// TextObject.cpp: 텍스트 객체의 생성, 설정, 그리기, 해제 기능을 구현하는 파일
#include "TextObject.h"
#include <stdlib.h> // malloc, free 사용

namespace CmdUI
{
	// 텍스트 생성 함수 // 텍스트의 위치, 문자열, 색상, 타입 등을 설정하여 새로운 텍스트 객체를 생성하는 함수
    TEXT* CreateText(int x, int y, const wchar_t* label, unsigned short color, TEXT_TYPE type)
    {
		TEXT* txt = (TEXT*)malloc(sizeof(TEXT)); // TEXT 객체를 가리키는 포인터 선언 및 동적 메모리 할당
		if (txt == NULL) return NULL; // 메모리 할당 실패 시 NULL 반환

		txt->x = x; // 텍스트의 x 위치 설정
		txt->y = y; // 텍스트의 y 위치 설정
		txt->color = color; // 텍스트의 색상 설정
		txt->type = type; // 텍스트의 타입 설정 (TEXT_STATIC 또는 TEXT_DYNAMIC)

		if (type == TEXT_STATIC) // TEXT_STATIC인 경우 문자열 크기만큼 메모리를 할당
        {
			int len = (int)wcslen(label) + 1; // 문자열 길이 계산 (NULL 문자 포함)
			txt->staticStr = (wchar_t*)malloc(sizeof(wchar_t) * len); // 문자열 크기만큼 메모리 할당

			if (txt->staticStr != NULL) // 메모리 할당 성공 시 문자열 복사
            {
				wcscpy_s(txt->staticStr, len, label); // 문자열 복사 함수 사용, 텍스트의 staticStr에 label 복사
            }
        }
		else if (type == TEXT_DYNAMIC) // TEXT_DYNAMIC인 경우 고정된 128자 버퍼를 사용
        {
            wcscpy_s(txt->dynamicStr, 128, label);
        }

        return txt; 
    }

    void SetText(TEXT* txt, const wchar_t* newLabel)
    {
        if (txt == NULL || newLabel == NULL) return;
        if (txt->type == TEXT_STATIC) 
        {
            wcscpy_s(txt->staticStr, sizeof(txt->staticStr), newLabel);
            return;
        }

        wcscpy_s(txt->dynamicStr, 128, newLabel);
    }

    void Draw(TEXT* txt)
    {
        if (txt == NULL) return;

        SetScreenColor(txt->color);

        if (txt->type == TEXT_STATIC) 
        {
            if (txt->staticStr) ScreenPrint(txt->x, txt->y, txt->staticStr);
        }
        else 
        {
            ScreenPrint(txt->x, txt->y, txt->dynamicStr);
        }
    }

    void Release(TEXT* txt)
    {
        if (txt == NULL) return;

        if (txt->type == TEXT_STATIC && txt->staticStr != NULL)
        {
            free(txt->staticStr);
        }

        free(txt);
    }
}
