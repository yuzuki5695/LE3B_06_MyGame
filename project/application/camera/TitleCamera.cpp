#include "TitleCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>
#include <Easing.h>

using namespace MyEngine;
using namespace MatrixVector;
using namespace Easing;
using namespace CameraDefs;

namespace MyGame {

    void TitleCamera::Initialize(Camera* camera) {
        stateData_.type = CameraType::Main;
        stateData_.state = CameraState::LockOn;

        transform_ = { 5.0f, -3.0f, 0.0f };
        camera->SetTranslate(transform_);
    }

    void TitleCamera::Update(Camera* camera) {
        switch (stateData_.type) {
        case CameraType::Main:
            switch (stateData_.state) {
            case CameraState::Default:



                break;
            case CameraState::LockOn:
				// 注視状態の更新
                UpdateLookAt(camera);

                break;
            case CameraState::Move:
				// 移動状態の更新
                UpdateMove(camera);             
                break;
            default:
                break;
            }
            break;
        case CameraType::Sub:
            break;
        }
    }

    void TitleCamera::UpdateLookAt(Camera* camera) {

        Vector3 camPos = camera->GetTranslate();
        Vector3 camrot = camera->GetRotate();
        Vector3 targetPos = targettransform_->translate;
        Vector3 dir= targetPos - camPos;

        if (Length(dir) > 0.0001f) {

            dir = Normalize(dir);

            float targetYaw = atan2f(dir.x, dir.z);
            float targetPitch = -asinf(dir.y);

            // 補間係数（小さいほどゆっくり）
            float rotTightness = 0.1f;

            camrot.x = LerpAngle(camrot.x, targetPitch, rotTightness);
            camrot.y = LerpAngle(camrot.y, targetYaw, rotTightness);
            camrot.z = 0.0f;

            camera->SetRotate(camrot);
        }
    }

    void TitleCamera::StartIntroMove() {
        stateData_.state = CameraState::Move; isIntroTargetLocked_ = false;
    }

    void TitleCamera::UpdateMove(Camera* camera) {
        if (!targettransform_) return;

        // ---------------------------
        // 1回だけ目標固定
        // ---------------------------
        if (!isIntroTargetLocked_) {

            Vector3 playerPos = targettransform_->translate;

            transform_.x = playerPos.x + offsetX_;
            transform_.y = playerPos.y + offsetY_;
            transform_.z = playerPos.z + offsetZ_;

            isIntroTargetLocked_ = true;
        }

        // ---------------------------
        // 補間移動
        // ---------------------------
        Vector3 camPos = camera->GetTranslate();
        camPos = Lerp(camPos, transform_, followTightness_);
        camera->SetTranslate(camPos);

        // ---------------------------
        // 到達判定
        // ---------------------------
        float distance = Length(transform_ - camPos);

        if (distance < 0.1f) {

            // ピタ止め
            camera->SetTranslate(transform_);

            // 注視解除
            targettransform_ = nullptr;

            // 状態遷移
            stateData_.state = CameraState::Default;
        } 
    }
}