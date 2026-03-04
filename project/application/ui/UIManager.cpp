#include "UIManager.h"
#include <SceneManager.h>
#include <GamePlayUI.h>
#include <TitleUI.h>

// 静的メンバ変数の定義
std::unique_ptr<UIManager> UIManager::instance = nullptr;

// シングルトンインスタンスの取得
UIManager* UIManager::GetInstance() {
	if (!instance) {
		instance = std::make_unique<UIManager>();
	}
	return instance.get();
}

// 終了
void UIManager::Finalize() {
	// シーンマネージャのインスタンスの解放
	instance.reset();
}

void UIManager::Initialize() {
    // 前のシーンのUIを削除
    Clear();

    // 現在のシーン状況を確認
    // SceneManagerが管理している実行中シーンから名前を取得する
    std::string currentScene = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();

    // シーン名に紐付いたUIクラスをインスタンス化
    if (currentScene == "TITLE") {
        AddUI("TITLE", std::make_unique<TitleUI>());
    } 
    else if (currentScene == "GAMEPLAY") {
        AddUI("GAMEPLAY", std::make_unique<GamePlayUI>());
    }

    // 生成したUIの個別初期化
    for (auto& ui : uiList_) {
        ui->Initialize();
    }
}

void UIManager::Update() {
    // リストに登録されているUIを順次更新
    for (auto& ui : uiList_) {
        if (ui->IsActive()) {
            ui->Update();
        }
    }
}

void UIManager::Draw() {
    // リストにあるUIを画面に描画
    for (auto& ui : uiList_) {
        if (ui->IsActive()) {
            ui->Draw();
        }
    }
}

void UIManager::AddUI(const std::string& sceneName, std::unique_ptr<BaseUI> ui) {
    // どのシーンに属しているかの情報をUI自身に持たせてリストに追加
    ui->SetTargetSceneName(sceneName);
    uiList_.push_back(std::move(ui));
}

void UIManager::Clear() {
    uiList_.clear();
}