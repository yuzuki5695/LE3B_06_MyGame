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
        CollisionManager::GetInstance()->UnregisterCollider(collider_.get());
    }

    void PlayerBullet::Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) {
        // 基底の初期化
        BaseBullet::Initialize(transform, velocity);

        // オブジェクト生成
        ModelManager::GetInstance()->LoadModel(Bullet::PlayerBullet);
        bullet = Object3d::Create(Bullet::PlayerBullet, transform_);

        // Collider生成 
        collider_ = Collider::Create({
            .profile = Profile::PlayerBullet,
            .obb = CollisionUtils::CreateOBB(bullet.get()),
            .callback = [this](Collider* other) {SetInactive(); } });

        CollisionManager::GetInstance()->RegisterCollider(collider_.get());

        //// コライダーの設定
        //SetCollision(Attribute::PlayerBullet, Mask::kPlayerBullet);
        //// マネージャーに登録
        //CollisionManager::GetInstance()->RegisterCollider(collider_.get());

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
        //// コライダーのOBBをObject3dの情報から更新
        //if (collider_) {
        //    collider_->SetOBB(CollisionUtils::CreateOBB(bullet.get()));
        //}

        // =============================
        // ③ 寿命処理
        // =============================
        UpdateLifeTime(1.0f / 60.0f); // 仮で60FPS固定
        // OBB更新
        collider_->SetOBB(CollisionUtils::CreateOBB(bullet.get(), { 0.5f,0.5f,0.5f }));
    }

    void PlayerBullet::Draw() {
        if (!active_) {
            return;
        }
        if (bullet) {
            bullet->Draw();
        }
    }
    
   // void PlayerBullet::OnCollision(Collider* other) {
        //if (active_) {
        //    SetInactive();
        //}
    //}
}