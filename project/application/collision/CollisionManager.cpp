#include "CollisionManager.h"
#include <Collision.h>

using namespace MyEngine;
using namespace MyGame;

// シングルトン用インスタンス
std::unique_ptr<CollisionManager> CollisionManager::instance = nullptr;

///====================================================
/// シングルトンインスタンスの取得
///====================================================
CollisionManager* CollisionManager::GetInstance() {
    if (instance == nullptr) {
        instance = std::unique_ptr<CollisionManager>(new CollisionManager());
    }
    return instance.get();
}

// 終了
void CollisionManager::Finalize() {
    instance.reset();  // `delete` 不要
}

void CollisionManager::CheckAllCollisions() {
    // 全てのコライダーの組み合わせをチェック
    auto itA = colliders_.begin();
    // 2重ループで全ての組み合わせをチェック
    for (; itA != colliders_.end(); ++itA) {
        Collider* colliderA = *itA;
        // 内側のループはitAの次の要素から開始して、同じ組み合わせを二度チェックしないようにする
        auto itB = std::next(itA);
        for (; itB != colliders_.end(); ++itB) {
            Collider* colliderB = *itB;

            // 1. フィルタリング：お互いのマスクが相手の属性を含んでいるか
            if (!(colliderA->GetCollisionMask() & colliderB->GetCollisionAttribute()) &&
                !(colliderB->GetCollisionMask() & colliderA->GetCollisionAttribute())) {
                continue;
            }

            // 2. 衝突判定
            OBB obbA = colliderA->GetOBB();
            OBB obbB = colliderB->GetOBB();
            
            if (MyEngine::Collision::IsOBBIntersect(colliderA->GetOBB(), colliderB->GetOBB())) {

                // 3. 衝突通知（Collider内のstd::functionを呼び出す）
                colliderA->OnCollision(colliderB);
                colliderB->OnCollision(colliderA);
            }
        }
    }
}