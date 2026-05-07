#pragma once
#include <cstdint>

namespace MyGame {
    namespace CollisionConfig {
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
        namespace Mask {
            const uint32_t kNone = None;
            // 敵はプレイヤーとプレイヤーの弾に当たる
            const uint32_t kEnemy = Player | PlayerBullet;
            // プレイヤーは敵と敵の弾に当たる
            const uint32_t kPlayer = Enemy | EnemyBullet;
            // プレイヤーの弾は敵に当たる
            const uint32_t kPlayerBullet = Enemy;
            // 敵の弾はプレイヤーに当たる
            const uint32_t kEnemyBullet = Player;
        }
    }
}