#include "Enemy.h"
#include <ModelManager.h>
#include <EnemyState.h>
#include <CameraManager.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>
#include <CollisionManager.h>
#include <CollisionConfig.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {
    
    using namespace CollisionConfig;

    Enemy::~Enemy() {}

    void Enemy::Finalize() {
        object_.reset();
    }

    void Enemy::Initialize() {
        ModelManager::GetInstance()->LoadModel(Character::Enemy);

        object_ = Object3d::Create(Character::Enemy, Transform{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        // コライダーの設定
        SetCollision(Attribute::Enemy, Mask::kEnemy);
        // マネージャーに登録
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
        
		// コライダーのOBBをObject3dの情報から更新
        if (collider_) {
            collider_->SetOBB(CollisionUtils::CreateOBB(object_.get()));
        }
    }

    void Enemy::Draw() {
        if (!isAlive_) { return; }

        if (object_) {
            object_->Draw();
        }
    }

    void Enemy::OnCollision(Collider* other) {
        if (isAlive_) {
            ChangeState(std::make_unique<EnemyDead>());
            // 判定リストから自分を外す
            CollisionManager::GetInstance()->UnregisterCollider(collider_.get());
            // そのあとで自身を破棄・フラグ折る
            this->isAlive_ = false;
        }
    }
}