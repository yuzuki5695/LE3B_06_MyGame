#include "CharacterManager.h"
#include <Player.h>

CharacterManager* CharacterManager::instance = nullptr;

CharacterManager* CharacterManager::GetInstance() {
    if (!instance) {
        instance = new CharacterManager();
    }
    return instance;
}

void CharacterManager::Initialize() {
    characters_.clear();
}

void CharacterManager::InitializeAllCharacters() {
    for (std::unique_ptr<Character>& character : characters_) {
        character->Initialize();
    }
}

void CharacterManager::Update() {
    for (std::unique_ptr<Character>& character : characters_) {
        character->Update();
    }
}

void CharacterManager::Draw() {
    for (std::unique_ptr<Character>& character : characters_) {
        character->Draw();
    }
}

void CharacterManager::AddCharacter(std::unique_ptr<Character> character) {
    characters_.push_back(std::move(character));
}

void CharacterManager::Clear() {
    characters_.clear();
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