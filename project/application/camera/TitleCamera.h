#pragma once
#include <ISceneCameraBehavior.h>

namespace MyGame {

    class TitleCamera : public ISceneCameraBehavior {
    public:

        void Initialize(MyEngine::Camera* camera)override;
        void Update(MyEngine::Camera* camera) override;
        
        void UpdateLookAt(MyEngine::Camera* camera);
          
        //// 角度の線形補間（ラジアン値で補間、ラップアラウンド対応）
        //static float LerpAngle(float a, float b, float t) {
        //    float diff = fmodf(b - a + 3.14159265f, 2.0f * 3.14159265f) - 3.14159265f;
        //    return a + diff * t;
        //}
        float LerpAngle(float a, float b, float t) {
            float diff = b - a;

            while (diff > 3.141592f)  diff -= 6.283184f;
            while (diff < -3.141592f) diff += 6.283184f;

            return a + diff * t;
        }
    private:
        MyEngine::Vector3 offset_;
    };
}