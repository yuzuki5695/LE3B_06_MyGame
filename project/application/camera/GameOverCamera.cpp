#include "GameOverCamera.h"
#include <CameraManager.h>

using namespace MyEngine;
using namespace CameraDefs;

namespace MyGame {

    void GameOverCamera::Initialize(MyEngine::Camera* camera) {
		stateData_.type = CameraType::Main;         // メインカメラを使用
		stateData_.state = CameraState::Default;    // デフォルト

        transform_ = { 0.0f, 3.0f, 0.0f };
		//  カメラの位置を設定
        camera->SetTranslate(transform_);
    }
    
    void GameOverCamera::Update(MyEngine::Camera* camera) {    
    
    }
}