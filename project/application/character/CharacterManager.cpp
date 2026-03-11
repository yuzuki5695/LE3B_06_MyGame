#include "CharacterManager.h"

// シングルトン用インスタンス
std::unique_ptr<CharacterManager> CharacterManager::instance = nullptr;

CharacterManager* CharacterManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<CharacterManager>();
    }
    return instance.get();
}

void CharacterManager::Finalize() {
    instance.reset();  // `delete` 不要
}

void CharacterManager::Initialize() {
    for (std::unique_ptr<BaseCharacter>& Character : characters_) {
        Character->Initialize();
    }
}

void CharacterManager::Update() {
    for (std::unique_ptr<BaseCharacter>& Character : characters_) {
        Character->Update();
    }
}

void CharacterManager::Draw() {
    for (std::unique_ptr<BaseCharacter>& Character : characters_) {
        Character->Draw();
    }
}

void CharacterManager::AddCharacter(std::unique_ptr<BaseCharacter> character) {
    characters_.push_back(std::move(character));
}
