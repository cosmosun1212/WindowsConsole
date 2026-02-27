#pragma once
#pragma once

typedef struct _Rect
{
    float x;
    float y;
    int w;
    int h;
} Rect;

bool IsColliding(Rect rect1, Rect rect2);
