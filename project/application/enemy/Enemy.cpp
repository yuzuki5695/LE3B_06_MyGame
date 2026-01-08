#include "Enemy.h"
#include <MatrixVector.h>
//#include<EnemyBullet.h>
#include <BulletManager.h>
#include <Player.h>
#include <algorithm> 
#include <ModelManager.h>
#include <CameraManager.h>
#include <Easing.h>

using namespace Easing;
using namespace MatrixVector;

///====================================================
/// デストラクタ
///====================================================
Enemy::~Enemy() {}

///====================================================
/// 初期化処理
///====================================================
void Enemy::Initialize() {
    ModelManager::GetInstance()->LoadModel("Enemy.obj");
    // 乱数エンジンを初期化
    std::random_device rd;// 乱数生成器
    randomEngine = std::mt19937(rd());
    // 初期座標をランダム生成
    std::uniform_real_distribution<float> distX(-12.0f, 12.0f);
    std::uniform_real_distribution<float> distY(0.0f, 1.0f);
    std::uniform_real_distribution<float> distZ(50.0f, 100.0f);
    // Transform設定
    Vector3 xyz = { distX(randomEngine), distY(randomEngine),distZ(randomEngine) };
    // 弾の発射間隔をランダム設定（2〜7秒）
    transform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },xyz };
    object = Object3d::Create("Enemy.obj", transform_);
    object->SetScale({ 1.0f, 1.0f, 1.0f }); // 安定したサイズを明示
    bulletIntervalDist_ = std::uniform_real_distribution<float>(0.2f, 3.0f); // 発射間隔を決定する分布
    bulletInterval_ = bulletIntervalDist_(randomEngine); // 最初の間隔を決定
    // 移動方向をランダムに設定
    std::uniform_int_distribution<int> typeDist(0, 2);
    int type = typeDist(randomEngine);
    moveType_ = static_cast<MoveType>(type);
    moveDirDist_ = std::uniform_real_distribution<float>(-1.5f, 1.5f);
    moveDirection_ = Vector3{ moveDirDist_(randomEngine),moveDirDist_(randomEngine),moveDirDist_(randomEngine) };
}

///====================================================
/// 更新処理
///====================================================
void Enemy::Update() {
    switch (state_) {
    case State::Spawn:
        UpdateSpawn();
        break;
    case State::Active:
        UpdateActive();
        break;
    case State::Dying:
        UpdateDying();
        break;
    case State::Dead:
        return;
    }
    

    // 位置をobjectから取得して同期する
    transform_.translate = object->GetTranslate(); // ← 追加

    // 現在の座標を同期
    object->SetTranslate(transform_.translate);
    object->Update();
}
///====================================================
/// 描画処理
///====================================================
void Enemy::Draw() {   
    object->Draw();
}
///====================================================
/// 弾の発射処理
///====================================================
void Enemy::AttachBullet(const Vector3& playerPos) {
    bulletTimer_ += 1.0f / 60.0f; // 毎フレーム経過時間を加算（60fps前提）
    // 30秒経過したら発射可能にする
    if (bulletTimer_ >= bulletInterval_) {
        canShoot_ = true;
        bulletTimer_ = 0.0f; // タイマーリセット
    }

    // 弾が撃てるか確認
    if (!canShoot_) return;

    // 弾の初期位置：敵の前方方向（敵向きに少し前進）
    Vector3 forward = Normalize(moveDirection_); // 敵の進行方向ベクトル
    Vector3 bulletStartPos = transform_.translate + forward * 1.0f;

    // プレイヤーを狙う
    Vector3 targetPos = playerPos;
    Vector3 dir = targetPos - bulletStartPos; // 弾からプレイヤー方向へのベクトル

    // カメラ情報
    GamePlayCamera* gameCam = CameraManager::GetInstance()->GetGameplayCamera();
    Vector3 shootDir;
    if (gameCam) {
        Vector3 camForward = gameCam->GetForward();
        Vector3 camRight   = gameCam->GetRight();
        Vector3 camUp      = gameCam->GetUp();

        // プレイヤー方向ベクトルをカメラ基準に変換
        float localX = Dot(dir, camRight);
        float localY = Dot(dir, camUp);
        float localZ = Dot(dir, camForward);

        Vector3 localDir = { localX, localY, localZ };

        // 必要に応じてY軸だけ補正するなら localY を調整
        // localY = 0.0f; // 水平射出の場合

        // 再びワールド座標系に変換
        shootDir = Normalize(camRight * localDir.x + camUp * localDir.y + camForward * localDir.z);
    } else {
        shootDir = Normalize(dir);
    }

     // 弾生成
    std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>();
    bullet->Initialize(
        bulletStartPos,
        bulletStartPos + shootDir * 10.0f,
        shootDir,
        0.8f
    );

    BulletManager::GetInstance()->AddEnemyBullet(std::move(bullet));
    bulletInterval_ = bulletIntervalDist_(randomEngine);
    canShoot_ = false;
}
///====================================================
/// OBB（当たり判定用）を取得
///====================================================
OBB Enemy::GetOBB() const {
    OBB obb;
    // 中心座標
    obb.center = transform_.translate;
    // ハーフサイズ（スケールの半分）
    obb.halfSize = {
        transform_.scale.x / 1.0f, 
        transform_.scale.y / 1.0f, 
        transform_.scale.z / 1.0f
    };
    // 回転行列（XYZ順で回転を合成）
    Matrix4x4 rotX = MakeRotateXMatrix(transform_.rotate.x);
    Matrix4x4 rotY = MakeRotateYMatrix(transform_.rotate.y);
    Matrix4x4 rotZ = MakeRotateZMatrix(transform_.rotate.z);
    // Z→X→Y の順に掛けることで、XYZ回転（ローカル空間回転）を再現
    Matrix4x4 rotMat = Multiply(Multiply(rotZ, rotX), rotY);
    // ローカル軸（X, Y, Z）をワールド空間へ回転適用
    obb.axis[0] = Normalize(Multiply4x4x3(rotMat, Vector3{ 1, 0, 0 })); // X軸
    obb.axis[1] = Normalize(Multiply4x4x3(rotMat, Vector3{ 0, 1, 0 })); // Y軸
    obb.axis[2] = Normalize(Multiply4x4x3(rotMat, Vector3{ 0, 0, 1 })); // Z軸
    return obb;
}

