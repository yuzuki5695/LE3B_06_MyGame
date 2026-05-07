#pragma once
#include <OBB.h>
#include <cstdint>
#include <functional>

namespace MyGame {

    class Collider {
    public:
        // 衝突時に呼び出す関数の型定義
        // 引数にCollider*を渡すことで、相手の属性(Attribute)などを確認できるようにします
        using CollisionCallback = std::function<void(Collider*)>;

        Collider() = default;
        virtual ~Collider() = default;

        // --- 衝突判定データ (OBB) ---
        // 毎フレーム、オブジェクトの移動後にセットすることを想定
        void SetOBB(const MyEngine::OBB& obb) { obb_ = obb; }
        const MyEngine::OBB& GetOBB() const { return obb_; }

        // --- フィルタリング設定 (CollisionConfigと連携) ---
        void SetCollisionAttribute(uint32_t attr) { attribute_ = attr; }
        uint32_t GetCollisionAttribute() const { return attribute_; }

        void SetCollisionMask(uint32_t mask) { mask_ = mask; }
        uint32_t GetCollisionMask() const { return mask_; }

        // --- コールバック設定 ---
        // 衝突した瞬間に呼び出す処理を登録
        void SetCallback(CollisionCallback callback) { callback_ = callback; }

        // 衝突通知 (CollisionManagerから呼ばれる)
        void OnCollision(Collider* other) {
            if (callback_) {
                callback_(other);
            }
        }

    private:
        // --- 判定形状データ ---
        MyEngine::OBB obb_;

        // --- フィルタリングデータ ---
        // 自身の種類
        uint32_t attribute_ = 0;
        // 衝突対象のフラグ
        uint32_t mask_ = 0;
        // 通知 
        CollisionCallback callback_ = nullptr;
    };
}