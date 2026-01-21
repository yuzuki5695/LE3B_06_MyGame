#pragma once
#include<SceneCameraBase.h>
#include <Object3d.h>

enum class TitleCameraState {
    IntroMove,
    Default
};

// タイトル専用カメラ
class TitleCamera : public SceneCameraBase {
public: // メンバ関数
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 更新処理
    /// </summary> 
    void Update() override;

    // サブカメラの追加
    void AddSubCamera(const CameraTransform& trans) override;
    // サブカメラの追加（複数登録に対応）
    void AddSubCameras(const std::vector<CameraTransform>& transforms) override;

    // --- 追従対象を設定 ---
    void SetTarget(Object3d* target) { target_ = target; }


    void UpdateIntroMove();
    void UpdateLookAt();

private: // メンバ変数 
    // 追従対象
    Object3d* target_ = nullptr;
    float followDistance_ = -20.0f; // 後ろから見る距離（調整可）

    TitleCameraState state_ = TitleCameraState::IntroMove;

    Vector3 startPos_ = { -30.0f, 10.0f, 0.0f };
    Vector3 endPos_ = { 30.0f, -3.0f,  20.0f };

    float moveTimer_ = 0.0f;
    float moveDuration_ = 2.0f; // 秒（60fpsなら120フレーム）
    float lookAtEaseSpeed_ = 5.0f;   // 追従速度（大きいほど速い）
    float targetPitch_ = 0.0f;       // 目標X回転
};