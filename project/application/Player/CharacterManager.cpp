#include "CharacterManager.h"
#include <Player.h>
#include <Enemy.h>

CharacterManager* CharacterManager::instance = nullptr;

CharacterManager* CharacterManager::GetInstance() {
    if (!instance) {
        instance = new CharacterManager();
    }
    return instance;
}

void CharacterManager::Clear() {
    // プレイヤーの解放（nullptrにすることで unique_ptr が delete）
    player_.reset();
    // 敵キャラクターリストの解放（中身を全て delete）
    enemies_.clear();
}

void CharacterManager::Initialize() {
    if (player_) player_->Initialize();
    for (auto& enemy : enemies_) {
        enemy->Initialize();
    }
}

void CharacterManager::Update() {
    if (player_) player_->Update();
    for (auto& enemy : enemies_) {
        enemy->Update();
    }

    // 死んだ敵の削除（IsActive() == false）
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& e) {
                return !e->IsActive();
            }),
        enemies_.end());
}

void CharacterManager::Draw() {
    if (player_) player_->Draw();
    for (auto& enemy : enemies_) {
        enemy->Draw();
    }
}


Player* CharacterManager::GetPlayer() {
    return player_.get();
}

std::vector<Enemy*> CharacterManager::GetEnemies() const {
    std::vector<Enemy*> result;
    for (const auto& e : enemies_) {
        result.push_back(e.get());
    }
    return result;
}

void CharacterManager::SetPlayer(std::unique_ptr<Player> player) {
    player_ = std::move(player);
}

void CharacterManager::AddEnemy(std::unique_ptr<Enemy> enemy) {
    enemies_.push_back(std::move(enemy));
}