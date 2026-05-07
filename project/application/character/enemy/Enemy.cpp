#include "Enemy.h"
#include <ModelManager.h>
#include <EnemyState.h>
#include <CameraManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>
#include <CollisionManager.h>
#include <CollisionConfig.h>

using namespace MyEngine;
using namespace MyGame;
using namespace AssetGen::LoadResourceID::Models;
using namespace CollisionConfig;

namespace MyGame {

    Enemy::~Enemy() {}

    void Enemy::Finalize() {
        CollisionManager::GetInstance()->UnregisterCollider(collider_.get());
        collider_.reset();
        object_.reset();
    }

    void Enemy::Initialize() {
        ModelManager::GetInstance()->LoadModel(Character::Enemy);

        object_ = Object3d::Create(Character::Enemy, Transform{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        // コライダーの生成
        collider_ = std::make_unique<OBBCollider>(object_.get());
        collider_->SetOwner(this); // コライダーの所有者をこのBlockオブジェクトに設定
        // 衝突属性とマスクの設定
        collider_->SetCollisionAttribute(Attribute::Enemy); // このオブジェクトは「Enemy」属性を持つ
        collider_->SetCollisionMask(Attribute::PlayerBullet);    // 「PlayerBullet」属性を持つオブジェクトと衝突するように設定
        // 設定したコライダーを衝突管理クラスに登録
        CollisionManager::GetInstance()->RegisterCollider(collider_.get());

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
        if (other->GetCollisionAttribute() == Attribute::PlayerBullet) {
            // 死亡ステートへ遷移
            ChangeState(std::make_unique<EnemyDead>());
        }
    }
}