#pragma once
#include <OBB.h>
#include <cstdint>
#include <functional>
#include <ColliderCreateInfo.h>

namespace MyGame {
    class Collider {
    public:
        // 衝突時に呼び出す関数の型定義
        using CollisionCallback = std::function<void(Collider*)>;
    public:
        Collider() = default;
        virtual ~Collider() = default;

        // 衝突通知
        void OnCollision(Collider* other) { if (callback_) { callback_(other); } }
        // 
        static std::unique_ptr<Collider> Create(const ColliderCreateInfo& info) {
            std::unique_ptr<Collider> collider = std::make_unique<Collider>();
            collider->attribute_ = info.profile.attribute;
            collider->mask_ = info.profile.mask;
            collider->obb_ = info.obb;
            collider->callback_ = info.callback;

            return collider;
        }

    private: //メンバ変数
        MyEngine::OBB obb_;
        uint32_t attribute_;
        uint32_t mask_;
        // 通知 
        CollisionCallback callback_ = nullptr;
    public: // アクセッサ
        // getter
        const MyEngine::OBB& GetOBB() const { return obb_; }
        uint32_t GetAttribute() const { return attribute_; }
        uint32_t GetMask() const { return mask_; }
        // setter
        void SetCallback(CollisionCallback callback) { callback_ = callback; }         // コールバック設定
        void SetOBB(const MyEngine::OBB& obb) { obb_ = obb; }
        void SetAttribute(uint32_t attribute) { attribute_ = attribute; }
        void SetMask(uint32_t mask) { mask_ = mask; }
    };
}