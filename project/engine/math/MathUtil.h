#pragma once
#include "Vector3.h"

namespace MathUtil {
   // float Clamp(float v, float min, float max);
   // Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
    Vector3 Slerp(const Vector3& a, const Vector3& b, float t);
    Vector3 LookAtRotation(const Vector3& forward);

}