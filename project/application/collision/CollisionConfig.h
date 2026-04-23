#pragma once
#include <cstdint>

namespace MyGame {
    namespace CollisionConfig {
        // カテゴリを定義
        const uint32_t None = 0;          // 判定なし
        const uint32_t Player = 0b1 << 0;   // プレイヤー
        const uint32_t Enemy = 0b1 << 1;   // 敵
        const uint32_t PlayerBullet = 0b1 << 2;  // プレイヤーの弾
        const uint32_t EnemyBullet = 0b1 << 3;  // 敵の弾
    }
}