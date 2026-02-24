#include "EditorManager.h"
#include <ImGuiManager.h>
#include <CopylmageCommon.h>
#include <LogManager.h>
#include <LogWindow.h>
#include <externals/imgui/imgui_internal.h>
#include <SettingWindow.h>

// 静的メンバ変数の定義
std::unique_ptr<EditorManager> EditorManager::instance = nullptr;

// シングルトンインスタンスの取得
EditorManager* EditorManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<EditorManager>();
    }
    return instance.get();
}

// 終了
void EditorManager::Finalize() {
    instance.reset();  // `delete` 不要
}

void EditorManager::Initialize() {
    MessageService::Load("Localization");
    // 1. レイアウト管理のインスタンス化
    layout_ = std::make_unique<EditorLayout>();
    // 2. 各ウィンドウの登録
    // 今後 InspectorWindow などが増えてもここに追加するだけ
   // AddWindow(std::make_unique<LogWindow>());
    // 設定ウィンドウの登録（これだけでメニューも機能も追加される）
    AddWindow(std::make_unique<SettingWindow>());
    LogManager::GetInstance()->AddLog(Editor::MessageId::EngineInitialized);
}


void EditorManager::Draw(SrvManager* srvmanager) {
#ifdef USE_IMGUI
    // 1. レイアウト（外枠とドッキング）の描画を委譲 
    // windows_ を渡すことでメニューバーからの表示切り替えも連動します
    layout_->Render(srvmanager, windows_);

    // 2. 個別のウィンドウ（中身）の描画
    // LogWindow::Draw() などがここで呼ばれる
    for (auto& window : windows_) {
        if (window->isOpen) {
            window->Draw();
        }
    }
#endif
}

void EditorManager::ShowMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        // オプション (Options)
        if (ImGui::BeginMenu(MessageService::GetText("Editor.Menu.Options").c_str())) {
            if (ImGui::MenuItem(MessageService::GetText("Editor.Menu.Settings").c_str())) {
                showSettings_ = true; // 設定モーダルを開く
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void EditorManager::ShowSettingsModal() {
    if (!showSettings_) return;

    // タイトルを取得
    ImGui::Begin(
        MessageService::GetText("Editor.Settings.Title").c_str(),
        &showSettings_,
        ImGuiWindowFlags_AlwaysAutoResize);

    {
        // "言語設定" ラベル
        ImGui::Text("%s", MessageService::GetText("Editor.Settings.Language").c_str());

        // 現在の言語設定を取得
        Editor::Language current = language_;

        // ---- 日本語 (Japanese) ----
        if (ImGui::RadioButton(
            MessageService::GetText("Editor.Settings.Japanese").c_str(),
            current == Editor::Language::Japanese))
        {
            SetLanguage(Editor::Language::Japanese);
        }

        ImGui::SameLine();

        // ---- 英語 (English) ----
        if (ImGui::RadioButton(
            MessageService::GetText("Editor.Settings.English").c_str(),
            current == Editor::Language::English))
        {
            SetLanguage(Editor::Language::English);
        }

        ImGui::Separator();

        // 閉じるボタン
        if (ImGui::Button(MessageService::GetText("Editor.Settings.Close").c_str()))
        {
            showSettings_ = false;
        }
    }

    ImGui::End();
}

void EditorManager::SetLanguage(Editor::Language lang) {
    // マネージャ内の変数を更新
    language_ = lang;
    // MessageService側の静的変数を更新（これで以降の GetText の結果が変わる）
    MessageService::SetLanguage(lang);
    // ログに出力
    Log(Editor::MessageId::LangChanged);
}

// 登録関数
void EditorManager::AddWindow(std::unique_ptr<IEditorWindow> window) {
    windows_.push_back(std::move(window));
}

// ログ中継
void EditorManager::Log(const std::string& message) {
    LogManager::GetInstance()->AddLog(message);
}

void EditorManager::Log(Editor::MessageId id) {
    LogManager::GetInstance()->AddLog(id);
}