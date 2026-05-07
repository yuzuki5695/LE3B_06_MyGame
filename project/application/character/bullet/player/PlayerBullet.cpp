#include "PlayerBullet.h"
#include <ModelManager.h>
#include <CollisionManager.h>
#include <CollisionConfig.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {

    using namespace CollisionConfig;

    void PlayerBullet::Finalize() {
        if (collider_) {
            CollisionManager::GetInstance()->UnregisterCollider(collider_.get());
        }
        collider_.reset();
    }

    void PlayerBullet::Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) {
        // 基底の初期化
        BaseBullet::Initialize(transform, velocity);
 
        // オブジェクト生成
        ModelManager::GetInstance()->LoadModel(Bullet::PlayerBullet);
        bullet = Object3d::Create(Bullet::PlayerBullet, transform_);

        // コライダーの生成
        collider_ = std::make_unique<OBBCollider>(bullet.get());
        collider_->SetOwner(this); // コライダーの所有者をこのBlockオブジェクトに設定
        // 衝突属性とマスクの設定
        collider_->SetCollisionAttribute(Attribute::PlayerBullet); // このオブジェクトは「PlayerBullet」属性を持つ
        collider_->SetCollisionMask(Attribute::Enemy);    // 「Enemy」属性を持つオブジェクトと衝突するように設定
        // 設定したコライダーを衝突管理クラスに登録 
        CollisionManager::GetInstance()->RegisterCollider(collider_.get());

        // 初期位置設定
        bullet->SetTranslate(transform_.translate);
    }

    void PlayerBullet::Update() {
        // =============================
        // ① 移動処理
        // =============================
        transform_.translate += velocity_;

        // =============================
        // ② Objectに反映
        // =============================
        if (bullet) {
            bullet->SetTranslate(transform_.translate);
            bullet->Update();
        }
        // =============================
        // ③ 寿命処理
        // =============================
        UpdateLifeTime(1.0f / 60.0f); // 仮で60FPS固定
    }

    void PlayerBullet::Draw() {
        if (!active_) {
            return;
        }
        if (bullet) {
            bullet->Draw();
        }
    }

    void PlayerBullet::OnCollision(Collider* other) {
        if (other->GetCollisionAttribute() == Attribute::Enemy) {
            SetInactive();
        }
    }
}