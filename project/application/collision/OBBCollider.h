#pragma once
#include <Collider.h>
#include <CollisionUtils.h>
#include <Object3d.h>

namespace MyGame {
    class OBBCollider : public Collider {
    public:
        OBBCollider(MyEngine::Object3d* object) : object_(object) {}

        MyEngine::OBB GetOBB() const override {
            return CollisionUtils::CreateOBB(object_->GetTransform());
        }

    private:
        MyEngine::Object3d* object_;
    };
}