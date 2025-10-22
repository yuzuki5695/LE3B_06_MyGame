#include "CharacterManager.h"
#include <Player.h>

// シングルトン用インスタンス
std::unique_ptr<CharacterManager> CharacterManager::instance = nullptr;

///====================================================
/// シングルトンインスタンスの取得
///====================================================
CharacterManager* CharacterManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<CharacterManager>();
    }
    return instance.get();
}

///====================================================
/// 終了処理
///====================================================
void CharacterManager::Finalize() {
    instance.reset();  // `delete` 不要
}
///====================================================
/// 初期化処理
///====================================================
void CharacterManager::Initialize() {
    for (auto& Character : characters_) Character->Initialize();
}
///====================================================
/// 更新処理
///====================================================
void CharacterManager::Update() {
    for (auto& Character : characters_) Character->Update();
}
///====================================================
/// 描画処理
///====================================================
void CharacterManager::Draw() {
    for (auto& Character : characters_) Character->Draw();
}
///====================================================
/// キャラクター追加処理
///====================================================
void CharacterManager::AddCharacter(std::unique_ptr<BaseCharacter> character) {
    // Playerかどうかを動的キャストでチェック
    if (Player* player = dynamic_cast<Player*>(character.get())) {
        player_ = player;
    }
    // キャラクターリストに登録
    characters_.push_back(std::move(character));
}
