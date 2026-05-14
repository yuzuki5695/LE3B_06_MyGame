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

    for (auto itrA = colliders_.begin(); itrA != colliders_.end(); ++itrA) {
        auto itrB = itrA;
        ++itrB;
        for (; itrB != colliders_.end(); ++itrB) {

            Collider* a = *itrA;
            Collider* b = *itrB;

            //========================
            // マスクチェック
            //========================

            bool canCollision = (a->GetAttribute() & b->GetMask()) || (b->GetAttribute() & a->GetMask());

            if (!canCollision) {
                continue;
            }

            //========================
            // OBB判定
            //========================

            if (CollisionUtils::CheckOBB(a->GetOBB(), b->GetOBB())) {

                a->OnCollision(b);
                b->OnCollision(a);
            }
        }
    }
}