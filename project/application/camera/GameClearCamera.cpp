#include "GameClearCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>
#include <Easing.h>
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
        statedata_.type = CameraType::Main;
        statedata_.state = CameraState::LockOn;
        // カメラ位置を初期位置へ設定
        transform_ = { 0.0f,0.0f,0.0f };
        camera->SetTranslate(transform_);

        // 変数を初期化
        movestartx_ = -15.0f;
        moveendx_ = 0.0f;
        movetimer_ = 0.0f;
        moveease_ = 0.0f;
        islookatInitialized_ = false;
        kmovedistance_ = 8.0f;
        kmovespeed_ = 0.02f;
        krotationlerp_ = 0.1f;
        kmindirectionlength_ = 0.0001f;
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void GameClearCamera::Update(MyEngine::Camera* camera) {
        switch (statedata_.state) {
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
            if (!islookatInitialized_) {
                movestartx_ = target_->GetTranslate().x;
                moveendx_ = movestartx_ + kmovedistance_;
                islookatInitialized_ = true;
            }

            // 補間タイマー更新
            movetimer_ += kmovespeed_;

            if (movetimer_ > 1.0f) {
                movetimer_ = 1.0f;
            }

            // イージング計算
            moveease_ = EaseOutCubic(movetimer_);
            // カメラ位置を補間
            transform_.x = movestartx_ + (moveendx_ - movestartx_) * moveease_;
            camera->SetTranslate(transform_);

            // 移動終了
            if (movetimer_ >= 1.0f) {

                statedata_.state = CameraState::Default;
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
        if (Length(direction) > kmindirectionlength_) {
            direction = Normalize(direction);
            // 目標回転角を計算
            float targetYaw = atan2f(direction.x, direction.z);
            float targetPitch = -asinf(direction.y);
            // 現在角度から目標角度へ滑らかに補間
            cameraRotation.x = LerpAngle(cameraRotation.x, targetPitch, krotationlerp_);
            cameraRotation.y = LerpAngle(cameraRotation.y, targetYaw, krotationlerp_);
            cameraRotation.z = 0.0f;
            camera->SetRotate(cameraRotation);
        }
    }
}