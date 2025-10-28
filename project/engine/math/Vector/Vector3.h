#pragma once
#include <cassert>

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
    float x, y, z;

    // 添字演算子（書き込み可）
    float& operator[](size_t i) {
        assert(i < 3);
        return *(&x + i);
    }
    // 添字演算子（読み取り専用）
    const float& operator[](size_t i) const {
        assert(i < 3);
        return *(&x + i);
    }
    ///====================================================
    /// 足し算 引き算 
    ///====================================================
    // (引数がfloat)
    Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
    Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }
    Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    ///====================================================
    /// スカラー演算  
    ///====================================================
    // (引数がfloat)
    Vector3 operator*(float scalar) const { return { x * scalar, y * scalar, z * scalar }; }
    Vector3 operator/(float scalar) const { return { x / scalar, y / scalar, z / scalar }; }
    Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }
    // (引数がVector3) 
    friend Vector3 operator*(float scalar, const Vector3& vec) { return { scalar * vec.x, scalar * vec.y, scalar * vec.z }; }
    Vector3 operator/(const Vector3& other) const { return { x / other.x, y / other.y, z / other.z }; }
    ///====================================================
    /// 比較
    ///====================================================
    // (引数がVector3) 
    bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!=(const Vector3& other) const { return !(*this == other); }
    ///====================================================
    /// 単項マイナス
    ///====================================================
    Vector3 operator-() const { return { -x, -y, -z }; }
    /// <summary>  
    /// 線形補間 (Lerp) 関数 (静的メソッドに変更)  
    /// </summary>  
    /// <param name="start">開始ベクトル</param>  
    /// <param name="end">終了ベクトル</param>  
    /// <param name="t">補間係数 (0.0f ～ 1.0f)</param>  
    /// <returns>補間されたベクトル</returns>  
    static Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
        return start + (end - start) * t;
    }
};