#pragma once
#include <string.h>
#include <wchar.h>
#include "ScreenBuffer.h"

namespace CmdUI // CmdUI 네임스페이스로 텍스트 관련 함수들을 묶어서 관리
{
	// 텍스트 타입을 나타내는 열거형 정의
    enum TEXT_TYPE 
    {
        TEXT_STATIC = 0, // Static Text //정적 문자열
        TEXT_DYNAMIC     // Dynamic Text //동적 문자열 
    };

    // 텍스트 구조체 정의

	typedef struct ___Text // 텍스트의 위치, 색상, 타입, 문자열 데이터를 저장하는 구조체
    {
        int x, y;               // 위치
        unsigned short color;   // 색상
        TEXT_TYPE type;         // 정적/동적 타입 저장

        union {
            wchar_t* staticStr;      // STATIC: 딱 글자 크기만 메모리 할당
            wchar_t  dynamicStr[128]; // DYNAMIC: 128자 고정 버퍼 확보
        };
	} TEXT; // 텍스트 구조체 정의

	TEXT* CreateText(int x, int y, const wchar_t* label, unsigned short color, TEXT_TYPE type = TEXT_STATIC); // 텍스트 생성 함수 // CreateText()는 텍스트의 위치(x, y), 문자열(label), 색상(color), 텍스트 타입(type)을 매개변수로 받아 새로운 TEXT 객체를 동적 메모리로 할당하여 초기화한 후 반환, 텍스트 타입은 기본값으로 TEXT_STATIC이 설정, TEXT_STATIC인 경우 문자열 크기만큼 메모리를 할당하고, TEXT_DYNAMIC인 경우 고정된 128자 버퍼를 사용합니다.
    void SetText(TEXT* txt, const wchar_t* newLabel); // 텍스트 설정 함수 
    void Draw(TEXT* txt); // 텍스트 그리기 함수 
    void Release(TEXT* txt); // 텍스트 해제 함수 

    inline void SetColor(TEXT* txt, const int color) { txt->color = color; } // 텍스트 색상 설정 함수
    inline TEXT_TYPE GetType(TEXT* txt) { return txt->type; } // 텍스트 타입 반환 함수
}
