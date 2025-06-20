#include "Enemy.h"
#include <MatrixVector.h>

using namespace MatrixVector;

Enemy::~Enemy() {
    //for (auto b : bullets_) {
    //    delete b;
    //}
    //bullets_.clear();
}

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
    //position_ = object->GetWorldPosition(); // ← 追加
	object->SetTranslate(position_); // ← 追加
    object->Update();
      
    if (target_) {
        Vector3 playerPos = target_->GetTransform().translate;
        attachBullet(playerPos); // プレイヤーの位置を使って弾発射
    }
}

void Enemy::Draw() {   
    object->Draw();
}

void Enemy::attachBullet(const Vector3& playerPos) {
    //bulletTimer_ += 1.0f / 60.0f; // 毎フレーム経過時間を加算（60fps前提）
    //// 30秒経過したら発射可能にする
    //if (bulletTimer_ >= bulletInterval_) {
    //    canShoot_ = true;
    //    bulletTimer_ = 0.0f; // タイマーリセット
    //}

    //// 弾が撃てるか確認
    //if (!canShoot_) return;
    //
    //if (canShoot_) {
    //    Bullet* newBullet = new Bullet();
    //    // 発射位置：敵の現在位置
    //    Vector3 startPos = position_;

    //    // プレイヤーに向かって撃つ
    //    Vector3 direction = Normalize(playerPos - startPos);

    //    newBullet->Initialize(startPos, direction, 0.5f);
    //    bullets_.push_back(newBullet);
    //}

    //canShoot_ = false;
}

void Enemy::SetTarget(Character* target) {
    target_ = target;
}