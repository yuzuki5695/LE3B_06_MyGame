#pragma once
#include <OBB.h>

class CollisionManager {
public:
    /// <summary>
    /// 2つのOBBが衝突しているか判定
    /// </summary>
    static bool CheckCollision(const OBB& a, const OBB& b);

    // 必要に応じて追加：
    // - プレイヤー弾 vs 敵
    // - 敵弾 vs プレイヤー
    // - オブジェクト同士の当たり判定
};