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
    // クールタイム中なら受け付けない
    if (!CanDash()) return;

    isDashing_ = true;
    dashTimer_ = 0.0f;
    
    // 入力方向に基づいて回転方向を決める（入力がなければ右回転にする）
    dashDirX_ = (std::abs(directionX) > 0.01f) ? (directionX > 0 ? 1.0f : -1.0f) : 1.0f;
}

void PlayerDash::Update() {
    float deltaTime = 1.0f / 60.0f;

    if (isDashing_) {
        // --- ダッシュ中の処理 ---
        dashTimer_ += deltaTime;
        
        // ダッシュの進捗率（0.0 ～ 1.0）を計算
        float progress = std::clamp(dashTimer_ / kDashDuration, 0.0f, 1.0f);

        // 回転の計算
        // 360度（2π）を現在の進捗率に掛けて、回転角を算出する
        float totalRotation = 2.0f * std::numbers::pi_v<float>;
        extraRotationZ_ = -dashDirX_ * (totalRotation * progress);

        // 速度倍率の計算
        // sin波を利用して、開始時と終了時は「1.0」、中間地点で「kMaxSpeedMult」になるように調整
        // これにより、滑らかに加速して滑らかに減速する
        speedMultiplier_ = 1.0f + (kMaxSpeedMult - 1.0f) * std::sin(progress * std::numbers::pi_v<float>);

        // 時間が経過したらダッシュ終了
        if (dashTimer_ >= kDashDuration) {
            isDashing_ = false;
            dashTimer_ = 0.0f;
            extraRotationZ_ = 0.0f;   // 回転をリセット
            speedMultiplier_ = 1.0f;  // 速度を通常に戻す
            coolDownTimer_ = kCoolDownTime; // クールタイム開始
        }
    }
    else if (coolDownTimer_ > 0.0f) {
        // --- クールタイムのカウントダウン ---
        coolDownTimer_ -= deltaTime;
    }
}