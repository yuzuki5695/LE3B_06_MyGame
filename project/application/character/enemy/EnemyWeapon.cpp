#include "EnemyWeapon.h"
#include <EnemyBullet.h>
#include <BulletManager.h>
#include <cmath>
#include <MatrixVector.h>
#include <CameraManager.h>

using namespace MatrixVector;

void EnemyWeapon::Initialize(float interval) {
    bulletInterval_ = interval;
}

void EnemyWeapon::Update(const Vector3& enemyPos, const Vector3& playerPos) {
    bulletTimer_ += 1.0f / 60.0f;
    if (bulletTimer_ < bulletInterval_) { return; }
    bulletTimer_ = 0.0f;

    // 1. プレイヤーの「真のワールド座標」を取得
    // 既存の playerPos 引数が Player::GetPosition() (data_.transform.translate) を
    // 指している場合、SyncWorldTransformByRail で計算された最終座標になっているか確認が必要です。
    Vector3 targetPos = playerPos; 

    // 2. 弾の生成
    auto bullet = std::make_unique<EnemyBullet>();

    // 【極端な例】
    // 第2引数(target)に playerPos を直接渡します。
    // EnemyBullet::Initialize の内部で「targetに向かって一定速度で進む」
    // もしくは「targetに吸い付く」処理になっていればこれで届きます。
    bullet->Initialize(
        enemyPos,    // 発射元
        targetPos,   // 最終目的地（プレイヤーの座標）
        {0,0,0},     // 一旦方向ベクトルは使わない（内部で計算させるため）
        1.0f         // スピード（適当な値）
    );

    BulletManager::GetInstance()->AddEnemyBullet(std::move(bullet));
}

