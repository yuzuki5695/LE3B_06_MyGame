#pragma once
#include <memory>
#include <Collider.h>

namespace MyGame {
    class CollisionComponent {
    public:
        void SetCollider(std::unique_ptr<Collider> collider) {
            collider_ = std::move(collider);
        }

        Collider* GetCollider() const {
            return collider_.get();
        }

    private:
        std::unique_ptr<Collider> collider_;
    };
}