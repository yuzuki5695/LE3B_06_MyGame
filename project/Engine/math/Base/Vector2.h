#pragma once

namespace MyEngine {
    /// <summary>
    /// 2次元ベクトル
    /// </summary>
    struct Vector2 final {
        float x;
        float y;

        // ===== 演算子 =====
        Vector2 operator+(const Vector2& v) const { return { x + v.x, y + v.y }; }  // ベクトル加算
        Vector2 operator-(const Vector2& v) const { return { x - v.x, y - v.y }; }  // ベクトル減算
        Vector2 operator*(float s) const { return { x * s, y * s }; }               // スカラー倍（乗算）
        Vector2 operator/(float s) const { return { x / s, y / s }; }               // スカラー除算

        // 複合代入演算子
        Vector2& operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
        Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
        Vector2& operator*=(float s) { x *= s; y *= s; return *this; }
        Vector2& operator/=(float s) { x /= s; y /= s; return *this; }

        // 線形補間
        static Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
            return a + (b - a) * t;
        }

    };
}