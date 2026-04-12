#pragma once
#include <ISceneCameraBehavior.h>

namespace MyGame {

    class GameOverCamera : public ISceneCameraBehavior {
    public:

        void Initialize(MyEngine::Camera* camera)override;
        void Update(MyEngine::Camera* camera) override;

    private:
        MyEngine::Vector3 targetOffset_ = { 0.0f, 0.0f, -100.0f };
    };
}