#pragma once

// 충돌 감지 관련 함수와 구조체 정의
typedef struct _Rect
{
	float x;// 사각형의 왼쪽 상단 X 좌표
	float y;// 사각형의 왼쪽 상단 Y 좌표
	int w;// 사각형의 너비
	int h;// 사각형의 높이
} Rect;// 사각형 구조체 정의

// 두 개의 사각형이 충돌하는지 여부를 체크하는 함수
bool IsColliding(Rect rect1, Rect rect2);
