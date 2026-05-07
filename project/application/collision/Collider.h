#pragma once
#include <OBB.h>
#include <cstdint>

namespace MyGame {
    /// <summary>
	/// コライダの基底クラス
    /// </summary>
    class Collider {
    public: // メンバ関数
        // コンストラクタとデストラクタ
        virtual ~Collider() = default;
        /// <summary>
        /// OBBを取得する関数
        /// </summary>
        /// <returns></returns>
        virtual MyEngine::OBB GetOBB() const = 0;
        /// <summary>
        /// 衝突したときの処理
        /// </summary>
        /// <param name="other"></param>
        /// <param name="mtv"></param>
        virtual void OnCollision(Collider* other) {}
        /// <summary>
		/// 衝突を通知する関数
        /// </summary>
        /// <param name="other"></param>
        /// <param name="mtv"></param>
        virtual void DispatchCollision(Collider* other) { OnCollision(other); }
    protected: // メンバ変数
        void* owner_ = nullptr;  // コライダーの所有者（例: プレイヤーオブジェクトやステージオブジェクトなど）へのポインタ
        uint32_t attribute_ = 0; // 衝突属性（例: プレイヤー、敵、アイテムなどをビットフラグで表現）
        uint32_t mask_ = 0;      // 衝突マスク（どの属性と衝突するかをビットフラグで表現）
    public: // アクセッサ
        /// getter
        
        /// 所有者の取得・設定関数
        void* GetOwner() const { return owner_; }
        /// 衝突属性とマスクの取得・設定関数
        uint32_t GetCollisionAttribute() const { return attribute_; }
        /// 衝突属性とマスクの取得・設定関数
        uint32_t GetCollisionMask() const { return mask_; }

        /// setter

        /// 所有者の取得・設定関数
        void SetOwner(void* owner) { owner_ = owner; }
        /// 衝突属性とマスクの取得・設定関数
        void SetCollisionAttribute(uint32_t attr) { attribute_ = attr; }
        /// 衝突属性とマスクの取得・設定関数
        void SetCollisionMask(uint32_t mask) { mask_ = mask; }
    };
}