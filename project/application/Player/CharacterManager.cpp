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

void CharacterManager::Initialize() {
    for (std::unique_ptr<Character>& character : characters_) {
        character->Initialize();
    }
}

void CharacterManager::Clear() {
    characters_.clear();
}

void CharacterManager::Update() {
    for (std::unique_ptr<Character>& character : characters_) {
        character->Update();
    }
    // 非アクティブなキャラを削除（erase-remove idiom）
    characters_.erase(
        std::remove_if(characters_.begin(), characters_.end(),
            [](const std::unique_ptr<Character>& character) {
                return !character->IsActive();
            }),
        characters_.end());
}

void CharacterManager::Draw() {
    for (std::unique_ptr<Character>& character : characters_) {
        character->Draw();
    }
}

void CharacterManager::AddCharacter(std::unique_ptr<Character> character) {
    characters_.push_back(std::move(character));
}

const std::vector<std::unique_ptr<Character>>& CharacterManager::GetCharacters() const {
    return characters_;
}

Player* CharacterManager::GetPlayer() {
    for (const auto& character : characters_) {
        // dynamic_cast を使って Player 型か確認
        if (Player* player = dynamic_cast<Player*>(character.get())) {
            return player;
        }
    }
    return nullptr; // プレイヤーがいない場合は nullptr を返す
}

std::vector<Enemy*> CharacterManager::GetEnemies() const {
    std::vector<Enemy*> enemies;
    for (const auto& character : characters_) {
        if (Enemy* enemy = dynamic_cast<Enemy*>(character.get())) {
            enemies.push_back(enemy);
        }
    }
    return enemies;
}