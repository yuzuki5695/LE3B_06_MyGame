#pragma once
#include <BaseBullet.h>

namespace MyGame {
    class PlayerBullet : public BaseBullet {
    public:
        void Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& vel)  override;
        void Update() override;
        void Draw() override;
    };

}