#pragma once
#include <Vector4.h>
#include <Velocity.h>

struct RandomParameter {
    // ランダムな速度の範囲
    Vector3 offsetMin{ 0,0,0 };
    Vector3 offsetMax{ 0,0,0 };
    // ランダムな回転の範囲
    Vector3 rotateMin{ 0,0,0 };
    Vector3 rotateMax{ 0,0,0 };
    // ランダムなスケールの範囲
    Vector3 scaleMin{ 0,0,0 };
    Vector3 scaleMax{ 0,0,0 };
    // ランダムな色の範囲
    float colorMin = 0.0f; // 最小値
    float colorMax = 0.0f; // 最大値
    // ランダムな寿命の範囲を追加
    float lifetimeMin = 0.0f;
    float lifetimeMax = 0.0f;
    // ランダムな速度の範囲を追加
    Velocity velocityMin{};
    Velocity velocityMax{};

    // ---------- setter ----------
    RandomParameter& SetOffset(const Vector3& min, const Vector3& max) {
        offsetMin = min; offsetMax = max;
        return *this;
    }
    RandomParameter& SetRotate(const Vector3& min, const Vector3& max) {
        rotateMin = min; rotateMax = max;
        return *this;
    }
    RandomParameter& SetScale(const Vector3& min, const Vector3& max) {
        scaleMin = min; scaleMax = max;
        return *this;
    }
    RandomParameter& SetColor(float min, float max) {
        colorMin = min; colorMax = max;
        return *this;
    }
    RandomParameter& SetLifetime(float min, float max) {
        lifetimeMin = min; lifetimeMax = max;
        return *this;
    }
    RandomParameter& SetVelocity(const Velocity& min, const Velocity& max) {
        velocityMin = min; velocityMax = max;
        return *this;
    }

    // --- 便利：軸ごとに設定するバージョン（必要なら追加） ---
    RandomParameter& SetRotateZ(float min, float max) {
        rotateMin.z = min; rotateMax.z = max;
        return *this;
    }
};