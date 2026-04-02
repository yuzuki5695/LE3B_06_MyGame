#include "EditorManager.h"
#include <ImGuiManager.h>
#include <CopylmageCommon.h>
#include <EditorConsole.h>
#include <ConsoleWindow.h>
#include <externals/imgui/imgui_internal.h>

namespace MyEngine {
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
        MessageService::Initialize(); // メッセージサービスの初期化（これで言語データがロードされる）
        // 1. レイアウト管理のインスタンス化
        layout_ = std::make_unique<EditorLayout>();
        // 重要：LogWindowを管理リストに追加する
        windows_.push_back(std::make_unique<ConsoleWindow>());
    }


    void EditorManager::Draw(SrvManager* srvmanager) {
#ifdef USE_IMGUI
        // 1. レイアウト（外枠とドッキング）の描画を委譲 
        // windows_ を渡すことでメニューバーからの表示切り替えも連動します
        layout_->Render(srvmanager, windows_);
#endif
    }

    void EditorManager::ShowMenuBar() {

    }

    void EditorManager::ShowSettingsModal() {
    }

    void EditorManager::SetLanguage(Editor::Language lang) {
        // マネージャ内の変数を更新
        language_ = lang;
        // MessageService側の静的変数を更新（これで以降の GetText の結果が変わる）
        MessageService::SetLanguage(lang);
        // ログに出力
        //Log(Editor::MessageId::LangChanged);
    }

    // 登録関数
    void EditorManager::AddWindow(std::unique_ptr<IEditorWindow> window) {
        windows_.push_back(std::move(window));
    }

    // ログ中継
    void EditorManager::Log(const std::string& message) {
        EditorConsole::GetInstance()->AddLog(message);
    }
}