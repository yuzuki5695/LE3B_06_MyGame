#include "Enemy.h"
#include <MatrixVector.h>
//#include<EnemyBullet.h>
#include <BulletManager.h>
#include <Player.h>
#include <algorithm> 
#include <ModelManager.h>
#include <CameraManager.h>

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
    // LevelLoader のインスタンスを生成
    loader_ = new CharacterLoader();
    levelData_ = loader_->LoadFile("enemy");  // ←ここでロード

    // --- レベルデータ内の全オブジェクトをEnemyとして生成 ---
    for (auto& objData : levelData_->objects) {
        // 各オブジェクトのTransform設定
        Transform tr;
        tr.scale = { 1.0f, 1.0f, 1.0f };
        tr.rotate = objData.rotation;
        tr.translate = objData.translation;

        // Enemyを生成（1体ごとに独立したインスタンス）
        std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();

        // Enemy内部のObject3dを設定
        enemy->object = Object3d::Create("Enemy.obj", tr);
        enemy->transform_ = tr;

        // --- 乱数設定など各個体用の初期化 ---
        enemy->randomEngine.seed(std::random_device{}());
        enemy->bulletIntervalDist_ = std::uniform_real_distribution<float>(2.0f, 7.0f);
        enemy->bulletInterval_ = enemy->bulletIntervalDist_(enemy->randomEngine);
        enemy->moveDirDist_ = std::uniform_real_distribution<float>(5.9f, 6.0f);
        enemy->moveDirection_ = Normalize(Vector3{
            enemy->moveDirDist_(enemy->randomEngine),
            enemy->moveDirDist_(enemy->randomEngine),
            enemy->moveDirDist_(enemy->randomEngine)
            });

        // moveTypeはランダムに決定（必要に応じて固定可）
        std::uniform_int_distribution<int> typeDist(0, 2);
        int type = typeDist(enemy->randomEngine);
        enemy->moveType_ = static_cast<MoveType>(type);
        // 敵リストに登録
        enemies_.push_back(std::move(enemy->object));
    }

}
/// <summary> 
///  敵の再初期化
/// </summary>
void Enemy::SetInitialize(float baseZ, MoveType moveType) {
    // 状態をリセットする！
    isDead_ = false;
    isDying_ = false;
    deathTimer_ = 0.0f;
    moveType_ = moveType;

    // 乱数エンジンを初期化
    std::random_device rd;// 乱数生成器
    randomEngine = std::mt19937(rd());
    std::uniform_real_distribution<float> distX(-13.0f, 13.0f);
    std::uniform_real_distribution<float> distY(-2.0f, 8.0f);
    std::uniform_real_distribution<float> distZ(50.0f, 100.0f);
    Vector3 xyz = { distX(randomEngine), distY(randomEngine), baseZ + distZ(randomEngine)};
    
    spawnBaseZ_ = xyz.z;  // 出現元Zを記録 
    bulletInterval_ = bulletIntervalDist_(randomEngine);
    object->SetTranslate(xyz);
}
///====================================================
/// 敵の新規出現（座標を直接指定）
///====================================================
void Enemy::SetnewTranslate(const Vector3& pos, MoveType moveType) {
    // isDead, isDying 状態をリセット
    isDead_ = false;
    isDying_ = false;
    deathTimer_ = 0.0f;
    moveType_ = moveType;
    // 出現位置をそのまま使う
    spawnBaseZ_ = pos.z;
    // 弾発射間隔などは既存の処理を流用
    bulletInterval_ = bulletIntervalDist_(randomEngine);
    // オブジェクトを指定位置に配置
    object->SetTranslate(pos);
}
///====================================================
/// 更新処理
///====================================================
void Enemy::Update() {
    for (auto& enemy : enemies_) {
        if (IsActive()) {
            enemy->Update();
        }
    }

    // 死亡判定中（スケール縮小アニメーション）
    if (isDying_) {
        deathTimer_ += 1.0f / 120.0f; // 60FPS前提
        float t = (std::min)(deathTimer_ / deathDuration_, 1.0f);
        Vector3 scale = { 1.0f - t,1.0f - t ,1.0f - t };

        object->SetScale(scale);
        if (t >= 1.0f) {
            isDead_ = true; // スケールが0になったので削除許可
        }
    }
    // --- 距離維持モードの場合 ---
    if (followPlayer_) {
        GameCamera* gameCam = CameraManager::GetInstance()->GetGameCamera();
        if (!gameCam) return;

        Vector3 playerPos = gameCam->GetbezierPos();
        Vector3 cameraForward = gameCam->GetForward();
        Vector3 cameraRight = Normalize(Cross({ 0,1,0 }, cameraForward));
        Vector3 cameraUp = Normalize(Cross(cameraForward, cameraRight));

        // 前方方向に指定距離だけ進んだ位置
        Vector3 basePos = playerPos + cameraForward * followDistance_;

        // 位置をobjectから取得して同期する
        transform_.translate = basePos; // ← 追加
    }

    if (!isDying_) {
        switch (moveType_) {
        case MoveType::None:
            // 動かない
            break;
        case MoveType::Vertical:
            transform_.translate.y += moveDirection_.y * moveSpeed_;
            break;
        case MoveType::Horizontal:
            transform_.translate.x += moveDirection_.x * moveSpeed_;
            break;
        }
    }
    // 現在の座標を同期
    object->SetTranslate(transform_.translate);

    // 弾発射処理（プレイヤーが存在する場合）
    if (player_ && !isDying_) {
        Vector3 playerPos = player_->GetPosition();
        AttachBullet(playerPos);// プレイヤーの位置を狙って弾発射
    }
    object->Update();

    // 死んだ敵の削除
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& e) {
                return e->IsDead();  // ← 出現前の非アクティブは残す！
            }),
        enemies_.end());
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
    
    if (canShoot_) {
        std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>();		// 弾を生成
        // 次の発射間隔をランダムに再設定
        bulletInterval_ = bulletIntervalDist_(randomEngine);
        bullet->Initialize(transform_.translate, playerPos, 0.8f);                     // 初期位置などを設定
        BulletManager::GetInstance()->AddEnemyBullet(std::move(bullet));                 // BulletManagerに追加
    }
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
        transform_.scale.x / 2.0f, 
        transform_.scale.y / 2.0f, 
        transform_.scale.z / 2.0f
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

void Enemy::SetFollowPlayer(float distance, MoveType moveType) {
    followPlayer_ = true;
    followDistance_ = distance;
    moveType_ = moveType;

    // X/Y方向のオフセットをランダムに決めても良い
    std::uniform_real_distribution<float> distX(-10.0f, 10.0f);
    std::uniform_real_distribution<float> distY(-3.0f, 5.0f);
    localOffset_ = { distX(randomEngine), distY(randomEngine), 0.0f };
}