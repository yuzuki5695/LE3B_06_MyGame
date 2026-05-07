#pragma once
#include <BaseBullet.h>
#include <ICollisionReceiver.h>
#include <OBBCollider.h>

namespace MyGame {
    class PlayerBullet : public BaseBullet, public ICollisionReceiver {
    public:
        void Finalize() override;
        void Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity)  override;
        void Update() override;
        void Draw() override;

    private: // 内部処理
        void OnCollision(Collider* other) override;

    private:
        std::unique_ptr<OBBCollider> collider_;
    };

}