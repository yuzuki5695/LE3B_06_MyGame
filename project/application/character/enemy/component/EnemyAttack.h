#pragma once
#include <Object3d.h>
#include <random>
#include <EnemyType.h>

namespace MyGame {
    /// <summary>
    /// 敵の攻撃状態
    /// </summary>
    enum class AttackState {
        Dash,   // 突撃中
        Charge, // 突撃前の溜め状態
        Idle    // 待機状態
    };
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
        void Update(EnemyType type, MyEngine::Object3d* object, const MyEngine::Vector3& aimWorldPos);
        /// <summary>
        /// 射撃タイプの攻撃更新
        /// </summary>
        /// <param name="object">敵オブジェクト</param>
        /// <param name="aimWorldPos">狙うワールド座標</param>
        void UpdateShot(MyEngine::Object3d* object, const MyEngine::Vector3& aimWorldPos);
        /// <summary>
        /// 突撃タイプの攻撃更新
        /// </summary>
        /// <param name="object">敵オブジェクト</param>
        /// <param name="aimWorldPos">狙うワールド座標</param>
        void UpdateTackle(MyEngine::Object3d* object, const MyEngine::Vector3& aimWorldPos);
    private: // メンバ変数
        float timer_;      // 攻撃タイマー
        float coolTime_;   // 発射間隔
        bool canShoot_;    // 発射可能フラグ
        float speed;       // 弾の移動速度
        // 乱数生成器
        std::random_device rd;                                  // 乱数生成器
        std::mt19937 randomEngine_;                             // 乱数生成器
        std::uniform_real_distribution<float> distcooltime_;    // クールタイム決定用乱数
        // 状態遷移
        AttackState state_;
        // 突撃用変数
        MyEngine::Vector3 dashVelocity_;    // 突撃速度
        float dashSpeed_;                   // 突撃速度
        float dashDistance_;                // 現在の突撃距離
        float maxDashDistance_;             // 最大突撃距離
        float dashRotationSpeed_;           // 突撃中の回転速度
        float chargeTimer_;                 // 溜め時間タイマー
        float chargeDuration_;              // 溜め時間
        MyEngine::Vector3 chargeStartPos_;  // 溜め開始位置
    };
}