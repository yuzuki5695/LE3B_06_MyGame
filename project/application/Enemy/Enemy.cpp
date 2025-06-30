#include "Enemy.h"
#include <MatrixVector.h>
#include<EnemyBullet.h>
#include <BulletManager.h>
#include <Player.h>

using namespace MatrixVector;

Enemy::~Enemy() {}

void Enemy::Initialize() {
    // 乱数エンジンを初期化
    std::random_device rd;// 乱数生成器
    randomEngine = std::mt19937(rd());
    std::uniform_real_distribution<float> distX(-12.0f, 12.0f);
    std::uniform_real_distribution<float> distY(3.0f, 12.0f);
    std::uniform_real_distribution<float> distZ(50.0f, 230.0f);
    transform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, distX(randomEngine), distY(randomEngine), distZ(randomEngine) };
    object = Object3d::Create("uvChecker.obj", transform_);
}

void Enemy::Update() {

    // 位置をobjectから取得して同期する
    transform_.translate = object->GetTranslate(); // ← 追加
    object->SetTranslate(transform_.translate); // ← 追加

      
    if (player_) {
        Vector3 playerPos = player_->GetPosition();
        AttachBullet(playerPos);// プレイヤーの位置を狙って弾発射
    }

    object->Update();
}

void Enemy::Draw() {   
    object->Draw();
}

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
         bullet->Initialize(transform_.translate, playerPos, 0.5f);                     // 初期位置などを設定
		BulletManager::GetInstance()->AddEnemyBullet(std::move(bullet));                 // BulletManagerに追加
    }

    canShoot_ = false;
}