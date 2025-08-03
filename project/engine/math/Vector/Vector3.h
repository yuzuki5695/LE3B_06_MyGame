#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
    float x, y, z;

        // 添字演算子（書き込み可）
    float& operator[](size_t i) {
        switch(i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }
    }

    // 添字演算子（読み取り専用）
    const float& operator[](size_t i) const {
        switch(i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }
    }

    // 足し算
    Vector3 operator+(const Vector3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    // 引き算  
    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    // 代入付き足し算
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // スカラー倍（左）
    friend Vector3 operator*(float scalar, const Vector3& vec) {
        return { scalar * vec.x, scalar * vec.y, scalar * vec.z };
    }

    // スカラー倍（右）
    Vector3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    // 単項マイナス演算子のオーバーロードを追加  
    Vector3 operator-() const {
        return { -x, -y, -z };
    }
};