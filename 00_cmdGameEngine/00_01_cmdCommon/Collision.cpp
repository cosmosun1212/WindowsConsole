#include "Collision.h"

// 두 개의 사각형이 충돌하는지 여부를 체크하는 함수
bool IsColliding(Rect rect1, Rect rect2) // rect1과 rect2가 충돌하는지 여부를 반환하는 함수
{
    // AABB (Axis-Aligned Bounding Box) 충돌 알고리즘
	if (rect1.x < rect2.x + rect2.w && rect1.x + rect1.w > rect2.x)// rect1의 왼쪽이 rect2의 오른쪽보다 왼쪽에 있고, rect1의 오른쪽이 rect2의 왼쪽보다 오른쪽에 있는지 체크
    {
        if (rect1.y < rect2.y + rect2.h && rect1.y + rect1.h > rect2.y)// rect1의 위쪽이 rect2의 아래쪽보다 위에 있고, rect1의 아래쪽이 rect2의 위쪽보다 아래에 있는지 체크
        {
            return true; // 겹침 (충돌됨)
        }
    }
    
    return false; // 안겹침 (충돌안됨)
}
