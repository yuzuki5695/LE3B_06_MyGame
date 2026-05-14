#pragma once
#include <cstdint>

namespace MyGame {
    namespace CollisionConfig {

        struct CollisionProfile {
            uint32_t attribute;
            uint32_t mask;
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

            constexpr uint32_t None = 0;
            constexpr uint32_t Player = Attribute::Enemy | Attribute::EnemyBullet;
            constexpr uint32_t Enemy = Attribute::PlayerBullet;
            constexpr uint32_t PlayerBullet = Attribute::Enemy;
            constexpr uint32_t EnemyBullet = Attribute::Player;
        }
    
        namespace Profile {
            constexpr CollisionProfile Player{ Attribute::Player,CollisionMask::Player };
            constexpr CollisionProfile Enemy{ Attribute::Enemy,CollisionMask::Enemy };
            constexpr CollisionProfile PlayerBullet{ Attribute::PlayerBullet,CollisionMask::PlayerBullet };
            constexpr CollisionProfile EnemyBullet{ Attribute::EnemyBullet,CollisionMask::EnemyBullet };
        } 
    }
}