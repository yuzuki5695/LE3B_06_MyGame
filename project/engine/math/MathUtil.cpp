#include"MathUtil.h"
#include <algorithm>
#include <MatrixVector.h>

using namespace MatrixVector;

namespace MathUtil {
    ///====================================================
    /// 球面線形補間 (Slerp)
    ///====================================================
    Vector3 Slerp(const Vector3& v0, const Vector3& v1, float t) {
        float dot = Dot(v0, v1);
        dot = std::clamp(dot, -1.0f, 1.0f); // 安全クランプ

        float theta = acosf(dot) * t;
        Vector3 relative = Normalize(v1 - v0 * dot);
        return Normalize(v0 * cosf(theta) + relative * sinf(theta));
    }  
    ///====================================================
    /// LookAt 用の回転計算（簡易版）
    /// forward: 向きベクトル
    ///====================================================
    Vector3 LookAtRotation(const Vector3& forward) {
        Vector3 rot;
        rot.y = atan2f(forward.x, forward.z); // Yaw
        rot.x = asinf(-forward.y);            // Pitch
        rot.z = 0.0f;                         // Roll
        return rot;
    }










}