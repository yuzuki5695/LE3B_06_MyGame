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
    
    ///====================================================
    /// デストラクタ
    ///====================================================
    EnemyBullet::~EnemyBullet() {
        if (collider_) {
            CollisionManager::GetInstance()->UnregisterCollider(collider_.get());
        }
    }
    ///====================================================
    /// 初期化処理
    ///====================================================
    void EnemyBullet::Initialize(const MyEngine::Transform& transform, const MyEngine::Vector3& velocity) {
        // 基底の初期化
        BaseBullet::Initialize(transform, velocity);

        // オブジェクト生成
        ModelManager::GetInstance()->LoadModel(Bullet::EnemyBullet);
        bullet_ = Object3d::Create(Bullet::EnemyBullet, transform_);
        // 当たり判定サイズ 
        collidersize_ = bullet_->GetScale();
        // コライダー生成
        collider_ = Collider::Create({ .profile = Profile::EnemyBullet,.obb = CollisionUtils::CreateOBB(bullet_.get(),collidersize_) });
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
        bullet_->SetTranslate(transform_.translate);
    }
    ///====================================================
    /// 更新処理
    ///====================================================
    void EnemyBullet::Update() {
        // 移動処理      
        transform_.translate += velocity_;

        // Objectに反映
        if (bullet_) {
            bullet_->SetTranslate(transform_.translate);
            bullet_->Update();
        }

        // 寿命処理
        UpdateLifeTime(1.0f / 60.0f); // 仮で60FPS固定
        // OBB更新
        collider_->SetOBB(CollisionUtils::CreateOBB(bullet_.get(), collidersize_));
#ifdef USE_IMGUI
        // デバッグ描画 
        if (active_) {
            const auto& debug = LineRenderer::GetInstance()->GetDebugSettings();
            if (debug.enable && bullet_ && collider_) {
                LineRenderer::GetInstance()->SetHit(false);
                Vector4 hitColor = debug.isHit ? Vector4{ 1,0,0,1 } : Vector4{ 0,1,0,1 };
                LineRenderer::GetInstance()->AddOBB(collider_->GetOBB(), hitColor);
            }
        }
#endif // USE_IMGUI
    }
    ///====================================================
    /// 描画処理
    ///====================================================
    void EnemyBullet::Draw() {
        // アクティブでない場合は描画しない
        if (!active_) { return; }
        bullet_->Draw();
    }
}