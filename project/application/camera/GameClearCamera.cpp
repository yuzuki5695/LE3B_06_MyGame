#include "GameClearCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>
#include<Easing.h>

using namespace MyEngine;
using namespace MatrixVector;
using namespace CameraDefs;
using namespace Easing;

namespace MyGame {

    void GameClearCamera::Initialize(MyEngine::Camera* camera) {
        stateData_.type = CameraType::Main;
        stateData_.state = CameraState::LockOn;
        transform_ = { 0.0f, 0.0f, 0.0f };
        camera->SetTranslate(transform_);
    }

    void GameClearCamera::Update(MyEngine::Camera* camera) {
        switch (stateData_.state) {
        case CameraState::Default:

            break;
        case CameraState::LockOn:
            // 注視状態の更新
            UpdateLookAt(camera);
            break;
        case CameraState::Move:
			// 移動開始準備（初回のみ）
            if (!lookAtInitialized_) {
                startX = target_->GetTranslate().x;
                endX = startX + 8.0f;
                lookAtInitialized_ = true;
            }

            t += 0.02f;
            if (t > 1.0f) {
                t = 1.0f;
            }

            // イージングで行う
            easeT= EaseOutCubic(t);
            transform_.x = startX + (endX - startX) * easeT;
            camera->SetTranslate(transform_);
            // 移動終了
            if (t >= 1.0f) {
                stateData_.state = CameraState::Default;
            }
            break;
        }
    }

    void GameClearCamera::UpdateLookAt(Camera* camera) {
        if (!target_) return;

        Vector3 camPos = camera->GetTranslate();
        Vector3 camrot = camera->GetRotate();
        Vector3 targetPos = target_->GetTranslate();
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
}