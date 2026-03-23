#include "PlayerMove.h"
#include <Input.h>
#include <algorithm>
#include <CameraManager.h>
#include <Object3d.h>
#include <numbers>

void PlayerMove::Initialize(const PlayerMoveData& data) {
    data_ = data;
    isDashing_ = false;
    dashTimer_ = 0.0f;
    coolDownTimer_ = 0.0f; // 初期化時にクールタイムもリセット
    currentDashSpeedMult_ = 1.0f;
    relativePos_ = { 0.0f, 0.0f, 0.0f };
}

void PlayerMove::Update(Transform& transform, const Vector3& cameraRotate) {
    // GetInstanceの省略
    Input* input = Input::GetInstance();
    float deltaTime = 1.0f / 60.0f; 

    // --- 1. クールタイムの更新 ---
    if (coolDownTimer_ > 0.0f) {
        coolDownTimer_ -= deltaTime;
    }

    // --- 2. 回避入力の受付 ---
    // 非回避中かつクールタイムが終わっていれば回避可能
    if (input->Triggrkey(DIK_LSHIFT) && !isDashing_ && coolDownTimer_ <= 0.0f) {
        // 現在の左右入力（A/D）から回避方向を決定。入力がなければ右(1.0)へ
        float dir = static_cast<float>(input->Pushkey(DIK_D) - input->Pushkey(DIK_A));
        StartDash(dir);
    }
       
    // --- 移動量の合成 ---
    Vector3 move = UpdateNormal();

    if (isDashing_) {
        move += UpdateDash(deltaTime); // ← ここが本質🔥
    }

    // 最後にまとめて適用
    relativePos_ += move;
    ApplyConstraint();


    // --- 4. プレイヤーの回転に反映 ---
    // ベースとなるカメラの回転に、操作による傾き(tilt)を合成する
    transform.rotate.x = cameraRotate.x + tiltX_;
    transform.rotate.y = cameraRotate.y;
    transform.rotate.z = cameraRotate.z + tiltZ_ + extraRotationZ_;
}

void PlayerMove::ApplyConstraint() {
    // 画面外（レールの移動可能範囲）に出ないようにクランプ
    relativePos_.x = std::clamp(relativePos_.x, -data_.maxX, data_.maxX);
    relativePos_.y = std::clamp(relativePos_.y, data_.minY, data_.maxY);
}

void PlayerMove::UpdateTilt(const Vector3& moveInput) {
    auto approach = [](float& value, float target, float speed) {
        value += (target - value) * speed;
    };

    // 入力方向だけ使う（強さは無視）
    float inputX = static_cast<float>((moveInput.x > 0) - (moveInput.x < 0));
    float inputY = static_cast<float>((moveInput.y > 0) - (moveInput.y < 0));

    float targetTiltZ = -inputX * data_.maxTilt;
    float targetTiltX = -inputY * data_.maxTilt;

    approach(tiltZ_, targetTiltZ, data_.returnSpeed);
    approach(tiltX_, targetTiltX, data_.returnSpeed);

    tiltX_ = std::clamp(tiltX_, -data_.maxTilt, data_.maxTilt);
    tiltZ_ = std::clamp(tiltZ_, -data_.maxTilt, data_.maxTilt);
}

void PlayerMove::StartDash(float directionX) {
    // クールタイム中なら受け付けない
    if (isDashing_ || coolDownTimer_ > 0.0f) return;

    isDashing_ = true;
    dashTimer_ = 0.0f;

    // 入力方向に基づいて回転方向を決める（入力がなければ右回転にする）
    dashDirX_ = (std::abs(directionX) > 0.01f) ? (directionX > 0 ? 1.0f : -1.0f) : 1.0f;
}

Vector3 PlayerMove::UpdateDash(float deltaTime) {
    dashTimer_ += deltaTime;

    float progress = std::clamp(dashTimer_ / data_.dashDuration, 0.0f, 1.0f);

    float totalRotation = 2.0f * std::numbers::pi_v<float>;
    extraRotationZ_ = -dashDirX_ * totalRotation * progress;

    // ダッシュ移動量だけ作る
    float dashSpeed = data_.speed * data_.dashSpeedMult;
    Vector3 dashMove{ dashDirX_ * dashSpeed, 0.0f, 0.0f };

    // 傾き演出
    tiltZ_ = -dashDirX_ * data_.maxTilt;

    if (progress >= 1.0f) {
        isDashing_ = false;
        extraRotationZ_ = 0.0f;
        coolDownTimer_ = data_.dashCoolDown;
    }

    return dashMove;
}

Vector3 PlayerMove::UpdateNormal() {
    Input* input = Input::GetInstance();
    const float speed = data_.speed;

    Vector3 moveInput{
        (input->Pushkey(DIK_D) - input->Pushkey(DIK_A)) * speed,
        (input->Pushkey(DIK_W) - input->Pushkey(DIK_S)) * speed,
        0.0f
    };

    UpdateTilt(moveInput);

    return moveInput;
}