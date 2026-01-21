#include "TitleCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>
#include <Easing.h>

using namespace Easing;
using namespace MatrixVector;

void TitleCamera::Initialize() {
    // カメラの値の初期化
    transform_ = { { 0.0f,0.0f,0.0f },{ 0.0f, 0.0f, 0.0f} };
    std::vector<CameraTransform> subCams = {
     { {0, 10, -50}, {0, 0, 0} },
     { {5, 15, -60}, {0, 30, 0} },
    };
    // サブカメラを追加
    AddSubCameras(subCams);
}

void TitleCamera::Update() {
    switch (state_) {
    case TitleCameraState::IntroMove:
        UpdateIntroMove();
        break;

    case TitleCameraState::Default:
        if (CameraManager::GetInstance()->GetMode() == CameraMode::Follow) {
            UpdateLookAt();
        }
        break;
    }
}

void TitleCamera::UpdateIntroMove() {
    moveTimer_ += 1.0f / 60.0f; // deltaTime があればそれを使う

    float t = std::clamp(moveTimer_ / moveDuration_, 0.0f, 1.0f);
    float easedT = EaseOutQuad(t);

    transform_.translate = Lerp(startPos_, endPos_, easedT);

    // Follow 中はターゲットを見る
    UpdateLookAt();

    if (t >= 1.0f) {
        // 終了処理
        transform_.translate = endPos_;
        state_ = TitleCameraState::Default;

        CameraManager::GetInstance()->SetCameraMode(CameraMode::Default);
    }
}

void TitleCamera::UpdateLookAt() {
    if (!target_) return;

    Vector3 camPos = transform_.translate;
    Vector3 targetPos = target_->GetWorldPosition();
    Vector3 dir = targetPos - camPos;

    if (Length(dir) > 0.0001f) {
        dir = Normalize(dir);
        float yaw = atan2f(dir.x, dir.z);
        float pitch = -asinf(dir.y);
        transform_.rotate = { pitch, yaw, 0.0f };
    }
}


void TitleCamera::AddSubCamera(const CameraTransform& trans) {
    // 新しいサブカメラインスタンスを生成
    std::unique_ptr<Camera> cam = std::make_unique<Camera>();

    // 渡されたCameraTransformを元に、位置と角度を設定
    cam->SetTranslate(trans.translate);
    cam->SetRotate(trans.rotate);

    // サブカメラリストへ追加
    subcameras_.push_back(std::move(cam)); // 所有権を移動
}

void TitleCamera::AddSubCameras(const std::vector<CameraTransform>& transforms) {
    for (const CameraTransform& trans : transforms) {
        // サブカメラを複数登録する
        AddSubCamera(trans);
    }
}