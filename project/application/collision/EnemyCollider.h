#pragma once
#include <OBBCollider.h>
#include <Enemy.h>
#include <CollisionConfig.h>

namespace MyGame {
    class EnemyCollider : public OBBCollider {
    public:
        // 親のEnemyへのポインタを保持
        EnemyCollider(Enemy* enemy, MyEngine::Object3d* object) 
            : OBBCollider(object), enemy_(enemy) {}

        // 衝突時に呼ばれる
        void OnCollision(Collider* other) override {
            // 相手がプレイヤーの弾(PlayerBullet)かチェック
            if (other->GetCollisionAttribute() == CollisionConfig::PlayerBullet) {
                // Enemy本体に通知し、ステートを変える
                enemy_->OnCollision(other);
            }
        }
    private:
        Enemy* enemy_;
    };
}