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
	position_ = { distX(randomEngine), distY(randomEngine), distZ(randomEngine) };    
    object = Object3d::Create("uvChecker.obj", Transform({ {1.0f, 1.0f, 1.0f},     {0.0f, 0.0f, 0.0f}, position_.x,position_.y,position_.z }));
}

void Enemy::Update() {

    // 位置をobjectから取得して同期する
    position_ = object->GetTranslate(); // ← 追加
	object->SetTranslate(position_); // ← 追加

      
    if (player_) {
        Vector3 playerPos = player_->GetTransform().translate;
        attachBullet(playerPos);// プレイヤーの位置を狙って弾発射
    }

    object->Update();
}

void Enemy::Draw() {   
    object->Draw();
}

void Enemy::attachBullet(const Vector3& playerPos) {
    bulletTimer_ += 1.0f / 60.0f; // 毎フレーム経過時間を加算（60fps前提）
    // 30秒経過したら発射可能にする
    if (bulletTimer_ >= bulletInterval_) {
        canShoot_ = true;
        bulletTimer_ = 0.0f; // タイマーリセット
    }

    // 弾が撃てるか確認
    if (!canShoot_) return;
    
    if (canShoot_) {
         std::unique_ptr<BaseBullet> bullet = std::make_unique<EnemyBullet>();		// 弾を生成
        bullet->Initialize(position_, playerPos, 0.5f);                     // 初期位置などを設定
		BulletManager::GetInstance()->AddBullet(std::move(bullet));                 // BulletManagerに追加
    }

    canShoot_ = false;
}

//void Enemy::SetTarget(Character* target) {
//    //target_ = target;
//}