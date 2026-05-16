#include "EditorManager.h"
#include <ImGuiManager.h>
#include <CopylmageCommon.h>
#include <EditorConsole.h>
#include <ConsoleWindow.h>
#include <externals/imgui/imgui_internal.h>
#include <EditorEntityRegistry.h>
#include <Input.h>
#include <ObjectMenu.h>
#include <CameraMenu.h>


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
#ifdef USE_IMGUI
        // メッセージサービスの初期化
        MessageService::Initialize();
        // レイアウト管理のインスタンス化
        layout_ = std::make_unique<EditorLayout>();
		// レイアウトの初期化
        layout_->Initialize();
        // LogWindowを管理リストに追加する
        windows_.push_back(std::make_unique<ConsoleWindow>());
#endif // USE_IMGUI
    }


    void EditorManager::Draw(SrvManager* srvmanager) {
#ifdef USE_IMGUI
        // =====================================
        // Fullscreen解除
        // =====================================

        if (layout_->IsGameViewFullscreen()) {
            // フルスクリーン状態でESCキーが押されたら解除
            if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
                // フルスクリーン解除
                layout_->SetGameViewFullscreen(false);
            }
        }
        // レイアウトの描画
        layout_->Render(srvmanager, windows_);
#endif // USE_IMGUI
    }

    void EditorManager::SetLanguage(EditorTypes::Language lang) {
#ifdef USE_IMGUI
        // マネージャ内の変数を更新
        language_ = lang;
        // MessageService側の静的変数を更新（これで以降の GetText の結果が変わる）
        MessageService::SetLanguage(lang);
#endif // USE_IMGUI
    }

    // 登録関数
    void EditorManager::AddWindow(std::unique_ptr<IEditorWindow> window) {
#ifdef USE_IMGUI
		// ウィンドウを管理リストに登録
        windows_.push_back(std::move(window));
#endif // USE_IMGUI
    }

    // ログ中継
    void EditorManager::Log(const std::string& message) {
#ifdef USE_IMGUI
		// EditorConsoleにログを追加
        EditorConsole::GetInstance()->AddLog(message);
#endif // USE_IMGUI
    }

    void EditorManager::OnSceneChanged() {
#ifdef USE_IMGUI
        // 登録オブジェクト削除
        EditorEntityRegistry::Instance().Clear();

        // 開いているObjectWindow削除
        if (layout_) { 
            ObjectMenu* objectMenu = layout_->GetMenuBar()->GetMenu<ObjectMenu>();
            CameraMenu* cameraMenu = layout_->GetMenuBar()->GetMenu<CameraMenu>();
            // 開いているウィンドウをすべて閉じる
            if (objectMenu) { objectMenu->ClearOpenWindows(); }
            if (cameraMenu) { cameraMenu->ClearOpenWindows(); }
        }
#endif // USE_IMGUI
    }
}