#include "Enemy.h"
#include <ModelManager.h>
#include <Tools/AssetGenerator/engine/math/LoadResourceID.h>
#include <CameraManager.h>
#include <MatrixVector.h>

using namespace LoadResourceID;
using namespace MatrixVector;

Enemy::~Enemy() {}

void Enemy::Initialize() {
    ModelManager::GetInstance()->LoadModel(model::Enemy);
    transform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },{-20.0f,15.0f,300.0f} };
    transform_.rotate.x = 10.0f;
    transform_.rotate.y = 10.0f;    
    transform_.rotate.z = 25.0f;
    object = Object3d::Create(model::Enemy, transform_);

    moveType_ = EnemyMoveType::Stationary;
}

void Enemy::Update() {

    // 1. 移動：レール同期タイプなら座標を更新
    if (moveType_ == EnemyMoveType::RailFollow) {
        // SyncWithRail();
    }

    object->SetTranslate(transform_.translate);
    object->SetScale(transform_.scale);
    object->Update();
}

void Enemy::Draw() {
	object->Draw();
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
