#include "GamePlayCamera.h"
#include <CameraManager.h>

using namespace MyEngine;

namespace MyGame {

    void GamePlayCamera::Initialize(MyEngine::Camera* camera) {

        CameraDefs::StateData data;
        data.type = CameraDefs::CameraType::Main;
        data.state = CameraDefs::CameraState::Default;

        CameraManager::GetInstance()->SetCameraState(data);
    }

    void GamePlayCamera::Update(MyEngine::Camera* camera) {
        CameraSet& camSet = CameraManager::GetInstance()->GetCameraSet();
        if (!camera) return;

        if (CameraManager::GetInstance()->GetCameraState().state == CameraDefs::CameraState::Default) {
            // 現在の座標を取得してXを移動
            MyEngine::Vector3 pos = camera->GetTranslate();
            camera->SetTranslate(pos);
        }
    }
}