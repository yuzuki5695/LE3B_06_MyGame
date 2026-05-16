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
	public: // メンバ関数
		// コンストラクタとデストラクタ
        Collider() = default;
        virtual ~Collider() = default; 
        /// <summary>
		/// 衝突が発生したときに呼び出される関数
        /// </summary>
        /// <param name="other"></param>
        void OnCollision(Collider* other) { if (callback_) { callback_(other); } }
        /// <summary>
		/// Colliderの生成
        /// </summary>
        /// <param name="info"></param>
        /// <returns></returns>
        static std::unique_ptr<Collider> Create(const ColliderCreateInfo& info) {
            std::unique_ptr<Collider> collider = std::make_unique<Collider>();
			// プロフィールとOBBをセット
            collider->attribute_ = info.profile.attribute;
            collider->mask_ = info.profile.mask;
            collider->obb_ = info.obb;
            return collider;
        }
    private: //メンバ変数
		MyEngine::OBB obb_;                     // 当たり判定のOBB
		uint32_t attribute_;                    // 属性ビットフィールド
		uint32_t mask_;                         // 衝突マスクビットフィールド        
		CollisionCallback callback_ = nullptr;  // 衝突時のコールバック関数
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