#include "Enemy.h"
#include <ModelManager.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <CameraManager.h>
#include <MatrixVector.h>
#include <EnemyDataLoader.h>
#include <BulletManager.h>
#include <Easing.h>
#include <algorithm> 

using namespace LoadResourceID;
using namespace MatrixVector;
using namespace Easing;

Enemy::~Enemy() {}

void Enemy::Initialize() {
    ModelManager::GetInstance()->LoadModel(model::Enemy);
    data_ = EnemyDataLoader::Load("enemy");
    transform_ = data_.transform;
    object = Object3d::Create(model::Enemy, transform_);
    // moveType_ = EnemyMoveType::Stationary;

    weapon_ = std::make_unique<EnemyWeapon>();
    weapon_->Initialize(1.0f); // 1秒間隔

    dashVelocity_ = { 0.01f }; // 突撃速度(dashSpeed)
}

void Enemy::Update() {

    //// 移動：レール同期タイプなら座標を更新
    //if (moveType_ == EnemyMoveType::RailFollow) {
    //    // SyncWithRail();
    //}

    weapon_->Update(transform_.translate, player_->GetWorldPosition());
    // UpdateDash();


    spawnTimer_ += 1.0f / 60.0f;

    // 0〜1に正規化
    float t = std::clamp(spawnTimer_ / spawnDuration_, 0.0f, 1.0f);

    // ===== スケール（オーバーシュート）=====
    float s = EaseOutBack(t);

    transform_.scale = { s,s,s };

    // ===== 状態遷移 =====
    if (t >= 1.0f) {
        transform_.scale = { 1.0f, 1.0f, 1.0f };
    }

    object->SetTranslate(transform_.translate);
    object->SetScale(transform_.scale);
    object->Update();
}


void Enemy::Draw() {
	object->Draw();
}

void Enemy::SyncWithRail() {
    //if (moveType_ != EnemyMoveType::RailFollow) return;

    //CameraManager* camMgr = CameraManager::GetInstance();
    //GamePlayCamera* gameCam = camMgr->GetGameplayCamera();

    //// カメラの現在のレール上の姿勢を取得
    //Vector3 camPos = gameCam->GetBezierPos();
    //Vector3 forward = gameCam->GetForward();
    //Vector3 right = gameCam->GetRight();
    //Vector3 up = gameCam->GetUp();

    //// カメラ座標系におけるオフセット位置をワールド座標に変換
    //// 例：カメラの右に10、上に5、前方に40の位置に固定する場合
    //Vector3 worldOffset = (right * railOffset_.x) + (up * railOffset_.y) + (forward * railOffset_.z);

    //transform_.translate = camPos + worldOffset;
}

// 突撃中
void Enemy::UpdateDash() {
    if (player_ && das == 0) {
        Vector3 finalDir = Normalize(player_->GetPosition() - transform_.translate);
        dashVelocity_ = finalDir * 1.8f; // 突撃速度(dashSpeed)
        das = 1;
    }
    
    // 溜め終了時に固定したベクトルで移動（ロックオフ状態）
    transform_.translate += dashVelocity_;

    // 突撃中の回転演出
    transform_.rotate.z += dashRotationSpeed_ * 2.0f;

    object->SetTranslate(transform_.translate);
    object->SetRotate(transform_.rotate);

    if (transform_.translate.z < -20.0f) {
    }

    // --- 削除判定（演出スキップ） ---
    // プレイヤーの背後、または画面外（Z軸）に到達したら即削除
    float limitZ = -20.0f; // カメラの後方など、適切な基準値を設定
    if (player_) {
        limitZ = player_->GetPosition().z - 20.0f;
    }

}