#include "TitleCamera.h"
#include <CameraManager.h>
#include <MatrixVector.h>

using namespace MyEngine;
using namespace MatrixVector;

namespace MyGame {

    void TitleCamera::Initialize(Camera* camera) {
        if (!camera) return;
        CameraDefs::StateData data;
        data.type = CameraDefs::CameraType::Main;
        data.state = CameraDefs::CameraState::Default;
        // カメラマネージャに状態をセット
        CameraManager::GetInstance()->SetCameraState(data);
        offset_ = { 3.0f, 0.0f, 0.0f };
        camera->SetTranslate(offset_);
    }

    void TitleCamera::Update(Camera* camera) {
        if (!camera) return;



        CameraSet& camSet = CameraManager::GetInstance()->GetCameraSet();
        // サブカメラ中なら何もしない
        if (camSet.IsUsingSubCamera()) {
            return;
        }

        // 常にプレイヤーを見る
       // UpdateLookAt(camera);
    }

    void TitleCamera::UpdateLookAt(Camera* camera) {
        // CameraManagerにセットされているターゲットを取得
        Object3d* target = CameraManager::GetInstance()->GetTarget();
        if (!target) return;

        Vector3 targetPos = target->GetWorldPosition();

        // 🔥 カメラ位置をターゲット基準にする
        Vector3 camPos = targetPos + offset_;
        camera->SetTranslate(camPos);

        // ターゲットへの方向ベクトル
        Vector3 dir = targetPos - camPos;

        // 距離が近すぎると計算が破綻するのでチェック
        if (Length(dir) > 0.001f) {
            dir = Normalize(dir);

            // 方向から角度（Yaw, Pitch）を計算
            float targetYaw = atan2f(dir.x, dir.z);
            float targetPitch = -asinf(dir.y);

            Vector3 rot = camera->GetRotate();

            // 線形補間で滑らかに回転させる
            rot.x = LerpAngle(rot.x, targetPitch, 0.1f);
            rot.y = LerpAngle(rot.y, targetYaw, 0.1f);
            rot.z = 0.0f;

             camera->SetRotate(rot);
        }
    }
}