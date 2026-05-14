#pragma once
#include <OBB.h>
#include <functional>
#include <cstdint>
#include <CollisionConfig.h>

namespace MyGame {
    
    class Collider;

    struct ColliderCreateInfo {
        using CollisionCallback = std::function<void(Collider*)>;
        CollisionConfig::CollisionProfile profile;
        MyEngine::OBB obb;
        CollisionCallback callback = nullptr;
    };
}