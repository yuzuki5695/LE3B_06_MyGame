#include "EnemyBullet.h"
#include <ModelManager.h>
#include <CollisionConfig.h>
#include <LineRenderer.h>
// AssetGeneratorからインクルード
#include <subproject/AssetGenerator/engine/generator/LoadResourceID.h>

using namespace MyEngine;
using namespace AssetGen::LoadResourceID::Models;

namespace MyGame {

    using namespace CollisionConfig;

    void EnemyBullet::Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) {
        // 基底の初期化
        BaseBullet::Initialize(transform, velocity);

        // オブジェクト生成
        ModelManager::GetInstance()->LoadModel(Bullet::EnemyBullet);
        bullet = Object3d::Create(Bullet::EnemyBullet, transform_);
        // 当たり判定サイズ 
        colliderSize_ = bullet->GetScale();
        // コライダー生成
        collider_ = Collider::Create({ .profile = Profile::EnemyBullet,.obb = CollisionUtils::CreateOBB(bullet.get(),colliderSize_) });
        // 衝突時の処理
        collider_->SetCallback([this](Collider* other) {
#ifdef USE_IMGUI
            // Debug用
            LineRenderer::GetInstance()->SetHit(true);
#endif // USE_IMGUI
            SetInactive(); }
        );
        // コライダー登録
        CollisionManager::GetInstance()->RegisterCollider(collider_.get());
     
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

        // OBB更新
        collider_->SetOBB(CollisionUtils::CreateOBB(bullet.get(), colliderSize_));
#ifdef USE_IMGUI
        if (active_) {
            const auto& debug = LineRenderer::GetInstance()->GetDebugSettings();
            if (debug.enable && bullet && collider_) {
                LineRenderer::GetInstance()->SetHit(false);
                Vector4 hitColor = debug.isHit ? Vector4{ 1,0,0,1 } : Vector4{ 0,1,0,1 };
                LineRenderer::GetInstance()->AddOBB(collider_->GetOBB(), hitColor);
            }
        }
#endif // USE_IMGUI
    }

    void EnemyBullet::Draw() {
        // アクティブでない場合は描画しない
        if (!active_) { return; }
        bullet->Draw();
    }
}