#pragma once
#include <BaseBullet.h>
#include <Collider.h>

namespace MyGame {
    /// <summary>
    /// プレイヤーの弾クラス
    /// </summary>
    class PlayerBullet : public BaseBullet {
    public: // メンバ関数
        /// <summary>
        /// 終了処理
        /// </summary>
        void Finalize() override;
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
        void SetColliderSize(const MyEngine::Vector3& size) override { colliderSize_ = size; }
        MyEngine::Vector3 GetColliderSize() const override { return colliderSize_; }
        void SetColliderSizelist(const MyEngine::Vector3& size) {
            colliderSize_ = size;
            if (collider_) {
                collider_->SetSize(size);
            }
        }
    };
}