#include "EditorLayout.h"
#include <MessageService.h>
#include <CopylmageCommon.h>
#include <ImGuiManager.h>
#include <EditorConsole.h>
#include <externals/imgui/imgui_internal.h>
#include <EditorManager.h>

using namespace Editor;

void EditorLayout::Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    // 多言語テキスト取得ラムダを初期化
    LT = [](const std::string& key) { return MessageService::GetText(key); };

    // 1. メインメニューバー描画
    ShowMenuBar(windows);

    // 2. 左側パネル描画（ゲームビュー + コンソール）
    DrawLeftPanel(srvmanager, windows);

    // 3. 右側パネル描画（ドッキング領域）
    DrawRightPanel(windows);
}

void EditorLayout::ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    // UI描画はMenuBarクラスへ委譲
    menuBar_.Render(LT);
}

void EditorLayout::DrawLeftPanel(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    // パネルの基本サイズ設定
    const float kGameViewW = 640.0f;
    const float kGameViewH = 360.0f;
    // メイン画面の表示領域を取得
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    // パネルの位置とサイズを固定（左端に配置）
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2(kGameViewW, viewport->WorkSize.y));
    // パネルの挙動フラグ（移動不可、リサイズ不可、タイトルバーなし、ドッキング不可）
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;

    ImGui::Begin("LeftPanel", nullptr, flags);
    {
        // 上段：ゲームビュー(描画結果をテクスチャとして表示）
        ImGui::BeginChild("GameView", ImVec2(kGameViewW, kGameViewH), true, ImGuiWindowFlags_NoDocking);
        {
            // レンダーターゲットからSRVインデックスを取得し、ImGui用のテクスチャIDに変換
            uint32_t srvIdx = CopylmageCommon::GetInstance()->GetSrvIndex();
            ImTextureID texID = (ImTextureID)(void*)(uintptr_t)srvmanager->GetGPUDescriptorHandle(srvIdx).ptr;

            // 指定サイズで画像を描画
            ImGui::Image(texID, ImVec2(kGameViewW, kGameViewH));
        }
        ImGui::EndChild();

        // 下段：ログ表示
        ImGui::BeginChild("SystemConsoleChild", ImVec2(0, 0), false);
        {
            // windowsリストの中から ConsoleWindow (System Console) を探して描画する
            for (std::unique_ptr<IEditorWindow>& window : windows) {
                if (std::string(window->GetName()) == "System Console") {
                    ImGui::Text(LT("Console.Title").c_str());
                    ImGui::Separator();
                    window->Draw();
                    break;
                }
            }
        }
        ImGui::EndChild();

    }
    ImGui::End();
}

void EditorLayout::DrawRightPanel(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    const float kGameViewW = 640.0f;
    ImGuiViewport* viewport = ImGui::GetMainViewport(); 
    // 右側領域計算
    float dockPosX = viewport->WorkPos.x + kGameViewW;
    float dockWidth = viewport->WorkSize.x - kGameViewW;

    ImGui::SetNextWindowPos(ImVec2(dockPosX, viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(dockWidth, viewport->WorkSize.y));
    // ルートウィンドウフラグ
    ImGuiWindowFlags root_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    // 右側ルートウィンドウ
    ImGui::Begin("RightPanelRoot", nullptr, root_flags);
    ImGuiID dockspace_id = ImGui::GetID("RightDockSpace");
    // 初回のみDock初期化
    static bool dock_initialized = false;
    if (!dock_initialized) {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderFinish(dockspace_id);
        dock_initialized = true;
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();

    // =============================
    // 開いているオブジェクトウィンドウ描画
    // =============================
    ObjectMenu* objectMenu = menuBar_.GetObjectMenu();
    const std::vector<Editor::EditorObjectInfo>& openWindows = objectMenu->GetOpenWindows();
    const auto& registeredObjects = EditorEntityRegistry::Instance().GetObjects();
    for (auto it = openWindows.begin(); it != openWindows.end(); ) {
      
        // オブジェクト生存チェック
        auto isAlive = std::any_of(registeredObjects.begin(), registeredObjects.end(),
            [&](const auto& reg) { return reg.objectPtr == it->objectPtr; });

        if (!isAlive) {
            it = objectMenu->CloseWindow(it->name); // 消えていたら閉じる
            continue;
        }

        // 次のウィンドウをドッキングスペースへ誘導
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);

        // 「×」ボタンを表示するためのフラグ
        bool is_open = true;
        
        // タブとして描画
        if (ImGui::Begin(it->name.c_str(), &is_open, ImGuiWindowFlags_NoCollapse)) {
            if (it->objectPtr) {
                // カテゴリに応じて DrawImGui を呼び出す
                switch (it->category) {
                    // カテゴリごとに描画処理を分岐
                case Editor::EditorObjectCategory::Object3D:
                    static_cast<Object3d*>(it->objectPtr)->DrawImGui(it->name.c_str());
                    break;
                case Editor::EditorObjectCategory::Object2D: 
                    static_cast<Sprite*>(it->objectPtr)->DrawImGui(it->name.c_str());
                    break;
                }
            }
        }
        ImGui::End();
        // ×ボタンが押された場合は閉じる
        if (!is_open) {
            it = objectMenu->CloseWindow(it->name); 
        } else {
            ++it;
        }
    }
}