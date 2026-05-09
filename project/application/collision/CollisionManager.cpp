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
    for (auto itA = colliders_.begin(); itA != colliders_.end(); ++itA) {
        Collider* colliderA = *itA;
        for (auto itB = std::next(itA); itB != colliders_.end(); ++itB) {
            Collider* colliderB = *itB;

            // 修正：両方のビットが立っている時だけ判定
            bool aCanHitB = colliderA->GetCollisionMask() & colliderB->GetCollisionAttribute();
            bool bCanHitA = colliderB->GetCollisionMask() & colliderA->GetCollisionAttribute();

            if (!(aCanHitB && bCanHitA)) {
                continue;
            }

            // 衝突判定
            if (MyEngine::Collision::IsOBBIntersect(colliderA->GetOBB(), colliderB->GetOBB())) {
                colliderA->OnCollision(colliderB);
                colliderB->OnCollision(colliderA);
            }
        }
    }
}