#include"MathUtil.h"
#include <algorithm>
#include <MatrixVector.h>

namespace MyEngine {

    using namespace MatrixVector;

    namespace MathUtil {
        Vector3 Slerp(const Vector3& v0, const Vector3& v1, float t) {
            float dot = Dot(v0, v1);
            dot = std::clamp(dot, -1.0f, 1.0f); // 安全クランプ

            float theta = acosf(dot) * t;
            Vector3 relative = Normalize(v1 - v0 * dot);
            return Normalize(v0 * cosf(theta) + relative * sinf(theta));
        }

        Vector3 LookAtRotation(const Vector3& forward) {
            Vector3 rot;
            rot.y = atan2f(forward.x, forward.z); // Yaw
            rot.x = asinf(-forward.y);            // Pitch
            rot.z = 0.0f;                         // Roll
            return rot;
        }

        Vector4 TransformCoord(const Vector4& v, const Matrix4x4& m) {
            Vector4 result;

            result.x =
                v.x * m.m[0][0] +
                v.y * m.m[1][0] +
                v.z * m.m[2][0] +
                v.w * m.m[3][0];

            result.y =
                v.x * m.m[0][1] +
                v.y * m.m[1][1] +
                v.z * m.m[2][1] +
                v.w * m.m[3][1];

            result.z =
                v.x * m.m[0][2] +
                v.y * m.m[1][2] +
                v.z * m.m[2][2] +
                v.w * m.m[3][2];

            result.w =
                v.x * m.m[0][3] +
                v.y * m.m[1][3] +
                v.z * m.m[2][3] +
                v.w * m.m[3][3];

            return result;
        }

        float LerpAngle(float a, float b, float t) {
            float diff = b - a;

            while (diff > 3.141592f)  diff -= 6.283184f;
            while (diff < -3.141592f) diff += 6.283184f;

            return a + diff * t;
        }
    }
}