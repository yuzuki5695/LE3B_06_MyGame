#include "EditorLayout.h"
#include <MessageService.h>
#include <CopylmageCommon.h>
#include <ImGuiManager.h>
#include <LogManager.h>
#include <externals/imgui/imgui_internal.h>
#include <EditorManager.h>

using namespace Editor;

void EditorLayout::Render(SrvManager* srvmanager, std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    LT = [](const std::string& key) { return MessageService::GetText(key); };
    // 1. メインメニュ-パネル
    ShowMenuBar(windows);
    // 2. 左側パネル
    DrawLeftPanel(srvmanager,windows);
}

void EditorLayout::ShowMenuBar(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
    if (ImGui::BeginMainMenuBar()) {
        // 表示メニュー
        if (ImGui::BeginMenu(LT("Menu.Settings").c_str())) {
            // 「言語設定」サブメニュー
            if (ImGui::BeginMenu(LT("Language.Title").c_str())) {
                // 日本語への切り替え
                bool isJp = (MessageService::GetCurrentLanguage() == Language::Japanese);
                if (ImGui::MenuItem(LT("Language.Japanese").c_str(), nullptr, isJp)) {
                    MessageService::SetLanguage(Language::Japanese);
                    // 言語切り替えのログを出力
                    LogManager::GetInstance()->AddLocalizedLog("System.LangChanged");
                }
                // 英語への切り替え
                bool isEn = (MessageService::GetCurrentLanguage() == Language::English);
                if (ImGui::MenuItem(LT("Language.English").c_str(), nullptr, isEn)) {
                    MessageService::SetLanguage(Language::English);
                    // 言語切り替えのログを出力
                    LogManager::GetInstance()->AddLocalizedLog("System.LangChanged");
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        // ... 他のメニュー項目（Optionsなど） ...
        ImGui::EndMainMenuBar();
    }
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
            // windowsリストの中から LogWindow (System Console) を探して描画する
            for (auto& window : windows) {
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


//
//void EditorLayout::DrawRightPanel(std::vector<std::unique_ptr<IEditorWindow>>& windows) {
//   const float kGameViewW = 640.0f;
//    ImGuiViewport* viewport = ImGui::GetMainViewport();
//
//    float dockPosX = viewport->WorkPos.x + kGameViewW;
//    float dockWidth = viewport->WorkSize.x - kGameViewW;
//
//    ImGui::SetNextWindowPos(ImVec2(dockPosX, viewport->WorkPos.y));
//    ImGui::SetNextWindowSize(ImVec2(dockWidth, viewport->WorkSize.y));
//
//    ImGuiWindowFlags root_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
//        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
//
//    ImGui::Begin("RightRoot", nullptr, root_flags);
//    ShowMenuBar(windows);
//
//    ImGuiID dockspace_id = ImGui::GetID("RightDockSpace");
//
//    // 初回起動時やリセット要求時にレイアウトを構築
//    if (requestResetLayout_ || !ImGui::DockBuilderGetNode(dockspace_id)) {
//        requestResetLayout_ = false;
//
//        // 既存のレイアウトを破棄して作り直し
//        ImGui::DockBuilderRemoveNode(dockspace_id);
//        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
//        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);
//
//        ImGuiID dock_main_id = dockspace_id;
//
//        // 登録されているすべてのウィンドウをメインのドッキングスペースに流し込む
//        for (auto& window : windows) {
//            ImGui::DockBuilderDockWindow(window->GetName(), dock_main_id);
//        }
//
//        ImGui::DockBuilderFinish(dockspace_id);
//    }
//
//    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
//    ImGui::End();
//}