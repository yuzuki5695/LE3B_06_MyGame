#include "Enemy.h"
#include <ModelManager.h>
#include <EnemyState.h>
#include <CameraManager.h>
#include <CollisionConfig.h>
#include <LineRenderer.h>
#include <EditorEntityRegistry.h>
#include <EditorTypes.h>
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
        // 乱数エンジンを初期化
        std::random_device rd;
        randomEngine = std::mt19937(rd());
        std::uniform_int_distribution<int> enemyTypeDist(0, 1);
        enemyType_ = static_cast<EnemyType>(enemyTypeDist(randomEngine));
		// モデルパスの選択
        const char* modelPath = nullptr;
        switch (enemyType_) {
        case EnemyType::Shot:
            modelPath = Character::Enemy;
            break;
        case EnemyType::Tackle:
            modelPath = Character::Enemy;
            break;
        }

        // モデルの読み込み
        ModelManager::GetInstance()->LoadModel(modelPath);
        // オブジェクトの生成、初期化
        object_ = Object3d::Create(modelPath, Transform{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        
        // 敵タイプごとの色設定
        switch (enemyType_) {
        case EnemyType::Shot:
            // 通常（白）
            object_->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            break;
        case EnemyType::Tackle:
            // 突撃タイプ（赤）
            object_->SetMaterialColor({ 1.0f, 0.0f, 0.0f, 1.0f });
            break;
        }

        // 状態フラグの初期化
        flags_.isAlive = true;
        flags_.isActive = false;
        expReward_ = 10; // 倒した時の経験値
        isExpGranted_ = false; // 経験値付与済みフラグ
        isKilledByPlayer_ = false;
        isDeathStarted_ = false;
        // 当たり判定サイズ
        colliderSize_ = object_->GetScale();
        // 当たり判定の生成、初期化
        collider_ = Collider::Create({ .profile = Profile::Enemy,.obb = CollisionUtils::CreateOBB(object_.get(),colliderSize_) });
        // 衝突時の処理
        collider_->SetCallback([this](Collider* other) {
#ifdef USE_IMGUI
            LineRenderer::GetInstance()->SetHit(true);
#endif // USE_IMGUI
            // 非アクティブなら無視
            if (!IsActive()) {
                return;
            }

            // すでに死んでいるなら無視
            if (!IsAlive()) {
                return;
            }

            // プレイヤー弾に当たった場合
            if (other->GetAttribute() & CollisionConfig::Attribute::PlayerBullet) {
                SetKilledByPlayer(true);
            }

            ChangeState(std::make_unique<EnemyDead>());
            });
        // コンポーネントの生成
        attack_ = std::make_unique<EnemyAttack>(); // 攻撃ロジックの生成
        attack_->Initialize();                     // 攻撃ロジックの初期化
        death_ = std::make_unique< EnemyDeath>(); // 死亡演出の生成
        // 初期ステートをセットする
        ChangeState(std::make_unique<EnemyIdle>());
    }

    void Enemy::Update() {
        // ステートの更新（現在のステートのUpdateが呼ばれる）
        state_.Update(*this);
        // オブジェクトの更新
        object_->Update();
#ifdef USE_IMGUI
        if (IsActive()) {
            const auto& debug = LineRenderer::GetInstance()->GetDebugSettings();
            if (debug.enable && object_) {     
                Vector4 hitColor = debug.isHit ? Vector4{ 1,0,0,1 } : Vector4{ 0,1,0,1 };
                LineRenderer::GetInstance()->AddOBB(collider_->GetOBB(), hitColor);
            }
        }
#endif // USE_IMGUI
    }

    void Enemy::Draw() {
        if (!IsAlive()) { return; }
        // オブジェクトの描画
        object_->Draw();
    }
}