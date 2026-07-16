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
        ~PlayerBullet() override;
        /// <summary>
        /// 初期化処理
        /// </summary>
        /// <param name="transform">初期Transform</param>
        /// <param name="velocity">初速度</param>
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
        // コライダーサイズ
        MyEngine::Vector3 collidersize_;
    public:
        // getter
        Collider* GetCollider() const override { return collider_.get(); }
        MyEngine::Vector3 GetColliderSize() const override { return collidersize_; }
        // setter
        void SetColliderSize(const MyEngine::Vector3& size) override {
            collidersize_ = size;
            if (collider_) {
                collider_->SetSize(size);
            }
        }        
    };
}