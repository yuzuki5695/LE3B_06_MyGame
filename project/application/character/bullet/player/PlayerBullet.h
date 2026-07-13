#pragma once
#include <BaseBullet.h>
#include <Collider.h>
#include <CollisionManager.h>

namespace MyGame {
    /// <summary>
    /// プレイヤーの弾クラス
    /// </summary>
    class PlayerBullet : public BaseBullet {
    public: // メンバ関数
        /// <summary>
        /// デストラクタ
        /// </summary>
        ~PlayerBullet() override {
            if (collider_) {
                CollisionManager::GetInstance()->UnregisterCollider(collider_.get());
            }
        }
        /// <summary>
        /// 初期化処理
        /// </summary>
        /// <param name="transform"></param>
        /// <param name="velocity"></param>
        void Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity)  override;
        /// <summary>
        /// 更新処理
        /// </summary>
        void Update() override;
        /// <summary>
        /// 描画処理
        /// </summary>
        void Draw() override;
    private: // メンバ変数
        // コライダー
        std::unique_ptr<Collider> collider_;
        MyEngine::Vector3 colliderSize_;
    public:
        Collider* GetCollider() const override { return collider_.get(); }
        MyEngine::Vector3 GetColliderSize() const override { return colliderSize_; }

        void SetColliderSize(const MyEngine::Vector3& size) override {
            colliderSize_ = size;
            if (collider_) {
                collider_->SetSize(size);
            }
        }        
    };
}