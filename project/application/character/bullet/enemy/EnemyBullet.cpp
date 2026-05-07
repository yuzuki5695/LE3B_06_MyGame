#include "EnemyBullet.h"
#include <ModelManager.h>
#include <CollisionManager.h>
#include <CollisionConfig.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {

    using namespace CollisionConfig;

    void EnemyBullet::Finalize() {

    }

    void EnemyBullet::Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) {
        // 基底の初期化
        BaseBullet::Initialize(transform, velocity);

        // オブジェクト生成
        ModelManager::GetInstance()->LoadModel(Bullet::EnemyBullet);
        bullet = Object3d::Create(Bullet::EnemyBullet, transform_);

        // 初期位置設定
        bullet->SetTranslate(transform_.translate);
    }

    void EnemyBullet::Update() {
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

    void EnemyBullet::Draw() {
        if (bullet) {
            bullet->Draw();
        }
    }

   // void EnemyBullet::OnCollision(Collider* other) {
        //if (other->GetCollisionAttribute() == CollisionConfig::Enemy) {
        //    SetInactive();
        //}
   // }
}