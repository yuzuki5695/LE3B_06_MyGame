#include "TitleCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>
#include <Easing.h>

using namespace Easing;
using namespace MatrixVector;

void TitleCamera::Initialize() {
	offset_ = { 30.0f, -5.0f, 300.0f };
    // カメラの値の初期化
    transform_ = { offset_,{ 0.0f, 0.0f, 0.0f} };
    std::vector<CameraTransform> subCams = {
     { {0, 10, -50}, {0, 0, 0} },
    };
    // サブカメラを追加
    AddSubCameras(subCams);

   isFollowAllowed_ = true;
   fixedHeight_ = 30.0f;
   fixeZ_ = 40.0f;
}

void TitleCamera::Update() {
    switch (state_) {
    case TitleCameraState::IntroMove:
        if (isFollowAllowed_) {
            UpdateIntroMove();
        } else {
            transform_.translate = Lerp(transform_.translate, secondTargetPos_, 0.08f);

            float distance = Length(secondTargetPos_ - transform_.translate);

            if (distance < 0.1f) {
                transform_.translate = secondTargetPos_;
                isFollowAllowed_ = false;
            }
        }

        break;

    case TitleCameraState::Default:
        
        UpdateLookAt();

        break;
    case TitleCameraState::Follow:
        if (CameraManager::GetInstance()->GetMode() == CameraMode::Follow) {
            UpdateLookAt();
        }
        break;
    }
}

void TitleCamera::UpdateIntroMove() {

    if (!isFollowAllowed_) return;
    if (!target_) return;

    // ---------------------------
    // 🔥 1回だけ目標を固定する
    // ---------------------------
    if (!isIntroTargetLocked_) {
        Vector3 playerPos = target_->GetWorldPosition();

        introTargetPos_.x = playerPos.x + fixedHeight_;
        introTargetPos_.y = playerPos.y;
        introTargetPos_.z = playerPos.z - fixeZ_;

        isIntroTargetLocked_ = true;
    }

    // ---------------------------
    // 固定目標へ補間移動
    // ---------------------------
    transform_.translate = Lerp(transform_.translate, introTargetPos_, followTightness_);

    // 移動中は注視
    UpdateLookAt();

    float distance = Length(introTargetPos_ - transform_.translate);

    if (distance < 0.1f) {

        // 完全一致させる
        transform_.translate = introTargetPos_;
        secondTargetPos_ = transform_.translate;
        secondTargetPos_.y += 4.0f;
        secondTargetPos_.z += 12.0f;
        // 🔥 注視停止（ここ重要）
        target_ = nullptr;
        isFollowAllowed_ = false;
        isIntroTargetLocked_ = false;
    }
}

void TitleCamera::UpdateLookAt() {
  if (!target_) return;

    Vector3 camPos = transform_.translate;
    Vector3 targetPos = target_->GetWorldPosition();
    Vector3 dir = targetPos - camPos;

    if (Length(dir) > 0.0001f) {

        dir = Normalize(dir);

        float targetYaw = atan2f(dir.x, dir.z);
        float targetPitch = -asinf(dir.y);

        // 🔥 補間係数（小さいほどゆっくり）
        float rotTightness = 0.1f;

        transform_.rotate.x =
            LerpAngle(transform_.rotate.x, targetPitch, 0.1f);

        transform_.rotate.y =
            LerpAngle(transform_.rotate.y, targetYaw, 0.1f);

        transform_.rotate.z = 0.0f;
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