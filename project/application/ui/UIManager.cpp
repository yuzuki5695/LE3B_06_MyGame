#include "UIManager.h"
#include <SceneManager.h>

void UIManager::Initialize() {
    CreateAndAddUI("GAMEPLAY");

    for (auto& ui : uiList_) {
        ui->Initialize();
    }
}

void UIManager::Update() {
    // 現在のシーン名を取得
    std::string currentScene = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();
    for (std::unique_ptr<BaseUI>& ui : uiList_) {
        // UIがアクティブ 且つ 所属シーンが現在のシーンと一致する場合のみ更新
        if (ui->IsActive() && ui->GetTargetSceneName() == currentScene) {
            ui->Update();
        }
    }
}

void UIManager::Draw() {
    // 現在のシーン名を取得
    std::string currentScene = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();
    for (std::unique_ptr<BaseUI>& ui : uiList_) {
        // UIがアクティブ 且つ 所属シーンが現在のシーンと一致する場合のみ描画
        if (ui->IsActive() && ui->GetTargetSceneName() == currentScene) {
            ui->Draw();
        }
    }
}

void UIManager::AddUI(const std::string& sceneName, std::unique_ptr<BaseUI> ui) {
    // 登録時にシーン名をセットしてしまう
    ui->SetTargetSceneName(sceneName);
    uiList_.push_back(std::move(ui));
}
void UIManager::Clear() {
    uiList_.clear();
}

void UIManager::CreateAndAddUI(const std::string& sceneName) {
    if (sceneName == "TITLE") {
        // まだタイトルUIは作ってないけど、こんな感じで作る予定
        //  auto ui = std::make_unique<TitleUI>();
        //        ui->Initialize();
        //        AddUI("TITLE", std::move(ui));
    } else if (sceneName == "GAMEPLAY") {
        auto ui = std::make_unique<GamePlayUI>();
        ui->Initialize();
        AddUI("GAMEPLAY", std::move(ui));
    } else if (sceneName == "GAMEPLAY") {
        auto ui = std::make_unique<GamePlayUI>();
        ui->Initialize();
        AddUI("GAMEPLAY", std::move(ui));
    }
}