#include "CollisionManager.h"
#include <Collision.h>

using namespace MyEngine;
using namespace MyGame;

// シングルトン用インスタンス
std::unique_ptr<CollisionManager> CollisionManager::instance_ = nullptr;

///====================================================
/// シングルトンインスタンスの取得
///====================================================
CollisionManager* CollisionManager::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = std::unique_ptr<CollisionManager>(new CollisionManager());
    }
    return instance_.get();
}

// 終了
void CollisionManager::Finalize() {
    instance_.reset();  // `delete` 不要
}

void CollisionManager::CheckAllCollisions() {
    // 登録されたコライダーの全ての組み合わせに対して衝突判定を行う
    for (std::list<Collider*>::iterator itrA = colliders_.begin(); itrA != colliders_.end(); ++itrA) {
        // 内側のループは、外側のループの次の要素から始めることで、同じ組み合わせを二度チェックしないようにする
        std::list<Collider*>::iterator itrB = itrA;
        ++itrB;
        for (; itrB != colliders_.end(); ++itrB) {

            Collider* a = *itrA;
            Collider* b = *itrB;

            //========================
            // マスクチェック
            //========================
            bool canCollision = (a->GetAttribute() & b->GetMask()) || (b->GetAttribute() & a->GetMask());

            if (!canCollision) {
                // 衝突可能な組み合わせでなければ次の組み合わせへ
                continue;
            }

            //========================
            // OBB判定
            //========================
            if (CollisionUtils::CheckOBB(a->GetOBB(), b->GetOBB())) {
                // 衝突があった場合、両方のコライダーに衝突を通知
                a->OnCollision(b);
                b->OnCollision(a);
            }
        }
    }
}