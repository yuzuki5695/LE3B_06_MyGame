#pragma once
#include <BaseBullet.h>
#include <Collider.h>

namespace MyGame {
    class PlayerBullet : public BaseBullet {
    public:
        void Finalize() override;
        void Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity)  override;
        void Update() override;
        void Draw() override;


    private: // メンバ変数
        std::unique_ptr<Collider> collider_;

        //void OnCollision(Collider* other) override;
    };
}