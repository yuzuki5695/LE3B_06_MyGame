#include "PlayerDeath.h"
#include <Object3d.h>
#include <cmath>

void PlayerDeath::Initialize() {
    timer_ = 0.0f;
}

void PlayerDeath::Update(Vector3& rotate, Vector3& offset, Object3d* object) {
    if (!object) return;

    // デルタタイム加算
    timer_ += 1.0f / 60.0f;

    // --- 1. 位置のオフセット加算 ---
    offset.y -= kFallSpeedY;
    offset.z += kFallSpeedZ;

    // --- 2. 回転の更新（sin/cosによるイージング風） ---
    rotate.x += 0.004f + 0.002f * std::sin(timer_ * 0.5f);
    rotate.z += 0.003f + 0.0015f * std::cos(timer_ * 0.4f);

    // --- 3. 点滅（パッシング）処理 ---
    // 0〜1 を高速で往復する値
    float s = (std::sin(timer_ * kBlinkSpeed) + 1.0f) * 0.5f;

    // 閾値を超えた時だけ赤く、それ以外は白
    if (s > 0.85f) {
        object->SetMaterialColor({ 1.0f, 0.0f, 0.0f, 1.0f }); // 赤
    } else {
        object->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 通常
    }
}