#pragma once
#include <OBB.h>
#include <functional>
#include <cstdint>
#include <CollisionConfig.h>

namespace MyGame {
	// 前方宣言
    class Collider;
	/// コライダー生成のための情報構造体
    struct ColliderCreateInfo {
        using CollisionCallback = std::function<void(Collider*)>;  // 衝突時に呼び出す関数の型定義
        CollisionConfig::CollisionProfile profile;                 // 衝突プロファイル（属性とマスク）
        MyEngine::OBB obb;                                         // 当たり判定のOBB
        CollisionCallback callback = nullptr;                      // 衝突時のコールバック関数
    };
}