#include "CharacterManager.h"

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
