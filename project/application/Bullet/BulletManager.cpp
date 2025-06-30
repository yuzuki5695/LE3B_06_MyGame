#include "BulletManager.h"

BulletManager* BulletManager::instance = nullptr;

BulletManager* BulletManager::GetInstance() {
	if (instance == nullptr) {
		instance = new BulletManager;
	}
	return instance;
}

void BulletManager::Finalize() {
	delete instance;
	instance = nullptr;
    playerBullets_.clear();
    enemyBullets_.clear();
}

void BulletManager::Initialize() {}

void BulletManager::Update() {
    // プレイヤー弾の更新
    for (auto& bullet : playerBullets_) {
        if (bullet->IsActive()) {
            bullet->Update();
        }
    }

    // 敵弾の更新
    for (auto& bullet : enemyBullets_) {
        if (bullet->IsActive()) {
            bullet->Update();
        }
    }
}

void BulletManager::Draw() {
    for (auto& bullet : playerBullets_) {
        if (bullet->IsActive()) {
            bullet->Draw();
        }
    }

    for (auto& bullet : enemyBullets_) {
        if (bullet->IsActive()) {
            bullet->Draw();
        }
    }
}

void BulletManager::AddPlayerBullet(std::unique_ptr<PlayerBullet> bullet) {
    playerBullets_.push_back(std::move(bullet));
}

void BulletManager::AddEnemyBullet(std::unique_ptr<EnemyBullet> bullet) {
    enemyBullets_.push_back(std::move(bullet));
}

const std::vector<std::unique_ptr<PlayerBullet>>& BulletManager::GetPlayerBullets() const {
    return playerBullets_;
}

const std::vector<std::unique_ptr<EnemyBullet>>& BulletManager::GetEnemyBullets() const {
    return enemyBullets_;
}