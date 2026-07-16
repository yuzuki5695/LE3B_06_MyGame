#include "GameOverCamera.h"
#include <CameraManager.h>

using namespace MyEngine;
using namespace CameraDefs;

namespace MyGame {

    void GameOverCamera::Initialize(MyEngine::Camera* camera) {
		statedata_.type = CameraType::Main;         // メインカメラを使用
		statedata_.state = CameraState::Default;    // デフォルト
		// カメラの位置を設定
        translate_ = { 0.0f, 3.0f, 0.0f };
        camera->SetTranslate(translate_);
    }
    
    void GameOverCamera::Update(MyEngine::Camera* camera) {    
    
    }
}