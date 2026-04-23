#pragma once
#include <BaseBullet.h>
#include <OBBCollider.h>

namespace MyGame {
    class EnemyBullet : public BaseBullet {
    public:
        void Finalize() override; 
        void Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity)  override;
        void Update() override;
        void Draw() override;

   
        void OnCollision(Collider* other);

	private:
        std::unique_ptr<OBBCollider> collider_;
    };

}