#pragma once
#include <Vector3.h>

class PlayerDash {
public:
    void Initialize();
    void Start(float directionX);
    void Update();


private:
    bool isDashing_ = false;
    float dashTimer_ = 0.0f;
    float coolDownTimer_ = 0.0f;

    const float kDashDuration = 0.5f;   // 回転にかかる時間
    const float kCoolDownTime = 3.0f;   // 終わった後の待ち時間
    const float kMaxSpeedMult = 2.5f;   // ダッシュ中の速度倍率

    float dashDirX_ = 0.0f;
    float extraRotationZ_ = 0.0f;
    float speedMultiplier_ = 0.2f;
public:
    // --- 外部から参照する情報 ---
    float GetRotationZ() const { return extraRotationZ_; }
    float GetSpeedMultiplier() const { return speedMultiplier_; }
    bool IsDashing() const { return isDashing_; }
    bool CanDash() const { return !isDashing_ && coolDownTimer_ <= 0.0f; }
    
    // UI表示用などに（0.0〜1.0）
    float GetCoolDownProgress() const { return coolDownTimer_ / kCoolDownTime; }
};