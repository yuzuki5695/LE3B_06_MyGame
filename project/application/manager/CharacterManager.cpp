#include "CharacterManager.h"
#include <Player.h>

// 静的メンバ変数の定義
std::unique_ptr<CharacterManager> CharacterManager::instance = nullptr;

// シングルトンインスタンスの取得
CharacterManager* CharacterManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<CharacterManager>();
    }
    return instance.get();
}

// 終了
void CharacterManager::Finalize() {
    instance.reset();  // `delete` 不要
}

void CharacterManager::Initialize() {
    for (auto& Character : characters_) Character->Initialize();
}

void CharacterManager::Update() {
    for (auto& Character : characters_) Character->Update();
}

void CharacterManager::Draw() {
    for (auto& Character : characters_) Character->Draw();
}

void CharacterManager::AddCharacter(std::unique_ptr<BaseCharacter> character) { 
    // Playerかどうかを動的キャストでチェック
    if (Player* player = dynamic_cast<Player*>(character.get())) {
        player_ = player;
    }
    characters_.push_back(std::move(character));
}
