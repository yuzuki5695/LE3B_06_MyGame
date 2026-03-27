#include "CollisionManager.h"
#include <Collision.h>

using namespace Collision;

// シングルトン用インスタンス
std::unique_ptr<CollisionManager> CollisionManager::instance_ = nullptr;

///====================================================
/// シングルトンインスタンスの取得
///====================================================
CollisionManager* CollisionManager::GetInstance() {
    if (instance_ == nullptr) {
        // privateコンストラクタなので new を unique_ptr に包む
        instance_ = std::unique_ptr<CollisionManager>(new CollisionManager());
    }
    return instance_.get();
}

void CollisionManager::CheckAllCollisions() {
    auto itA = colliders_.begin();
    for (; itA != colliders_.end(); ++itA) {
        Collider* colliderA = *itA;

        auto itB = std::next(itA);
        for (; itB != colliders_.end(); ++itB) {
            Collider* colliderB = *itB;

            // 1. フィルタリング：お互いのマスクが相手の属性を含んでいるか
            if (!(colliderA->GetCollisionMask() & colliderB->GetCollisionAttribute()) &&
                !(colliderB->GetCollisionMask() & colliderA->GetCollisionAttribute())) {
                continue;
            }

            // 2. 衝突判定 (OBB vs OBB)
            OBB obbA = colliderA->GetOBB();
            OBB obbB = colliderB->GetOBB();

            if (IsOBBIntersect(obbA, obbB)) {
                // 3. 衝突通知
                colliderA->OnCollision(colliderB);
                colliderB->OnCollision(colliderA);
            }
        }
    }
}