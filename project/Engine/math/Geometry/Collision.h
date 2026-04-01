#pragma once
#include <OBB.h>

namespace MyEngine {
    namespace Collision {
        // OBB vs OBB
        bool IsOBBIntersect(const OBB& a, const OBB& b);
    };
}