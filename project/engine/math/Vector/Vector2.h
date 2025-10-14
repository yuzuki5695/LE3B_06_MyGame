#pragma once
/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x;
	float y;
    
    Vector2 operator*(float scalar) const {
        return { x * scalar, y * scalar };
    }

    Vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
};