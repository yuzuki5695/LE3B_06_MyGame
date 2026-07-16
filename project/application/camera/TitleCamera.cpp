#include "TitleCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>
#include <Easing.h>
#include <MathUtil.h>

using namespace MyEngine;
using namespace MatrixVector;
using namespace Easing;
using namespace CameraDefs;
using namespace MathUtil;

namespace MyGame {
    
    ///====================================================
    /// 初期化処理
    ///====================================================
    void TitleCamera::Initialize(Camera* camera) {
        // カメラ状態を初期化
        statedata_.type = CameraType::Main;
        statedata_.state = CameraState::LockOn;
        // カメラ位置を初期位置へ設定
        kInitialposition_ = { 5.0f, -3.0f, 0.0f };
        camera->SetTranslate(kInitialposition_);
        // メンバ変数を初期化
        targetPosition_ = kInitialposition_;
        kOffset_ = { 6.0f,-1.0f,-20.0f };
        kfollowtightness_ = 0.05f;
        krotationlerp_ = 0.1f;
        kmindirectionlength_ = 0.0001f;
        kmovecompletedistance_ = 0.1f;
        isIntrotargetlocked_ = false;
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void TitleCamera::Update(Camera* camera) {
        switch (statedata_.type) {
        case CameraType::Main:
            switch (statedata_.state) {
                // 通常状態
            case CameraState::Default:
                break;

                // ターゲットを注視する
            case CameraState::LockOn:
                UpdateLookAt(camera);
                break;

                // 演出移動
            case CameraState::Move:
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
    ///====================================================
    /// ターゲットを注視する
    ///====================================================
    void TitleCamera::UpdateLookAt(Camera* camera) {

        // ターゲットが存在しない場合
        if (!target_) {
            return;
        }

        // カメラ情報を取得
        Vector3 cameraPosition = camera->GetTranslate();
        Vector3 cameraRotation = camera->GetRotate();
        // ターゲット位置を取得
        Vector3 targetPosition = target_->GetTranslate();
        // ターゲット方向
        Vector3 direction = targetPosition - cameraPosition;

        // 十分な長さがある場合のみ回転更新
        if (Length(direction) > kmindirectionlength_) {
            direction = Normalize(direction);
            float targetYaw = atan2f(direction.x, direction.z);
            float targetPitch = -asinf(direction.y);
            // 滑らかに回転
            cameraRotation.x = LerpAngle(cameraRotation.x, targetPitch, krotationlerp_);
            cameraRotation.y = LerpAngle(cameraRotation.y, targetYaw, krotationlerp_);
            cameraRotation.z = 0.0f;
            camera->SetRotate(cameraRotation);
        }
    }
    ///====================================================
    /// タイトル演出用カメラ移動開始
    ///====================================================
    void TitleCamera::StartIntroMove() {
        statedata_.state = CameraState::Move;
        isIntrotargetlocked_ = false;
    }
    ///====================================================
    /// タイトル演出用カメラ移動更新
    ///====================================================
    void TitleCamera::UpdateMove(Camera* camera) {

        // ターゲットが存在しない場合
        if (!target_) {
            return;
        }
        //==========================================
        // 目標位置を一度だけ決定
        //==========================================
        if (!isIntrotargetlocked_) {
            Vector3 playerPosition = target_->GetTranslate();
            targetPosition_ = playerPosition + kOffset_;
            isIntrotargetlocked_ = true;
        }

        //==========================================
        // カメラ位置を補間
        //==========================================
        Vector3 cameraPosition = camera->GetTranslate();
        cameraPosition = Lerp(cameraPosition, targetPosition_, kfollowtightness_);
        camera->SetTranslate(cameraPosition);

        //==========================================
        // 移動完了判定
        //==========================================
        float distance = Length(targetPosition_ - cameraPosition);

        if (distance < kmovecompletedistance_) {
            // 最終位置へ補正
            camera->SetTranslate(targetPosition_);
            // 通常状態へ戻す
            statedata_.state = CameraState::Default;
        }
    }
}