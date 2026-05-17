#include "Enemy.h"
#include <ModelManager.h>
#include <EnemyState.h>
#include <CameraManager.h>
#include <CollisionConfig.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {
    
    using namespace CollisionConfig;

    Enemy::~Enemy() {}

    void Enemy::Finalize() {
		CollisionManager::GetInstance()->UnregisterCollider(collider_.get()); // 当たり判定の登録解除
		collider_.reset(); // コライダーの破棄
		object_.reset();   // 3Dオブジェクトの破棄
    }

    void Enemy::Initialize() {
        // モデルの読み込み
        ModelManager::GetInstance()->LoadModel(Character::Enemy);
        // オブジェクトの生成、初期化
        object_ = Object3d::Create(Character::Enemy, Transform{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        // 状態フラグの初期化
        flags_.isAlive = true;
		flags_.isActive = false;
        // 当たり判定の生成、初期化
        collider_ = Collider::Create({ .profile = Profile::Enemy,.obb = CollisionUtils::CreateOBB(object_.get()) });
        // 衝突時の処理
        collider_->SetCallback([this](Collider* other) {if (!IsAlive()) { return; }ChangeState(std::make_unique<EnemyDead>()); });
        // 初期ステートをセットする
        ChangeState(std::make_unique<EnemyIdle>());
    }

    void Enemy::Update() {
        // ステートの更新（現在のステートのUpdateが呼ばれる）
        state_.Update(*this);
        // オブジェクトの更新
        object_->Update();
    }

    void Enemy::Draw() {
        if (!IsAlive()) { return; }
        // オブジェクトの描画
        object_->Draw();
    }
}