#pragma once
#include <ISceneCameraBehavior.h>

namespace MyGame {

    class TitleCamera : public ISceneCameraBehavior {
    public:

        void Initialize(MyEngine::Camera* camera)override;
        void Update(MyEngine::Camera* camera) override;

        void UpdateLookAt(MyEngine::Camera* camera);

        float LerpAngle(float a, float b, float t) {
            float diff = b - a;

            while (diff > 3.141592f)  diff -= 6.283184f;
            while (diff < -3.141592f) diff += 6.283184f;

            return a + diff * t;
        }

        void StartIntroMove();
        
        void UpdateMove(MyEngine::Camera* camera);

    private:
        MyEngine::Vector3 offset_;

        MyEngine::Vector3 introTargetPos_;
        bool isIntroTargetLocked_ = false;

        float followTightness_ = 0.05f;

        float offsetX_ = 15.0f;
        float offsetY_ = 1.0f;
        float offsetZ_ = -30.0f;
    };
}