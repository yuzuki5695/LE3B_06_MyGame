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
    };
}