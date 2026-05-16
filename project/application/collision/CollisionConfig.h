#pragma once
#include <cstdint>

namespace MyGame {
    /// <summary>
	/// 衝突判定のための設定をまとめた名前空間 
    /// </summary>
    namespace CollisionConfig {
		// 衝突プロファイル構造体
        struct CollisionProfile {
			uint32_t attribute;  // 属性をビットフラグで定義
			uint32_t mask;       // 衝突マスクをビットフラグで定義
        };

        // 衝突属性をビットフラグで定義
        enum Attribute : uint32_t {
            // カテゴリを定義
            None = 0,          // 判定なし
            Player = 1 << 0,   // プレイヤー
            Enemy = 1 << 1,   // 敵
            PlayerBullet = 1 << 2,  // プレイヤーの弾
            EnemyBullet = 1 << 3,  // 敵の弾
        };

        // 衝突マスク（相手が誰なら反応するか）
        namespace CollisionMask {
			// 反応する相手をビットフラグで定義
            constexpr uint32_t None = 0;
            constexpr uint32_t Player = Attribute::Enemy | Attribute::EnemyBullet;
            constexpr uint32_t Enemy = Attribute::PlayerBullet;
            constexpr uint32_t PlayerBullet = Attribute::Enemy;
            constexpr uint32_t EnemyBullet = Attribute::Player;
        }

        // 具体的なプロファイルの定義
        namespace Profile {
			// それぞれのプロファイルは、属性とマスクを組み合わせて定義
            constexpr CollisionProfile Player{ Attribute::Player,CollisionMask::Player };
            constexpr CollisionProfile Enemy{ Attribute::Enemy,CollisionMask::Enemy };
            constexpr CollisionProfile PlayerBullet{ Attribute::PlayerBullet,CollisionMask::PlayerBullet };
            constexpr CollisionProfile EnemyBullet{ Attribute::EnemyBullet,CollisionMask::EnemyBullet };
        } 
    }
}