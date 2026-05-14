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
        CollisionManager::GetInstance()->UnregisterCollider(collider_.get());
        collider_.reset();
        object_.reset();
    }

    void Enemy::Initialize() {
        ModelManager::GetInstance()->LoadModel(Character::Enemy);

        object_ = Object3d::Create(Character::Enemy, Transform{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        flags_.isAlive = true;
        flags_.isActive = false;


        collider_ = Collider::Create({ 
            .profile = Profile::Enemy,
            .obb = CollisionUtils::CreateOBB(object_.get()),
            .callback = [this](Collider* other) {if (!IsAlive()) { return; }ChangeState(std::make_unique<EnemyDead>()); } }); 

        CollisionManager::GetInstance()->RegisterCollider(collider_.get());

        //attack_ = std::make_unique<EnemyAttack>();
        //attack_->SetOwner(this);
        //attack_->Initialize();
        // 初期ステートをセットする
        ChangeState(std::make_unique<EnemyIdle>());
    }

    void Enemy::Update() {
        // ステートの更新（現在のステートのUpdateが呼ばれる）
        state_.Update(*this);

        // 毎フレーム当たり判定を更新
        collider_->SetOBB(CollisionUtils::CreateOBB(object_.get()));
        // オブジェクトの更新
        object_->Update();
    }

    void Enemy::Draw() {
        if (!IsAlive()) { return; }
        // オブジェクトの描画
        object_->Draw();
    }
}