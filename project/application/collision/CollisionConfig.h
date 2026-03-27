#pragma once
#include <cstdint>

namespace CollisionConfig {
    // カテゴリ（自分は何者か）をビットで定義
    // 0b0001, 0b0010, 0b0100... と重ならないように設定する
    const uint32_t kGroupNone   = 0;          // 判定なし
    const uint32_t kGroupPlayer = 0b1 << 0;   // プレイヤー
    const uint32_t kGroupEnemy  = 0b1 << 1;   // 敵
    const uint32_t kGroupPlayerBullet = 0b1 << 2;  // プレイヤーの弾
    const uint32_t kGroupEnemyBullet = 0b1 << 3;  // 敵の弾
}