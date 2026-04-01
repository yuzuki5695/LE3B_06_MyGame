#pragma once

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4 final {
    float x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};