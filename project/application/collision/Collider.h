#pragma once
#include <OBB.h>
#include <cstdint>

namespace MyGame {
    class Collider {
    public:
        virtual ~Collider() = default;

        virtual MyEngine::OBB GetOBB() const = 0;

        virtual void OnCollision(Collider* other) {}

        uint32_t GetCollisionAttribute() const { return attribute_; }
        uint32_t GetCollisionMask() const { return mask_; }

        void SetCollisionAttribute(uint32_t attr) { attribute_ = attr; }
        void SetCollisionMask(uint32_t mask) { mask_ = mask; }

    protected:
        uint32_t attribute_ = 0;
        uint32_t mask_ = 0;
    };
}