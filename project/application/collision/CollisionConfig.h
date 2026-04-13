#pragma once
#include <cstdint>

namespace MyGame {
    namespace CollisionConfig {
        // カテゴリを定義
        const uint32_t kGroupNone = 0;          // 判定なし
        const uint32_t kGroupPlayer = 0b1 << 0;   // プレイヤー
        const uint32_t kGroupEnemy = 0b1 << 1;   // 敵
        const uint32_t kGroupPlayerBullet = 0b1 << 2;  // プレイヤーの弾
        const uint32_t kGroupEnemyBullet = 0b1 << 3;  // 敵の弾
    }
}