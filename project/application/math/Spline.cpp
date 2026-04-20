#include "Spline.h"

using namespace MyEngine;

namespace MyGame::Math::Spline {

    Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
        float t2 = t * t;
        float t3 = t2 * t;

        return 0.5f * (
            (2.0f * p1) +
            (-p0 + p2) * t +
            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
            );
    }

    Vector3 CatmullRomTangent(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
        float t2 = t * t;

        return 0.5f * (
            (-p0 + p2) +
            2.0f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t +
            3.0f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t2
            );
    }
}