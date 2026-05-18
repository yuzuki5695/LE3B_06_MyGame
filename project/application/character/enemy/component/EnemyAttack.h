#pragma once
#include <Object3d.h>
#include <random>

namespace MyGame {
    /// <summary>
    /// 敵の攻撃クラス
    /// </summary>
    class EnemyAttack {
	public: // メンバ関数
        /// <summary>
		/// 初期化処理
        /// </summary>
        void Initialize();
        /// <summary>
		/// 更新処理
        /// </summary>
        /// <param name="transform"></param>
        /// <param name="aimWorldPos"></param>
        void Update(const MyEngine::Transform& transform, const MyEngine::Vector3& aimWorldPos);
	private: // メンバ変数
        float timer_ = 0.0f;
        float coolTime_ = 5.0f;   // 発射間隔
		bool canShoot_; // 発射可能フラグ
        float speed;
		// 乱数生成器
        std::random_device rd;// 乱数生成器
        std::mt19937 randomEngine_;
        std::uniform_real_distribution<float> distcooltime_;
    };
}