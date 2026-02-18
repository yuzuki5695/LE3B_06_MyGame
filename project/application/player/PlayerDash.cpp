#include "PlayerDash.h"
#include <cmath>
#include <numbers>
#include <algorithm>

void PlayerDash::Initialize() {
    isDashing_ = false;
    dashTimer_ = 0.0f;
    coolDownTimer_ = 0.0f;
    speedMultiplier_ = 1.0f;
    extraRotationZ_ = 0.0f;
}

void PlayerDash::Start(float directionX) {
    if (!CanDash()) return;

    isDashing_ = true;
    dashTimer_ = 0.0f;
    // 入力がない場合は右(1.0)とする
    dashDirX_ = (std::abs(directionX) > 0.01f) ? (directionX > 0 ? 1.0f : -1.0f) : 1.0f;
}

void PlayerDash::Update() {
    float deltaTime = 1.0f / 60.0f;

    if (isDashing_) {
        dashTimer_ += deltaTime;
        float progress = std::clamp(dashTimer_ / kDashDuration, 0.0f, 1.0f);

        // 1. 回転の計算 (2π回転)
        float totalRotation = 2.0f * std::numbers::pi_v<float>;
        extraRotationZ_ = -dashDirX_ * (totalRotation * progress);

        // 2. 速度倍率の計算 (最初速く、終わり際にかけて戻る)
        // sinカーブを使って 1.0 -> kMaxSpeedMult -> 1.0 と変化させる
        speedMultiplier_ = 1.0f + (kMaxSpeedMult - 1.0f) * std::sin(progress * std::numbers::pi_v<float>);

        if (progress >= 1.0f) {
            isDashing_ = false;
            extraRotationZ_ = 0.0f;
            speedMultiplier_ = 1.0f;
            coolDownTimer_ = kCoolDownTime; // クールタイム開始
        }
    } else {
        // クールタイムの消化
        if (coolDownTimer_ > 0.0f) {
            coolDownTimer_ -= deltaTime;
        }
    }
}