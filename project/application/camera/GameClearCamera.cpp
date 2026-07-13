#include "GameClearCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>
#include<Easing.h>
#include <MathUtil.h>

using namespace MyEngine;
using namespace MatrixVector;
using namespace MathUtil;
using namespace CameraDefs;
using namespace Easing;

namespace MyGame {
       
    ///====================================================
    /// 初期化処理
    ///====================================================
    void GameClearCamera::Initialize(MyEngine::Camera* camera) {
        // カメラ状態を初期化
        stateData_.type = CameraType::Main;
        stateData_.state = CameraState::LockOn;
        // カメラ位置を初期位置へ設定
        transform_ = { 0.0f,0.0f,0.0f };
        camera->SetTranslate(transform_);

        // 変数を初期化
        moveStartX_ = -15.0f;
        moveEndX_ = 0.0f;
        moveTimer_ = 0.0f;
        moveEase_ = 0.0f;
        isLookAtInitialized_ = false;
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void GameClearCamera::Update(MyEngine::Camera* camera) {
        switch (stateData_.state) {
            // 通常状態
        case CameraState::Default:
            break;
            // ターゲットを注視する
        case CameraState::LockOn:
            UpdateLookAt(camera);
            break;
            // カメラ移動
        case CameraState::Move:
            // 移動開始位置を一度だけ設定
            if (!isLookAtInitialized_) {
                moveStartX_ = target_->GetTranslate().x;
                moveEndX_ = moveStartX_ + kMoveDistance;
                isLookAtInitialized_ = true;
            }

            // 補間タイマー更新
            moveTimer_ += kMoveSpeed;

            if (moveTimer_ > 1.0f) {
                moveTimer_ = 1.0f;
            }

            // イージング計算
            moveEase_ = EaseOutCubic(moveTimer_);
            // カメラ位置を補間
            transform_.x = moveStartX_ + (moveEndX_ - moveStartX_) * moveEase_;
            camera->SetTranslate(transform_);

            // 移動終了
            if (moveTimer_ >= 1.0f) {

                stateData_.state = CameraState::Default;
            }
            break;
        }
    }
    ///====================================================
    /// ターゲットを注視する
    ///====================================================
    void GameClearCamera::UpdateLookAt(Camera* camera) {

        // ターゲットが存在しない場合は終了
        if (!target_) {
            return;
        }

        // カメラ・ターゲット情報を取得
        Vector3 cameraPosition = camera->GetTranslate();
        Vector3 cameraRotation = camera->GetRotate();
        Vector3 targetPosition = target_->GetTranslate();

        // ターゲット方向ベクトル
        Vector3 direction = targetPosition - cameraPosition;

        // ベクトル長が十分ある場合のみ回転を更新
        if (Length(direction) > kMinDirectionLength) {
            direction = Normalize(direction);
            // 目標回転角を計算
            float targetYaw = atan2f(direction.x, direction.z);
            float targetPitch = -asinf(direction.y);
            // 現在角度から目標角度へ滑らかに補間
            cameraRotation.x = LerpAngle(cameraRotation.x, targetPitch, kRotationLerp);
            cameraRotation.y = LerpAngle(cameraRotation.y, targetYaw, kRotationLerp);
            cameraRotation.z = 0.0f;
            camera->SetRotate(cameraRotation);
        }
    }
}