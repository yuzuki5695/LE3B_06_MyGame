#include "GameOverCamera.h"
#include <CameraManager.h>

using namespace MyEngine;
using namespace CameraDefs;

namespace MyGame {

    void GameOverCamera::Initialize(MyEngine::Camera* camera) {
        stateData_.type = CameraType::Main;
        stateData_.state = CameraState::Default;

        transform_ = { 0.0f, 3.0f, 0.0f };
        camera->SetTranslate(transform_);
    }
    
    void GameOverCamera::Update(MyEngine::Camera* camera) {    
    
    }
}