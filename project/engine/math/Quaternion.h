#pragma once
#include <Vector3.h>
#include <cmath>

///====================================================
/// クォータニオン構造体（簡易版）
///====================================================
struct Quaternion {
    float w, x, y, z;

    static Quaternion FromEuler(const Vector3& euler) {
        float cy = cosf(euler.y * 0.5f);
        float sy = sinf(euler.y * 0.5f);
        float cp = cosf(euler.x * 0.5f);
        float sp = sinf(euler.x * 0.5f);
        float cr = cosf(euler.z * 0.5f);
        float sr = sinf(euler.z * 0.5f);

        Quaternion q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
        return q;
    }

    Vector3 ToEuler() const {
       Vector3 euler;

       float sinr_cosp = 2 * (w * x + y * z);
       float cosr_cosp = 1 - 2 * (x * x + y * y);
       euler.z = static_cast<float>(std::atan2(sinr_cosp, cosr_cosp)); // Roll, cast to float to avoid C4244

       float sinp = 2 * (w * y - z * x);
       if (fabs(sinp) >= 1)
           euler.x = std::copysign(3.14159265f / 2, sinp); // Pitch
       else
           euler.x = std::asin(sinp);

       float siny_cosp = 2 * (w * z + x * y);
       float cosy_cosp = 1 - 2 * (y * y + z * z);
       euler.y = static_cast<float>(std::atan2(siny_cosp, cosy_cosp)); // Yaw, cast to float to avoid C4244

       return euler;
    }

    static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t) {
        float dot = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
        Quaternion q2b = q2;

        // 反転防止
        if (dot < 0.0f) {
            q2b.w = -q2b.w;
            q2b.x = -q2b.x;
            q2b.y = -q2b.y;
            q2b.z = -q2b.z;
            dot = -dot;
        }

        if (dot > 0.9995f) {
            // 線形補間で十分
            Quaternion result = {
                q1.w + t * (q2b.w - q1.w),
                q1.x + t * (q2b.x - q1.x),
                q1.y + t * (q2b.y - q1.y),
                q1.z + t * (q2b.z - q1.z)
            };
            return Normalize(result);
        }

        float theta_0 = acosf(dot);
        float theta = theta_0 * t;

        float sin_theta = sinf(theta);
        float sin_theta_0 = sinf(theta_0);

        float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
        float s1 = sin_theta / sin_theta_0;

        Quaternion result = {
            s0 * q1.w + s1 * q2b.w,
            s0 * q1.x + s1 * q2b.x,
            s0 * q1.y + s1 * q2b.y,
            s0 * q1.z + s1 * q2b.z
        };
        return Normalize(result);
    }

    static Quaternion Normalize(const Quaternion& q) {
        float len = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
        return { q.w / len, q.x / len, q.y / len, q.z / len };
    }

    static Vector3 QuaternionToForward(const Quaternion& q) {
        // ワールドZ+方向をクォータニオンで回転させた結果
        return Vector3{
            2.0f * (q.x * q.z + q.w * q.y), // X
            2.0f * (q.y * q.z - q.w * q.x), // Y
            1.0f - 2.0f * (q.x * q.x + q.y * q.y) // Z
        };
    }
};
