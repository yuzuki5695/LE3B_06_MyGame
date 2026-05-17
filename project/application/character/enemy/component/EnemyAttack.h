#pragma once
#include <Object3d.h>                   

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
    private:
        float coolTime_ = 0.2f;   // 発射間隔
        float timer_ = 0.0f;
        float interval_; // 1秒ごとに発射
		bool canShoot_; // 発射可能フラグ
    };
}