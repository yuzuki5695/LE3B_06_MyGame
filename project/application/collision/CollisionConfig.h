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
    }
}