void Enemy::Spawn(const Vector3& pos, MoveType moveType) {
    // ===== 状態リセット（SetnewTranslate 由来）=====
    isDead_ = false;
    isDying_ = false;
    deathTimer_ = 0.0f;
    canShoot_ = false;
    bulletTimer_ = 0.0f;

    moveType_ = moveType;

    spawnBaseZ_ = pos.z;
    bulletInterval_ = bulletIntervalDist_(randomEngine);

    // ===== Spawn ステート開始 =====
    state_ = State::Spawn;
    spawnTimer_ = 0.0f;

    spawnTargetPos_ = pos;
    spawnStartPos_ = pos + Vector3{ 0, 0, 10.0f };

    // 出現開始状態
    transform_.translate = spawnStartPos_;
    transform_.scale = { 0.0f, 0.0f, 0.0f };
    transform_.rotate = { 0.0f, 0.0f, 0.0f };

    // Object3d 側にも即反映（1フレ遅延防止）
    object->SetTranslate(transform_.translate);
    object->SetScale(transform_.scale);
    object->SetRotate(transform_.rotate);
}

void Enemy::UpdateSpawn() {
    spawnTimer_ += 1.0f /60.0f;
    float t = std::min(spawnTimer_ / spawnDuration_, 1.0f);

    // 位置は普通に
    transform_.translate = Lerp(spawnStartPos_, spawnTargetPos_, t);

    // スケールはオーバーシュート
    float s = EaseOutBack(t);

    transform_.scale = {
        s,
        s,
        s
    };
    object->SetScale(transform_.scale);

    if (t >= 0.6) {

    }

    if (t >= 1.0f) {
        transform_.scale = { 1,1,1 }; // 念のため収束
        state_ = State::Active;
    }
}

void Enemy::OnHit() {
    // すでに死にかけ or 死亡済みなら無視
    if (state_ == State::Dying || state_ == State::Dead) {
        return;
    }

    state_ = State::Dying;
    isDying_ = true;
    deathTimer_ = 0.0f;
}
void Enemy::UpdateActive() {
    if (player_) {
        AttachBullet(player_->GetPosition());
    }
    switch (moveType_) {
    case MoveType::Vertical:
        transform_.translate.y += moveDirection_.y * moveSpeed_;
        object->SetTranslate(transform_.translate);
        break;
    case MoveType::Horizontal:
        transform_.translate.x += moveDirection_.x * moveSpeed_;
        object->SetTranslate(transform_.translate);
        break;
    case MoveType::Diagonal: {
        moveDirection_ = Normalize(moveDirection_); // 斜めの場合は正規化
        transform_.translate.x += moveDirection_.x * moveSpeed_;
        transform_.translate.y += moveDirection_.y * moveSpeed_;
        object->SetTranslate(transform_.translate);
        break;
    case MoveType::None:
        break;
    }
    }
}
void Enemy::UpdateDying() {
    // 死亡判定中（スケール縮小アニメーション）
    if (isDying_) {
        deathTimer_ += 1.0f / 120.0f; // 60FPS前提
        float t = (std::min)(deathTimer_ / deathDuration_, 1.0f);
        Vector3 scale = { 1.0f - t,1.0f - t ,1.0f - t };

        object->SetScale(scale);


        // ランダム回転（毎フレーム少しずつ変化）
        std::uniform_real_distribution<float> rotDist(-5.0f, 5.0f); // 1フレームあたりの回転量（度）
        Vector3 currentRot = object->GetRotate(); // Object3d側にGetRotateが必要
        currentRot.x += rotDist(randomEngine);
        currentRot.y += rotDist(randomEngine);
        currentRot.z += rotDist(randomEngine);
        object->SetRotate(currentRot);

        // ランダム移動（吹っ飛ぶ演出）
        std::uniform_real_distribution<float> moveDist(-0.2f, 0.2f); // x,y方向
        std::uniform_real_distribution<float> moveZDist(0.1f, 0.3f);  // z方向は必ず前方へ
        Vector3 currentPos = object->GetTranslate();
        currentPos.x += moveDist(randomEngine);
        currentPos.y += moveDist(randomEngine);
        currentPos.z += moveZDist(randomEngine); // zは正方向
        object->SetTranslate(currentPos);
         
        // パーティクルを早めに発動
        if (!hasTriggeredParticle_ && t >= 0.5f) { // 30%経過したら発動
            hasTriggeredParticle_ = true;
            // 死亡時コールバックを呼ぶ
            if (onDeathCallback) {
                onDeathCallback(currentPos);
            }
        }
        if (t >= 1.0f) {
            isDead_ = true; // スケールが0になったので削除許可
            SetActive(false);
        }
    }
}