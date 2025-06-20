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
    bullets_.clear();
}

void BulletManager::Initialize() {}

void BulletManager::Update() {
    for (std::unique_ptr<BaseBullet>& bullet : bullets_) {
        bullet->Update();
    }
}

void BulletManager::Draw() {
    for (std::unique_ptr<BaseBullet>& bullet : bullets_) {
        bullet->Draw();
    }
}

void BulletManager::AddBullet(std::unique_ptr<BaseBullet> bullet) {
    bullets_.push_back(std::move(bullet));
}

const std::vector<std::unique_ptr<BaseBullet>>& BulletManager::GetBullets() const {
    return bullets_;
}