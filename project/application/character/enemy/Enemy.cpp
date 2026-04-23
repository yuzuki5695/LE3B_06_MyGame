#include "Enemy.h"
#include <ModelManager.h>
#include <EnemyState.h>
#include <CameraManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>
#include <CollisionManager.h>
#include <EnemyCollider.h>

using namespace MyEngine;
using namespace MyGame;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {

    Enemy::~Enemy() {}

    void Enemy::Finalize() {
        if (collision_.GetCollider()) {
            CollisionManager::GetInstance()->RemoveCollider(collision_.GetCollider());
        }
        object_.reset();
    }

    void Enemy::Initialize() {
        ModelManager::GetInstance()->LoadModel(Character::Enemy);

        object_ = Object3d::Create(Character::Enemy, Transform{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        auto collider = std::make_unique<EnemyCollider>(this, object_.get());
        collider->SetCollisionAttribute(CollisionConfig::Enemy);
        collider->SetCollisionMask(CollisionConfig::PlayerBullet);
        collision_.SetCollider(std::move(collider));
        CollisionManager::GetInstance()->AddCollider(collision_.GetCollider());

        //attack_ = std::make_unique<EnemyAttack>();
        //attack_->SetOwner(this);
        //attack_->Initialize();
        // 初期ステートをセットする
        ChangeState(std::make_unique<EnemyIdle>());
    }

    void Enemy::Update() {
		// アクティブ状態になるまでステートを更新しない
        if (isActive_) {
            // ステートの更新（現在のステートのUpdateが呼ばれる）
            state_.Update(*this);
        }

        object_->Update();
    }

    void Enemy::Draw() {
        if (!isAlive_) { return; }

        if (object_) {
            object_->Draw();
        }
    }

    void Enemy::OnCollision(Collider* other) {
        if (!isAlive_) return;
        if (other->GetCollisionAttribute() == CollisionConfig::PlayerBullet) {
            // 死亡ステートへ遷移
            ChangeState(std::make_unique<EnemyDead>());
        }
    }
}