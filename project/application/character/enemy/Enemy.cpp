#include "Enemy.h"
#include <ModelManager.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <CameraManager.h>

using namespace LoadResourceID;

Enemy::~Enemy() {}

void Enemy::Initialize() {
    ModelManager::GetInstance()->LoadModel(model::Enemy);
    transform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },{0.0f,10.0f,300.0f} };
    object = Object3d::Create(model::Enemy, transform_);

    moveType_ = EnemyMoveType::Stationary;
}

void Enemy::Update() {
    // 1. 移動：レール同期タイプなら座標を更新
    if (moveType_ == EnemyMoveType::RailFollow) {
        SyncWithRail();
    }
    
    if (moveType_ == EnemyMoveType::RailFollow) {
        SyncWithPlayerFront();
    }

    // 2. 向き：プレイヤーの方を向く
    LookAtPlayer();

    // 3. 描画オブジェクトへの反映
    object->SetTranslate(transform_.translate);
    object->SetRotate(transform_.rotate);
    object->SetScale(transform_.scale);
    object->Update();
}

void Enemy::Draw() {
	object->Draw();
}

void Enemy::LookAtPlayer() {
    if (!player_) return;

    // 敵からプレイヤーへのベクトル
    Vector3 toPlayer = player_->GetPosition() - transform_.translate;
    
    // Y軸周りの回転（Yaw）を計算
    transform_.rotate.y = atan2f(toPlayer.x, toPlayer.z);

    // 上下方向（Pitch）も合わせる場合
    float distanceXZ = sqrtf(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
    transform_.rotate.x = -atan2f(toPlayer.y, distanceXZ);
}

void Enemy::SyncWithRail() {
    if (moveType_ != EnemyMoveType::RailFollow) return;

    CameraManager* camMgr = CameraManager::GetInstance();
    GamePlayCamera* gameCam = camMgr->GetGameplayCamera();

    // カメラの現在のレール上の姿勢を取得
    Vector3 camPos = gameCam->GetBezierPos();
    Vector3 forward = gameCam->GetForward();
    Vector3 right = gameCam->GetRight();
    Vector3 up = gameCam->GetUp();

    // カメラ座標系におけるオフセット位置をワールド座標に変換
    // 例：カメラの右に10、上に5、前方に40の位置に固定する場合
    Vector3 worldOffset = (right * railOffset_.x) + (up * railOffset_.y) + (forward * railOffset_.z);

    transform_.translate = camPos + worldOffset;
}

void Enemy::SyncWithPlayerFront() {
    if (!player_) return;

    // プレイヤーの情報取得
    Vector3 playerPos = player_->GetPosition();
    Vector3 forward = player_->GetForward(); // ← これ必要

    float distance = 30.0f; // 前にどれだけ出すか

    // プレイヤーの前方に配置
    transform_.translate = playerPos + forward * distance;

    // 向きはプレイヤーと同じ
    transform_.rotate = player_->GetPlayerObject()->GetRotate();
}