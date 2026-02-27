#include "Collision.h"

bool IsColliding(Rect rect1, Rect rect2)
{
    // AABB (Axis-Aligned Bounding Box) 충돌 알고리즘
    if (rect1.x < rect2.x + rect2.w && rect1.x + rect1.w > rect2.x)
    {
        if (rect1.y < rect2.y + rect2.h && rect1.y + rect1.h > rect2.y)
        {
            return true; // 겹침 (충돌됨)
        }
    }

    return false; // 안겹침 (충돌안됨)
}