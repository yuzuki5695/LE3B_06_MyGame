#pragma once
#include<SceneCameraBase.h>
#include <Object3d.h>

enum class TitleCameraState {
    IntroMove,
    Default,
    Follow
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
    
    void SetIsFollowAllowed(bool isAllowed) { isFollowAllowed_ = isAllowed; }
    bool GetIsFollowAllowed() const { return isFollowAllowed_; }

	void SetTitleCameraState(TitleCameraState state) { state_ = state; }
        float LerpAngle(float a, float b, float t)
{
    float diff = b - a;

    while (diff > 3.141592f)  diff -= 6.283184f;
    while (diff < -3.141592f) diff += 6.283184f;

    return a + diff * t;
}

private: // メンバ変数 
    // 追従対象
    Object3d* target_ = nullptr;
    float followDistance_ = -20.0f; // 後ろから見る距離（調整可）

    TitleCameraState state_ = TitleCameraState::Default;
 
    Vector3 secondTargetPos_;
    // 追従開始フラグ（外部からセットするか、特定の条件でtrueにする）
    bool isFollowAllowed_;


    float moveTimer_ = 0.0f;
    float moveDuration_ = 2.0f; // 秒（60fpsなら120フレーム）
    float lookAtEaseSpeed_ = 5.0f;   // 追従速度（大きいほど速い）
    float targetPitch_ = 0.0f;       // 目標X回転

    // --- 追従設定 ---
    float followTightness_ = 0.1f;    // 追従の強さ (0.0~1.0): 小さいほどゆっくり遅れてくる
    float fixedHeight_;       // 固定したいY軸の高さ
    float fixeZ_;       // 固定したいY軸の高さ
    Vector3 offset_ = { 0.0f, 0.0f, -100.0f }; // プレイヤーからどれだけ離れるか（例：後方20）

    Vector3 introTargetPos_;
    bool isIntroTargetLocked_ = false;
};